//#version 420 // Keep it for text editor detection

// Require for bit operation
//#extension GL_ARB_gpu_shader5 : enable

#define FMT_32 0
#define FMT_24 1
#define FMT_16 2

// APITRACE_DEBUG enables forced pixel output to easily detect
// the fragment computed by primitive
#define APITRACE_DEBUG 0
// TEX_COORD_DEBUG output the uv coordinate as color. It is useful
// to detect bad sampling due to upscaling
//#define TEX_COORD_DEBUG
// Just copy directly the texture coordinate
#ifdef TEX_COORD_DEBUG
#define PS_TFX 1
#define PS_TCC 1
#endif

#define SW_BLEND (PS_BLEND_A || PS_BLEND_B || PS_BLEND_D)
#define SW_BLEND_NEEDS_RT (SW_BLEND && (PS_BLEND_A == 1 || PS_BLEND_B == 1 || PS_BLEND_C == 1 || PS_BLEND_D == 1))

#ifdef FRAGMENT_SHADER

#if !defined(BROKEN_DRIVER) && defined(GL_ARB_enhanced_layouts) && GL_ARB_enhanced_layouts
layout(location = 0)
#endif
in SHADER
{
    vec4 t_float;
    vec4 t_int;

    #if PS_IIP != 0
      vec4 c;
    #else
      flat vec4 c;
    #endif
} PSin;

#define TARGET_0_QUALIFIER out

// Only enable framebuffer fetch when we actually need it.
#if HAS_FRAMEBUFFER_FETCH && (PS_TEX_IS_FB == 1 || PS_FBMASK || SW_BLEND_NEEDS_RT || PS_DATE != 0)
  // We need to force the colour to be defined here, to read from it.
  // Basically the only scenario where this'll happen is RGBA masked and DATE is active.
  #undef PS_NO_COLOR
  #define PS_NO_COLOR 0
  #if defined(GL_EXT_shader_framebuffer_fetch)
    #undef TARGET_0_QUALIFIER
    #define TARGET_0_QUALIFIER inout
    #define LAST_FRAG_COLOR SV_Target0
  #endif
#endif

#if !PS_NO_COLOR
#if !defined(DISABLE_DUAL_SOURCE) && !PS_NO_COLOR1
  // Same buffer but 2 colors for dual source blending
  layout(location = 0, index = 0) TARGET_0_QUALIFIER vec4 SV_Target0;
  layout(location = 0, index = 1) out vec4 SV_Target1;
#else
  layout(location = 0) TARGET_0_QUALIFIER vec4 SV_Target0;
#endif
#endif

layout(binding = 1) uniform sampler2D PaletteSampler;

#if !HAS_FRAMEBUFFER_FETCH
layout(binding = 2) uniform sampler2D RtSampler; // note 2 already use by the image below
#endif

#ifndef DISABLE_GL42_image
#if PS_DATE > 0
// Performance note: images mustn't be declared if they are unused. Otherwise it will
// require extra shader validation.

// FIXME how to declare memory access
layout(r32i, binding = 3) uniform iimage2D img_prim_min;
// WARNING:
// You can't enable it if you discard the fragment. The depth is still
// updated (shadow in Shin Megami Tensei Nocturne)
//
// early_fragment_tests must still be enabled in the first pass of the 2 passes algo
// First pass search the first primitive that will write the bad alpha value. Value
// won't be written if the fragment fails the depth test.
//
// In theory the best solution will be do
// 1/ copy the depth buffer
// 2/ do the full depth (current depth writes are disabled)
// 3/ restore the depth buffer for 2nd pass
// Of course, it is likely too costly.
#if PS_DATE == 1 || PS_DATE == 2
layout(early_fragment_tests) in;
#endif

// I don't remember why I set this parameter but it is surely useless
//layout(pixel_center_integer) in vec4 gl_FragCoord;
#endif
#else
// use basic stencil
#endif

vec4 sample_c(vec2 uv)
{
#if PS_TEX_IS_FB == 1
#if HAS_FRAMEBUFFER_FETCH
    return LAST_FRAG_COLOR;
#else
    return texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0);
#endif
#else

