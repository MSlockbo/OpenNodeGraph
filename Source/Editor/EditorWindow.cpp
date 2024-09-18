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

#include <Editor/EditorWindow.h>

using namespace OpenShaderDesigner;

void EditorWindow::Open()
{
	if(bOpen_) return;
	bOpen_ = true;
	OnOpen();
}

void EditorWindow::Draw()
{
	const bool prev = bOpen_;
	if(ImGui::Begin(Title.c_str(), &bOpen_, Flags_))
	{
		if(HasMenuBar())
		{
			if(ImGui::BeginMenuBar())
			{
				DrawMenu();

				ImGui::EndMenuBar();
			}
		}

		DrawWindow();
	}
	ImGui::End();

	if(bOpen_ != prev)
	{
		bOpen_ ? OnOpen() : OnClose();
	}
}

void EditorWindow::Close()
{
	if(!bOpen_) return;
	bOpen_ = false;

	OnClose();
}

EditorWindow::EditorWindow(const std::string &title, ImGuiWindowFlags flags)
	: Title(title)
	, Flags_(flags)
	, bOpen_(false)
{

}