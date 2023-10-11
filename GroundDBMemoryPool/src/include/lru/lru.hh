#pragma once
#include <unordered_map>
namespace mempool {
template <typename T, typename V> class LRUCache {
private:
  template <typename NodeT, typename NodeV> class Node {
  public:
    NodeT key;
    NodeV val;
    Node *prev;
    Node *next;

    Node(NodeT key, NodeV val) {
      this->key = key;
      this->val = val;
    }
  };

  Node<T, V> *head = new Node<T, V>(-1, -1);
  Node<T, V> *tail = new Node<T, V>(-1, -1);

  int cap;
  std::unordered_map<int, Node<T, V> *> m;

  void addNode(Node<T, V> *newnode) {
    Node<T, V> *temp = head->next;

    newnode->next = temp;
    newnode->prev = head;

    head->next = newnode;
    temp->prev = newnode;
  }

  void deleteNode(Node<T, V> *delnode) {
    Node<T, V> *prevv = delnode->prev;
    Node<T, V> *nextt = delnode->next;

    prevv->next = nextt;
    nextt->prev = prevv;
  }

public:
  LRUCache(const int capacity) {
    cap = capacity;
    head->next = tail;
    tail->prev = head;
  }

  V get(const T key) {
    if (m.find(key) != m.end()) {
      Node<T, V> *resNode = m[key];
      int ans = resNode->val;

      m.erase(key);
      deleteNode(resNode);
      addNode(resNode);

      m[key] = head->next;
      return ans;
    }
    return -1;
  }

  void put(const T key, const V value) {
    if (m.find(key) != m.end()) {
      Node<T, V> *curr = m[key];
      m.erase(key);
      deleteNode(curr);
    }

    if (m.size() == cap) {
      m.erase(tail->prev->key);
      deleteNode(tail->prev);
    }

    addNode(new Node<T, V>(key, value));
    m[key] = head->next;
  }
};
} // namespace mempool