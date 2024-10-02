// SPDX-FileCopyrightText: 2002-2024 PCSX2 Dev Team
// SPDX-License-Identifier: GPL-3.0+

#include "SymbolImporter.h"

#include "CDVD/CDVD.h"
#include "DebugInterface.h"
#include "Elfheader.h"
#include "MIPSAnalyst.h"
#include "VMManager.h"

#include "common/Console.h"
#include "common/Error.h"
#include "common/FileSystem.h"
#include "common/StringUtil.h"
#include "common/Threading.h"

#include <ccc/ast.h>
#include <ccc/elf.h>
#include <ccc/importer_flags.h>
#include <ccc/symbol_file.h>

#include <demangle.h>

SymbolImporter R3000SymbolImporter(R3000SymbolGuardian);
SymbolImporter R5900SymbolImporter(R5900SymbolGuardian);

struct DefaultBuiltInType
{
	const char* name;
	ccc::ast::BuiltInClass bclass;
};

static const std::vector<DefaultBuiltInType> DEFAULT_BUILT_IN_TYPES = {
	{"char", ccc::ast::BuiltInClass::UNQUALIFIED_8},
	{"signed char", ccc::ast::BuiltInClass::SIGNED_8},
	{"unsigned char", ccc::ast::BuiltInClass::UNSIGNED_8},
	{"short", ccc::ast::BuiltInClass::SIGNED_16},
	{"unsigned short", ccc::ast::BuiltInClass::UNSIGNED_16},
	{"int", ccc::ast::BuiltInClass::SIGNED_32},
	{"unsigned int", ccc::ast::BuiltInClass::UNSIGNED_32},
	{"unsigned", ccc::ast::BuiltInClass::UNSIGNED_32},
	{"long", ccc::ast::BuiltInClass::SIGNED_64},
	{"unsigned long", ccc::ast::BuiltInClass::UNSIGNED_64},
	{"long long", ccc::ast::BuiltInClass::SIGNED_64},
	{"unsigned long long", ccc::ast::BuiltInClass::UNSIGNED_64},
	{"bool", ccc::ast::BuiltInClass::BOOL_8},
	{"float", ccc::ast::BuiltInClass::FLOAT_32},
	{"double", ccc::ast::BuiltInClass::FLOAT_64},
	{"void", ccc::ast::BuiltInClass::VOID_TYPE},
	{"s8", ccc::ast::BuiltInClass::SIGNED_8},
	{"u8", ccc::ast::BuiltInClass::UNSIGNED_8},
	{"s16", ccc::ast::BuiltInClass::SIGNED_16},
	{"u16", ccc::ast::BuiltInClass::UNSIGNED_16},
	{"s32", ccc::ast::BuiltInClass::SIGNED_32},
	{"u32", ccc::ast::BuiltInClass::UNSIGNED_32},
	{"s64", ccc::ast::BuiltInClass::SIGNED_64},
	{"u64", ccc::ast::BuiltInClass::UNSIGNED_64},
	{"s128", ccc::ast::BuiltInClass::SIGNED_128},
	{"u128", ccc::ast::BuiltInClass::UNSIGNED_128},
	{"f32", ccc::ast::BuiltInClass::FLOAT_32},
	{"f64", ccc::ast::BuiltInClass::FLOAT_64},
};

static void error_callback(const ccc::Error& error, ccc::ErrorLevel level)
{
	switch (level)
	{
		case ccc::ERROR_LEVEL_ERROR:
			Console.Error("Error while importing symbol table: %s", error.message.c_str());
			break;
		case ccc::ERROR_LEVEL_WARNING:
			Console.Warning("Warning while importing symbol table: %s", error.message.c_str());
			break;
	}
}

SymbolImporter::SymbolImporter(SymbolGuardian& guardian)
	: m_guardian(guardian)
{
	ccc::set_custom_error_callback(error_callback);
}

void SymbolImporter::OnElfChanged(std::vector<u8> elf, const std::string& elf_file_name)
{
	Reset();
	AnalyseElf(std::move(elf), elf_file_name);
}