#if PS_POINT_SAMPLER
    // Weird issue with ATI/AMD cards,
    // it looks like they add 127/128 of a texel to sampling coordinates
    // occasionally causing point sampling to erroneously round up.
    // I'm manually adjusting coordinates to the centre of texels here,
    // though the centre is just paranoia, the top left corner works fine.
    // As of 2018 this issue is still present.
    uv = (trunc(uv * WH.zw) + vec2(0.5, 0.5)) / WH.zw;
#endif
    uv *= STScale;

#if PS_AUTOMATIC_LOD == 1
    return texture(TextureSampler, uv);
#elif PS_MANUAL_LOD == 1
    // FIXME add LOD: K - ( LOG2(Q) * (1 << L))
    float K = MinMax.x;
    float L = MinMax.y;
    float bias = MinMax.z;
    float max_lod = MinMax.w;

    float gs_lod = K - log2(abs(PSin.t_float.w)) * L;
    // FIXME max useful ?
    //float lod = max(min(gs_lod, max_lod) - bias, 0.0f);
    float lod = min(gs_lod, max_lod) - bias;

    return textureLod(TextureSampler, uv, lod);
#else
    return textureLod(TextureSampler, uv, 0); // No lod
#endif

#endif
}

vec4 sample_p(float idx)
{
    return texture(PaletteSampler, vec2(idx, 0.0f));
}

vec2 clamp_wrap_uv_2(uint mode, vec2 uv, float tex_size, vec2 min_max, uvec2 msk_fix)
{
    if (mode == 2)
    {
        return clamp(uv, min_max.xx, min_max.yy);
    }
    if (mode == 3)
    {
        #if PS_FST == 0
            // wrap negative uv coords to avoid an off by one error that shifted
            // textures. Fixes Xenosaga's hair issue.
            uv = fract(uv);
        #endif
        return vec2((uvec2(uv * tex_size) & msk_fix.xx) | msk_fix.yy) / tex_size;
    }
    return uv;
}

vec4 clamp_wrap_uv(vec4 uv)
{
#if PS_INVALID_TEX0 == 1
    vec2 tex_size = WH.zw;
#else
    vec2 tex_size = WH.xy;
#endif

    uv.xz = clamp_wrap_uv_2(PS_WMS, uv.xz, tex_size.x, MinMax.xz, MskFix.xz);
    uv.yw = clamp_wrap_uv_2(PS_WMT, uv.yw, tex_size.y, MinMax.yw, MskFix.yw);

    return uv;
}

mat4 sample_4c(vec4 uv)
{
    mat4 c;

    // Note: texture gather can't be used because of special clamping/wrapping
    // Also it doesn't support lod
    c[0] = sample_c(uv.xy);
    c[1] = sample_c(uv.zy);
    c[2] = sample_c(uv.xw);
    c[3] = sample_c(uv.zw);

    return c;
}

vec4 sample_4_index(vec4 uv)
{
    vec4 c;

    // Either GS will send a texture that contains a single channel
    // in this case the red channel is remapped as alpha channel
    //
    // Or we have an old RT (ie RGBA8) that contains index (4/8) in the alpha channel

    // Note: texture gather can't be used because of special clamping/wrapping
    // Also it doesn't support lod
    c.x = sample_c(uv.xy).a;
    c.y = sample_c(uv.zy).a;
    c.z = sample_c(uv.xw).a;
    c.w = sample_c(uv.zw).a;

    uvec4 i = uvec4(c * 255.0f + 0.5f); // Denormalize value

#if PS_PAL_FMT == 1
    // 4HL
    return vec4(i & 0xFu) / 255.0f;

#elif PS_PAL_FMT == 2
    // 4HH
    return vec4(i >> 4u) / 255.0f;

#else
    // Most of texture will hit this code so keep normalized float value

    // 8 bits
    return c;
#endif

}

mat4 sample_4p(vec4 u)
{
    mat4 c;

    c[0] = sample_p(u.x);
    c[1] = sample_p(u.y);
    c[2] = sample_p(u.z);
    c[3] = sample_p(u.w);

    return c;
}

