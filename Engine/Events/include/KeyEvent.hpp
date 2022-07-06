#pragma once
#include <sstream>

#include "Event.hpp"

namespace Engine {
class KeyEvent : public Event {
 public:
  inline int GetKeyCode() const { return m_KeyCode; }
  inline int GetMods() const { return m_Mods; }
  EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);

 protected:
  KeyEvent(int keycode, int mods) : m_KeyCode(keycode), m_Mods(mods) {}
  int m_KeyCode;
  int m_Mods;
};

class KeyPressedEvent : public KeyEvent {
 public:
  KeyPressedEvent(int keycode, int mods, int repeatCount)
      : KeyEvent(keycode, mods), m_RepeatCount(repeatCount) {}
  inline int GetRepeatCount() const { return m_RepeatCount; }

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << m_KeyCode << " (" << m_RepeatCount
       << " repeats)";
    return ss.str();
  }
  EVENT_CLASS_TYPE(KeyPressed)

 private:
  int m_RepeatCount;
};

class KeyReleasedEvent : public KeyEvent {
 public:
  KeyReleasedEvent(int keycode, int mods) : KeyEvent(keycode, mods) {}

  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << m_KeyCode;
    return ss.str();
  }
  EVENT_CLASS_TYPE(KeyReleased)

 private:
};

class KeyTypedEvent : public Event {
 public:
  KeyTypedEvent(unsigned int character) : m_Character(character) {}
  inline char GetCharacter() const { return m_Character; };
  std::string ToString() const override {
    std::stringstream ss;
    ss << "KeyTypedEvent: " << m_Character;
    ;
    return ss.str();
  }
  EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput);
  EVENT_CLASS_TYPE(KeyTyped);

 private:
  unsigned int m_Character;
};
}  // namespace Engine