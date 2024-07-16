//
// Created by Maddie on 7/1/2024.
//

#ifndef PROFILER_H
#define PROFILER_H

#include <Core/EventSystem.h>
#include <Core/Window.h>
#include <Editor/EditorWindow.h>
#include <Utility/Timer.h>

namespace OpenShaderDesigner
{

	class Profiler
		: public EditorWindow
		, public EventHandler<BeginFrame>
		, public EventHandler<EndFrame>
	{
	public:
		Profiler();
		~Profiler();

		void DrawWindow() override;

		bool HandleEvent(const EventHandler<BeginFrame>::HandledType* event) override;
		bool HandleEvent(const EventHandler<EndFrame>::HandledType* event) override;

	private:
		enum
		{
			EVENTS = 0
		,	RENDER
		,	EDITOR
		,	END

		,	COUNT
		,	LAST = COUNT - 1
		};

		uint64_t Frame;
		double   Deltas[COUNT];
		Timer    Timer;
	};

}



#endif //PROFILER_H
