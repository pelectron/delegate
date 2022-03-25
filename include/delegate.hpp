/**
 * \file delegate.hpp
 * \author Pele Constam (pelectron1602\gmail.com)
 * \anchor delegate-page
 * \brief This file defines and implements the \ref pc::delegate<Ret(Args...)>
 * delegate class.
 * \version 0.1
 * \date 2022-03-14
 *
 * Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 */
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

    /// \anchor max_storage_size
    /// the maximum size a callable can be before it gets allocated on the heap.
    /// 16 was chosen because this is the byte size of two pointers in a 64 bit
    /// system, i.e. just what is needed to invoke a member function on an
    /// object without allocating with new.
    static constexpr size_t max_storage_size = 16u;
  } // namespace impl

  /**
   * \brief \anchor delegate-brief This class can be used to execute free
   * functions, member functions and functors/ function objects, as long as they
   * have a call signature of Ret(Args...).
   *
   * \anchor delegate-details
   * \section delegate-allocation Allocation
   * The class will never heap allocate when binding a free
   * function, object and member function, or a function object smaller than or
   * equal to [max_storage_size](#max_storage_size) (16) bytes. The total size
   * of a delegate is 32 bytes. To increase the buffer size,
   * change [max_storage_size](#max_storage_size).
   *
   * \section delegate-invocation Invoking
   * Some details to consider when invoking a delegate:
   * If Ret is anything but void, then a statically
   * allocated variable of type Ret is returned while the
   * delegate is invalid. Because of static initialization,
   * the value returned from a invalid delegate is always
   * either zero initialized or default constructed.
   *
   * \section delegate-theory-of-operation Theory of operation
   * The class consists of three main elements.
   *  1. a raw memory buffer of 16 bytes called *storage*
   *  2. a pointer to a free function with
   * signature Ret(void*,Args...) called [invoke](#delegate-invoke) and
   *  3. a pointer to a static vtable called [table](#delegate-table).
   *
   * The raw memory buffer is used to hold the callable data, a.k.a. either a
   * pointer a free function, a instance of mfn_holder_t/const_mfn_holder_t, a
   * instance of a passed in functor (sizeof(functor)<=16), or a pointer to a
   * heap allocated instance of a functor (sizeof(functor)>16)).
   *
   * To call the delegate, the address of
   * *storage* is passed to the [invoke](#delegate-invoke) as
   * a void*, along with the arguments of the caller. The function
   * [invoke](#delegate-invoke) points to knows how to correctly cast the type
   * back from the void* and execute the free function/member function/function
   * object stored in the delegate. The private static member
   * functions null_invoke(), mfn_invoke<T>(), const_mfn_invoke<T>(),
   * inline_invoke<T>() and heap_invoke<T>() implement this behaviour.
   *
   * The [table](#delegate-table) member is not a "real" compiler generated
   * vtable, but a instance of a custom 'vtable' type. The vtable holds
   * pointers to the functions needed to copy, move and destroy a callable of a
   * certain type correctly. The [table](#delegate-table) knows how to copy/move
   * it's delegate's *storage* into
   * another's, but not the reverse. This should become clear in the
   * [copy](delegate_8hpp_source.html#delegate-copy-ctor-src)/[move](delegate_8hpp_source.html#delegate-move-ctor-src)
   * constructors and
   * [copy](delegate_8hpp_source.html#delegate-copy-assign-src)/[move](delegate_8hpp_source.html#delegate-move-assign-src)
   * assignment operators.
   *
   * \tparam Ret return type of the delegate
   * \tparam Args argument types of the delegate
   */
  template <typename Ret, typename... Args>
  class delegate<Ret(Args...)> {
    static_assert(!std::is_rvalue_reference_v<Ret>,
                  "Ret cannot be an r value reference type");

  public:
    /// default constructor. Creates an invalid , i.e. unbound, delegate.
    delegate();

    /**
     * construct from free function
     * \param free_function pointer to free function
     */
    delegate(Ret (*free_function)(Args...));

    /**
     * construct from object and pointer to member function
     * \tparam T object type
     * \param object object instance
     * \param member_func pointer to member function
     */
    template <typename T>
    delegate(T& object, Ret (T::* member_func)(Args...));

    /**
     * \brief construct from object and pointer to  const member function
     * \tparam T object type
     * \param object object instance
     * \param member_func pointer to const member function
     */
    template <typename T>
    delegate(T& object, Ret (T::*member_func)(Args...) const);

    /**
     * \brief construct from function object
     * \tparam F function object type
     * \param f function object
     */
    template <typename F,
              std::enable_if_t<!std::is_same_v<
                  std::decay_t<F>, delegate<Ret(Args...)>>>* = nullptr>
    delegate(F&& f);

    /**
     * \brief copy construct from other delegate
     * \param other delegate to copy
     */
    delegate(const delegate& other);

    /**
     * \brief move construct other delegate
     * \param other delegate to move
     * \note other will be invalid after the move.
     */
    delegate(delegate&& other);

    /**
     * \brief copy assignment operator
     * \param other delegate to copy
     * \return delegate<Ret,Args...>& reference to this
     */
    delegate& operator=(const delegate& other);

    /**
     * \brief move assignment operator
     * \param other delegate to move
     * \return delegate<Ret,Args...>& reference to this
     * \note other will be invalid after the move.
     */
    delegate& operator=(delegate&& other);

    /// \brief destructor
    ~delegate();

    /**
     * \brief invoke the delegate. Executes the bound callable.
     * \note \ref delegate-invocation "look here" for a more detailed
     * explanation on invoking invalid delegates.
     * \see delegate-invocation
     * \param args arguments
     * \return Ret return type
     */
    Ret operator()(Args... args);

    /**
     * \brief bind a free function.
     * \param free_function pointer to free function
     */
    void bind(Ret (*free_function)(Args...)) noexcept;

    /**
     * \brief bind an object and member function.
     * \tparam T object type
     * \param object object instance
     * \param member_func pointer to member function to bind
     */
    template <typename T>
    void bind(T& object, Ret (T::*member_func)(Args...)) noexcept;

    /**
     * \brief bind an object and const member function.
     * \tparam T object type
     * \param object object instance
     * \param member_func pointer to const member function.
     */
    template <typename T>
    void bind(T& object, Ret (T::*member_func)(Args...) const) noexcept;

    /**
     * \brief bind a function object.
     * \tparam F function object type
     * \param f function object instance
     */
    template <typename F,
              std::enable_if_t<!std::is_same_v<
                  std::decay_t<F>, delegate<Ret(Args...)>>>* = nullptr>
    void bind(F&& f);

    /**
     * \brief query if a callable is bound to the delegate.
     * \return true callable is bound to the delegate
     * \return false no callable bound to delegate.
     */
    bool is_valid() const;

    /**
     * \brief reset the delegate. This unbinds the callable from the delegate.
     * \note is_valid() returns false after a call to this function.
     */
    void reset();

  private:
    /// \brief knows how to invoke a free function.
    static Ret free_func_invoke(void* object, Args... args);

    /// \brief knows how to invoke \ref pc::impl::mfn_holder_t<T>.
    /// \tparam T object type
    template <typename T>
    static Ret mfn_invoke(void* object, Args... args);

    /// \brief knows how to invoke \ref pc::impl::const_mfn_holder_t<T>.
    /// \tparam T object type
    template <typename T>
    static Ret const_mfn_invoke(void* object, Args... args);

    /// \brief knows to to invoke a inline stored functor.
    /// \tparam F Functor type
    template <typename F>
    static Ret inline_invoke(void* f, Args... args);

    /// \brief knows how to invoke a heap stored functor.
    /// \tparam F Functor type
    template <typename F>
    static Ret heap_invoke(void* f, Args... args);

    /// \brief invokes nothing.
    /// Returns a statically allocated value if Ret != void.
    /// With this function, no switch/if is needed to check if invoke is valid
    /// when the delegate gets called.
    static Ret null_invoke(void*, Args...);

    /// raw storage type
    using Storage_t = std::aligned_storage_t<pc::impl::max_storage_size, 8>;
    /// type of invoke member
    using InvokeFuncPtr_t = Ret (*)(void*, Args...);

    // clang-format off
    /// \anchor delegate-storage
    Storage_t storage; ///< holds either pointer free function, inline stored functor, const_/mfn_holder_t or pointer to heap allocated functor.
    /// \anchor delegate-invoke
    InvokeFuncPtr_t invoke; ///< points to the free function that can actually execute the stored callable
    /// \anchor delegate-table
    const impl::vtable* table; ///< pointer to vtable containing pointers to move/copy/destroy functions
    // clang-format on
  };

  namespace impl {

    /**
     * \brief holds a pointer to T and a member function pointer with signature
     * Ret(Args...)
     * \tparam T object type
     * \tparam Ret member function return type.
     * \tparam Args member function argument types.
     */
    template <typename T, typename Ret, typename... Args>
    struct mfn_holder_t {
      /** \brief Constructor
       * \param object object instance
       * \param member_func pointer to member function
       */
      mfn_holder_t(T& object, Ret (T::*member_func)(Args...))
          : t(&object), func(member_func) {}
      mfn_holder_t(const mfn_holder_t&) = default;
      mfn_holder_t(mfn_holder_t&&) = default;

      T* t;
      Ret (T::*func)(Args...);
    };

    /**
     * \brief holds a pointer to T and a const member function pointer with
     * signature Ret(Args...) const
     * \tparam T object type
     * \tparam Ret member function return type.
     * \tparam Args member function argument types.
     */
    template <typename T, typename Ret, typename... Args>
    struct const_mfn_holder_t {
      /** \brief Constructor
       * \param object object instance
       * \param member_func pointer to const member function
       */
      const_mfn_holder_t(T& object, Ret (T::*member_func)(Args...) const)
          : t(&object), func(member_func) {}

      const_mfn_holder_t(const const_mfn_holder_t&) = default;
      const_mfn_holder_t(const_mfn_holder_t&&) = default;

      T* t;
      Ret (T::*func)(Args...) const;
    };

    /**
     * vtable type holding a free copy, move and destroy function.
     * The void* parameters will always be the address of a delegates storage
     * member. Usage should become clear in the copy/move
     * constructors/assignment operators of the delegate class. The static
     * member functions implement the functions needed by the delegate class.
     */
    struct vtable {
      void (*copy)(void* dest, const void* src); //< copies callables
      void (*move)(void* dest, void* src);       //< moves callables
      void (*destroy)(void* dest);               //< destroys callables

      /// \brief copies inline function objects.
      /// \tparam T function object type
      template <typename T>
      static void inline_copy(void* dest, const void* source);
      /// \brief moves inline function objects.
      /// \tparam T function object type
      template <typename T>
      static void inline_move(void* dest, void* source);
      /// \brief destroys inline function objects.
      /// \tparam T function object type
      template <typename T>
      static void inline_destroy(void* dest);

      /// \brief trivial copy from source to dest.
      static void trivial_copy(void* dest, const void* source);
      /// \brief trivially move from source to dest.
      static void trivial_move(void* dest, void* source);

      /// \brief copies heap allocated function object from source to dest.
      /// \tparam T function object type
      template <typename T>
      static void heap_copy(void* dest, const void* source);
      /// \brief moves functor allocated on the heap into dest.
      /// \tparam T function object type
      template <typename T>
      static void heap_move(void* dest, void* source);
      /// \brief destroys heap allocated function object.
      /// \tparam T function object type
      template <typename T>
      static void heap_destroy(void* dest);

      /// \brief null copy. does nothing.
      static void null_copy(void*, const void*);
      /// \brief null move. Does nothing.
      static void null_move(void*, void*);
      /// \brief null destroy. does nothing.
      static void null_destroy(void*);

      /// \brief provides vtable for inline stored function objects of type T
      /// \tparam T function object type
      template <typename T>
      static const impl::vtable* make_inline();

      /// \brief provides vtable for heap allocated function objects of type T
      /// \tparam T function object type
      template <typename T>
      static const impl::vtable* make_heap();

      /// \brief provides vtable for inline stored memcopyable structures, i.e.
      /// mfn_holder_t/const_mfn_holder_t/free function pointer or any other
      /// trivially destructible, trivially constructible and trivially movable
      /// function object type.
      static const impl::vtable* make_trivial();

      /// \brief provides vtable which does nothing. This exists to simplify the
      /// control flow, i.e. the delegate never has to check if table is nullptr
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
    /// \anchor delegate-copy-ctor-src
    // no check needed in case other is invalid, because table will always point
    // to a valid vtable instance.
    other.table->copy(&storage,
                      &other.storage); // other knows how to copy itself
    table = other.table;
    invoke = other.invoke;
  }

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>::delegate(delegate&& other) {
    /// \anchor delegate-move-ctor-src
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
    /// \anchor delegate-copy-assign-src
    table->destroy(&storage); // destroy callable stored in this first
    other.table->copy(&storage, &other.storage); // then copy from other
    invoke = other.invoke;
    table = other.table;
    return *this;
  }

  template <typename Ret, typename... Args>
  delegate<Ret(Args...)>& delegate<Ret(Args...)>::operator=(delegate&& other) {
    /// \anchor delegate-move-assign-src
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
      /// statically allocated variable in case of Ret != void. Ret must be
      /// constructible with no arguments.
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
