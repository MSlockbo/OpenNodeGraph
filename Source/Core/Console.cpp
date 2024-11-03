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

#include <Core/Console.h>
#include <imgui-docking/misc/cpp/imgui_stdlib.h>

using namespace OpenShaderDesigner;

void Console::DrawMenu()
{
    std::lock_guard guard(Lock_);

    if(ImGui::BeginMenu("\uf0e5 Settings"))
    {
        for(uint8_t setting = 0; setting < sizeof(SettingNames) / sizeof(std::string); ++setting)
        {
            int flag = 0x1 << setting;
            if(ImGui::MenuItem(SettingNames[setting].c_str(), nullptr, Settings_ & flag))
            {
                Settings_ = Settings_ ^ flag;
            }
        }

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("\ued22 Filters"))
    {
        for(int severity = 0; severity < static_cast<int>(Severity::COUNT); ++severity)
        {
            int flag = 0x1 << severity;
            if(ImGui::MenuItem(Severities[severity].c_str(), NULL, Filter_ & flag))
            {
                Filter_ = Filter_ ^ flag;
            }
        }

        ImGui::EndMenu();
    }
}

void Console::DrawWindow()
{
    std::lock_guard guard(Lock_);

    const auto& Log = EntryLog_;

    const float padding = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if(ImGui::BeginChild("##scrolling", ImVec2(0, -padding),
                         ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY
                       , ImGuiWindowFlags_AlwaysVerticalScrollbar | (Settings_ & Settings_WrapText ? ImGuiWindowFlags_None : ImGuiWindowFlags_AlwaysHorizontalScrollbar)))
    {
        for(const LogEntry& entry : Log)
        {
            int flag = 0x1 << static_cast<int>(entry.Severity);
            if((flag & Filter_) == 0) continue;

            ImGui::PushStyleColor(ImGuiCol_Text, SeverityColors[static_cast<int>(entry.Severity)]);
            if(Settings_ & Settings_WrapText) ImGui::TextWrapped("%s", Format(entry, Settings_).c_str());
            else ImGui::Text("%s", Format(entry, Settings_).c_str());
            ImGui::PopStyleColor();
        }

    }
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::Text(">"); ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x);
    if(ImGui::InputText("##commandline", &CommandBuffer_, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        ProcessCommand(CommandBuffer_);
        CommandBuffer_.clear();
        ImGui::SetKeyboardFocusHere(-1);
    }
}

std::string Console::Format(const LogEntry& entry, uint8_t settings)
{
    std::string severity = std::format("<{}>", Severities[static_cast<int>(entry.Severity)]);

    std::string message;

    // Get Timestamp and Thread bits.
    const bool showTimestamp = (settings & Settings_ShowTimeStamp);
    const bool showThreadID = (settings & Settings_ShowThread) && entry.Severity != Severity::Command;
    const bool check1 = showTimestamp || showThreadID;

    // Print Timestamp and/or Thread if enabled.
    if(showTimestamp) message += std::format("[{}] ", entry.Timestamp);
    if(showThreadID) message += std::format("Thread #{:0>3} | ", entry.Thread);

    // Get Severity and File bits.
    const bool showSeverity = (settings & Settings_Severity) && entry.Severity != Severity::Command;
    const bool showFile = (settings & Settings_ShowFileInfo) && entry.Severity != Severity::Command;
    const bool check2 = showSeverity || showFile;

    // Print Severity and/or File if enabled.
    if(showSeverity) message += std::format("{:>9} ", severity);
    if(showFile) message += std::format("{}({})", entry.File, entry.Line);

    // Print separator if any are enabled
    if(check1 || check2) message += entry.Severity == Severity::Command ? "> " : ": ";

    // Add message.
    message += entry.Message;

    return message;
}

void Console::ProcessCommand(const std::string&)
{
    // TODO
}
