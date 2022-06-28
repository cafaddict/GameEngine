#pragma once
#include "Event.hpp"

namespace Engine {
class Layer {
 public:
  Layer(const std::string &name = "Layer");
  virtual ~Layer();

  virtual void OnAttach() {}
  virtual void OnDetach() {}
  virtual void OnUpdate() {}
  virtual void OnWait() {}
  virtual void OnEvent(Event &event) {}

  inline const std::string &GetName() const { return m_DebugName; }

 private:
  std::string m_DebugName;
};
}  // namespace Engine