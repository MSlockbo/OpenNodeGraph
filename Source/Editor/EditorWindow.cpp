//
// Created by Maddie on 6/20/2024.
//

#include <Editor/EditorWindow.h>

using namespace OpenShaderDesigner;

void EditorWindow::Open()
{
	if(bOpen) return;
	bOpen = true;
	OnOpen();
}

void EditorWindow::Draw()
{
	const bool prev = bOpen;
	if(ImGui::Begin(Title.c_str(), &bOpen, Flags))
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

	if(bOpen != prev)
	{
		bOpen ? OnOpen() : OnClose();
	}
}

void EditorWindow::Close()
{
	if(!bOpen) return;
	bOpen = false;

	OnClose();
}

EditorWindow::EditorWindow(const std::string &title, ImGuiWindowFlags flags)
	: Title(title)
	, Flags(flags)
	, bOpen(false)
{

}