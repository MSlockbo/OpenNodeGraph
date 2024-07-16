//
// Created by Maddie on 7/1/2024.
//

#include <Editor/Profiler.h>

using namespace OpenShaderDesigner;

Profiler::Profiler()
	: EditorWindow("\uf214 Profiler", 0)
	, Frame(0), Deltas{ 0 }
	, Timer()
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
	ImGui::Text("Frame #%d", Frame);
	ImGui::Text("FPS: %07.02f (%06.02f ms)", 1.0 / Deltas[LAST], 1000.0 * Deltas[LAST]);
}

bool Profiler::HandleEvent(const EventHandler<BeginFrame>::HandledType* event)
{
	++Frame;
	Timer.Reset();
	return false;
}

bool Profiler::HandleEvent(const EventHandler<EndFrame>::HandledType* event)
{
	Deltas[END] = Timer.Poll();
	return false;
}
