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
