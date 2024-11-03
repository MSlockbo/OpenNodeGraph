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

	uint64_t Frame_;
	double   Deltas_[COUNT];
	Timer    Timer_;
};

}



#endif //PROFILER_H
