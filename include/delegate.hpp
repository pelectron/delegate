/**
 * @file delegate.hpp
 * @author Pele Constam (pele.constam@gmail.com)
 * @brief This file contains an implementation of a delegate class.
 * @version 0.1
 * @date 2022-03-14
 *
 * Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef PCON_DELEGATE_HPP
#define PCON_DELEGATE_HPP
#include <cstring>
#include <new>
#include <type_traits>

namespace delegate_impl {
  struct vtable; // forward declaration
  /// the maximum size a callable can be before it gets allocated on the heap.
  /// 16 was chosen because this is the byte size of two pointers in a 64 bit
  /// system, i.e. just what is needed to invoke a member function on an object
  static constexpr size_t max_storage_size = 16u;
} // namespace delegate_impl

template <typename Signature>
class delegate; // froward declaration, intentionally left unimplemented

/**
 * @brief This class can be used to execute free functions, member functions and
 * functors/ function objects, as long as they have a call signature of
 * Ret(Args...). Invoking the delegate bound to a object and member function
 * exhibits undefined behaviour, if the object's lifetime ends before invoking
 * the delegate. Invoking a empty/invalid delegate is perfectly legal.
 * Use the bind member function to bind to callables, use the reset member
 * function to reset/unbind a delegate.
 *
 *  @details The class consists of three main elements. A raw memory buffer of
 * 16 bytes(called storage), a pointer to a free function with signature
 * Ret(void*,Args...) (called invoke) and a pointer to a static vtable (called
 * table).
 *
 * The raw memory buffer is used to hold the callable data, a.k.a. either a
 * pointer a free function, a instance of mfn_holder_t/const_mfn_holder_t, a
 * instance of a passed in functor (sizeof(functor)<=16), or a pointer to a heap
 * allocated instance of a functor (sizeof(functor)>16)).
 *
 * To call the delegate, the address of the raw memory storage is passed to
 * the invoke member as a void*, along with the arguments. invoke must know
 * how to correctly cast the type back from the void* and execute the free
 * function/member function/function object stored in the delegate. The private
 * static member functions null_invoke, mfn_invoke<T>, const_mfn_invoke<T>,
 * inline_invoke<T> and heap_invoke<T> implement this behaviour.
 *
 * The table member is not a "real" compiler generated vtable, but a instance
 * of a custom 'vtable' type which holds the functions needed to copy, move and
 * destroy a callable correctly. See the delegate_impl namespace for more
 * details.
 *
 * A empty delegate will have invoke set to &null_invoke, table set to
 * delegate_impl::vtable::make_null() and storage cleared to 0.
 *
 * @tparam Ret return type of callable
 * @tparam Args argument types of callable
 */
template <typename Ret, typename... Args>
class delegate<Ret(Args...)> {
public:
  /// default constructor
  delegate();

  /// construct from free function pointer
  delegate(Ret (*free_function)(Args...));

  /// construct from object and member function
  template <typename T>
  delegate(T &obj, Ret (T::*member_func)(Args...));

  /// construct from object and const member function
  template <typename T>
  delegate(T &obj, Ret (T::*member_func)(Args...) const);

  /// construct from function object
  template <typename Lambda,
            std::enable_if_t<!std::is_same_v<
                std::decay_t<Lambda>, delegate<Ret(Args...)>>> * = nullptr>
  delegate(Lambda &&lambda);

  // copy constructors
  delegate(const delegate &other);

  /// move constructor
  delegate(delegate &&other);

  /// copy assignment operator
  delegate &operator=(const delegate &other);

  /// move assignment operator
  delegate &operator=(delegate &&other);

  /// destructor
  ~delegate();

  /// invoke the delegate
  Ret operator()(Args &&...args);

  /// bind to free function
  void bind(Ret (*free_function)(Args...)) noexcept;

  /// bind to object and member function
  template <typename T>
  void bind(T &obj, Ret (T::*member_function)(Args...)) noexcept;

  /// bind to object and const member function
  template <typename T>
  void bind(T &obj, Ret (T::*member_function)(Args...) const) noexcept;

