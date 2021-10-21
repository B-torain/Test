/*  PCSX2 - PS2 Emulator for PCs
 *  Copyright (C) 2002-2021  PCSX2 Dev Team
 *
 *  PCSX2 is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU Lesser General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  PCSX2 is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with PCSX2.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#include "PrecompiledHeader.h"

#include "OpenGLHostDisplay.h"
#include "common/Assertions.h"
#include "common/Console.h"
#include "common/ScopedGuard.h"
#include "common/StringUtil.h"
#include "common/GL/Program.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include <array>
#include <tuple>

class OpenGLHostDisplayTexture : public HostDisplayTexture
{
public:
	OpenGLHostDisplayTexture(GLuint texture, u32 width, u32 height, u32 layers, u32 levels)
		: m_texture(texture)
		, m_width(width)
		, m_height(height)
		, m_layers(layers)
		, m_levels(levels)
	{
	}
	~OpenGLHostDisplayTexture() override = default;

	void* GetHandle() const override { return reinterpret_cast<void*>(static_cast<uintptr_t>(m_texture)); }
	u32 GetWidth() const override { return m_width; }
	u32 GetHeight() const override { return m_height; }
	u32 GetLayers() const override { return m_layers; }
	u32 GetLevels() const override { return m_levels; }

	GLuint GetGLID() const { return m_texture; }

private:
	GLuint m_texture;
	u32 m_width;
	u32 m_height;
	u32 m_layers;
	u32 m_levels;
};

OpenGLHostDisplay::OpenGLHostDisplay() = default;

OpenGLHostDisplay::~OpenGLHostDisplay()
{
	pxAssertMsg(!m_gl_context, "Context should have been destroyed by now");
}

HostDisplay::RenderAPI OpenGLHostDisplay::GetRenderAPI() const
{
	return m_gl_context->IsGLES() ? RenderAPI::OpenGLES : RenderAPI::OpenGL;
}

void* OpenGLHostDisplay::GetRenderDevice() const
{
	return nullptr;
}

void* OpenGLHostDisplay::GetRenderContext() const
{
	return m_gl_context.get();
}

void* OpenGLHostDisplay::GetRenderSurface() const
{
	return nullptr;
}

std::unique_ptr<HostDisplayTexture> OpenGLHostDisplay::CreateTexture(u32 width, u32 height, u32 layers, u32 levels,
	const void* data, u32 data_stride, bool dynamic /* = false */)
{
	// clear error
	glGetError();

	GLuint id;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	if ((GLAD_GL_ARB_texture_storage || GLAD_GL_ES_VERSION_3_0) && !data)
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
	{
		Console.Error("Failed to create texture: 0x%X", error);
		glDeleteTextures(1, &id);
		return nullptr;
	}

	return std::make_unique<OpenGLHostDisplayTexture>(id, width, height, layers, levels);
}

void OpenGLHostDisplay::UpdateTexture(HostDisplayTexture* texture, u32 x, u32 y, u32 width, u32 height,
	const void* texture_data, u32 texture_data_stride)
{
	OpenGLHostDisplayTexture* tex = static_cast<OpenGLHostDisplayTexture*>(texture);

	GLint alignment;
	if (texture_data_stride & 1)
		alignment = 1;
	else if (texture_data_stride & 2)
		alignment = 2;
	else
		alignment = 4;

	GLint old_texture_binding = 0, old_alignment = 0, old_row_length = 0;
	glGetIntegerv(GL_TEXTURE_BINDING_2D, &old_texture_binding);
	glBindTexture(GL_TEXTURE_2D, tex->GetGLID());

	glGetIntegerv(GL_UNPACK_ALIGNMENT, &old_alignment);
	glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);

	glGetIntegerv(GL_UNPACK_ROW_LENGTH, &old_row_length);
	glPixelStorei(GL_UNPACK_ROW_LENGTH, texture_data_stride / sizeof(u32));

	glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, GL_RGBA8, GL_UNSIGNED_BYTE, texture_data);

	glPixelStorei(GL_UNPACK_ROW_LENGTH, old_row_length);

	glPixelStorei(GL_UNPACK_ALIGNMENT, old_alignment);
	glBindTexture(GL_TEXTURE_2D, old_texture_binding);
}

void OpenGLHostDisplay::SetVSync(VsyncMode mode)
{
	if (m_gl_context->GetWindowInfo().type == WindowInfo::Type::Surfaceless)
		return;

	// Window framebuffer has to be bound to call SetSwapInterval.
	GLint current_fbo = 0;
	glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &current_fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	if (mode != VsyncMode::Adaptive || !m_gl_context->SetSwapInterval(-1))
		m_gl_context->SetSwapInterval(static_cast<s32>(mode != VsyncMode::Off));

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, current_fbo);
}

const char* OpenGLHostDisplay::GetGLSLVersionString() const
{
	if (GetRenderAPI() == RenderAPI::OpenGLES)
	{
		if (GLAD_GL_ES_VERSION_3_0)
			return "#version 300 es";
		else
			return "#version 100";
	}
	else
	{
		if (GLAD_GL_VERSION_3_3)
			return "#version 330";
		else
			return "#version 130";
	}
}

