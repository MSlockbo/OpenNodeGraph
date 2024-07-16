//
// Created by Maddie on 6/21/2024.
//

#ifndef CONSOLEWINDOW_H
#define CONSOLEWINDOW_H

#include <Editor/EditorSystem.h>

namespace OpenShaderDesigner
{

	class ConsoleWindow : public EditorWindow
	{
	public:
		ConsoleWindow();

		void DrawMenu() override;
		void DrawWindow() override;

	private:
	};

} // OpenShaderDesigner

#endif //CONSOLEWINDOW_H
