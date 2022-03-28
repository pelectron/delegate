---
title: pc::multicast_delegate< Ret(Args...)>
summary: the multicast_delegate class can bind any amount of callables, execute them and collect the returned values. 

---

# pc::multicast_delegate< Ret(Args...)>



the multicast_delegate class can bind any amount of callables, execute them and collect the returned values.  [More...](#detailed-description)


`#include <multicast_delegate.hpp>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using ::pc::delegate< Ret(Args...)> | **[delegate_t](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-t)** <br>single delegate type.  |
| using std::vector< [delegate_t](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-t) > | **[delegate_vector_t](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-vector-t)** <br>delegate vector type.  |
| using [impl::value_collector](structpc_1_1impl_1_1value__collector.md)< Ret > | **[result_storage_t](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-result-storage-t)** <br>type that stores returned values.  |
| using typename [result_storage_t::iterator](structpc_1_1impl_1_1value__collector.md#using-iterator) | **[result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-result-iterator)** <br>result iterator type.  |
| using typename [result_storage_t::const_iterator](structpc_1_1impl_1_1value__collector.md#using-const-iterator) | **[const_result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-result-iterator)** <br>const result iterator type.  |
| using typename delegate_vector_t::iterator | **[delegate_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-iterator)** <br>delegate iterator type.  |
| using typename delegate_vector_t::const_iterator | **[const_delegate_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-delegate-iterator)** <br>const delegate iterator type.  |
| using [impl::ret_val_t](namespacepc_1_1impl.md#using-ret-val-t)< Ret > | **[value_type](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-value-type)** <br>value_type of the results vector.  |

## Public Functions

|                | Name           | Brief          |
| -------------- | -------------- | -------------- |
| | **[multicast_delegate](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-multicast-delegate)**() =default | default constructor  | 
| | **[multicast_delegate](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-multicast-delegate)**(const multicast_delegate &  ) =default | default copy constructor  | 
| | **[multicast_delegate](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-multicast-delegate)**(multicast_delegate &&  ) =default | default move constructor  | 
| void | **[operator()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-operator())**(Args... args ) | invoke the multicast_delegate  | 
| size_t | **[num_callables](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-num-callables)**() const | get the number of callables bound to the multicast_delegate  | 
| size_t | **[num_results](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-num-results)**() const |  | 
| void | **[clear_results](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-clear-results)**() | clear the results vector. The delegate vector will be unchanged.  | 
| void | **[reset](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-reset)**() |  | 
| void | **[total_reset](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-total-reset)**() | equivalent to calling [clear_results()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-clear-results) and [reset()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-reset).  | 
| void | **[bind](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(Ret(*)(Args...) free_function ) |  | 
| void | **[bind](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(T & object , Ret(T::*)(Args...) member_func ) |  | 
| void | **[bind](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(T & object , Ret(T::*)(Args...) const member_func ) |  | 
| void | **[bind](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(F && f ) |  | 
| void | **[bind](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**([delegate_t](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-t) && d ) |  | 
| void | **[bind](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(const [delegate_t](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-t) & d ) |  | 
| [delegate_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-iterator) | **[delegate_begin](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate-begin)**() | get iterator to the beginning of the delegate array.  | 
| [delegate_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-delegate-iterator) | **[delegate_end](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate-end)**() | get iterator to the end of the delegate array.  | 
| [const_delegate_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-delegate-iterator) | **[delegate_begin](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate-begin)**() const | get const iterator to the beginning of the delegate array.  | 
| [const_delegate_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-delegate-iterator) | **[delegate_end](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate-end)**() const | get const iterator to the end of the delegate array.  | 
| [result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-result-iterator) | **[begin](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-begin)**() |  | 
| [result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-result-iterator) | **[end](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-end)**() |  | 
| [const_result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-result-iterator) | **[begin](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-begin)**() const |  | 
| [const_result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-result-iterator) | **[end](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-end)**() const |  | 
| [const_result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-result-iterator) | **[cbegin](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-cbegin)**() const |  | 
| [const_result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-const-result-iterator) | **[cend](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-cend)**() const |  | 

## Detailed Description

```cpp
template <typename Ret ,
typename... Args>
class pc::multicast_delegate< Ret(Args...)>;
```

the multicast_delegate class can bind any amount of callables, execute them and collect the returned values. 

**Template Parameters**: 

  * **Ret** return type of the delegate 
  * **Args** argument types of the delegate 


**See**: multicast_delegate_example.cpp 

**Note**: Depending on the return type of the delegate, the underlying way of storing the returned values can change quite a bit. Suppose T is the type one gets when removing all reference qualifiers from the return type Ret.

* If Ret is a reference type, i.e. Ret = T& or Ret = const T&, then the results vector must store the return values in a std::reference_wrapper<T>.
* If Ret = void, then there is no results vector and all calls to the result iteration functions will result in compilation failure via static_assert.

The class is essentially a wrapper around a std::vector of [delegate<Ret(Args...)>](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md) (called delegate vector from now on) and std::vector of Ret (called results vector from now on), and provides a simple interface for them. Using [bind()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind) with a callable appends a delegate to the delegate vector. Calling the multicast_delegate means calling each delegate in the delegate vector and **appending** the returned value in the results vector. The results vector can then be iterated through the various the begin/end and cbegin/cend member functions. Note that result iterators are invalidated when calling the multicast_delegate again, as the results vector may need to reallocate when pushing back new values. The results vector can then be cleared with the [clear_results()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-clear-results) member function. Using [reset()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-reset) on a multicast_delegate has the same effect as on a normal delegate. It unbinds all callables by clearing the delegate vector.

To clear both vectors at once, use the [total_reset()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-total-reset) member function.

## Public Types Documentation

### using delegate_t

```cpp
using delegate_t =  ::pc::delegate<Ret(Args...)>;
```

single delegate type. 

### using delegate_vector_t

```cpp
using delegate_vector_t =  std::vector<delegate_t>;
```

delegate vector type. 

### using result_storage_t

```cpp
using result_storage_t =  impl::value_collector<Ret>;
```

type that stores returned values. 

### using result_iterator

```cpp
using result_iterator =  typename result_storage_t::iterator;
```

result iterator type. 

### using const_result_iterator

```cpp
using const_result_iterator =  typename result_storage_t::const_iterator;
```

const result iterator type. 

### using delegate_iterator

```cpp
using delegate_iterator =  typename delegate_vector_t::iterator;
```

delegate iterator type. 

### using const_delegate_iterator

```cpp
using const_delegate_iterator =  typename delegate_vector_t::const_iterator;
```

const delegate iterator type. 

### using value_type

```cpp
using value_type =  impl::ret_val_t<Ret>;
```

value_type of the results vector. 

## Public Functions Documentation

### function multicast_delegate

```cpp
multicast_delegate() =default
```

default constructor 

### function multicast_delegate

```cpp
multicast_delegate(
    const multicast_delegate & 
) =default
```

default copy constructor 

### function multicast_delegate

```cpp
multicast_delegate(
    multicast_delegate && 
) =default
```

default move constructor 

### function operator()

```cpp
void operator()(
    Args... args
)
```

invoke the multicast_delegate 

### function num_callables

```cpp
size_t num_callables() const
```

get the number of callables bound to the multicast_delegate 

### function num_results

```cpp
size_t num_results() const
```


**Return**: size_t 

get the number of results stored in the multicast_delegate 


### function clear_results

```cpp
void clear_results()
```

clear the results vector. The delegate vector will be unchanged. 

### function reset

```cpp
void reset()
```


reset the multicast_delegate, i.e. clear the delegate vector. The results vector will be unchanged. 


### function total_reset

```cpp
void total_reset()
```

equivalent to calling [clear_results()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-clear-results) and [reset()](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#function-reset). 

### function bind

```cpp
void bind(
    Ret(*)(Args...) free_function
)
```


**Parameters**: 

  * **free_function** pointer to free function 


bind a free function. This appends a new delegate to the delegate vector. 


### function bind

```cpp
template <typename T >
void bind(
    T & object,
    Ret(T::*)(Args...) member_func
)
```


**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to member function to bind 


**Template Parameters**: 

  * **T** object type 


bind an object and member function. This appends a new delegate to the delegate vector. 


### function bind

```cpp
template <typename T >
void bind(
    T & object,
    Ret(T::*)(Args...) const member_func
)
```


**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to const member function. 


**Template Parameters**: 

  * **T** object type 


bind an object and const member function. This appends a new delegate to the delegate vector. 


### function bind

```cpp
template <typename F ,
std::enable_if_t<!std::is_same_v< std::decay_t< F >, delegate_t > > *  =nullptr>
void bind(
    F && f
)
```


**Parameters**: 

  * **f** function object instance 


**Template Parameters**: 

  * **F** function object type 


bind a function object. This appends a new delegate to the delegate vector. 


### function bind

```cpp
void bind(
    delegate_t && d
)
```


**Parameters**: 

  * **d** delegate 


bind a delegate. This appends d to the delegate vector. 


### function bind

```cpp
void bind(
    const delegate_t & d
)
```


**Parameters**: 

  * **d** delegate 


bind a delegate. This appends d to the delegate vector. 


### function delegate_begin

```cpp
delegate_iterator delegate_begin()
```

get iterator to the beginning of the delegate array. 

### function delegate_end

```cpp
delegate_iterator delegate_end()
```

get iterator to the end of the delegate array. 

### function delegate_begin

```cpp
const_delegate_iterator delegate_begin() const
```

get const iterator to the beginning of the delegate array. 

### function delegate_end

```cpp
const_delegate_iterator delegate_end() const
```

get const iterator to the end of the delegate array. 

### function begin

```cpp
result_iterator begin()
```


**Return**: [multicast_delegate<Ret(Args...)>::result_iterator](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md#using-result-iterator)

get iterator to the beggining of the results array.


### function end

```cpp
result_iterator end()
```


**Return**: multicast_delegate::result_iterator 

get iterator to the end of the results array.


### function begin

```cpp
const_result_iterator begin() const
```


**Return**: multicast_delegate::const_result_iterator 

get const iterator to the begin of the results array.


### function end

```cpp
const_result_iterator end() const
```


**Return**: multicast_delegate::const_result_iterator 

get const iterator to the end of the results array.


### function cbegin

```cpp
const_result_iterator cbegin() const
```


**Return**: multicast_delegate::const_result_iterator 

get const iterator to the end of the results array.


### function cend

```cpp
const_result_iterator cend() const
```


**Return**: multicast_delegate::const_result_iterator 

get const iterator to the end of the results array.


-------------------------------

Updated on 2022-03-28 at 10:25:43 +0200