void SymbolImporter::Reset()
{
	ShutdownWorkerThread();

	m_guardian.ReadWrite([&](ccc::SymbolDatabase& database) {
		database.clear();

		ccc::Result<ccc::SymbolSourceHandle> source = database.get_symbol_source("Built-in");
		if (!source.success())
			return;

		// Create some built-in data type symbols so that users still have some
		// types to use even if there isn't a symbol table loaded. Maybe in the
		// future we could add PS2-specific types like DMA tags here too.
		for (const DefaultBuiltInType& default_type : DEFAULT_BUILT_IN_TYPES)
		{
			ccc::Result<ccc::DataType*> symbol = database.data_types.create_symbol(default_type.name, *source, nullptr);
			if (!symbol.success())
				return;

			std::unique_ptr<ccc::ast::BuiltIn> type = std::make_unique<ccc::ast::BuiltIn>();
			type->name = default_type.name;
			type->size_bytes = ccc::ast::builtin_class_size(default_type.bclass);
			type->bclass = default_type.bclass;
			(*symbol)->set_type(std::move(type));
		}
	});
}

void SymbolImporter::AnalyseElf(std::vector<u8> elf, const std::string& elf_file_name)
{
	// Search for a .sym file to load symbols from.
	std::string nocash_path;
	CDVD_SourceType source_type = CDVDsys_GetSourceType();
	if (source_type == CDVD_SourceType::Iso)
	{
		std::string iso_file_path = CDVDsys_GetFile(source_type);

		std::string::size_type n = iso_file_path.rfind('.');
		if (n == std::string::npos)
			nocash_path = iso_file_path + ".sym";
		else
			nocash_path = iso_file_path.substr(0, n) + ".sym";
	}

	ccc::Result<ccc::ElfFile> parsed_elf = ccc::ElfFile::parse(std::move(elf));
	if (!parsed_elf.success())
	{
		ccc::report_error(parsed_elf.error());
		return;
	}

	ccc::ElfSymbolFile symbol_file(std::move(*parsed_elf), std::move(elf_file_name));

	ShutdownWorkerThread();

	m_import_thread = std::thread([this, nocash_path, worker_symbol_file = std::move(symbol_file)]() {
		Threading::SetNameOfCurrentThread("Symbol Worker");

		ccc::SymbolDatabase temp_database;
		ImportSymbolTables(temp_database, worker_symbol_file, &m_interrupt_import_thread);

		if (m_interrupt_import_thread)
			return;

		ImportNocashSymbols(temp_database, nocash_path);

		if (m_interrupt_import_thread)
			return;

		const ccc::ElfProgramHeader* entry_segment = worker_symbol_file.elf().entry_point_segment();
		if (entry_segment)
		{
			ElfMemoryReader reader(worker_symbol_file.elf());
			MIPSAnalyst::ScanForFunctions(temp_database, reader, entry_segment->vaddr, entry_segment->vaddr + entry_segment->filesz);
		}

		if (m_interrupt_import_thread)
			return;

		ComputeOriginalFunctionHashes(temp_database, worker_symbol_file.elf());

		if (m_interrupt_import_thread)
			return;

		m_guardian.ReadWrite([&](ccc::SymbolDatabase& database) {
			database.merge_from(temp_database);
		});
	});
}

void SymbolImporter::ShutdownWorkerThread()
{
	m_interrupt_import_thread = true;
	if (m_import_thread.joinable())
		m_import_thread.join();
	m_interrupt_import_thread = false;
}


ccc::ModuleHandle SymbolImporter::ImportSymbolTables(
	ccc::SymbolDatabase& database, const ccc::SymbolFile& symbol_file, const std::atomic_bool* interrupt)
{
	ccc::Result<std::vector<std::unique_ptr<ccc::SymbolTable>>> symbol_tables = symbol_file.get_all_symbol_tables();
	if (!symbol_tables.success())
	{
		ccc::report_error(symbol_tables.error());
		return ccc::ModuleHandle();
	}

	ccc::DemanglerFunctions demangler;
	demangler.cplus_demangle = cplus_demangle;
	demangler.cplus_demangle_opname = cplus_demangle_opname;

	u32 importer_flags =
		ccc::DEMANGLE_PARAMETERS |
		ccc::DEMANGLE_RETURN_TYPE |
		ccc::NO_MEMBER_FUNCTIONS |
		ccc::NO_OPTIMIZED_OUT_FUNCTIONS |
		ccc::UNIQUE_FUNCTIONS;

	ccc::Result<ccc::ModuleHandle> module_handle = ccc::import_symbol_tables(
		database, symbol_file.name(), *symbol_tables, importer_flags, demangler, interrupt);
	if (!module_handle.success())
	{
		ccc::report_error(module_handle.error());
		return ccc::ModuleHandle();
	}

	Console.WriteLn("Imported %d symbols.", database.symbol_count());

	return *module_handle;
}

