#include "GameObject.h"

void GameObject::start()
{
  for (u32 i = 0; i < components.count; ++i)
    components[i]->start();
}

void GameObject::update(const ApplicationContext &context)
{
  for (u32 i = 0; i < components.count; ++i)
    components[i]->update(context);
}

void GameObject::render(Renderer2D &renderer, f32 alpha)
{
  for (u32 i = 0; i < components.count; ++i)
    components[i]->render(renderer, alpha);
}

u32 GameObject::get_id() const
{
  return id;
}

Array<Component *> const &GameObject::get_all_components() const
{
  return components;
}
