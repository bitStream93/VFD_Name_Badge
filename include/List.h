#include <Arduino.h>
#ifndef LIST_H
#define LIST_H

template <class T>
struct Entry {
  T data;
  Entry<T> *next;
};

template <typename T>
class List {
  public:
  List();
  List(int sizeIndex, T _t);

  virtual ~List();

  virtual int size();
  virtual bool push(int index, T);
  virtual bool push(T);
  virtual T pop();
  virtual T get(int index);
  virtual bool unshift(T);

  inline T &operator[](int index);
  inline T &operator[](size_t &i) { return this->get(i); }
  inline const T &operator[](const size_t &i) const { return this->get(i); }

  protected:
  int _size;
  Entry<T> *root;
  Entry<T> *last;
  Entry<T> *lastEntryGot;
  int lastIndexGot;
  bool isCached;
  Entry<T> *getEntry(int index);
};

template <typename T>
List<T>::List() {
  root = NULL;
  last = NULL;
  _size = 0;

  lastEntryGot = root;
  lastIndexGot = 0;
  isCached = false;
}

template <typename T>
List<T>::~List() {
  Entry<T> *tmp;
  while (root != NULL) {
    tmp = root;
    root = root->next;
    delete tmp;
  }
  last = NULL;
  _size = 0;
  isCached = false;
}

template <typename T>
Entry<T> *List<T>::getEntry(int index) {

  int _pos = 0;
  Entry<T> *current = root;

  // Check if the node trying to get is
  // immediatly AFTER the previous got one
  if (isCached && lastIndexGot <= index) {
    _pos = lastIndexGot;
    current = lastEntryGot;
  }

  while (_pos < index && current) {
    current = current->next;

    _pos++;
  }

  // Check if the object index got is the same as the required
  if (_pos == index) {
    isCached = true;
    lastIndexGot = index;
    lastEntryGot = current;

    return current;
  }

  return NULL;
}

template <typename T>
int List<T>::size() {
  return _size;
}

template <typename T>
List<T>::List(int sizeIndex, T _t) {
  for (int i = 0; i < sizeIndex; i++) {
    push(_t);
  }
}

template <typename T>
bool List<T>::push(int index, T _t) {

  if (index >= _size)
    return push(_t);

  if (index == 0)
    return unshift(_t);

  Entry<T> *tmp = new Entry<T>(), *_prev = getEntry(index - 1);
  tmp->data = _t;
  tmp->next = _prev->next;
  _prev->next = tmp;

  _size++;
  isCached = false;

  return true;
}

template <typename T>
bool List<T>::push(T _t) {

  Entry<T> *tmp = new Entry<T>();
  tmp->data = _t;
  tmp->next = NULL;

  if (root) {
    // Already have elements inserted
    last->next = tmp;
    last = tmp;
  } else {
    // First element being inserted
    root = tmp;
    last = tmp;
  }

  _size++;
  isCached = false;

  return true;
}

template <typename T>
T &List<T>::operator[](int index) {
  return getEntry(index)->data;
}

template <typename T>
T List<T>::pop() {
  if (_size <= 0)
    return T();

  isCached = false;

  if (_size >= 2) {
    Entry<T> *tmp = getEntry(_size - 2);
    T ret = tmp->next->data;
    delete (tmp->next);
    tmp->next = NULL;
    last = tmp;
    _size--;
    return ret;
  } else {
    // Only one element left on the list
    T ret = root->data;
    delete (root);
    root = NULL;
    last = NULL;
    _size = 0;
    return ret;
  }
}

template <typename T>
T List<T>::get(int index) {
  Entry<T> *tmp = getEntry(index);

  return (tmp ? tmp->data : T());
}

template<typename T>
bool List<T>::unshift(T _t){

	if(_size == 0)
		return push(_t);

	Entry<T> *tmp = new Entry<T>();
	tmp->next = root;
	tmp->data = _t;
	root = tmp;
	
	_size++;
	isCached = false;
	
	return true;
}

#endif