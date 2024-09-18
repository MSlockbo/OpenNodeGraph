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

#ifndef EDITORWINDOW_H
#define EDITORWINDOW_H

#include <string>
#include <imgui-docking/imgui.h>

namespace OpenShaderDesigner
{

/**
 * \brief EditorWindow class for wrapping ImGui window functionality.
 */
class EditorWindow
{
public:
	/**
	 * \brief Open the EditorWindow.
	 */
	void Open();

	/**
	 * \brief Draw the EditorWindow.
	 */
	void Draw();

	/**
	 * \brief Close the EditorWindow.
	 */
	void Close();

	/**
	 * \brief Check if the EditorWindow is open.
	 * \return
	 */
	[[nodiscard]] bool IsOpen() const { return bOpen_; }

	const std::string Title; //!< Title for the EditorWindow.

	void SetFlags(ImGuiWindowFlags flags) { Flags_ |= flags; }
	void ClearFlags(ImGuiWindowFlags flags) { Flags_ &= ~flags; }
	void ToggleFlags(ImGuiWindowFlags flags) { Flags_ ^= flags; }
	[[nodiscard]] bool CheckFlag(ImGuiWindowFlags flag) const { return Flags_ & flag; }

	[[nodiscard]] bool HasMenuBar() const { return CheckFlag(ImGuiWindowFlags_MenuBar); }

protected:
	~EditorWindow() = default;
	EditorWindow(const std::string& title
		         , ImGuiWindowFlags flags);

	/**
	 * \brief OnOpen callback for when the EditorWindow is opened.
	 */
	virtual void OnOpen()
	{
	};

	/**
	 * \brief DrawWindow function for when the EditorWindow is being drawn.
	 */
	virtual void DrawWindow()
	{
	};

	/**
	 * \brief DrawMenu function for when the EditorWindow Menu is being drawn.
	 */
	virtual void DrawMenu()
	{
	};

	/**
	 * \brief OnClose callback for when the EditorWindow is closed.
	 */
	virtual void OnClose()
	{
	};

private:
	EditorWindow(const EditorWindow&) = delete;

	EditorWindow(EditorWindow&&) = delete;

	int Flags_;
	bool bOpen_;

	friend class EditorSystem;
};

}


#endif //EDITORWINDOW_H
