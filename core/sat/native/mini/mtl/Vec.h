/*******************************************************************************************[Vec.h]
Copyright (c) 2003-2007, Niklas Een, Niklas Sorensson
Copyright (c) 2007-2010, Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#ifndef Mini_Vec_h
#define Mini_Vec_h

#include <assert.h>
#include <algorithm>

#include <limits>
#include <cstring>
#include <new>

#include "core/sat/native/mini/mtl/IntTypes.h"
#include "core/sat/native/mini/mtl/XAlloc.h"

namespace Mini {

//=================================================================================================
// Automatically resizable arrays
//
// NOTE! Don't use this vector on datatypes that cannot be re-located in memory (with realloc)

template <class T, class _Size = int>
class vec {
 public:
  typedef _Size Size;

 private:
  T* data = nullptr;
  Size sz = 0;
  Size cap = 0;

  static inline Size max(Size x, Size y) {
    return (x > y) ? x : y;
  }

 public:
  // Constructors:
  vec() = default;

  explicit vec(Size size) : data(NULL), sz(0), cap(0) {
    growTo(size);
  }

  vec(Size size, const T& pad) : data(NULL), sz(0), cap(0) {
    growTo(size, pad);
  }

  ~vec() {
    clear(true);
  }

  vec<T>& operator=(vec<T> const& other) {
    capacity(other.size());
    sz = other.sz;
    std::copy(other.data, other.data + other.sz, data);
//    for (int i = 0; i < other.size(); ++i) {
//      (*this)[i] = other[i];
//    }
    return *this;
  }

  vec(vec<T> const& other) : vec(other.sz) {
    std::copy(other.data, other.data + other.sz, data);
//    for (int i = 0; i < other.size(); ++i)
//      (*this)[i] = other[i];
  }

  vec(vec<T>&& other) {
    std::swap(data, other.data);
    std::swap(sz, other.sz);
    std::swap(cap, other.cap);
  }

  // Pointer to first element:
  operator T*() noexcept {
    return data;
  }

  // Size operations:
  Size size() const noexcept {
    return sz;
  }

  void shrink(Size nelems) {
    assert(nelems <= sz);
    for (Size i = 0; i < nelems; i++)
      sz--, data[sz].~T();
  }
  void shrink_(Size nelems) {
    assert(nelems <= sz);
    sz -= nelems;
  }

  int capacity() const {
    return cap;
  }

  void capacity(Size min_cap);
  void growTo(Size size);
  void growTo(Size size, const T& pad);
  void clear(bool dealloc = false);

  // Stack interface:
  void push() {
    if (sz == cap)
      capacity(sz + 1);
    new (&data[sz]) T();
    sz++;
  }

  void push(const T& elem) {
    if (sz == cap)
      capacity(sz + 1);
    new (&data[sz++]) T(elem);
  }

  void push_(const T& elem) {
    assert(sz < cap);
    data[sz++] = elem;
  }

  void pop() noexcept {
    assert(sz > 0);
    sz--, data[sz].~T();
  }
  // NOTE: it seems possible that overflow can happen in the 'sz+1' expression of 'push()', but
  // in fact it can not since it requires that 'cap' is equal to INT_MAX. This in turn can not
  // happen given the way capacities are calculated (below). Essentially, all capacities are
  // even, but INT_MAX is odd.

  const T& last() const noexcept {
    return data[sz - 1];
  }

  T& last() noexcept {
    return data[sz - 1];
  }

  // Vector interface:
  const T& operator[](Size index) const noexcept {
    return data[index];
  }

  T& operator[](Size index) noexcept {
    return data[index];
  }

  // Duplicatation (preferred instead):
  void copyTo(vec<T>& copy) const noexcept {
    copy.clear();
    copy.growTo(sz);
    for (Size i = 0; i < sz; i++)
      copy[i] = data[i];
  }

  void moveTo(vec<T>& dest) {
    dest.clear(true);
    dest.data = data;
    dest.sz = sz;
    dest.cap = cap;
    data = NULL;
    sz = 0;
    cap = 0;
  }

  void memCopyTo(vec<T>& copy) const {
    copy.capacity(cap);
    copy.sz = sz;
    memcpy(copy.data, data, sizeof(T) * cap);
  }
};

template <class T, class _Size>
void vec<T, _Size>::capacity(Size min_cap) {
  if (cap >= min_cap)
    return;
  Size add =
      max((min_cap - cap + 1) & ~1, ((cap >> 1) + 2) & ~1);  // NOTE: grow by approximately 3/2
  const Size size_max = std::numeric_limits<Size>::max();
  data = (T*) ::realloc(data, (cap += add) * sizeof(T));
//  if (((size_max <= std::numeric_limits<int>::max()) && (add > size_max - cap)) ||
//      (((data = (T*) ::realloc(data, (cap += add) * sizeof(T))) == NULL) && errno == ENOMEM))
//    throw OutOfMemoryException();
}

template <class T, class _Size>
void vec<T, _Size>::growTo(Size size, const T& pad) {
  if (sz >= size)
    return;
  capacity(size);
  for (Size i = sz; i < size; i++)
    data[i] = pad;
  sz = size;
}

template <class T, class _Size>
void vec<T, _Size>::growTo(Size size) {
  if (sz >= size)
    return;
  capacity(size);
  for (Size i = sz; i < size; i++)
    new (&data[i]) T();
  sz = size;
}

template <class T, class _Size>
void vec<T, _Size>::clear(bool dealloc) {
  if (data != NULL) {
    for (Size i = 0; i < sz; i++)
      data[i].~T();
    sz = 0;
    if (dealloc)
      free(data), data = NULL, cap = 0;
  }
}

//=================================================================================================
}  // namespace Mini

#endif
