#include "SandboxApp.h"
#include <imgui/imgui.h>

GameLayer::GameLayer()
	: Layer("Game")
{
}

void GameLayer::OnAttach()
{
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnEnable()
{
}

void GameLayer::OnDisable()
{
}

void GameLayer::OnUpdate(float dt)
{
}

void GameLayer::OnImGuiRender()
{
	ImGui::Begin("Example Window");
	static float v[3];
	ImGui::SliderFloat3("World Position", v, -10, 10);
	ImGui::End();
}

void GameLayer::OnEvent(Vision::Event& event)
{
}

Sandbox::Sandbox()
{
	PushLayer(new GameLayer);
}

Sandbox::~Sandbox()
{
}

Vision::Application* Vision::CreateApplication()
{
	return new Sandbox();
}