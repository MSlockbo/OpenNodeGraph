//
// Created by Maddie on 6/18/2024.
//

#include <Core/Console.h>
#include <imgui-docking/misc/cpp/imgui_stdlib.h>

using namespace OpenShaderDesigner;

void Console::DrawMenu()
{
    std::lock_guard guard(Lock);

    if(ImGui::BeginMenu("\uf0e5 Settings"))
    {
        for(uint8_t setting = 0; setting < sizeof(SettingNames) / sizeof(std::string); ++setting)
        {
            int flag = 0x1 << setting;
            if(ImGui::MenuItem(SettingNames[setting].c_str(), nullptr, Settings & flag))
            {
                Settings = static_cast<Setting>(Settings ^ flag);
            }
        }

        ImGui::EndMenu();
    }

    if(ImGui::BeginMenu("\ued22 Filters"))
    {
        for(int severity = 0; severity < static_cast<int>(Severity::COUNT); ++severity)
        {
            int flag = 0x1 << severity;
            if(ImGui::MenuItem(Severities[severity].c_str(), NULL, Filter & flag))
            {
                Filter = Filter ^ flag;
            }
        }

        ImGui::EndMenu();
    }
}

void Console::DrawWindow()
{
    std::lock_guard guard(Lock);

    const auto& Log = EntryLog;

    const float padding = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    if(ImGui::BeginChild("##scrolling", ImVec2(0, -padding),
                         ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_AutoResizeY
                       , ImGuiWindowFlags_AlwaysVerticalScrollbar | (Settings & WRAP_TEXT ? ImGuiWindowFlags_None : ImGuiWindowFlags_AlwaysHorizontalScrollbar)))
    {
        for(const LogEntry& entry : Log)
        {
            int flag = 0x1 << static_cast<int>(entry.Severity);
            if((flag & Filter) == 0) continue;

            ImGui::PushStyleColor(ImGuiCol_Text, SeverityColors[static_cast<int>(entry.Severity)]);
            if(Settings & WRAP_TEXT) ImGui::TextWrapped("%s", Format(entry, Settings).c_str());
            else ImGui::Text("%s", Format(entry, Settings).c_str());
            ImGui::PopStyleColor();
        }

    }
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::Text(">"); ImGui::SameLine();
    ImGui::SetNextItemWidth(ImGui::GetWindowWidth() - ImGui::GetCursorPosX() - ImGui::GetStyle().WindowPadding.x);
    if(ImGui::InputText("##commandline", &Command, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        ProcessCommand(Command);
        Command.clear();
        ImGui::SetKeyboardFocusHere(-1);
    }
}

std::string Console::Format(const LogEntry& entry, Setting settings)
{
    std::string severity = std::format("<{}>", Severities[static_cast<int>(entry.Severity)]);

    std::string message;

    // Get Timestamp and Thread bits.
    const bool showTimestamp = (settings & SHOW_TIMESTAMP);
    const bool showThreadID = (settings & SHOW_THREAD) && entry.Severity != Severity::COMMAND;
    const bool check1 = showTimestamp || showThreadID;

    // Print Timestamp and/or Thread if enabled.
    if(showTimestamp) message += std::format("[{}] ", entry.Timestamp);
    if(showThreadID) message += std::format("Thread #{:0>3} | ", entry.Thread);

    // Get Severity and File bits.
    const bool showSeverity = (settings & SHOW_SEVERITY) && entry.Severity != Severity::COMMAND;
    const bool showFile = (settings & SHOW_FILE_INFO) && entry.Severity != Severity::COMMAND;
    const bool check2 = showSeverity || showFile;

    // Print Severity and/or File if enabled.
    if(showSeverity) message += std::format("{:>9} ", severity);
    if(showFile) message += std::format("{}({})", entry.File, entry.Line);

    // Print separator if any are enabled
    if(check1 || check2) message += entry.Severity == Severity::COMMAND ? "> " : ": ";

    // Add message.
    message += entry.Message;

    return message;
}

void Console::ProcessCommand(const std::string&)
{
    // TODO
}