int fetch_raw_depth()
{
#if PS_TEX_IS_FB == 1
#if HAS_FRAMEBUFFER_FETCH
    return int(LAST_FRAG_COLOR.r * exp2(32.0f));
#else
    return int(texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0).r * exp2(32.0f));
#endif
#else
    return int(texelFetch(TextureSampler, ivec2(gl_FragCoord.xy), 0).r * exp2(32.0f));
#endif
}

vec4 fetch_raw_color()
{
#if PS_TEX_IS_FB == 1
#if HAS_FRAMEBUFFER_FETCH
    return LAST_FRAG_COLOR;
#else
    return texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0);
#endif
#else
    return texelFetch(TextureSampler, ivec2(gl_FragCoord.xy), 0);
#endif
}

vec4 fetch_c(ivec2 uv)
{
    return texelFetch(TextureSampler, ivec2(uv), 0);
}

//////////////////////////////////////////////////////////////////////
// Depth sampling
//////////////////////////////////////////////////////////////////////
int clamp_wrap_uv_depth_1(uint mode, int uv, ivec2 msk_fix)
{
    // Keep the full precision
    // It allow to multiply the ScalingFactor before the 1/16 coeff
    ivec2 mask = msk_fix << 4;

    if (mode == 2)
        return clamp(uv, mask.x, mask.y);
    if (mode == 3)
        return (uv & mask.x) | mask.y;
    return uv;
}

ivec2 clamp_wrap_uv_depth(ivec2 uv)
{
    uv.x = clamp_wrap_uv_depth_1(PS_WMS, uv.x, ivec2(MskFix.xz));
    uv.y = clamp_wrap_uv_depth_1(PS_WMT, uv.y, ivec2(MskFix.yw));
    return uv;
}

vec4 sample_depth(vec2 st)
{
    vec2 uv_f = vec2(clamp_wrap_uv_depth(ivec2(st))) * vec2(float(PS_SCALE_FACTOR)) * vec2(1.0f/16.0f);
    ivec2 uv = ivec2(uv_f);

    vec4 t = vec4(0.0f);
#if PS_TALES_OF_ABYSS_HLE == 1
    // Warning: UV can't be used in channel effect
    int depth = fetch_raw_depth();

    // Convert msb based on the palette
    t = texelFetch(PaletteSampler, ivec2((depth >> 8) & 0xFF, 0), 0) * 255.0f;

#elif PS_URBAN_CHAOS_HLE == 1
    // Depth buffer is read as a RGB5A1 texture. The game try to extract the green channel.
    // So it will do a first channel trick to extract lsb, value is right-shifted.
    // Then a new channel trick to extract msb which will shifted to the left.
    // OpenGL uses a FLOAT32 format for the depth so it requires a couple of conversion.
    // To be faster both steps (msb&lsb) are done in a single pass.

    // Warning: UV can't be used in channel effect
    int depth = fetch_raw_depth();

    // Convert lsb based on the palette
    t = texelFetch(PaletteSampler, ivec2((depth & 0xFF), 0), 0) * 255.0f;

    // Msb is easier
    float green = float((depth >> 8) & 0xFF) * 36.0f;
    green = min(green, 255.0f);

    t.g += green;


#elif PS_DEPTH_FMT == 1
    // Based on ps_convert_float32_rgba8 of convert
    // Convert a GL_FLOAT32 depth texture into a RGBA color texture
    uint d = uint(fetch_c(uv).r * exp2(32.0f));
    t = vec4(uvec4((d & 0xFFu), ((d >> 8) & 0xFFu), ((d >> 16) & 0xFFu), (d >> 24)));

#elif PS_DEPTH_FMT == 2
    // Based on ps_convert_float16_rgb5a1 of convert
    // Convert a GL_FLOAT32 (only 16 lsb) depth into a RGB5A1 color texture
    uint d = uint(fetch_c(uv).r * exp2(32.0f));
    t = vec4(uvec4((d & 0x1Fu), ((d >> 5) & 0x1Fu), ((d >> 10) & 0x1Fu), (d >> 15) & 0x01u)) * vec4(8.0f, 8.0f, 8.0f, 128.0f);

#elif PS_DEPTH_FMT == 3
    // Convert a RGBA/RGB5A1 color texture into a RGBA/RGB5A1 color texture
    t = fetch_c(uv) * 255.0f;

#endif


    // warning t ranges from 0 to 255
#if (PS_AEM_FMT == FMT_24)
    t.a = ( (PS_AEM == 0) || any(bvec3(t.rgb))  ) ? 255.0f * TA.x : 0.0f;
#elif (PS_AEM_FMT == FMT_16)
    t.a = t.a >= 128.0f ? 255.0f * TA.y : ( (PS_AEM == 0) || any(bvec3(t.rgb)) ) ? 255.0f * TA.x : 0.0f;
#endif


    return t;
}

