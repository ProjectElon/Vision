#pragma once

#include <Vision.h>

class GameLayer : public Vision::Layer
{
public:
	GameLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Vision::Event& event) override;
};

class Sandbox : public Vision::Application
{
public:
	Sandbox();
	~Sandbox();
};