---
title: C:/Users/pelec/OneDrive/Dokumente/Programming/C++Libs/delegate/include/delegate.hpp
summary: This file defines and implements the pc::delegate<Ret(Args...)> delegate class. 

---

# C:/Users/pelec/OneDrive/Dokumente/Programming/C++Libs/delegate/include/delegate.hpp

This file defines and implements the [pc::delegate<Ret(Args...)>](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md) delegate class.  [More...](#detailed-description)

## Namespaces

| Name           |
| -------------- |
| **[pc](namespacepc.md)**  |
| **[pc::impl](namespacepc_1_1impl.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[pc::delegate< Ret(Args...)>](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md)** <br>This class can be used to execute free functions, member functions and functors/ function objects, as long as they have a call signature of Ret(Args...).  |
| struct | **[pc::impl::mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md)** <br>holds a pointer to T and a member function pointer with signature Ret(Args...)  |
| struct | **[pc::impl::const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md)** <br>holds a pointer to T and a const member function pointer with signature Ret(Args...) const  |
| struct | **[pc::impl::vtable](structpc_1_1impl_1_1vtable.md)**  |

## Detailed Description

This file defines and implements the [pc::delegate<Ret(Args...)>](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md) delegate class. 

**Author**: Pele Constam (pelectron1602\gmail.com) 

**Version**: 0.1 

**Date**: 2022-03-14


Copyright Pele Constam 2022. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at [https://www.boost.org/LICENSE_1_0.txt](https://www.boost.org/LICENSE_1_0.txt)) 




## Source code

```cpp

#ifndef PC_DELEGATE_HPP
#define PC_DELEGATE_HPP
#include <cstring>
#include <new>
#include <type_traits>

namespace pc {
#ifndef GENERATING_DOCUMENTATION
  // forward declaration, intentionally left unimplemented
  template <typename>
  class delegate;
#endif

  namespace impl {
    struct vtable; // intentional forward declaration

    static constexpr size_t max_storage_size = 16u;
  } // namespace impl

  template <typename Ret, typename... Args>
  class delegate<Ret(Args...)> {
    static_assert(!std::is_rvalue_reference_v<Ret>,
                  "Ret cannot be an r value reference type");

  public:
    delegate();

    delegate(Ret (*free_function)(Args...));

    template <typename T>
    delegate(T& object, Ret (T::* member_func)(Args...));

    template <typename T>
    delegate(T& object, Ret (T::*member_func)(Args...) const);

    template <typename F,
              std::enable_if_t<!std::is_same_v<
                  std::decay_t<F>, delegate<Ret(Args...)>>>* = nullptr>
    delegate(F&& f);

    delegate(const delegate& other);

    delegate(delegate&& other);

    delegate& operator=(const delegate& other);

    delegate& operator=(delegate&& other);

    ~delegate();

    Ret operator()(Args... args);

    void bind(Ret (*free_function)(Args...)) noexcept;

    template <typename T>
    void bind(T& object, Ret (T::*member_func)(Args...)) noexcept;

    template <typename T>
    void bind(T& object, Ret (T::*member_func)(Args...) const) noexcept;

    template <typename F,
              std::enable_if_t<!std::is_same_v<
                  std::decay_t<F>, delegate<Ret(Args...)>>>* = nullptr>
    void bind(F&& f);

    bool is_valid() const;

    void reset();

  private:
    static Ret free_func_invoke(void* object, Args... args);

    template <typename T>
    static Ret mfn_invoke(void* object, Args... args);

    template <typename T>
    static Ret const_mfn_invoke(void* object, Args... args);

    template <typename F>
    static Ret inline_invoke(void* f, Args... args);

    template <typename F>
    static Ret heap_invoke(void* f, Args... args);

    static Ret null_invoke(void*, Args...);

    using Storage_t = std::aligned_storage_t<pc::impl::max_storage_size, 8>;
    using InvokeFuncPtr_t = Ret (*)(void*, Args...);

    // clang-format off
    Storage_t storage; 
    InvokeFuncPtr_t invoke; 
    const impl::vtable* table; 
    // clang-format on
  };

  namespace impl {

    template <typename T, typename Ret, typename... Args>
    struct mfn_holder_t {
      mfn_holder_t(T& object, Ret (T::*member_func)(Args...))
          : t(&object), func(member_func) {}
      mfn_holder_t(const mfn_holder_t&) = default;
      mfn_holder_t(mfn_holder_t&&) = default;

      T* t;
      Ret (T::*func)(Args...);
    };

    template <typename T, typename Ret, typename... Args>
    struct const_mfn_holder_t {
      const_mfn_holder_t(T& object, Ret (T::*member_func)(Args...) const)
          : t(&object), func(member_func) {}

      const_mfn_holder_t(const const_mfn_holder_t&) = default;
      const_mfn_holder_t(const_mfn_holder_t&&) = default;

      T* t;
      Ret (T::*func)(Args...) const;
    };

    struct vtable {
      void (*copy)(void* dest, const void* src); //< copies callables
      void (*move)(void* dest, void* src);       //< moves callables
      void (*destroy)(void* dest);               //< destroys callables

      template <typename T>
      static void inline_copy(void* dest, const void* source);
      template <typename T>
      static void inline_move(void* dest, void* source);
      template <typename T>
      static void inline_destroy(void* dest);

      static void trivial_copy(void* dest, const void* source);
      static void trivial_move(void* dest, void* source);

      template <typename T>
      static void heap_copy(void* dest, const void* source);
      template <typename T>
      static void heap_move(void* dest, void* source);
      template <typename T>
      static void heap_destroy(void* dest);

      static void null_copy(void*, const void*);
      static void null_move(void*, void*);
      static void null_destroy(void*);

      template <typename T>
      static const impl::vtable* make_inline();

      template <typename T>
      static const impl::vtable* make_heap();

      static const impl::vtable* make_trivial();

      static const impl::vtable* make_null();
    };
  } // namespace impl

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>::delegate()
      : storage{0}, invoke(&null_invoke), table(impl::vtable::make_null()) {}

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>::delegate(Ret (*free_function)(Args...)) : delegate() {
    bind(free_function);
  }

  template <typename Ret, typename... Args>
  template <typename T>
  delegate<Ret(Args...)>::delegate(T& object, Ret (T::*member_func)(Args...))
      : delegate() {
    bind(object, member_func);
  }

  template <typename Ret, typename... Args>
  template <typename T>
  delegate<Ret(Args...)>::delegate(T& object,
                                   Ret (T::*member_func)(Args...) const)
      : delegate() {
    bind(object, member_func);
  }

  template <typename Ret, typename... Args>
  template <typename F, std::enable_if_t<!std::is_same_v<
                            std::decay_t<F>, delegate<Ret(Args...)>>>*>
  delegate<Ret(Args...)>::delegate(F&& f) : delegate() {
    bind(std::forward<std::decay_t<F>>(f));
  }

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>::delegate(const delegate& other) {
    // no check needed in case other is invalid, because table will always point
    // to a valid vtable instance.
    other.table->copy(&storage,
                      &other.storage); // other knows how to copy itself
    table = other.table;
    invoke = other.invoke;
  }

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>::delegate(delegate&& other) {
    other.table->move(&storage,
                      &other.storage); // other knows how to move itself.
    table = other.table;
    invoke = other.invoke;
    // the other delegate will be invalid after the move.
    other.table = impl::vtable::make_null();
    other.invoke = &null_invoke;
  }

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>&
      delegate<Ret(Args...)>::operator=(const delegate& other) {
    table->destroy(&storage); // destroy callable stored in this first
    other.table->copy(&storage, &other.storage); // then copy from other
    invoke = other.invoke;
    table = other.table;
    return *this;
  }

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>& delegate<Ret(Args...)>::operator=(delegate&& other) {
    table->destroy(&storage); // destroy callable stored in this first
    other.table->move(&storage, &other.storage); // then move from other.
    invoke = other.invoke;
    // other will be invalid after the move
    other.table = impl::vtable::make_null();
    other.invoke = &null_invoke;
    return *this;
  }

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>::~delegate() {
    reset();
  }

  template <typename Ret, typename... Args>
  Ret delegate<Ret(Args...)>::operator()(Args... args) {
    // because invoke will always contain a valid address of a function, no
    // check needed to execute this.
    return static_cast<Ret>(
        invoke(static_cast<void*>(&storage), std::forward<Args>(args)...));
  }

  template <typename Ret, typename... Args>
  void delegate<Ret(Args...)>::bind(Ret (*free_function)(Args...)) noexcept {
    using type = Ret (*)(Args...);
    reset();
    invoke = &free_func_invoke;
    table = impl::vtable::make_trivial();
    new (&storage) type(free_function);
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void delegate<Ret(Args...)>::bind(T& object,
                                    Ret (T::*member_func)(Args...)) noexcept {
    reset();
    invoke = &mfn_invoke<T>;
    table = impl::vtable::make_trivial();
    new (&storage) impl::mfn_holder_t<T, Ret, Args...>{object, member_func};
    static_assert(sizeof(impl::mfn_holder_t<T, Ret, Args...>) <=
                      pc::impl::max_storage_size,
                  "Something wrong in the implementation. The structure "
                  "impl::mfn_holder_t<T, Ret, Args...> is too big to fit into "
                  "aligned storage of size 'max_storage_size'");
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void delegate<Ret(Args...)>::bind(T& object, Ret (T::*member_func)(Args...)
                                                   const) noexcept {
    reset();
    invoke = &const_mfn_invoke<T>;
    table = impl::vtable::make_trivial();
    new (&storage)
        impl::const_mfn_holder_t<T, Ret, Args...>(object, member_func);
    static_assert(sizeof(impl::const_mfn_holder_t<T, Ret, Args...>) <=
                      pc::impl::max_storage_size,
                  "Something wrong in the implementation. The structure "
                  "impl::const_mfn_holder_t<T, Ret, Args...> is too big "
                  "to fit into "
                  "aligned storage of size 'max_storage_size'");
  }

  template <typename Ret, typename... Args>
  template <typename F, std::enable_if_t<!std::is_same_v<
                            std::decay_t<F>, delegate<Ret(Args...)>>>*>
  void delegate<Ret(Args...)>::bind(F&& f) {
    static_assert(
        std::is_invocable_r_v<Ret, decltype(f), Args...>,
        "The function object must have a call signature of Ret(Args...)");
    reset();
    if constexpr (sizeof(std::decay_t<F>) <= 16) {
      // store the f inline with placement new into storage.
      new (&storage) std::decay_t<F>(std::forward<F>(f));
      table = impl::vtable::template make_inline<std::decay_t<F>>();
      invoke = &inline_invoke<std::decay_t<F>>;
    } else {
      // cannot store inline -> have to use the heap
      *reinterpret_cast<std::decay_t<F>**>(&storage) =
          new std::decay_t<F>(std::forward<F>(f));
      table = impl::vtable::template make_heap<std::decay_t<F>>();
      invoke = &heap_invoke<std::decay_t<F>>;
    }
  }

  template <typename Ret, typename... Args>
  bool delegate<Ret(Args...)>::is_valid() const {
    return invoke != &null_invoke;
  }

  template <typename Ret, typename... Args>
  void delegate<Ret(Args...)>::reset() {
    table->destroy(&storage); // properly deleting our contained object.
    invoke = &null_invoke;    // setting the delegate up to do nothing.
    table = impl::vtable::make_null(); // setting the table up to do
                                       // nothing.
  }

  template <typename Ret, typename... Args>
  Ret delegate<Ret(Args...)>::free_func_invoke(void* object, Args... args) {
    // storage will contain a function pointer. The void* param will be the
    // address of storage. This means the real type of the param is 'pointer to
    // function pointer'.
    using type = Ret (*)(Args...); // type alias to easily cast
    // static_cast to Ret to account for the case where Ret = void. Else there
    // would be a compile error.
    return static_cast<Ret>((*static_cast<type*>(object))(args...));
  }

  template <typename Ret, typename... Args>
  template <typename T>
  Ret delegate<Ret(Args...)>::mfn_invoke(void* object, Args... args) {
    // storage will contain a mfn_holder_t<T, Ret, Args...> instance inline. As
    // such, object's real type is 'pointer to mfn_holder_t<T, Ret, Args...>'.
    using type = impl::mfn_holder_t<T, Ret, Args...>;
    // funky member function invocation through member function pointer.
    // without the casts, it would look like this: (t->*func)(args...), where t
    // is a pointer to the instance and func is the member function pointer of
    // t.
    return static_cast<Ret>(
        (static_cast<type*>(object)->t->*(static_cast<type*>(object)->func))(
            args...));
  }

  template <typename Ret, typename... Args>
  template <typename T>
  Ret delegate<Ret(Args...)>::const_mfn_invoke(void* object, Args... args) {
    // look at mfn_invoke for detailed explanation. exactly the same principle,
    // just with the type being const_mfn_holder_t<T, Ret, Args...>.
    using type = impl::const_mfn_holder_t<T, Ret, Args...>;
    return static_cast<Ret>(
        (static_cast<type*>(object)->t->*(static_cast<type*>(object)->func))(
            args...));
  }

  template <typename Ret, typename... Args>
  Ret delegate<Ret(Args...)>::null_invoke(void*, Args...) {
    // this function does a null invoke, i.e. does nothing. In case Ret != void
    // and Ret is constructible with no arguments, a statically allocated value
    // of Ret is returned.
    using type = std::remove_cv_t<std::remove_reference_t<Ret>>;
    if constexpr ((!std::is_same_v<Ret, void>)&&std::is_constructible_v<type>) {
      static type r{};
      return r;
    }
  }

  template <typename Ret, typename... Args>
  template <typename F>
  Ret delegate<Ret(Args...)>::heap_invoke(void* f, Args... args) {
    // storage will contain a pointer to a heap allocated functor. This means
    // f's correct type is F**.
    return static_cast<Ret>((*(*static_cast<F**>(f)))(args...));
  }

  template <typename Ret, typename... Args>
  template <typename F>
  Ret delegate<Ret(Args...)>::inline_invoke(void* f, Args... args) {
    // storage will contain an instance of f inline -> f's correct type
    // is 'pointer to F'
    return static_cast<Ret>((*static_cast<F*>(f))(args...));
  }

  template <typename T>
  void impl::vtable::inline_copy(void* dest, const void* source) {
    // simply emplace new.
    new (dest) T(*static_cast<const T*>(source));
  }

  template <typename T>
  void impl::vtable::inline_move(void* dest, void* source) {
    // simply emplace new.
    new (dest) T(std::move(*static_cast<T*>(source)));
  }

  template <typename T>
  void impl::vtable::inline_destroy(void* dest) {
    std::destroy_at(static_cast<T*>(dest));
  }

  inline void impl::vtable::trivial_copy(void* dest, const void* source) {
    // just memcpy the whole whole storage
    std::memcpy(dest, source, pc::impl::max_storage_size);
  }

  inline void impl::vtable::trivial_move(void* dest, void* source) {
    // just memcpy the whole whole storage
    std::memcpy(dest, source, pc::impl::max_storage_size);
  }

  template <typename T>
  void impl::vtable::heap_copy(void* dest, const void* source) {
    // when copying, we have to allocate a new function object of type T.
    // source is the this delegates's storage's address. The storage contains a
    // pointer to T. As such, the correct type for source is 'pointer to pointer
    // to T'.
    T* t = new T(*(*static_cast<const T* const *>(source)));
    std::memcpy(dest, &t, sizeof(T*));
  }

  template <typename T>
  void impl::vtable::heap_move(void* dest, void* source) {
    // when moving a delegate, whe dont actually have to move the callable. we
    // only have to memcpy the storage correctly.
    std::memcpy(dest, source, sizeof(T*));
  }

  template <typename T>
  void impl::vtable::heap_destroy(void* dest) {
    delete *static_cast<T**>(dest);
  }

  inline void impl::vtable::null_copy(void*, const void*) {}
  inline void impl::vtable::null_move(void*, void*) {}
  inline void impl::vtable::null_destroy(void*) {}

  template <typename T>
  const impl::vtable* impl::vtable::make_inline() {
    if constexpr (std::is_trivially_constructible_v<T> &&
                  std::is_trivially_destructible_v<T> &&
                  std::is_trivially_move_constructible_v<T>) {
      return impl::vtable::make_trivial();
    }
    static const impl::vtable impl{&vtable::inline_copy<T>,
                                   &vtable::inline_move<T>,
                                   &vtable::inline_destroy<T>};
    return &impl;
  }

  template <typename T>
  const impl::vtable* impl::vtable::make_heap() {
    static const impl::vtable impl{&impl::vtable::heap_copy<T>,
                                   &impl::vtable::trivial_move,
                                   &impl::vtable::heap_destroy<T>};
    return &impl;
  }

  inline const impl::vtable* impl::vtable::make_trivial() {
    static const impl::vtable impl{&impl::vtable::trivial_copy,
                                   &impl::vtable::trivial_move,
                                   &impl::vtable::null_destroy};
    return &impl;
  }

  inline const impl::vtable* impl::vtable::make_null() {
    static const impl::vtable impl{&impl::vtable::null_copy,
                                   &impl::vtable::null_move,
                                   &impl::vtable::null_destroy};
    return &impl;
  }
} // namespace pc

#endif
```


-------------------------------

Updated on 2022-03-28 at 10:25:43 +0200
