#pragma once

#include <Atreus/Graphics/Renderer2D.h>
#include <Atreus/Memory.h>
#include <Atreus/Runtime/Application.h>
#include <Atreus/Types.h>
#include <new>

class GameObject;

class Component
{
public:
  virtual ~Component() = default;

  virtual void start()
  {
  }

  virtual void update(const ApplicationContext &context)
  {
  }

  virtual void render(Renderer2D &renderer, f32 alpha)
  {
  }

  void *operator new(usize size)
  {
    return mem_alloc(default_allocator(), size);
  }

  void operator delete(void *ptr, usize size)
  {
    mem_free(default_allocator(), ptr, size);
  }

  void *operator new(usize size, std::align_val_t alignment)
  {
    return mem_alloc(default_allocator(), size, (usize)alignment);
  }

  void operator delete(void *ptr, usize size, std::align_val_t alignment)
  {
    mem_free(default_allocator(), ptr, size);
  }

  GameObject *owner = nullptr;
};