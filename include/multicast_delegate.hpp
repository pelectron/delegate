#ifndef PCON_MULTICAST_DELEGATE_HPP
#define PCON_MULTICAST_DELEGATE_HPP
#include "delegate.hpp"

#include <vector>

template <typename Signature>
class multicast_delegate;

template <typename T>
struct ret_val {
  using type = T;
};
template <typename T>
struct ret_val<T &> {
  using type = std::reference_wrapper<T>;
};

template <typename T>
using ret_val_t = typename ret_val<std::remove_cv_t<T>>::type;

template <typename T, bool is_void = true>
struct value_collector {
  using iterator = void;
  using const_iterator = void;
};
template <typename T>
struct value_collector<T, false> {
  using iterator = typename std::vector<ret_val_t<T>>::iterator;
  using const_iterator = typename std::vector<ret_val_t<T>>::const_iterator;
  std::vector<ret_val_t<T>> values;
};
template <typename T>
using value_collector_t = value_collector<T, std::is_void_v<T>>;

/**
 * @brief the multicast_delegate class can bind any amount of callables, execute
 * them and collect the returned values. The class is essentially a wrapper
 * around a vector of delegate<Ret(Args...)> and vector of Ret Everytime a
 * multi_cast_delegate gets called, the retrun values get pushed back into the
 * results vector. The results vector can be cleared with the clear_results
 * member function. The delegate vector can be cleared with the unbind member
 * function.
 *
 *
 *
 * @tparam Ret
 * @tparam Args
 */
template <typename Ret, typename... Args>
class multicast_delegate<Ret(Args...)> {
public:
  static constexpr bool void_ret = std::is_same_v<Ret, void>;
  using delegate_t = delegate<Ret(Args...)>;
  using vector_t = std::vector<delegate_t>;
  using iterator = typename value_collector_t<Ret>::iterator;
  using const_iterator = typename value_collector_t<Ret>::const_iterator;
  multicast_delegate() = default;
  multicast_delegate(const multicast_delegate &) = default;
  multicast_delegate(multicast_delegate &&) = default;
  void operator()(Args &&...args) {
    if constexpr (void_ret) {
      for (auto &delegate : delegates) {
        delegate(std::forward<Args>(args)...);
      }
    } else {
      for (auto &delegate : delegates) {
        collector.values.push_back(delegate(std::forward<Args>(args)...));
      }
    }
  }

  size_t num_callables() const { return delegates.size(); }

  size_t num_results() const {
    if constexpr (!void_ret) {
      return collector.values.size();
    } else
      return 0;
  }

  Ret get(size_t n) {
    if constexpr (void_ret)
      static_assert(!void_ret, "Cannot call this function with Ret = void.");
    else {
      return collector[n];
    }
  }

  /// clear the results vector
  void clear_results() {
    if constexpr (!void_ret) {
      collector.values.clear();
    }
  }

  /// unbind the callables stored in this.
  void unbind() { delegates.clear(); }

  /// reset the whole multicast_delegate. Equivalent to calling unbind and
  /// clear_results on the multicast_delegate.
  void reset() {
    unbind();
    clear_results();
  }

  void bind(Ret (*free_function)(Args...)) {
    delegates.push_back(delegate<Ret(Args...)>(free_function));
  }

  template <typename T>
  void bind(T &obj, Ret (T::*member_func)(Args...)) {
    delegates.push_back(delegate<Ret(Args...)>(obj, member_func));
  }

  template <typename T>
  void bind(T &obj, Ret (T::*member_func)(Args...) const) {
    delegates.push_back(delegate<Ret(Args...)>(obj, member_func));
  }

  template <
      typename F,
      std::enable_if_t<!std::is_same_v<std::decay_t<F>, delegate<Ret(Args...)>>>
          * = nullptr>
  void bind(F &&f) {
    delegates.push_back(delegate<Ret(Args...)>(std::forward<F>(f)));
  }

  iterator begin() {
    if constexpr (void_ret)
      static_assert(!void_ret, "Cannot call this function with Ret = void.");
    else {
      return collector.values.begin();
    }
  }

  iterator end() {
    if constexpr (void_ret)
      static_assert(!void_ret, "Cannot call this function with Ret = void.");
    else {
      return collector.values.end();
    }
  }

  const_iterator begin() const {
    if constexpr (void_ret)
      static_assert(!void_ret, "Cannot call this function with Ret = void.");
    else {
      return collector.values.begin();
    }
  }

  const_iterator end() const {
    if constexpr (void_ret)
      static_assert(!void_ret, "Cannot call this function with Ret = void.");
    else {
      return collector.values.end();
    }
  }

  const_iterator cbegin() const {
    if constexpr (void_ret)
      static_assert(!void_ret, "Cannot call this function with Ret = void.");
    else {
      return collector.values.begin();
    }
  }

  const_iterator cend() const {
    if constexpr (void_ret)
      static_assert(!void_ret, "Cannot call this function with Ret = void.");
    else {
      return collector.values.end();
    }
  }

private:
  vector_t                                delegates;
  [[maybe_unused]] value_collector_t<Ret> collector;
};
#endif