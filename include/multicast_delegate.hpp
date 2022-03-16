#ifndef PCON_MULTICAST_DELEGATE_HPP
#define PCON_MULTICAST_DELEGATE_HPP
#include "delegate.hpp"

#include <vector>

template <typename Signature>
class multicast_delegate;

namespace mc_delegate_detail {

  template <typename T>
  struct ret_val {
    using type = T;
  };

  template <typename T>
  struct ret_val<T &> {
    using type = std::reference_wrapper<T>;
  };

  template <typename T>
  struct ret_val<T &&> {
    using type = T;
  };
  // get the type used to store the return values of the multicast_delegate,
  // i.e. T or std::reference_wrapper<std::remove_reference_t<T>>
  template <typename T>
  using ret_val_t = typename ret_val<T>::type;

  template <typename T, bool is_void = true>
  struct value_collector {
    using vector_type = void;
    using iterator = void;
    using const_iterator = void;
  };

  /// specialization for the case of non void return type.
  template <typename T>
  struct value_collector<T, false> {
    using vector_type = std::vector<ret_val_t<T>>;
    using iterator = typename vector_type::iterator;
    using const_iterator = typename vector_type::const_iterator;

    std::vector<ret_val_t<T>> values;
  };

  // aslias for helper struct which stores the collected values of the
  // multicast_delegate.
  template <typename T>
  using value_collector_t = value_collector<T, std::is_void_v<T>>;
} // namespace mc_delegate_detail

/**
 * @brief the multicast_delegate class can bind any amount of callables, execute
 * them and collect the returned values.
 * @details The class is essentially a wrapper around a std::vector of
 * delegate<Ret(Args...)> (called delegate vector from now on) and std::vector
 * of Ret (called results vector from now on), and provides a simple interface
 * for them. Using bind() with a callable appends a delegate to the delegate
 * vector. Calling the multicast_delegate means calling each delegate in the
 * delegate vector and **appending** the returned value in the results vector.
 * The results vector can then be iterated through the various the begin/end and
 * cbegin/cend member functions. Note that result iterators are invalidated when
 * calling the multicast_delegate again, as the results vector may need to
 * reallocate when pushing back new values.
 * The results vector can then be cleared with the clear_results() member
 * function.
 * Using reset() on a multicast_delegate has the same effect as on a normal
 * delegate. It unbinds all callables by clearing the delegate vector.
 *
 * To clear both vectors at once, use the total_reset() member function.
 *
 * @note Depending on the return type of the delegate, the underlying way of
 * storing the returned values can change quite a bit. Suppose T is the type one
 * gets when removing all reference and const qualifiers from the return type
 * Ret. If Ret is a reference type, i.e. Ret = T& or Ret = const T&, then the
 * results vector must store the return values in a std::reference_wrapper.
 * If Ret is an rvalue reference, i.e. Ret = T&&, then the underlying storage is
 * a std::vector<T>. The return values are simply moved into the vector.
 * If Ret = void, then there is no results vector and all calls to the result
 * iteration functions will result in compilation failure via static_assert.
 *
 * @tparam Ret return type of the delegate
 * @tparam Args argument types of the delegate
 */
template <typename Ret, typename... Args>
class multicast_delegate<Ret(Args...)> {
public:
  // delegate type
  using delegate_t = delegate<Ret(Args...)>;
  // delegate vector type
  using delegate_vector_t = std::vector<delegate_t>;
  // value_type of the results vector
  using result_storage_t = mc_delegate_detail::value_collector_t<Ret>;
  // results vector type
  using result_vector_t = typename result_storage_t::vector_type;
  // result iterator type
  using result_iterator = typename result_storage_t::iterator;
  // const result iterator type
  using const_result_iterator = typename result_storage_t::const_iterator;
  // delegate iterator type
  using delegate_iterator = typename delegate_vector_t::iterator;
  // const delegate iterator type
  using const_delegate_iterator = typename delegate_vector_t::const_iterator;

  multicast_delegate() = default;
  multicast_delegate(const multicast_delegate &) = default;
  multicast_delegate(multicast_delegate &&) = default;

  /// invoke the multicast_delegate
  void operator()(Args &&...args);

  /// get the number of callables bound to the multicast_delegate
  size_t num_callables() const;

  /// get the number of results stored in the multicast_delegate
  size_t num_results() const;

  /// get access to the n-th result.
  Ret get(size_t n);

  /// clear the results vector. The delegate vector will be unchanged.
  void clear_results();

  /// reset the multicast_delegate, i.e. clear the delegate vector. The results
  /// vector will be unchanged.
  void reset();

  /// equivalent to calling clear_results() and reset().
  void total_reset();
  
  /// append a new delegate
  void bind(Ret (*free_function)(Args...));

  /// append a new delegate
  template <typename T>
  void bind(T &obj, Ret (T::*member_func)(Args...));

  /// append a new delegate
  template <typename T>
  void bind(T &obj, Ret (T::*member_func)(Args...) const);

  /// append a new delegate
  template <
      typename F,
      std::enable_if_t<!std::is_same_v<std::decay_t<F>, delegate<Ret(Args...)>>>
          * = nullptr>
  void bind(F &&f);

  /// append a new delegate
  void bind(const delegate<Ret(Args...)> &d);

  /// append a new delegate
  void bind(delegate<Ret(Args...)> &&d);

  // delegate iteration functions
  delegate_iterator       delegate_begin();
  delegate_iterator       delegate_end();
  const_delegate_iterator delegate_begin() const;
  const_delegate_iterator delegate_end() const;

  // result iteration functions
  result_iterator       begin();
  result_iterator       end();
  const_result_iterator begin() const;
  const_result_iterator end() const;
  const_result_iterator cbegin() const;
  const_result_iterator cend() const;

private:
  delegate_vector_t                 delegates;
  [[maybe_unused]] result_storage_t collector;
};

template <typename Ret, typename... Args>
void multicast_delegate<Ret(Args...)>::operator()(Args &&...args) {
  if constexpr (std::is_same_v<Ret, void>) {
    for (auto &del : delegates) {
      del(std::forward<Args>(args)...);
    }
  } else if constexpr (std::is_rvalue_reference_v<Ret>) {
    collector.values.reserve(collector.values.size() + delegates.size());
    for (auto &del : delegates) {
      collector.values.push_back(std::move(del(std::forward<Args>(args)...)));
    }
  } else {
    collector.values.reserve(collector.values.size() + delegates.size());
    for (auto &del : delegates) {
      collector.values.push_back(del(std::forward<Args>(args)...));
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
Ret multicast_delegate<Ret(Args...)>::get(size_t n) {
  if constexpr (std::is_same_v<Ret, void>)
    static_assert(!std::is_same_v<Ret, void>,
                  "Cannot call this function with Ret = void.");
  else {
    return collector[n];
  }
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
void multicast_delegate<Ret(Args...)>::bind(T &obj,
                                            Ret (T::*member_func)(Args...)) {
  delegates.push_back(delegate<Ret(Args...)>(obj, member_func));
}

template <typename Ret, typename... Args>
template <typename T>
void multicast_delegate<Ret(Args...)>::bind(T &obj,
                                            Ret (T::*member_func)(Args...)
                                                const) {
  delegates.push_back(delegate<Ret(Args...)>(obj, member_func));
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

#endif