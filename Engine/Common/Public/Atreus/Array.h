#pragma once

#include "Atreus/Log.h"
#include "Atreus/Memory.h"

#include <type_traits>

template <typename T> struct Array
{
  static_assert(std::is_trivially_copyable_v<T>, "Array only supports trivially copyable types");
  static_assert(
      std::is_trivially_constructible_v<T>, "Array only supports trivially constructible types"
  );
  static_assert(
      std::is_trivially_destructible_v<T>, "Array only supports trivially destructible types"
  );

  T               *data      = nullptr;
  u32              count     = 0;
  u32              capacity  = 0;
  const Allocator *allocator = nullptr;

  T &operator[](u32 index)
  {
    ATR_ASSERT(index < count, "index out of bounds");
    return data[index];
  }

  const T &operator[](u32 index) const
  {
    ATR_ASSERT(index < count, "index out of bounds");
    return data[index];
  }
};

template <typename T> void ArrayInit(Array<T> *array, const Allocator *allocator)
{
  array->data      = nullptr;
  array->count     = 0;
  array->capacity  = 0;
  array->allocator = allocator;
}

template <typename T> void ArrayFree(Array<T> *array)
{
  if (array->data != nullptr)
  {
    MemFree(array->allocator, array->data, sizeof(T) * array->capacity);

    array->data     = nullptr;
    array->count    = 0;
    array->capacity = 0;
  }
}

template <typename T> bool ArrayReserve(Array<T> *array, u32 min_capacity)
{
  if (array->capacity >= min_capacity) return true;

  ATR_ASSERT(array->allocator != nullptr, "Cannot reserve on Array before calling ArrayInit()");

  u32 new_capacity = array->capacity < 8 ? 8 : array->capacity;
  while (new_capacity < min_capacity)
  {
    new_capacity *= 2;
  }

  T *new_data = static_cast<T *>(
      MemAlloc(array->allocator, sizeof(T) * new_capacity, alignof(T) < 16 ? 16 : alignof(T))
  );
  if (new_data == nullptr) return false;

  if (array->data != nullptr)
  {
    memcpy(new_data, array->data, sizeof(T) * array->count);
    MemFree(array->allocator, array->data, sizeof(T) * array->capacity);
  }

  array->data     = new_data;
  array->capacity = new_capacity;
  return true;
}

// Resizes the array; new elements are zero-initialized
template <typename T> bool ArrayResize(Array<T> *array, u32 new_count)
{
  if (!ArrayReserve(array, new_count)) return false;

  if (new_count > array->count)
    memset(array->data + array->count, 0, sizeof(T) * (new_count - array->count));

  array->count = new_count;
  return true;
}

template <typename T> void ArrayClear(Array<T> *array)
{
  array->count = 0;
}

template <typename T> T *ArrayPush(Array<T> *array, const T &value)
{
  if (!ArrayReserve(array, array->count + 1)) return nullptr;

  T *slot = array->data + array->count;
  *slot   = value;
  array->count += 1;

  return slot;
}

// Appends an element and returns it zero-initialized
template <typename T> T *ArrayPushZeroed(Array<T> *array)
{
  if (!ArrayReserve(array, array->count + 1)) return nullptr;

  T *slot = array->data + array->count;
  memset(slot, 0, sizeof(T));
  array->count += 1;

  return slot;
}

template <typename T> bool ArrayAppend(Array<T> *array, const T *values, u32 value_count)
{
  if (value_count == 0) return true;
  if (!ArrayReserve(array, array->count + value_count)) return false;

  memcpy(array->data + array->count, values, sizeof(T) * value_count);
  array->count += value_count;
  return true;
}

template <typename T> T ArrayPop(Array<T> *array)
{
  ATR_ASSERT(array->count > 0, "Cannot pop on empty array");
  array->count -= 1;
  return array->data[array->count];
}

// Removes the element at index by moving the last element into it's place; O(1), unordered
template <typename T> void ArrayRemoveSwap(Array<T> *array, u32 index)
{
  ATR_ASSERT(array->count > index, "Cannot swap-remove with out of bounds index");
  array->data[index] = array->data[array->count - 1];
  array->count -= 1;
}

// Removes the element at index preserving order; O(n)
template <typename T> void ArrayRemoveOrdered(Array<T> *array, u32 index)
{
  ATR_ASSERT(array->count > index, "Cannot remove with out of bounds index");
  memmove(array->data + index, array->data + index + 1, sizeof(T) * (array->count - index - 1));
  array->count -= 1;
}

template <typename T> bool ArrayInsert(Array<T> *array, u32 index, const T &value)
{
  ATR_ASSERT(index <= array->count, "Cannot insert into full array");
  if (!ArrayReserve(array, array->count + 1)) return false;

  memmove(array->data + index + 1, array->data + index, sizeof(T) * (array->count - index));
  array->data[index] = value;
  array->count += 1;
  return true;
}

template <typename T> T *ArrayBegin(Array<T> *array)
{
  return array->data;
}

template <typename T> T *ArrayEnd(Array<T> *array)
{
  return array->data + array->count;
}

template <typename T> T *ArrayLast(Array<T> *array)
{
  ATR_ASSERT(array->count > 0, "Cannot get last element of empty array");
  return array->data + array->count - 1;
}
