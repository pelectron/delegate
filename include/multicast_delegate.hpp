/**
 * @file multicast_delegate.hpp
 * @author your name (you@domain.com)
 * @brief This file defines the multicast_delegate class.
 * @version 0.1
 * @date 2022-03-18
 *
 * Copyright Pele Constam 2022.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt) *
 */
#ifndef PCON_MULTICAST_DELEGATE_HPP
#define PCON_MULTICAST_DELEGATE_HPP
#include "delegate.hpp"

#include <vector>
namespace pc {
  /// forward declaration, intentionally left unimplemented.
  /// @sa delegate<Ret(Args...)>
  template <typename Signature>
  class multicast_delegate;

  namespace impl {

    /// meta function for the results vector's value_type.
    /// @tparam Ret multicast_delegate return type.
    template <typename Ret>
    struct ret_val {
      /// result of meta function
      using type = Ret;
    };

    /// specialization for (possibly const) the case where Ret is a reference
    /// type.
    /// @tparam Ret multicast_delegate return type.
    template <typename Ret>
    struct ret_val<Ret &> {
      /// result of meta function
      using type = std::reference_wrapper<Ret>;
    };

    /// specialization for r value reference return type.
    /// @tparam Ret multicast_delegate return type.
    template <typename Ret>
    struct ret_val<Ret &&> {
      /// result of meta function
      using type = Ret;
    };

    // get the result vector's value_type.
    template <typename Ret>
    using ret_val_t = typename ret_val<Ret>::type;

    /**
     * @brief This class stores the values returned by the delegates
     *
     * @tparam Ret return type of the multicast_delegate.
     */
    template <typename Ret>
    struct value_collector {
      /// vector type
      using vector_type = std::vector<ret_val_t<Ret>>;
      /// iterator type
      using iterator = typename vector_type::iterator;
      /// const iterator type
      using const_iterator = typename vector_type::const_iterator;

      vector_type values; //< storage for returned values
    };

    /// specialization for Ret = void.
    template <>
    struct value_collector<void> {
      /// no vector type
      using vector_type = void;
      /// no iterator type
      using iterator = void;
      /// no const iterator type
      using const_iterator = void;
      /// no storage for values
    };

  } // namespace impl

  /**
   * @brief the multicast_delegate class can bind any amount of callables,
   * execute them and collect the returned values.
   * @details The class is essentially a wrapper around a std::vector of
   * delegate<Ret(Args...)> (called delegate vector from now on) and std::vector
   * of Ret (called results vector from now on), and provides a simple interface
   * for them. Using bind() with a callable appends a delegate to the delegate
   * vector. Calling the multicast_delegate means calling each delegate in the
   * delegate vector and **appending** the returned value in the results vector.
   * The results vector can then be iterated through the various the begin/end
   * and cbegin/cend member functions. Note that result iterators are
   * invalidated when calling the multicast_delegate again, as the results
   * vector may need to reallocate when pushing back new values. The results
   * vector can then be cleared with the clear_results() member function. Using
   * reset() on a multicast_delegate has the same effect as on a normal
   * delegate. It unbinds all callables by clearing the delegate vector.
   *
   * To clear both vectors at once, use the total_reset() member function.
   *
   * @note Depending on the return type of the delegate, the underlying way of
   * storing the returned values can change quite a bit. Suppose T is the type
   * one gets when removing all reference qualifiers from the return
   * type Ret. If Ret is a reference type, i.e. Ret = T& or Ret = const T&, then
   * the results vector must store the return values in a
   * std::reference_wrapper. If Ret is an rvalue reference, i.e. Ret = T&&, then
   * the underlying storage is a std::vector<T>. The return values are simply
   * moved into the vector. If Ret = void, then there is no results vector and
   * all calls to the result iteration functions will result in compilation
   * failure via static_assert.
   *
   * @tparam Ret return type of the delegate
   * @tparam Args argument types of the delegate
   * @see multicast_delegate.cpp
   */
  template <typename Ret, typename... Args>
  class multicast_delegate<Ret(Args...)> {
  public:
    /// single delegate type.
    using delegate_t = delegate<Ret(Args...)>;
    /// delegate vector type.
    using delegate_vector_t = std::vector<delegate_t>;
    /// types that stores returned values.
    using result_storage_t = impl::value_collector<Ret>;
    /// result iterator type.
    using result_iterator = typename result_storage_t::iterator;
    /// const result iterator type.
    using const_result_iterator = typename result_storage_t::const_iterator;
    /// delegate iterator type.
    using delegate_iterator = typename delegate_vector_t::iterator;
    /// const delegate iterator type.
    using const_delegate_iterator = typename delegate_vector_t::const_iterator;
    /// value_type of the results vector.
    using value_type = impl::ret_val_t<Ret>;

