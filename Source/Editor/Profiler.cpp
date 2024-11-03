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

#include <Editor/Profiler.h>

using namespace OpenShaderDesigner;

Profiler::Profiler()
	: EditorWindow("\uf214 Profiler", 0)
	, Frame_(0), Deltas_{ 0 }
	, Timer_()
{
	EventSystem::RegisterHandler<BeginFrame>(this);
	EventSystem::RegisterHandler<EndFrame>(this);
}

Profiler::~Profiler()
{
	EventSystem::UnregisterHandler<BeginFrame>(this);
	EventSystem::UnregisterHandler<EndFrame>(this);
}

void Profiler::DrawWindow()
{
	ImGui::Text("Frame #%d", Frame_);
	ImGui::Text("FPS: %07.02f (%06.02f ms)", 1.0 / Deltas_[LAST], 1000.0 * Deltas_[LAST]);
}

bool Profiler::HandleEvent(const EventHandler<BeginFrame>::HandledType* event)
{
	++Frame_;
	Timer_.Reset();
	return false;
}

bool Profiler::HandleEvent(const EventHandler<EndFrame>::HandledType* event)
{
	Deltas_[END] = Timer_.Poll();
	return false;
}
