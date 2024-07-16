//
// Created by Maddie on 6/21/2024.
//

#include <Core/Console.h>
#include <Editor/ConsoleWindow.h>

using namespace OpenShaderDesigner;

ConsoleWindow::ConsoleWindow()
	: EditorWindow("\uf1f5 Console", ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar)
{
}

void ConsoleWindow::DrawMenu()
{
	Console::DrawMenu();
}

void ConsoleWindow::DrawWindow()
{
	Console::DrawWindow();
}