//////////////////////////////////////////////////////////////////////
// Fetch a Single Channel
//////////////////////////////////////////////////////////////////////
vec4 fetch_red()
{
#if PS_DEPTH_FMT == 1 || PS_DEPTH_FMT == 2
    int depth = (fetch_raw_depth()) & 0xFF;
    vec4 rt = vec4(depth) / 255.0f;
#else
    vec4 rt = fetch_raw_color();
#endif
    return sample_p(rt.r) * 255.0f;
}

vec4 fetch_green()
{
#if PS_DEPTH_FMT == 1 || PS_DEPTH_FMT == 2
    int depth = (fetch_raw_depth() >> 8) & 0xFF;
    vec4 rt = vec4(depth) / 255.0f;
#else
    vec4 rt = fetch_raw_color();
#endif
    return sample_p(rt.g) * 255.0f;
}

vec4 fetch_blue()
{
#if PS_DEPTH_FMT == 1 || PS_DEPTH_FMT == 2
    int depth = (fetch_raw_depth() >> 16) & 0xFF;
    vec4 rt = vec4(depth) / 255.0f;
#else
    vec4 rt = fetch_raw_color();
#endif
    return sample_p(rt.b) * 255.0f;
}

vec4 fetch_alpha()
{
    vec4 rt = fetch_raw_color();
    return sample_p(rt.a) * 255.0f;
}

vec4 fetch_rgb()
{
    vec4 rt = fetch_raw_color();
    vec4 c = vec4(sample_p(rt.r).r, sample_p(rt.g).g, sample_p(rt.b).b, 1.0f);
    return c * 255.0f;
}

vec4 fetch_gXbY()
{
#if PS_DEPTH_FMT == 1 || PS_DEPTH_FMT == 2
    int depth = fetch_raw_depth();
    int bg = (depth >> (8 + ChannelShuffle.w)) & 0xFF;
    return vec4(bg);
#else
    ivec4 rt = ivec4(fetch_raw_color() * 255.0f);
    int green = (rt.g >> ChannelShuffle.w) & ChannelShuffle.z;
    int blue  = (rt.b << ChannelShuffle.y) & ChannelShuffle.x;
    return vec4(green | blue);
#endif
}

//////////////////////////////////////////////////////////////////////

