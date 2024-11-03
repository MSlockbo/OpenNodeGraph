// =====================================================================================================================
//  OpenShaderDesigner, an open source software utility to create materials and shaders.
//  Copyright (C) 2024  Medusa Slockbower
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
// =====================================================================================================================

#ifndef CONSOLE_H
#define CONSOLE_H

#include <imgui-docking/imgui.h>
#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <thread>
#include <list>
#include <mutex>

namespace OpenShaderDesigner
{
    
class Console
{
public:
	/**
	 * \brief Setting for displaying log entries.
	 */
	enum Settings
		: uint8_t
	{
		Settings_ShowTimeStamp = 0b00000001
    ,   Settings_ShowThread    = 0b00000010
    ,   Settings_Severity      = 0b00000100
    ,   Settings_ShowFileInfo  = 0b00001000
    ,   Settings_WrapText      = 0b00010000

	,	Settings_ALL     = 0xFF
	,	Settings_Default = Settings_ALL ^ Settings_WrapText
	};

	/**
	 * \brief String representations of the settings.
	 */
	inline static const std::string SettingNames[] =
	{
		"Timestamps", "Thread IDs", "Severity", "File Info", "Wrapping"
	};

	/**
	 * \brief Severity levels for log entries.
	 */
	enum Severity
		: int
	{
		Message = 0,
		Warning,
		Error,
		Fatal,
		Alert,
		Command,
		COUNT,
		DEFAULT = Warning
	};

	/**
	 * \brief String representations of the Severity levels.
	 */
	static inline const std::string Severities[] =
	{
		"Message", "Warning", "Error", "Fatal", "Alert", "Command"
	};

	/**
	 * \brief Integer to floating point color. (ImGui APIVersion)
	 * \param RGB The Integer color to convert.
	 * \return The rgba floating point color.
	 */
	inline static constexpr ImVec4 ImGuiColor(unsigned int RGB)
	{
		return {
			static_cast<float>((RGB >> 24) & 255) / 255.0f, static_cast<float>((RGB >> 16) & 255) / 255.0f,
			static_cast<float>((RGB >> 8) & 255) / 255.0f, static_cast<float>((RGB >> 0) & 255) / 255.0f
		};
	}

	/**
	 * \brief Color for rendering each Severity level text in editor.
	 */
	inline static const ImVec4 SeverityColors[] = {
		ImGuiColor(0xA4B9C4FF), ImGuiColor(0xF2C554FF), ImGuiColor(0xE57327FF), ImGuiColor(0xCC211EFF),
		ImGuiColor(0x9CDCFEFF),
	};

	static std::string ThreadID()
	{
		std::stringstream ss;
		ss << std::this_thread::get_id();
		return ss.str();
	}

	/**
	 * \brief Thread-Safe Log function for debugging.
	 * \tparam Args Variadic Arguments template for PixelLayout Parameters
	 * \param file The name of the file this was called from.
	 * \param line The line number this was called from.
	 * \param severity The severity level of the log entry.
	 * \param message A format string for the entry message.
	 * \param vargs Arguments for the format string.
	 */
	template <typename... Args>
	static void Log(const std::string& file
		            , const int line
		            , Severity severity = Severity::DEFAULT
		            , const std::format_string<Args...>& message = ""
		            , Args&&... vargs);

	static void DrawMenu();
	static void DrawWindow();

	static inline bool Open = true;

private:
	struct LogEntry
	{
		const std::string Message;
		const Severity Severity;
		const std::string File, Timestamp, Thread;
		const int Line;
	};

	/**
	 * \brief Get a formatted Entry for printing.
	 * \param entry The Entry to format.
	 * \return The Entry formatted into a string.
	 */
	static std::string Format(const LogEntry& entry, uint8_t settings);

	/**
	 * \brief Command handling.
	 * \param command Command string to process.
	 */
	static void ProcessCommand(const std::string& command);

	inline static std::list<LogEntry> EntryLog_;
	inline static std::mutex          Lock_;
	inline static int                 Filter_ = static_cast<int>(0xFFFFFFFF);
	inline static uint8_t             Settings_ = Settings_Default;
	inline static std::string         CommandBuffer_;
};

template <typename... Args>
void Console::Log(
	const std::string& file
	, const int line
	, Severity severity
	, const std::format_string<Args...>& fmt
	, Args&&... vargs)
{
	auto t = std::time(nullptr);
#ifdef _MSC_VER
#pragma warning(disable:4996)
#endif
	auto tm = *std::localtime(&t);
    const auto rel = std::filesystem::relative(file, PROJECT_DIR).string();

	std::lock_guard guard(Lock_);
	LogEntry entry{
		std::vformat(fmt.get(), std::make_format_args(vargs...)), severity, rel, std::format(
			"{:0>2}:{:0>2}:{:0>2}", tm.tm_hour, tm.tm_min, tm.tm_sec),
		ThreadID(), line
	};
	EntryLog_.push_back(entry);
	std::cout << Format(entry, Settings_ALL) << std::endl;
}
    
}

#define Log(...) Log(__FILE__, __LINE__, __VA_ARGS__)

#endif //CONSOLE_H
