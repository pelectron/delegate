---
title: C:/Users/pelec/OneDrive/Dokumente/Programming/C++Libs/delegate/include/multicast_delegate.hpp
summary: This file defines the pc::multicast_delegate<Ret(Args...)> multicast_delegate class. 

---

# C:/Users/pelec/OneDrive/Dokumente/Programming/C++Libs/delegate/include/multicast_delegate.hpp

This file defines the [pc::multicast_delegate<Ret(Args...)>](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md) multicast_delegate class.  [More...](#detailed-description)

## Namespaces

| Name           |
| -------------- |
| **[pc](namespacepc.md)**  |
| **[pc::impl](namespacepc_1_1impl.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[pc::impl::ret_val](structpc_1_1impl_1_1ret__val.md)**  |
| struct | **[pc::impl::ret_val< Ret & >](structpc_1_1impl_1_1ret__val_3_01_ret_01_6_01_4.md)**  |
| struct | **[pc::impl::ret_val< Ret && >](structpc_1_1impl_1_1ret__val_3_01_ret_01_6_6_01_4.md)**  |
| struct | **[pc::impl::value_collector](structpc_1_1impl_1_1value__collector.md)**  |
| struct | **[pc::impl::value_collector< void >](structpc_1_1impl_1_1value__collector_3_01void_01_4.md)** <br>specialization for Ret = void.  |
| class | **[pc::multicast_delegate< Ret(Args...)>](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md)** <br>the multicast_delegate class can bind any amount of callables, execute them and collect the returned values.  |

## Detailed Description

This file defines the [pc::multicast_delegate<Ret(Args...)>](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md) multicast_delegate class. 

**Author**: Pele Constam (pelectron1602\gmail.com) 

**Version**: 0.1 

**Date**: 2022-03-18


Copyright Pele Constam 2022. Distributed under the Boost Software License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at [https://www.boost.org/LICENSE_1_0.txt](https://www.boost.org/LICENSE_1_0.txt)) 




## Source code

```cpp

#ifndef PC_MULTICAST_DELEGATE_HPP
#define PC_MULTICAST_DELEGATE_HPP
#include "delegate.hpp"

#include <vector>

namespace pc {
#ifndef GENERATING_DOCUMENTATION
  template <typename>
  class multicast_delegate;
#endif

  namespace impl {

    template <typename Ret>
    struct ret_val {
      using type = Ret;
    };

    template <typename Ret>
    struct ret_val<Ret &> {
      using type = std::reference_wrapper<Ret>;
    };

    template <typename Ret>
    struct ret_val<Ret &&> {
      using type = Ret;
    };

    // get the result vector's value_type.
    template <typename Ret>
    using ret_val_t = typename ret_val<Ret>::type;

    template <typename Ret>
    struct value_collector {
      using vector_type = std::vector<ret_val_t<Ret>>;
      using iterator = typename vector_type::iterator;
      using const_iterator = typename vector_type::const_iterator;

      vector_type values; //< storage for returned values
    };

    template <>
    struct value_collector<void> {
      using vector_type = void;
      using iterator = void;
      using const_iterator = void;
    };

  } // namespace impl

  template <typename Ret, typename... Args>
  class multicast_delegate<Ret(Args...)> {
  public:
    using delegate_t = ::pc::delegate<Ret(Args...)>;
    using delegate_vector_t = std::vector<delegate_t>;
    using result_storage_t = impl::value_collector<Ret>;
    using result_iterator = typename result_storage_t::iterator;
    using const_result_iterator = typename result_storage_t::const_iterator;
    using delegate_iterator = typename delegate_vector_t::iterator;
    using const_delegate_iterator = typename delegate_vector_t::const_iterator;
    using value_type = impl::ret_val_t<Ret>;

    multicast_delegate() = default;
    multicast_delegate(const multicast_delegate &) = default;
    multicast_delegate(multicast_delegate &&) = default;

    void operator()(Args... args);

    size_t num_callables() const;

    size_t num_results() const;

    void clear_results();

    void reset();

    void total_reset();

    void bind(Ret (*free_function)(Args...));

    template <typename T>
    void bind(T &object, Ret (T::*member_func)(Args...));

    template <typename T>
    void bind(T &object, Ret (T::*member_func)(Args...) const);

    template <typename F,
              std::enable_if_t<!std::is_same_v<std::decay_t<F>, delegate_t>> * =
                  nullptr>
    void bind(F &&f);

    void bind(delegate_t &&d);

    void bind(const delegate_t &d);

    delegate_iterator delegate_begin();
    delegate_iterator delegate_end();
    const_delegate_iterator delegate_begin() const;
    const_delegate_iterator delegate_end() const;

    result_iterator begin();

    result_iterator end();

    const_result_iterator begin() const;

    const_result_iterator end() const;

    const_result_iterator cbegin() const;

    const_result_iterator cend() const;

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
    delegates.push_back(delegate_t(free_function));
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void multicast_delegate<Ret(Args...)>::bind(T &object,
                                              Ret (T::*member_func)(Args...)) {
    delegates.push_back(delegate_t(object, member_func));
  }

  template <typename Ret, typename... Args>
  template <typename T>
  void multicast_delegate<Ret(Args...)>::bind(T &object,
                                              Ret (T::*member_func)(Args...)
                                                  const) {
    delegates.push_back(delegate_t(object, member_func));
  }

  template <typename Ret, typename... Args>
  template <typename F, std::enable_if_t<!std::is_same_v<
                            std::decay_t<F>, delegate<Ret(Args...)>>> *>
  void multicast_delegate<Ret(Args...)>::bind(F &&f) {
    delegates.push_back(delegate_t(std::forward<F>(f)));
  }

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::bind(const delegate_t &d) {
    delegates.push_back(d);
  }

  template <typename Ret, typename... Args>
  void multicast_delegate<Ret(Args...)>::bind(delegate_t &&d) {
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
```


-------------------------------

Updated on 2022-03-28 at 10:25:43 +0200
