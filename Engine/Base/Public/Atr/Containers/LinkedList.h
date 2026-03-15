#pragma once

#include "Atr/Core/Assertions/Assert.h"
#include "Atr/Core/Logging/Logger.h"

namespace Atr
{
  // Doubly linked list
  template <typename T> class LinkedList
  {
  public:
    struct Node
    {
      T data;
      Node *previous;
      Node *next;
    };

    Node *head;

  public:
    LinkedList()
    {
    }

    void insert(Node *previous_node, Node *new_node)
    {
      if (previous_node == nullptr)
      {
        if (head != nullptr)
        {
          new_node->next = head;
          new_node->next->previous = new_node;
        }
        else
        {
          new_node->next = nullptr;
        }

        head = new_node;
        head->previous = nullptr;
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

          if (new_node->next != nullptr)
          {
            new_node->next->previous = new_node;
          }

          previous_node->next = new_node;
          new_node->previous = previous_node;
        }
      }
    }

    void remove(Node *delete_node)
    {
      if (delete_node->previous == nullptr)
      {
        if (delete_node->next == nullptr)
        {
          head = nullptr;
        }
        else
        {
          head = delete_node->next;
          head->previous = nullptr;
        }
      }
      else
      {
        if (delete_node->next == nullptr)
        {
          delete_node->previous->next = nullptr;
        }
        else
        {
          delete_node->previous->next = delete_node->next;
          delte_node->next->previous = delte_node->previous;
        }
      }
    }

  private:
    LinkedList(LinkedList &L);
  };
}