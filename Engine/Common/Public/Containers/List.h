#pragma once

#include "Definitions.h"

namespace Atr
{
  template <typename T> class List
  {
  public:
    struct Node
    {
      T value;
      Node *next;
    };

    Node *head;

    List() : head(nullptr)
    {
    }

    void Insert(Node *previous_node, Node *new_node)
    {
      if (previous_node == nullptr)
      {
        new_node->next = head;
        head = new_node;
      }
      else
      {
        if (previous_node->next == nullptr)
        {
          previous_node->next = new_node;
          new_node->next = nullptr;
        }
        else
        {
          new_node->next = previous_node->next;
          previous_node->next = new_node;
        }
      }
    }

    void Remove(Node *previous_node, Node *delete_node)
    {
      if (previous_node == nullptr)
      {
        if (delete_node->next == nullptr)
        {
          head = nullptr;
        }
        else
        {
          head = delete_node->next;
        }
      }
      else
      {
        previous_node->next = delete_node->next;
      }
    }

    void Push(Node *new_node)
    {
      new_node->next = head;
      head = new_node;
    }

    Node *Pop()
    {
      Node *top = head;
      head = head->next;
      return top;
    }
  };
}