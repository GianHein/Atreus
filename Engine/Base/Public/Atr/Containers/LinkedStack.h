#pragma once

#include "Atr/Definitions.h"

namespace Atr
{
  template <typename T> class LinkedStack
  {
  public:
    struct Node
    {
      T data;
      Node *next;
    };

    Node *top;

  public:
    LinkedStack() = default;
    LinkedStack(LinkedStack &Other) = delete;

    void push(Node *new_node)
    {
      new_node->next = top;
      top = new_node;
    }

    Node *pop()
    {
      Node *popped_node = top;
      top = top->next;

      return popped_node;
    }
  };
}