vec4 sample_color(vec2 st)
{
#if (PS_TCOFFSETHACK == 1)
    st += TC_OffsetHack.xy;
#endif

    vec4 t;
    mat4 c;
    vec2 dd;

    // FIXME I'm not sure this condition is useful (I think code will be optimized)
#if (PS_LTF == 0 && PS_AEM_FMT == FMT_32 && PS_PAL_FMT == 0 && PS_WMS < 2 && PS_WMT < 2)
    // No software LTF and pure 32 bits RGBA texure without special texture wrapping
    c[0] = sample_c(st);
#ifdef TEX_COORD_DEBUG
    c[0].rg = st.xy;
#endif

#else
    vec4 uv;

    if(PS_LTF != 0)
    {
        uv = st.xyxy + HalfTexel;
        dd = fract(uv.xy * WH.zw);
#if (PS_FST == 0)
        // Background in Shin Megami Tensei Lucifers
        // I suspect that uv isn't a standard number, so fract is outside of the [0;1] range
        // Note: it is free on GPU but let's do it only for float coordinate
        dd = clamp(dd, vec2(0.0f), vec2(1.0f));
#endif
    }
    else
    {
        uv = st.xyxy;
    }

    uv = clamp_wrap_uv(uv);

#if PS_PAL_FMT != 0
    c = sample_4p(sample_4_index(uv));
#else
    c = sample_4c(uv);
#endif

#ifdef TEX_COORD_DEBUG
    c[0].rg = uv.xy;
    c[1].rg = uv.xy;
    c[2].rg = uv.xy;
    c[3].rg = uv.xy;
#endif

#endif

    // PERF note: using dot product reduces by 1 the number of instruction
    // but I'm not sure it is equivalent neither faster.
    for (int i = 0; i < 4; i++)
    {
        //float sum = dot(c[i].rgb, vec3(1.0f));
#if (PS_AEM_FMT == FMT_24)
            c[i].a = ( (PS_AEM == 0) || any(bvec3(c[i].rgb))  ) ? TA.x : 0.0f;
            //c[i].a = ( (PS_AEM == 0) || (sum > 0.0f) ) ? TA.x : 0.0f;
#elif (PS_AEM_FMT == FMT_16)
            c[i].a = c[i].a >= 0.5 ? TA.y : ( (PS_AEM == 0) || any(bvec3(c[i].rgb)) ) ? TA.x : 0.0f;
            //c[i].a = c[i].a >= 0.5 ? TA.y : ( (PS_AEM == 0) || (sum > 0.0f) ) ? TA.x : 0.0f;
#endif
    }

#if(PS_LTF != 0)
    t = mix(mix(c[0], c[1], dd.x), mix(c[2], c[3], dd.x), dd.y);
#else
    t = c[0];
#endif

    // The 0.05f helps to fix the overbloom of sotc
    // I think the issue is related to the rounding of texture coodinate. The linear (from fixed unit)
    // interpolation could be slightly below the correct one.
    return trunc(t * 255.0f + 0.05f);
}

vec4 tfx(vec4 T, vec4 C)
{
    vec4 C_out;
    vec4 FxT = trunc(trunc(C) * T / 128.0f);

#if (PS_TFX == 0)
    C_out = FxT;
#elif (PS_TFX == 1)
    C_out = T;
#elif (PS_TFX == 2)
    C_out.rgb = FxT.rgb + C.a;
    C_out.a = T.a + C.a;
#elif (PS_TFX == 3)
    C_out.rgb = FxT.rgb + C.a;
    C_out.a = T.a;
#else
    C_out = C;
#endif

#if (PS_TCC == 0)
    C_out.a = C.a;
#endif

#if (PS_TFX == 0) || (PS_TFX == 2) || (PS_TFX == 3)
    // Clamp only when it is useful
    C_out = min(C_out, 255.0f);
#endif

    return C_out;
}

void atst(vec4 C)
{
    float a = C.a;

#if (PS_ATST == 0)
    // nothing to do
#elif (PS_ATST == 1)
    if (a > AREF) discard;
#elif (PS_ATST == 2)
    if (a < AREF) discard;
#elif (PS_ATST == 3)
    if (abs(a - AREF) > 0.5f) discard;
#elif (PS_ATST == 4)
    if (abs(a - AREF) < 0.5f) discard;
#endif
}

void fog(inout vec4 C, float f)
{
#if PS_FOG != 0
    C.rgb = trunc(mix(FogColor, C.rgb, f));
#endif
}

vec4 ps_color()
{
    //FIXME: maybe we can set gl_Position.w = q in VS
#if (PS_FST == 0) && (PS_INVALID_TEX0 == 1)
    // Re-normalize coordinate from invalid GS to corrected texture size
    vec2 st = (PSin.t_float.xy * WH.xy) / (vec2(PSin.t_float.w) * WH.zw);
    // no st_int yet
#elif (PS_FST == 0)
    vec2 st = PSin.t_float.xy / vec2(PSin.t_float.w);
    vec2 st_int = PSin.t_int.zw / vec2(PSin.t_float.w);
#else
    // Note xy are normalized coordinate
    vec2 st = PSin.t_int.xy;
    vec2 st_int = PSin.t_int.zw;
#endif

#if PS_CHANNEL_FETCH == 1
    vec4 T = fetch_red();
#elif PS_CHANNEL_FETCH == 2
    vec4 T = fetch_green();
#elif PS_CHANNEL_FETCH == 3
    vec4 T = fetch_blue();
#elif PS_CHANNEL_FETCH == 4
    vec4 T = fetch_alpha();
#elif PS_CHANNEL_FETCH == 5
    vec4 T = fetch_rgb();
#elif PS_CHANNEL_FETCH == 6
    vec4 T = fetch_gXbY();
#elif PS_DEPTH_FMT > 0
    // Integral coordinate
    vec4 T = sample_depth(st_int);
#else
    vec4 T = sample_color(st);
#endif

    vec4 C = tfx(T, PSin.c);

    atst(C);

    fog(C, PSin.t_float.z);

    return C;
}

