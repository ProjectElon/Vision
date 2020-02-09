#pragma once

#include "Vision/Events/Event.h"

namespace Vision
{
	class WindowCloseEvent : public Event
	{
	public:
		WindowCloseEvent() {}

		EVENT_TYPE(WindowClose)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowResizeEvent : public Event
	{
	public:
		WindowResizeEvent(unsigned int width, unsigned int height)
			: m_Width(width)
			, m_Height(height)
		{
		}

		inline unsigned int GetWidth()  const { return m_Width;  }
		inline unsigned int GetHeight() const { return m_Height; }

		const std::string& ToString() const override
		{
			std::stringstream ss;
			ss << "WidnowResizeEvent : (" << m_Width << ", " << m_Height << ")";

			return ss.str();
		}

		EVENT_TYPE(WindowResize)
		EVENT_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_Width;
		unsigned int m_Height;
	};

	class WindowFocusEvent : public Event
	{
	public:
		WindowFocusEvent() {}

		EVENT_TYPE(WindowFocus)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowLostFocusEvent : public Event
	{
	public:
		WindowLostFocusEvent() {}

		EVENT_TYPE(WindowLostFoucs)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowMovedEvent : public Event
	{
	public:
		WindowMovedEvent(unsigned int windowX, unsigned int windowY)
			: m_WindowX(windowX)
			, m_WindowY(windowY)
		{
		}

		inline unsigned int GetX() const { return m_WindowX; }
		inline unsigned int GetY() const { return m_WindowY; }

		const std::string& ToString() const override
		{
			std::stringstream ss;
			ss << "WidnowMovedEvent : (" << m_WindowX << ", " << m_WindowY << ")";

			return ss.str();
		}

		EVENT_TYPE(WindowMoved)
		EVENT_CATEGORY(EventCategoryApplication)

	private:
		unsigned int m_WindowX;
		unsigned int m_WindowY;
	};

	class WindowMinimizedEvent : public Event
	{
	public:
		WindowMinimizedEvent() 
		{
		}

		EVENT_TYPE(WindowMinimized)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowMaximizedEvent : public Event
	{
	public:
		WindowMaximizedEvent() 
		{
		}

		EVENT_TYPE(WindowMaximized)
		EVENT_CATEGORY(EventCategoryApplication)
	};

	class WindowRestoredEvent : public Event
	{
	public:
		WindowRestoredEvent()
		{
		}

		EVENT_TYPE(WindowRestored)
		EVENT_CATEGORY(EventCategoryApplication)
	};
}