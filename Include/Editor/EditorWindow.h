//
// Created by Maddie on 6/20/2024.
//

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
		[[nodiscard]] bool IsOpen() const { return bOpen; }

		const std::string Title; //!< Title for the EditorWindow.

		void SetFlags(ImGuiWindowFlags flags) { Flags |= flags; }
		void ClearFlags(ImGuiWindowFlags flags) { Flags &= ~flags; }
		void ToggleFlags(ImGuiWindowFlags flags) { Flags ^= flags; }
		[[nodiscard]] bool CheckFlag(ImGuiWindowFlags flag) const { return Flags & flag; }

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

		int Flags;
		bool bOpen;

		friend class EditorSystem;
	};
}


#endif //EDITORWINDOW_H