bool SymbolImporter::ImportNocashSymbols(ccc::SymbolDatabase& database, const std::string& file_name)
{
	ccc::Result<ccc::SymbolSourceHandle> source = database.get_symbol_source("Nocash Symbols");
	if (!source.success())
		return false;

	FILE* f = FileSystem::OpenCFile(file_name.c_str(), "r");
	if (!f)
		return false;

	while (!feof(f))
	{
		char line[256], value[256] = {0};
		char* p = fgets(line, 256, f);
		if (p == NULL)
			break;

		if (char* end = strchr(line, '\n'))
			*end = '\0';

		u32 address;
		if (sscanf(line, "%08x %255s", &address, value) != 2)
			continue;
		if (address == 0 && strcmp(value, "0") == 0)
			continue;

		if (value[0] == '.')
		{
			// data directives
			char* s = strchr(value, ':');
			if (s != NULL)
			{
				*s = 0;

				u32 size = 0;
				if (sscanf(s + 1, "%04x", &size) != 1)
					continue;

				std::unique_ptr<ccc::ast::BuiltIn> scalar_type = std::make_unique<ccc::ast::BuiltIn>();
				if (StringUtil::Strcasecmp(value, ".byt") == 0)
				{
					scalar_type->size_bytes = 1;
					scalar_type->bclass = ccc::ast::BuiltInClass::UNSIGNED_8;
				}
				else if (StringUtil::Strcasecmp(value, ".wrd") == 0)
				{
					scalar_type->size_bytes = 2;
					scalar_type->bclass = ccc::ast::BuiltInClass::UNSIGNED_16;
				}
				else if (StringUtil::Strcasecmp(value, ".dbl") == 0)
				{
					scalar_type->size_bytes = 4;
					scalar_type->bclass = ccc::ast::BuiltInClass::UNSIGNED_32;
				}
				else if (StringUtil::Strcasecmp(value, ".asc") == 0)
				{
					scalar_type->size_bytes = 1;
					scalar_type->bclass = ccc::ast::BuiltInClass::UNQUALIFIED_8;
				}
				else
				{
					continue;
				}

				ccc::Result<ccc::GlobalVariable*> global_variable = database.global_variables.create_symbol(
					line, address, *source, nullptr);
				if (!global_variable.success())
				{
					fclose(f);
					return false;
				}

				if (scalar_type->size_bytes == (s32)size)
				{
					(*global_variable)->set_type(std::move(scalar_type));
				}
				else
				{
					std::unique_ptr<ccc::ast::Array> array = std::make_unique<ccc::ast::Array>();
					array->size_bytes = (s32)size;
					array->element_type = std::move(scalar_type);
					array->element_count = size / array->element_type->size_bytes;
					(*global_variable)->set_type(std::move(array));
				}
			}
		}
		else
		{ // labels
			u32 size = 1;
			char* seperator = strchr(value, ',');
			if (seperator != NULL)
			{
				*seperator = 0;
				sscanf(seperator + 1, "%08x", &size);
			}

			if (size != 1)
			{
				ccc::Result<ccc::Function*> function = database.functions.create_symbol(value, address, *source, nullptr);
				if (!function.success())
				{
					fclose(f);
					return false;
				}

				(*function)->set_size(size);
			}
			else
			{
				ccc::Result<ccc::Label*> label = database.labels.create_symbol(value, address, *source, nullptr);
				if (!label.success())
				{
					fclose(f);
					return false;
				}
			}
		}
	}

	fclose(f);
	return true;
}

void SymbolImporter::ComputeOriginalFunctionHashes(ccc::SymbolDatabase& database, const ccc::ElfFile& elf)
{
	for (ccc::Function& function : database.functions)
	{
		if (!function.address().valid())
			continue;

		if (function.size() == 0)
			continue;

		ccc::Result<std::span<const u32>> text = elf.get_array_virtual<u32>(
			function.address().value, function.size() / 4);
		if (!text.success())
		{
			ccc::report_warning(text.error());
			break;
		}

		ccc::FunctionHash hash;
		for (u32 instruction : *text)
			hash.update(instruction);

		function.set_original_hash(hash.get());
	}
}

void SymbolImporter::UpdateFunctionHashes(DebugInterface& cpu)
{
	m_guardian.ReadWrite([&](ccc::SymbolDatabase& database) {
		for (ccc::Function& function : database.functions)
		{
			if (!function.address().valid())
				continue;

			if (function.size() == 0)
				continue;

			if (function.original_hash() == 0)
				continue;

			ccc::FunctionHash hash;
			for (u32 i = 0; i < function.size() / 4; i++)
				hash.update(cpu.read32(function.address().value + i * 4));

			function.set_current_hash(hash);
		}

		for (ccc::SourceFile& source_file : database.source_files)
			source_file.check_functions_match(database);
	});
}