  /// bind to function object
  template <typename Lambda,
            std::enable_if_t<!std::is_same_v<
                std::decay_t<Lambda>, delegate<Ret(Args...)>>> * = nullptr>
  void bind(Lambda &&lambda);

  /// returns true if delegate is bound to a callable
  bool is_valid() const;

  /// reset the delegate. This makes the delegate invalid.
  void reset();

private:
  /// knows how to invoke a free function
  static Ret free_func_invoke(void *obj, Args... args);
  /// knows how to invoke mfn_holder_t
  template <typename T>
  static Ret mfn_invoke(void *obj, Args... args);
  /// knows how to invoke const_mfn_holder_t
  template <typename T>
  static Ret const_mfn_invoke(void *obj, Args... args);
  /// knows to to invoke a inline stored functor
  template <typename Lambda>
  static Ret inline_invoke(void *lambda, Args... args);
  /// knows how to invoke a heap stored functor
  template <typename F>
  static Ret heap_invoke(void *f, Args... args);
  /// invokes nothing
  static Ret null_invoke(void *, Args...);

  using Storage_t = std::aligned_storage_t<delegate_impl::max_storage_size, 8>;
  using InvokeFuncPtr_t = Ret (*)(void *, Args...);
  /// holds either free function pointer, inline stored functor or pointer to
  /// heap allocated functor
  Storage_t storage;
  /// invokes the actual function/functor stored in the delegate
  InvokeFuncPtr_t invoke;
  /// vtable containing move/copy/destroy functions
  const delegate_impl::vtable *table;
};

namespace delegate_impl {
  /**
   * @brief holds a pointer to T and a member function pointer with signature
   * Ret(Args...)
   *
   * @tparam T object type
   */
  template <typename T, typename Ret, typename... Args>
  struct mfn_holder_t {
    mfn_holder_t(T &obj, Ret (T::*member_func)(Args...))
        : t(&obj), func(member_func) {}
    mfn_holder_t(const mfn_holder_t &) = default;
    mfn_holder_t(mfn_holder_t &&) = default;

    T *t;
    Ret (T::*func)(Args...);
  };

  /**
   * @brief holds a pointer to T and a const member function pointer with
   * signature Ret(Args...) const
   *
   * @tparam T object type
   */
  template <typename T, typename Ret, typename... Args>
  struct const_mfn_holder_t {
    const_mfn_holder_t(T &obj, Ret (T::*member_func)(Args...) const)
        : t(&obj), func(member_func) {}

    const_mfn_holder_t(const const_mfn_holder_t &) = default;
    const_mfn_holder_t(const_mfn_holder_t &&) = default;

    T *t;
    Ret (T::*func)(Args...) const;
  };
  /**
   * @brief vtable type holding a free copy, move and destroy function.
   * The void* parameters will always be the address of a delegates storage
   * member. Usage should become clear in the copy/move constructors/assignment
   * operators of the delegate class. The static member functions implement the
   * functions needed by the delegate class.
   */
  struct vtable {
    // members
    void (*copy)(void *dest, const void *src); // copy function
    void (*move)(void *dest, void *src);       // move function
    void (*destroy)(void *dest);               // destroy function

    // static functions which implement the members

    // inline copy/move/destroy
    template <typename T>
    static void inline_copy(void *dest, const void *source);
    template <typename T>
    static void inline_move(void *dest, void *source);
    template <typename T>
    static void inline_destroy(void *dest);

    // simple copy/move/detroy
    static void trivial_copy(void *dest, const void *source);
    static void trivial_move(void *dest, void *source);
    static void trivial_destroy(void *dest);

    // heap copy/move/destroy
    template <typename T>
    static void heap_copy(void *dest, const void *source);
    template <typename T>
    static void heap_move(void *dest, void *source);
    template <typename T>
    static void heap_destroy(void *dest);

    // null copy/move/destroy functions
    static void null_copy(void *, const void *);
    static void null_move(void *, void *);
    static void null_destroy(void *);

    // provides vtable for inline stored function objects of type T
    template <typename T>
    static const vtable *make_inline();

    // provides vtable for heap allocated function objects of type T
    template <typename T>
    static const vtable *make_heap();