void ps_fbmask(inout vec4 C)
{
    // FIXME do I need special case for 16 bits
#if PS_FBMASK
#if HAS_FRAMEBUFFER_FETCH
    vec4 RT = trunc(LAST_FRAG_COLOR * 255.0f + 0.1f);
#else
    vec4 RT = trunc(texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0) * 255.0f + 0.1f);
#endif
    C = vec4((uvec4(C) & ~FbMask) | (uvec4(RT) & FbMask));
#endif
}

void ps_dither(inout vec3 C)
{
#if PS_DITHER
    #if PS_DITHER == 2
    ivec2 fpos = ivec2(gl_FragCoord.xy);
    #else
    ivec2 fpos = ivec2(gl_FragCoord.xy / float(PS_SCALE_FACTOR));
    #endif
    C += DitherMatrix[fpos.y&3][fpos.x&3];
#endif
}

void ps_color_clamp_wrap(inout vec3 C)
{
    // When dithering the bottom 3 bits become meaningless and cause lines in the picture
    // so we need to limit the color depth on dithered items
#if SW_BLEND || PS_DITHER || PS_FBMASK

    // Correct the Color value based on the output format
#if PS_COLCLIP == 0 && PS_HDR == 0
    // Standard Clamp
    C = clamp(C, vec3(0.0f), vec3(255.0f));
#endif

    // FIXME rouding of negative float?
    // compiler uses trunc but it might need floor

    // Warning: normally blending equation is mult(A, B) = A * B >> 7. GPU have the full accuracy
    // GS: Color = 1, Alpha = 255 => output 1
    // GPU: Color = 1/255, Alpha = 255/255 * 255/128 => output 1.9921875
#if PS_DFMT == FMT_16 && (PS_HDR == 1 || PS_BLEND_MIX == 0)
    // In 16 bits format, only 5 bits of colors are used. It impacts shadows computation of Castlevania
    C = vec3(ivec3(C) & ivec3(0xF8));
#elif PS_COLCLIP == 1 && PS_HDR == 0
    C = vec3(ivec3(C) & ivec3(0xFF));
#endif

#endif
}

void ps_blend(inout vec4 Color, float As)
{
#if SW_BLEND

    // PABE
#if PS_PABE
    // No blending so early exit
    if (As < 1.0f)
        return;
#endif

    vec3 Cs = Color.rgb;

#if SW_BLEND_NEEDS_RT
#if HAS_FRAMEBUFFER_FETCH
    vec4 RT = trunc(LAST_FRAG_COLOR * 255.0f + 0.1f);
#else
    vec4 RT = trunc(texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0) * 255.0f + 0.1f);
#endif

#if PS_DFMT == FMT_24
    float Ad = 1.0f;
#else
    // FIXME FMT_16 case
    // FIXME Ad or Ad * 2?
    float Ad = RT.a / 128.0f;
#endif

    // Let the compiler do its jobs !
    vec3 Cd = RT.rgb;
#endif

#if PS_BLEND_A == 0
    vec3 A = Cs;
#elif PS_BLEND_A == 1
    vec3 A = Cd;
#else
    vec3 A = vec3(0.0f);
#endif

#if PS_BLEND_B == 0
    vec3 B = Cs;
#elif PS_BLEND_B == 1
    vec3 B = Cd;
#else
    vec3 B = vec3(0.0f);
#endif

#if PS_BLEND_C == 0
    float C = As;
#elif PS_BLEND_C == 1
    float C = Ad;
