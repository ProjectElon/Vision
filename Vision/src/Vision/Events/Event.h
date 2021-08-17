#pragma once

#include "pch.hpp"
#include "Vision/Core/Defines.h"

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
		EventCategoryApplication = VnBitU32Mask(0),
		EventCategoryInput		 = VnBitU32Mask(1),
		EventCategoryKeyboard    = VnBitU32Mask(2),
		EventCategoryMouse       = VnBitU32Mask(3)
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