std::string OpenGLHostDisplay::GetGLSLVersionHeader() const
{
	std::string header = GetGLSLVersionString();
	header += "\n\n";
	if (GetRenderAPI() == RenderAPI::OpenGLES)
	{
		header += "precision highp float;\n";
		header += "precision highp int;\n\n";
	}

	return header;
}

bool OpenGLHostDisplay::HasRenderDevice() const
{
	return static_cast<bool>(m_gl_context);
}

bool OpenGLHostDisplay::HasRenderSurface() const
{
	return m_window_info.type != WindowInfo::Type::Surfaceless;
}

bool OpenGLHostDisplay::CreateRenderDevice(const WindowInfo& wi, std::string_view adapter_name, bool threaded_presentation, bool debug_device)
{
	m_gl_context = GL::Context::Create(wi);
	if (!m_gl_context)
	{
		Console.Error("Failed to create any GL context");
		m_gl_context.reset();
		return false;
	}

	m_window_info = m_gl_context->GetWindowInfo();
	return true;
}

bool OpenGLHostDisplay::InitializeRenderDevice(std::string_view shader_cache_directory, bool debug_device)
{
	// Start with vsync off.
	m_gl_context->SetSwapInterval(0);
	GL::Program::ResetLastProgram();
	return true;
}

bool OpenGLHostDisplay::MakeRenderContextCurrent()
{
	if (!m_gl_context->MakeCurrent())
	{
		Console.Error("Failed to make GL context current");
		return false;
	}

	return true;
}

bool OpenGLHostDisplay::DoneRenderContextCurrent()
{
	return m_gl_context->DoneCurrent();
}

void OpenGLHostDisplay::DestroyRenderDevice()
{
	if (!m_gl_context)
		return;

	m_gl_context->DoneCurrent();
	m_gl_context.reset();
}

bool OpenGLHostDisplay::ChangeRenderWindow(const WindowInfo& new_wi)
{
	pxAssert(m_gl_context);

	if (!m_gl_context->ChangeSurface(new_wi))
	{
		Console.Error("Failed to change surface");
		return false;
	}

	m_window_info = m_gl_context->GetWindowInfo();

	if (new_wi.type != WindowInfo::Type::Surfaceless)
	{
		// reset vsync rate, since it (usually) gets lost
		if (m_vsync_mode != VsyncMode::Adaptive || !m_gl_context->SetSwapInterval(-1))
			m_gl_context->SetSwapInterval(static_cast<s32>(m_vsync_mode != VsyncMode::Off));
	}

	return true;
}

void OpenGLHostDisplay::ResizeRenderWindow(s32 new_window_width, s32 new_window_height, float new_window_scale)
{
	if (!m_gl_context)
		return;

	m_window_info.surface_scale = new_window_scale;
	if (m_window_info.surface_width == static_cast<u32>(new_window_width) &&
		m_window_info.surface_height == static_cast<u32>(new_window_height))
	{
		return;
	}

	m_gl_context->ResizeSurface(static_cast<u32>(new_window_width), static_cast<u32>(new_window_height));
	m_window_info = m_gl_context->GetWindowInfo();
}

bool OpenGLHostDisplay::SupportsFullscreen() const
{
	return false;
}

bool OpenGLHostDisplay::IsFullscreen()
{
	return false;
}

bool OpenGLHostDisplay::SetFullscreen(bool fullscreen, u32 width, u32 height, float refresh_rate)
{
	return false;
}

HostDisplay::AdapterAndModeList OpenGLHostDisplay::GetAdapterAndModeList()
{
	AdapterAndModeList aml;

	if (m_gl_context)
	{
		for (const GL::Context::FullscreenModeInfo& fmi : m_gl_context->EnumerateFullscreenModes())
			aml.fullscreen_modes.push_back(GetFullscreenModeString(fmi.width, fmi.height, fmi.refresh_rate));
	}

	return aml;
}

void OpenGLHostDisplay::DestroyRenderSurface()
{
	if (!m_gl_context)
		return;

	m_window_info = {};
	if (!m_gl_context->ChangeSurface(m_window_info))
		Console.Error("Failed to switch to surfaceless");
}

bool OpenGLHostDisplay::CreateImGuiContext()
{
	return ImGui_ImplOpenGL3_Init(GetGLSLVersionString());
}

void OpenGLHostDisplay::DestroyImGuiContext()
{
	ImGui_ImplOpenGL3_Shutdown();
}

bool OpenGLHostDisplay::UpdateImGuiFontTexture()
{
	ImGui_ImplOpenGL3_DestroyFontsTexture();
	return ImGui_ImplOpenGL3_CreateFontsTexture();
}

bool OpenGLHostDisplay::BeginPresent(bool frame_skip)
{
	if (frame_skip || m_window_info.type == WindowInfo::Type::Surfaceless)
	{
		ImGui::EndFrame();
		return false;
	}

	glDisable(GL_SCISSOR_TEST);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, m_window_info.surface_width, m_window_info.surface_height);

	return true;
}

void OpenGLHostDisplay::EndPresent()
{
	// clear out pipeline bindings, since imgui doesn't use them
	glBindProgramPipeline(0);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
	glActiveTexture(GL_TEXTURE0);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	GL::Program::ResetLastProgram();

	m_gl_context->SwapBuffers();
}