#else
    float C = Af;
#endif

#if PS_BLEND_D == 0
    vec3 D = Cs;
#elif PS_BLEND_D == 1
    vec3 D = Cd;
#else
    vec3 D = vec3(0.0f);
#endif

    // As/Af clamp alpha for Blend mix
#if PS_BLEND_MIX
    C = min(C, float(1.0f));
#endif

#if PS_BLEND_A == PS_BLEND_B
    Color.rgb = D;
#else
    Color.rgb = trunc((A - B) * C + D);
#endif

#else
    // Needed for Cd * (As/Ad/F + 1) blending modes
#if PS_CLR_HW == 1 || PS_CLR_HW == 5
    Color.rgb = vec3(255.0f);
#elif PS_CLR_HW == 2 || PS_CLR_HW == 4
    // Cd*As,Cd*Ad or Cd*F

#if PS_BLEND_C == 2
    float Alpha = Af;
#else
    float Alpha = As;
#endif

    Color.rgb = max(vec3(0.0f), (Alpha - vec3(1.0f)));
    Color.rgb *= vec3(255.0f);
#elif PS_CLR_HW == 3
    // Needed for Cs*Ad, Cs*Ad + Cd, Cd - Cs*Ad
    // Multiply Color.rgb by (255/128) to compensate for wrong Ad/255 value

    Color.rgb *= (255.0f / 128.0f);
#endif

#endif
}

void ps_main()
{
#if PS_SCANMSK & 2
    // fail depth test on prohibited lines
 	if ((int(gl_FragCoord.y) & 1) == (PS_SCANMSK & 1))
 	 	discard;
#endif

#if PS_DATE != 0
#if ((PS_DATE & 3) == 1 || (PS_DATE & 3) == 2)

#if PS_WRITE_RG == 1
    // Pseudo 16 bits access.
#if HAS_FRAMEBUFFER_FETCH
    float rt_a = LAST_FRAG_COLOR.g;
#else
    float rt_a = texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0).g;
#endif
#else
#if HAS_FRAMEBUFFER_FETCH
    float rt_a = LAST_FRAG_COLOR.a;
#else
    float rt_a = texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0).a;
#endif
#endif

#if (PS_DATE & 3) == 1
    // DATM == 0: Pixel with alpha equal to 1 will failed
    bool bad = (127.5f / 255.0f) < rt_a;
#elif (PS_DATE & 3) == 2
    // DATM == 1: Pixel with alpha equal to 0 will failed
    bool bad = rt_a < (127.5f / 255.0f);
#endif

    if (bad) {
#if PS_DATE >= 5 || defined(DISABLE_GL42_image)
        discard;
#else
        imageStore(img_prim_min, ivec2(gl_FragCoord.xy), ivec4(-1));
        return;
#endif
    }

#endif

#if PS_DATE == 3 && !defined(DISABLE_GL42_image)
    int stencil_ceil = imageLoad(img_prim_min, ivec2(gl_FragCoord.xy)).r;
    // Note gl_PrimitiveID == stencil_ceil will be the primitive that will update
    // the bad alpha value so we must keep it.

    if (gl_PrimitiveID > stencil_ceil) {
        discard;
    }
#endif
#endif

    vec4 C = ps_color();
#if (APITRACE_DEBUG & 1) == 1
    C.r = 255f;
#endif
#if (APITRACE_DEBUG & 2) == 2
    C.g = 255f;
#endif
#if (APITRACE_DEBUG & 4) == 4
    C.b = 255f;
#endif
#if (APITRACE_DEBUG & 8) == 8
    C.a = 128f;
#endif

#if PS_SHUFFLE
    uvec4 denorm_c = uvec4(C);
    uvec2 denorm_TA = uvec2(vec2(TA.xy) * 255.0f + 0.5f);

    // Write RB part. Mask will take care of the correct destination
#if PS_READ_BA
    C.rb = C.bb;
#else
    C.rb = C.rr;
#endif

    // FIXME precompute my_TA & 0x80

    // Write GA part. Mask will take care of the correct destination
    // Note: GLSL 4.50/GL_EXT_shader_integer_mix support a mix instruction to select a component\n"
    // However Nvidia emulate it with an if (at least on kepler arch) ...\n"
