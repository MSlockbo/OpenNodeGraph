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