    // provides vtable for inline stored memcopyable structures, i.e.
    // mfn_holder_t/const_mfn_holder_t/free function pointer
    static const vtable *make_trivial();

    // provides vtable which does nothing
    static const vtable *make_null();

    // provides vtable for trivially constructible, destructible and moveable
    // type
  };
} // namespace delegate_impl

template <typename Ret, typename... Args>
delegate<Ret(Args...)>::delegate()
    : storage{0}, invoke(&null_invoke),
      table(delegate_impl::vtable::make_null()) {}

template <typename Ret, typename... Args>
delegate<Ret(Args...)>::delegate(Ret (*free_function)(Args...)) : delegate() {
  bind(free_function);
}

template <typename Ret, typename... Args>
template <typename T>
delegate<Ret(Args...)>::delegate(T &obj, Ret (T::*member_func)(Args...))
    : delegate() {
  bind(obj, member_func);
}

template <typename Ret, typename... Args>
template <typename T>
delegate<Ret(Args...)>::delegate(T &obj, Ret (T::*member_func)(Args...) const)
    : delegate() {
  bind(obj, member_func);
}

template <typename Ret, typename... Args>
template <typename Lambda, std::enable_if_t<!std::is_same_v<
                               std::decay_t<Lambda>, delegate<Ret(Args...)>>> *>
delegate<Ret(Args...)>::delegate(Lambda &&lambda) : delegate() {
  bind(std::forward<std::decay_t<Lambda>>(lambda));
}

template <typename Ret, typename... Args>
delegate<Ret(Args...)>::delegate(const delegate<Ret(Args...)> &other) {
  // no check needed in case other is invalid, because table will always point
  // to a valid vtable instance.
  other.table->copy(&storage, &other.storage); // other knows how to copy itself
  table = other.table;
  invoke = other.invoke;
}

template <typename Ret, typename... Args>
delegate<Ret(Args...)>::delegate(delegate<Ret(Args...)> &&other) {
  other.table->move(&storage,
                    &other.storage); // other knows how to move itself.
  table = other.table;
  invoke = other.invoke;
  // the other delegate will be invalid after the move.
  other.table = delegate_impl::vtable::make_null();
  other.invoke = &null_invoke;
}

template <typename Ret, typename... Args>
delegate<Ret(Args...)> &
    delegate<Ret(Args...)>::operator=(const delegate<Ret(Args...)> &other) {
  table->destroy(&storage); // destroy our stored callable first
  other.table->copy(&storage, &other.storage); // then copy from other
  invoke = other.invoke;
  table = other.table;
  return *this;
}

template <typename Ret, typename... Args>
delegate<Ret(Args...)> &
    delegate<Ret(Args...)>::operator=(delegate<Ret(Args...)> &&other) {
  table->destroy(&storage);
  other.table->move(&storage, &other.storage);
  invoke = other.invoke;
  other.table = delegate_impl::vtable::make_null();
  other.invoke = &null_invoke;
  return *this;
}

template <typename Ret, typename... Args>
delegate<Ret(Args...)>::~delegate() {
  reset();
}

template <typename Ret, typename... Args>
Ret delegate<Ret(Args...)>::operator()(Args &&...args) {
  // because invoke will always have a valid address of a function, no check
  // needed to execute this.
  return static_cast<Ret>(
      invoke(static_cast<void *>(&storage), std::forward<Args>(args)...));
}

template <typename Ret, typename... Args>
void delegate<Ret(Args...)>::bind(Ret (*free_function)(Args...)) noexcept {
  using type = Ret (*)(Args...);
  reset();
  invoke = &free_func_invoke;
  table = delegate_impl::vtable::make_trivial();
  new (&storage) type(free_function);
}

template <typename Ret, typename... Args>
template <typename T>
void delegate<Ret(Args...)>::bind(T &obj,
                                  Ret (T::*member_function)(Args...)) noexcept {
  reset();
  invoke = &mfn_invoke<T>;
  table = delegate_impl::vtable::make_trivial();
  new (&storage)
      delegate_impl::mfn_holder_t<T, Ret, Args...>{obj, member_function};
  static_assert(
      sizeof(delegate_impl::mfn_holder_t<T, Ret, Args...>) <=
          delegate_impl::max_storage_size,
      "Something wrong in the implementation. The structure "
      "delegate_impl::mfn_holder_t<T, Ret, Args...> is too big to fit into "
      "aligned storage of size 'delegate_impl::max_storage_size'");
}