#if PS_READ_BA
    // bit field operation requires GL4 HW. Could be nice to merge it with step/mix below
    // uint my_ta = (bool(bitfieldExtract(denorm_c.a, 7, 1))) ? denorm_TA.y : denorm_TA.x;
    // denorm_c.a = bitfieldInsert(denorm_c.a, bitfieldExtract(my_ta, 7, 1), 7, 1);
    // c.ga = vec2(float(denorm_c.a));

    if (bool(denorm_c.a & 0x80u))
        C.ga = vec2(float((denorm_c.a & 0x7Fu) | (denorm_TA.y & 0x80u)));
    else
        C.ga = vec2(float((denorm_c.a & 0x7Fu) | (denorm_TA.x & 0x80u)));

#else
    if (bool(denorm_c.g & 0x80u))
        C.ga = vec2(float((denorm_c.g & 0x7Fu) | (denorm_TA.y & 0x80u)));
    else
        C.ga = vec2(float((denorm_c.g & 0x7Fu) | (denorm_TA.x & 0x80u)));

    // Nice idea but step/mix requires 4 instructions
    // set / trunc / I2F / Mad
    //
    // float sel = step(128.0f, c.g);
    // vec2 c_shuffle = vec2((denorm_c.gg & 0x7Fu) | (denorm_TA & 0x80u));
    // c.ga = mix(c_shuffle.xx, c_shuffle.yy, sel);
#endif

#endif

    // Must be done before alpha correction
#if (PS_BLEND_C == 1 && PS_CLR_HW > 3)
#if HAS_FRAMEBUFFER_FETCH
    vec4 RT = trunc(LAST_FRAG_COLOR * 255.0f + 0.1f);
#else
    vec4 RT = trunc(texelFetch(RtSampler, ivec2(gl_FragCoord.xy), 0) * 255.0f + 0.1f);
#endif
    float alpha_blend = (PS_DFMT == FMT_24) ? 1.0f : RT.a / 128.0f;
#else
    float alpha_blend = C.a / 128.0f;
#endif

    // Correct the ALPHA value based on the output format
#if (PS_DFMT == FMT_16)
    float A_one = 128.0f; // alpha output will be 0x80
    C.a = (PS_FBA != 0) ? A_one : step(128.0f, C.a) * A_one;
#elif (PS_DFMT == FMT_32) && (PS_FBA != 0)
    if(C.a < 128.0f) C.a += 128.0f;
#endif

    // Get first primitive that will write a failling alpha value
#if PS_DATE == 1 && !defined(DISABLE_GL42_image)
    // DATM == 0
    // Pixel with alpha equal to 1 will failed (128-255)
    if (C.a > 127.5f) {
        imageAtomicMin(img_prim_min, ivec2(gl_FragCoord.xy), gl_PrimitiveID);
    }
    return;
#elif PS_DATE == 2 && !defined(DISABLE_GL42_image)
    // DATM == 1
    // Pixel with alpha equal to 0 will failed (0-127)
    if (C.a < 127.5f) {
        imageAtomicMin(img_prim_min, ivec2(gl_FragCoord.xy), gl_PrimitiveID);
    }
    return;
#endif

    ps_blend(C, alpha_blend);

    ps_dither(C.rgb);

    // Color clamp/wrap needs to be done after sw blending and dithering
    ps_color_clamp_wrap(C.rgb);

    ps_fbmask(C);

#if !PS_NO_COLOR
    SV_Target0 = C / 255.0f;
#if !defined(DISABLE_DUAL_SOURCE) && !PS_NO_COLOR1
    SV_Target1 = vec4(alpha_blend);
#endif

#if PS_NO_ABLEND
    // write alpha blend factor into col0
    SV_Target0.a = alpha_blend;
#endif
#if PS_ONLY_ALPHA
    // rgb isn't used
    SV_Target0.rgb = vec3(0.0f);
#endif
#endif

#if PS_ZCLAMP
	gl_FragDepth = min(gl_FragCoord.z, MaxDepthPS);
#endif 
}

#endif