    /// default constructor
    multicast_delegate() = default;
    /// default copy constructor
    multicast_delegate(const multicast_delegate &) = default;
    /// default move constructor
    multicast_delegate(multicast_delegate &&) = default;

    /// invoke the multicast_delegate
    void operator()(Args... args);

    /// get the number of callables bound to the multicast_delegate
    size_t num_callables() const;

    /**
     * @brief get the number of results stored in the multicast_delegate
     * @return size_t
     */
    size_t num_results() const;

    /// clear the results vector. The delegate vector will be unchanged.
    void clear_results();

    /// reset the multicast_delegate, i.e. clear the delegate vector. The
    /// results vector will be unchanged.
    void reset();

    /// equivalent to calling clear_results() and reset().
    void total_reset();

    /**
     * @brief bind a free function. This appends a new delegate to the delegate
     * vector.
     * @param free_function pointer to free function
     */
    void bind(Ret (*free_function)(Args...));

    /**
     * @brief bind an object and member function. This appends a new delegate to
     * the delegate vector.
     * @tparam T object type
     * @param object object instance
     * @param member_func pointer to member function to bind
     */
    template <typename T>
    void bind(T &object, Ret (T::*member_func)(Args...));

    /**
     * @brief bind an object and const member function. This appends a new
     * delegate to the delegate vector.
     * @tparam T object type
     * @param object object instance
     * @param member_func pointer to const member function.
     */
    template <typename T>
    void bind(T &object, Ret (T::*member_func)(Args...) const);

    /**
     * @brief bind a function object. This appends a new delegate to the
     * delegate vector.
     * @tparam F function object type
     * @param f function object instance
     */
    template <typename F,
              std::enable_if_t<!std::is_same_v<
                  std::decay_t<F>, delegate<Ret(Args...)>>> * = nullptr>
    void bind(F &&f);

    /**
     * @brief bind a delegate. This appends d to the delegate vector.
     * @param d delegate
     */
    void bind(delegate<Ret(Args...)> &&d);

    /**
     * @brief bind a delegate. This appends d to the delegate vector.
     * @param d delegate
     */
    void bind(const delegate<Ret(Args...)> &d);

    /// get iterator to the beginning of the delegate array.
    multicast_delegate::delegate_iterator delegate_begin();
    /// get iterator to the end of the delegate array.
    multicast_delegate::delegate_iterator delegate_end();
    /// get const iterator to the beginning of the delegate array.
    multicast_delegate::const_delegate_iterator delegate_begin() const;
    /// get const iterator to the end of the delegate array.
    multicast_delegate::const_delegate_iterator delegate_end() const;

    /**
     * @brief get iterator to the beggining of the results array.
     *
     * @return multicast_delegate<Ret(Args...)>::result_iterator
     */
    multicast_delegate::result_iterator begin();

    /**
     * @brief get iterator to the end of the results array.
     *
     * @return multicast_delegate::result_iterator
     */
    multicast_delegate::result_iterator end();

    /**
     * @brief get const iterator to the begin of the results array.
     *
     * @return multicast_delegate::const_result_iterator
     */
    multicast_delegate::const_result_iterator begin() const;

    /**
     * @brief get const iterator to the end of the results array.
     *
     * @return multicast_delegate::const_result_iterator
     */
    multicast_delegate::const_result_iterator end() const;

    /**
     * @brief get const iterator to the end of the results array.
     *
     * @return multicast_delegate::const_result_iterator
     */
    multicast_delegate::const_result_iterator cbegin() const;

    /**
     * @brief get const iterator to the end of the results array.
     *
     * @return multicast_delegate::const_result_iterator
     */
    multicast_delegate::const_result_iterator cend() const;

