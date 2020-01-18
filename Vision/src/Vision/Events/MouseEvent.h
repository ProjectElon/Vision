#pragma once

#include "Vision/Events/Event.h"

namespace Vision
{
	class MouseButtonPressed : public Event
	{
	public:
		MouseButtonPressed(unsigned int button)
			:  m_Button(button)
		{
		}

		inline unsigned int GetButton() const { return m_Button; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonPressedEvent : (" << m_Button << ")";

			return ss.str();
		}

		EVENT_TYPE(MouseButtonPressed)
		EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		unsigned int m_Button;
	};

	class MouseButtonReleased : public Event
	{
	public:
		MouseButtonReleased(unsigned int button)
			: m_Button(button)
		{
		}

		inline unsigned int GetButton() const { return m_Button; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseButtonReleasedEvent : (" << m_Button << ")";

			return ss.str();
		}

		EVENT_TYPE(MouseButtonReleased)
		EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		unsigned int m_Button;
	};

	class MouseMovedEvent : public Event
	{
	public:
		MouseMovedEvent(float mouseX, float mouseY)
			: m_MouseX(mouseX)
			, m_MouseY(mouseY)
		{
		}

		inline float GetMouseX() const { return m_MouseX; }
		inline float GetMouseY() const { return m_MouseY; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseMovedEvent : (" << m_MouseX << ", " << m_MouseY << ")";
			
			return ss.str();
		}

		EVENT_TYPE(MouseMoved);
		EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		float m_MouseX;
		float m_MouseY;
	};

	class MouseWheelScrolledEvent : public Event
	{
	public:
		MouseWheelScrolledEvent(float xOffset, float yOffset)
			: m_XOffset(xOffset),
			  m_YOffset(yOffset)
		{
		}

		inline float GetXOffset() const { return m_XOffset; }
		inline float GetYOffset() const { return m_XOffset; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "MouseWheelScrolledEvent : (" << m_XOffset << ", " << m_XOffset << ")";

			return ss.str();
		}

		EVENT_TYPE(MouseWheelScrolled)
		EVENT_CATEGORY(EventCategoryInput | EventCategoryMouse)

	private:
		float m_XOffset;
		float m_YOffset;
	};
}