/*      std::allocator_hybrid for container classes, allows to link
        external buffer. more info in README for PreCore Project

        Copyright 2018-2019 Lukas Cone

        Licensed under the Apache License, Version 2.0 (the "License");
        you may not use this file except in compliance with the License.
        You may obtain a copy of the License at

                http://www.apache.org/licenses/LICENSE-2.0

        Unless required by applicable law or agreed to in writing, software
        distributed under the License is distributed on an "AS IS" BASIS,
        WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
        See the License for the specific language governing permissions and
        limitations under the License.
*/

#pragma once
#ifndef _DECLSPEC_ALLOCATOR
#define _DECLSPEC_ALLOCATOR
#endif
namespace std {

template <class _Ty> struct allocator_hybrid {
  typedef _Ty value_type;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef value_type &reference;
  typedef const value_type &const_reference;
  typedef true_type propagate_on_container_copy_assignment;
  typedef true_type propagate_on_container_move_assignment;

  const static uintptr_t isDestroyed = static_cast<uintptr_t>(1)
                                       << (sizeof(uintptr_t) * 8 - 1);

  pointer buffer;

  allocator_hybrid() : buffer(nullptr) {}

  template <class _Uty> void destroy(_Uty *ptr) {
    if (!buffer)
      ptr->~_Uty();
    else
      reinterpret_cast<uintptr_t &>(buffer) = isDestroyed;
  }

  allocator_hybrid(pointer iBuff) : buffer(iBuff){};

  template <class U>
  allocator_hybrid(const allocator_hybrid<U> &) : allocator_hybrid() {}

  allocator_hybrid(const allocator_hybrid<value_type> &input) {
    buffer = input.buffer;
  }

  _DECLSPEC_ALLOCATOR pointer allocate(size_t count) {
    if (!buffer || isDestroyed & reinterpret_cast<uintptr_t>(buffer))
      return static_cast<pointer>(operator new(count * sizeof(value_type)));
    else
      return buffer;
  }

  void deallocate(pointer ptr, size_t count) {
    if (isDestroyed & reinterpret_cast<uintptr_t>(buffer)) {
      buffer = nullptr;
      return;
    }

    // for some reason GCC cannot pick correct delete (void*,size_t) function
    // std::size_t sizeToDestroy = count * sizeof(value_type);

    if (buffer != ptr)
      operator delete(ptr);
    else
      buffer = nullptr;
  }

  template <class C> static void DisposeStorage(C &input) {
#ifdef _MSC_VER
    input.~vector();
#endif
    input.clear();
  }
};

template <class T, class O>
bool operator==(const allocator_hybrid<T> &t, const allocator_hybrid<O> &o) {
  return t.buffer == o.buffer;
}
template <class T, class O>
bool operator!=(const allocator_hybrid<T> &t, const allocator_hybrid<O> &o) {
  return !(t == o);
}
} // namespace std