  private:
    delegate_vector_t                 delegates;
    [[maybe_unused]] result_storage_t collector;
  };

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::operator()(Args... args) {
    if constexpr (std::is_same_v<Ret, void>) {
      for (auto &del : delegates) {
        del(args...);
      }
    } else if constexpr (std::is_rvalue_reference_v<Ret>) {
      collector.values.reserve(collector.values.size() + delegates.size());
      for (auto &del : delegates) {
        collector.values.push_back(std::move(del(args...)));
      }
    } else {
      collector.values.reserve(collector.values.size() + delegates.size());
      for (auto &del : delegates) {
        collector.values.push_back(del(args...));
      }
    }
  }

  template <typename Ret, typename... Args>
  size_t multicast_delegate<Ret(Args...)>::num_callables() const {
    return delegates.size();
  }

  template <typename Ret, typename... Args>
  size_t multicast_delegate<Ret(Args...)>::num_results() const {
    if constexpr (!std::is_same_v<Ret, void>) {
      return collector.values.size();
    } else
      return 0;
  }

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::clear_results() {
    if constexpr (!std::is_same_v<Ret, void>) {
      collector.values.clear();
    }
  }

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::reset() {
    delegates.clear();
  }

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::bind(Ret (*free_function)(Args...)) {
    delegates.push_back(delegate<Ret(Args...)>(free_function));
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void multicast_delegate<Ret(Args...)>::bind(T &object,
                                              Ret (T::*member_func)(Args...)) {
    delegates.push_back(delegate<Ret(Args...)>(object, member_func));
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void multicast_delegate<Ret(Args...)>::bind(T &object,
                                              Ret (T::*member_func)(Args...)
                                                  const) {
    delegates.push_back(delegate<Ret(Args...)>(object, member_func));
  }

  template <typename Ret, typename... Args>
  template <typename F, std::enable_if_t<!std::is_same_v<
                            std::decay_t<F>, delegate<Ret(Args...)>>> *>
  void multicast_delegate<Ret(Args...)>::bind(F &&f) {
    delegates.push_back(delegate<Ret(Args...)>(std::forward<F>(f)));
  }

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::bind(const delegate<Ret(Args...)> &d) {
    delegates.push_back(d);
  }

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::bind(delegate<Ret(Args...)> &&d) {
    delegates.push_back(std::move(d));
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::delegate_iterator
      multicast_delegate<Ret(Args...)>::delegate_begin() {
    return delegates.begin();
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::const_delegate_iterator
      multicast_delegate<Ret(Args...)>::delegate_begin() const {
    return delegates.begin();
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::delegate_iterator
      multicast_delegate<Ret(Args...)>::delegate_end() {
    return delegates.end();
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::const_delegate_iterator
      multicast_delegate<Ret(Args...)>::delegate_end() const {
    return delegates.end();
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::result_iterator
      multicast_delegate<Ret(Args...)>::begin() {
    if constexpr (std::is_same_v<Ret, void>)
      static_assert(!std::is_same_v<Ret, void>,
                    "Cannot call this function with Ret = void.");
    else {
      return collector.values.begin();
    }
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::result_iterator
      multicast_delegate<Ret(Args...)>::end() {
    if constexpr (std::is_same_v<Ret, void>)
      static_assert(!std::is_same_v<Ret, void>,
                    "Cannot call this function with Ret = void.");
    else {
      return collector.values.end();
    }
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::const_result_iterator
      multicast_delegate<Ret(Args...)>::begin() const {
    if constexpr (std::is_same_v<Ret, void>)
      static_assert(!std::is_same_v<Ret, void>,
                    "Cannot call this function with Ret = void.");
    else {
      return collector.values.begin();
    }
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::const_result_iterator
      multicast_delegate<Ret(Args...)>::end() const {
    if constexpr (std::is_same_v<Ret, void>)
      static_assert(!std::is_same_v<Ret, void>,
                    "Cannot call this function with Ret = void.");
    else {
      return collector.values.end();
    }
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::const_result_iterator
      multicast_delegate<Ret(Args...)>::cbegin() const {
    if constexpr (std::is_same_v<Ret, void>)
      static_assert(!std::is_same_v<Ret, void>,
                    "Cannot call this function with Ret = void.");
    else {
      return collector.values.begin();
    }
  }

  template <typename Ret, typename... Args>
  typename multicast_delegate<Ret(Args...)>::const_result_iterator
      multicast_delegate<Ret(Args...)>::cend() const {
    if constexpr (std::is_same_v<Ret, void>)
      static_assert(!std::is_same_v<Ret, void>,
                    "Cannot call this function with Ret = void.");
    else {
      return collector.values.end();
    }
  }
} // namespace pc

#endif