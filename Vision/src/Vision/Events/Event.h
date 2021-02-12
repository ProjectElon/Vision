#pragma once

#include "pch.h"
#include "Vision/Core/Common.h"

namespace Vision
{
	enum class EventType : uint8_t
	{
		WindowClose,
		WindowResize,
		WindowFocus,
		WindowLostFoucs,
		WindowMoved,
		WindowMinimized,
		WindowMaximized,
		WindowRestored,

		KeyPressed,
		KeyReleased,
		KeyTyped,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,
		MouseWheelScrolled,
		MouseCursorEntered,
		MouseCursorLeft
	};

	enum EventCategory : uint8_t
	{
		EventCategoryApplication = VnBitUInt32(0),
		EventCategoryInput		 = VnBitUInt32(1),
		EventCategoryKeyboard    = VnBitUInt32(2),
		EventCategoryMouse       = VnBitUInt32(3)
	};

	class Event
	{
	public:
		bool Handled = false;

		virtual EventType GetType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory category) const
		{
			return GetCategoryFlags() & category;
		}
	};

	class EventDispatcher
	{
	public:
		EventDispatcher(Event& event)
			: m_Event(event)
		{
		}

		template<typename T, typename F>
		bool Dispatch(const F& fn)
		{
			if (m_Event.GetType() == T::GetStaticType())
			{
				m_Event.Handled = fn(static_cast<T&>(m_Event));
				return true;
			}

			return false;
		}

	private:
		Event& m_Event;
	};

	inline std::ostream& operator<<(std::ostream& os, const Event& e)
	{
		return os << e.ToString();
	}

#define EVENT_CATEGORY(category) int GetCategoryFlags() const override { return (int)EventCategory::category; }

#define EVENT_TYPE(type) static EventType GetStaticType()     { return EventType::type; }\
						 EventType GetType() const override   { return GetStaticType(); }\
						 const char* GetName() const override { return #type; }
}