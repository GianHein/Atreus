#pragma once

#include "Component.h"

#include <Atreus/Array.h>
#include <Atreus/Types.h>

class GameObject
{
public:
  GameObject()                              = default;
  GameObject(const GameObject &)            = delete;
  GameObject &operator=(const GameObject &) = delete;

  void init()
  {
    id = NEXT_ID++;
    array_init(&components, default_allocator());
  }

  void shutdown()
  {
    for (u32 i = 0; i < components.count; ++i)
    {
      delete components[i];
    }

    array_free(&components);
  }

  void start();
  void update(const ApplicationContext &context);
  void render(Renderer2D &renderer, f32 alpha);

  u32                       get_id() const;
  Array<Component *> const &get_all_components() const;

  template <typename T> void add_component(T value = {})
  {
    ATR_ASSERT(
        get_component<T>() == nullptr, "Cannot add two components of same type to one GameObject"
    );

    T *component     = new T(value);
    component->owner = this;

    array_push(&components, (Component *)component);
  }

  template <typename T> T *get_component()
  {
    for (u32 i = 0; i < components.count; ++i)
    {
      if (T *component = dynamic_cast<T *>(components[i])) return component;
    }

    return nullptr;
  }

  template <typename T> void remove_component()
  {
    for (u32 i = 0; i < components.count; ++i)
    {
      if (T *component = dynamic_cast<T *>(components[i]))
      {
        delete component;
        array_remove_swap(&components, i);
        return;
      }
    }
  }

private:
  inline static u32 NEXT_ID = 1;
  u32               id      = 0;

  Array<Component *> components;
};