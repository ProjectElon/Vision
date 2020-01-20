#pragma once

#include "Vision/Events/Event.h"

namespace Vision
{
	class KeyPressedEvent : public Event
	{
	public:
		KeyPressedEvent(unsigned int keyCode, unsigned int repeatCount)
			: m_KeyCode(keyCode),
			  m_RepeatCount(repeatCount)
		{
		}

		inline unsigned int GetKeyCode()     const { return m_KeyCode; }
		inline unsigned int GetRepeatCount() const { return m_RepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent : (" << m_KeyCode << ", " << m_RepeatCount << ")";

			return ss.str();
		}

		EVENT_TYPE(KeyPressed)
		EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
		
	private:
		unsigned int m_KeyCode;
		unsigned int m_RepeatCount;
	};

	class KeyReleasedEvent : public Event
	{
	public:
		KeyReleasedEvent(unsigned int keyCode)
			: m_KeyCode(keyCode)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent : (" << m_KeyCode << ")";

			return ss.str();
		}

		inline unsigned int GetKeyCode() const { return m_KeyCode; }
		
		EVENT_TYPE(KeyReleased)
		EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)

	private:
		unsigned int m_KeyCode;
	};

	class KeyTypedEvent : public Event
	{
	public:
		KeyTypedEvent(char key)
			: m_Key(key)
		{
		}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent : (" << m_Key << ")";

			return ss.str();
		}

		EVENT_TYPE(KeyTyped)
		EVENT_CATEGORY(EventCategoryInput | EventCategoryKeyboard)
	
	private:
		char m_Key;
	};
}