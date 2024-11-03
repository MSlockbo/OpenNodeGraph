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