template <typename Ret, typename... Args>
template <typename T>
void delegate<Ret(Args...)>::bind(T &obj, Ret (T::*member_function)(Args...)
                                              const) noexcept {
  reset();
  invoke = &const_mfn_invoke<T>;
  table = delegate_impl::vtable::make_trivial();
  new (&storage)
      delegate_impl::const_mfn_holder_t<T, Ret, Args...>(obj, member_function);
  static_assert(sizeof(delegate_impl::const_mfn_holder_t<T, Ret, Args...>) <=
                    delegate_impl::max_storage_size,
                "Something wrong in the implementation. The structure "
                "delegate_impl::const_mfn_holder_t<T, Ret, Args...> is too big "
                "to fit into "
                "aligned storage of size 'delegate_impl::max_storage_size'");
}

template <typename Ret, typename... Args>
template <typename Lambda, std::enable_if_t<!std::is_same_v<
                               std::decay_t<Lambda>, delegate<Ret(Args...)>>> *>
void delegate<Ret(Args...)>::bind(Lambda &&lambda) {
  static_assert(
      std::is_invocable_r_v<Ret, decltype(lambda), Args...>,
      "The function object must have a call signature of Ret(Args...)");
  reset();
  if constexpr (sizeof(std::decay_t<Lambda>) <= 16) {
    // store the lambda inline with placement new into storage.
    new (&storage) std::decay_t<Lambda>(std::forward<Lambda>(lambda));
    table = delegate_impl::vtable::template make_inline<std::decay_t<Lambda>>();
    invoke = &inline_invoke<std::decay_t<Lambda>>;
  } else {
    // cannot store inline -> have to use
    *static_cast<std::decay_t<Lambda> **>(static_cast<void *>(&storage)) =
        new std::decay_t<Lambda>(std::forward<Lambda>(lambda));
    table = delegate_impl::vtable::template make_heap<std::decay_t<Lambda>>();
    invoke = &heap_invoke<std::decay_t<Lambda>>;
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
  table =
      delegate_impl::vtable::make_null(); // setting the table up to do nothing.
}

template <typename Ret, typename... Args>
Ret delegate<Ret(Args...)>::free_func_invoke(void *obj, Args... args) {
  // storage will contain a function pointer. The void* param will be the
  // address of storage. This means the real type of the param is 'pointer to
  // function pointer'.
  using type = Ret (*)(Args...); // type alias to easily cast
  // static_cast to Ret to account for the case where Ret = void
  return static_cast<Ret>((*static_cast<type *>(obj))(args...));
}

template <typename Ret, typename... Args>
template <typename T>
Ret delegate<Ret(Args...)>::mfn_invoke(void *obj, Args... args) {
  // storage will contain a mfn_holder_t<T, Ret, Args...> object. As such, obj's
  // real type is 'pointer to mfn_holder_t<T, Ret, Args...>'.
  using type = delegate_impl::mfn_holder_t<T, Ret, Args...>;
  // funky member function invocation through member function pointer.
  // without the casts, it would look like this: (t->*func)(args), where t is a
  // pointer to the object and func is the member function pointer.
  return static_cast<Ret>((
      static_cast<type *>(obj)->t->*(static_cast<type *>(obj)->func))(args...));
}

template <typename Ret, typename... Args>
template <typename T>
Ret delegate<Ret(Args...)>::const_mfn_invoke(void *obj, Args... args) {
  // look at mfn_invoke for detailed explanation. exactly the same principle,
  // just with the type being const_mfn_holder_t<T, Ret, Args...>.
  using type = delegate_impl::const_mfn_holder_t<T, Ret, Args...>;
  return static_cast<Ret>((
      static_cast<type *>(obj)->t->*(static_cast<type *>(obj)->func))(args...));
}

template <typename Ret, typename... Args>
Ret delegate<Ret(Args...)>::null_invoke(void *, Args...) {
  // this function does a null invoke, i.e. does nothing. In case Ret != void
  // and Ret is constructible with no arguments, a statically allocated value of
  // Ret is returned.
  if constexpr ((!std::is_same_v<Ret, void>)&&std::is_constructible_v<
                    std::decay_t<Ret>>) {
    static std::decay_t<Ret> r{};
    if constexpr (std::is_rvalue_reference_v<Ret>) {
      return std::move(r);
    } else
      return r;
  }
}

template <typename Ret, typename... Args>
template <typename F>
Ret delegate<Ret(Args...)>::heap_invoke(void *f, Args... args) {
  // storage will contain a pointer to a heap allocated functor. This means f's
  // correct type is F**.
  return static_cast<Ret>((*(*static_cast<F **>(f)))(args...));
}

template <typename Ret, typename... Args>
template <typename Lambda>
Ret delegate<Ret(Args...)>::inline_invoke(void *lambda, Args... args) {
  // storage will contain an instance of lambda inline -> lambda's correct type
  // is 'pointer to Lambda'
  return static_cast<Ret>((*static_cast<Lambda *>(lambda))(args...));
}

template <typename T>
void delegate_impl::vtable::inline_copy(void *dest, const void *source) {
  new (dest) T(*static_cast<const T *>(source));
}

template <typename T>
void delegate_impl::vtable::inline_move(void *dest, void *source) {
  new (dest) T(std::move(*static_cast<T *>(source)));
}

template <typename T>
void delegate_impl::vtable::inline_destroy(void *dest) {
  std::destroy_at(static_cast<T *>(dest));
}

inline void delegate_impl::vtable::trivial_copy(void *dest, const void *source) {
  std::memcpy(dest, source, delegate_impl::max_storage_size);
}

inline void delegate_impl::vtable::trivial_move(void *dest, void *source) {
  std::memcpy(dest, source, delegate_impl::max_storage_size);
}

inline void delegate_impl::vtable::trivial_destroy(void *dest) {
}

template <typename T>
void delegate_impl::vtable::heap_copy(void *dest, const void *source) {
  T *t = new T(*static_cast<const T *>(source));
  std::memcpy(dest, &t, sizeof(T *));
}

template <typename T>
void delegate_impl::vtable::heap_move(void *dest, void *source) {
  std::memcpy(dest, source, sizeof(T*));
}

template <typename T>
void delegate_impl::vtable::heap_destroy(void *dest) {
    delete *static_cast<T **>(dest);
}

inline void delegate_impl::vtable::null_copy(void *, const void *) {}
inline void delegate_impl::vtable::null_move(void *, void *) {}
inline void delegate_impl::vtable::null_destroy(void *) {}

template <typename T>
const delegate_impl::vtable *delegate_impl::vtable::make_inline() {
  if constexpr (std::is_trivially_constructible_v<T> &&
                std::is_trivially_destructible_v<T> &&
                std::is_trivially_move_constructible_v<T>) {
    delegate_impl::vtable::make_trivial();
  }
  static const delegate_impl::vtable impl{&vtable::inline_copy<T>,
                                          &vtable::inline_move<T>,
                                          &vtable::inline_destroy<T>};
  return &impl;
}

template <typename T>
const delegate_impl::vtable *delegate_impl::vtable::make_heap() {
  static const delegate_impl::vtable impl{
      &delegate_impl::vtable::heap_copy<T>, &delegate_impl::vtable::trivial_move,
      &delegate_impl::vtable::heap_destroy<T>};
  return &impl;
}

inline const delegate_impl::vtable *delegate_impl::vtable::make_trivial() {
  static const delegate_impl::vtable impl{
      &delegate_impl::vtable::trivial_copy, &delegate_impl::vtable::trivial_move,
      &delegate_impl::vtable::null_destroy};
  return &impl;
}

inline const delegate_impl::vtable *delegate_impl::vtable::make_null() {
  static const delegate_impl::vtable impl{&delegate_impl::vtable::null_copy,
                                          &delegate_impl::vtable::null_move,
                                          &delegate_impl::vtable::null_destroy};
  return &impl;
}

#endif