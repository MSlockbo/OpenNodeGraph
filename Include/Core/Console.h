// =====================================================================================================================
// Copyright 2024 Medusa Slockbower
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =====================================================================================================================


#ifndef CONSOLE_H
#define CONSOLE_H

#include <imgui-docking/imgui.h>
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
		enum Setting
			: uint8_t
		{
			SHOW_TIMESTAMP = 0b00000001
        ,   SHOW_THREAD = 0b00000010
        ,   SHOW_SEVERITY = 0b00000100
        ,   SHOW_FILE_INFO = 0b00001000
        ,   WRAP_TEXT = 0b00010000

		,	ALL_SETTINGS     = 0xFF
		,	DEFAULT_SETTINGS = ALL_SETTINGS ^ WRAP_TEXT
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
		enum class Severity
			: int
		{
			MESSAGE = 0,
			WARNING,
			ERROR,
			FATAL,
			ALERT,
			COMMAND,
			COUNT,
			DEFAULT = WARNING
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
		static std::string Format(const LogEntry& entry, Setting settings);

		/**
		 * \brief Command handling.
		 * \param command Command string to process.
		 */
		static void ProcessCommand(const std::string& command);

		inline static std::list<LogEntry> EntryLog;
		inline static std::mutex Lock;
		inline static int Filter = static_cast<int>(0xFFFFFFFF);
		inline static Setting Settings = DEFAULT_SETTINGS;
		inline static std::string Command;
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

		std::lock_guard guard(Lock);
		LogEntry entry{
			std::vformat(fmt.get(), std::make_format_args(vargs...)), severity, file, std::format(
				"{:0>2}:{:0>2}:{:0>2}", tm.tm_hour, tm.tm_min, tm.tm_sec),
			ThreadID(), line
		};
		EntryLog.push_back(entry);
		std::cout << Format(entry, ALL_SETTINGS) << std::endl;
	}
}

#define Log(...) Log(__FILE__, __LINE__, __VA_ARGS__)

#endif //CONSOLE_H
