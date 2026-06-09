#pragma once

#include "Allocators.h"
#include "Definitions.h"

#include <type_traits>

namespace Atr
{
  // https://github.com/ocornut/imgui/blob/75f985998b0b0759470a46cd02e73d1c675487fa/imgui.h#L2211
  template <typename T> class Array
  {
  public:
    ATR_STATIC_ASSERT(std::is_trivially_copyable_v<T>,
                      "Array<T> only supports trivially copyable types");

    Array();
    ~Array();
    Array(const Array &other);
    Array &operator=(const Array &other);
    Array(Array &&other);
    Array &operator=(Array &&other);

    void Assign(u32 count, const T &value);

    // Element access
    T &At(u32 index);
    const T &At(u32 index) const;
    T &operator[](u32 index);
    const T &operator[](u32 index) const;
    T &Front();
    const T &Front() const;
    T &Back();
    const T &Back() const;
    T *Data();
    const T *Data() const;

    b8 Contains(const T &value) const;
    u32 Find(const T &value) const;

    // Iterators
    T *Begin();
    const T *Begin() const;
    T *End();
    const T *End() const;

    // Capacity
    b8 Empty() const;
    u32 Size() const;
    void Reserve(u32 new_capacity);
    u32 Capacity() const;
    void Shrink(u32 new_size);
    void ShrinkToFit();

    // Modifiers
    void Clear();
    T *Insert(u32 index, const T &value);
    T *Insert(u32 index, T &&value);
    T *Insert(u32 index, u32 count, const T &value);
    T *Erase(u32 index);
    T *Erase(u32 first_index, u32 last_index);
    void PushBack(const T &value);
    void PushBack(T &&value);
    template <typename... Args> T &EmplaceBack(Args &&...args);
    void PopBack();
    void Resize(u32 count);
    void Resize(u32 count, const T &value);
    void SwapAndPop(u32 index);

    u32 size;
    u32 capacity;
    T *data;
  };
}