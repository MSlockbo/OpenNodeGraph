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
