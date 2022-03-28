---
title: pc::delegate< Ret(Args...)>
summary: This class can be used to execute free functions, member functions and functors/ function objects, as long as they have a call signature of Ret(Args...). 

---

# pc::delegate< Ret(Args...)>



This class can be used to execute free functions, member functions and functors/ function objects, as long as they have a call signature of Ret(Args...).  [More...](#detailed-description)


`#include <delegate.hpp>`

## Public Functions

|                | Name           | Brief          |
| -------------- | -------------- | -------------- |
| | **[delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate)**() | default constructor. Creates an invalid , i.e. unbound, delegate.  | 
| | **[delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate)**(Ret(*)(Args...) free_function ) |  | 
| | **[delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate)**(T & object , Ret(T::*)(Args...) member_func ) |  | 
| | **[delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate)**(T & object , Ret(T::*)(Args...) const member_func ) | construct from object and pointer to const member function  | 
| | **[delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate)**(F && f ) | construct from function object  | 
| | **[delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate)**(const delegate & other ) | copy construct from other delegate  | 
| | **[delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate)**(delegate && other ) | move construct other delegate  | 
| [delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate) & | **[operator=](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-operator=)**(const [delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate) & other ) | copy assignment operator  | 
| [delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate) & | **[operator=](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-operator=)**([delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-delegate) && other ) | move assignment operator  | 
| | **[~delegate](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-~delegate)**() | destructor  | 
| Ret | **[operator()](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-operator())**(Args... args ) | invoke the delegate. Executes the bound callable.  | 
| void | **[bind](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(Ret(*)(Args...) free_function ) | bind a free function.  | 
| void | **[bind](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(T & object , Ret(T::*)(Args...) member_func ) | bind an object and member function.  | 
| void | **[bind](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(T & object , Ret(T::*)(Args...) const member_func ) | bind an object and const member function.  | 
| void | **[bind](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-bind)**(F && f ) | bind a function object.  | 
| bool | **[is_valid](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-is-valid)**() const | query if a callable is bound to the delegate.  | 
| void | **[reset](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-reset)**() | reset the delegate. This unbinds the callable from the delegate.  | 

## Detailed Description

```cpp
template <typename Ret ,
typename... Args>
class pc::delegate< Ret(Args...)>;
```

This class can be used to execute free functions, member functions and functors/ function objects, as long as they have a call signature of Ret(Args...). 




# Allocation

The class will never heap allocate when binding a free function, object and member function, or a function object smaller than or equal to [max_storage_size] (16) bytes. The total size of a delegate is 32 bytes. To increase the buffer size, change [max_storage_size].


# Invoking

Some details to consider when invoking a delegate: If Ret is anything but void, then a statically allocated variable of type Ret is returned while the delegate is invalid. Because of static initialization, the value returned from a invalid delegate is always either zero initialized or default constructed.


# Theory of operation

The class consists of three main elements.

1. a raw memory buffer of 16 bytes called _storage_
2. a pointer to a free function with signature Ret(void*,Args...) called [invoke] and
3. a pointer to a static vtable called [table].
The raw memory buffer is used to hold the callable data, a.k.a. either a pointer a free function, a instance of mfn_holder_t/const_mfn_holder_t, a instance of a passed in functor (sizeof(functor)<=16), or a pointer to a heap allocated instance of a functor (sizeof(functor)>16)).

To call the delegate, the address of _storage_ is passed to the [invoke] as a void*, along with the arguments of the caller. The function [invoke] points to knows how to correctly cast the type back from the void* and execute the free function/member function/function object stored in the delegate. The private static member functions [null_invoke()](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-null-invoke), [mfn_invoke<T>()](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-mfn-invoke), [const_mfn_invoke<T>()](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-const-mfn-invoke), [inline_invoke<T>()](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-inline-invoke) and [heap_invoke<T>()](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-heap-invoke) implement this behaviour.

The [table] member is not a "real" compiler generated vtable, but a instance of a custom 'vtable' type. The vtable holds pointers to the functions needed to copy, move and destroy a callable of a certain type correctly. The [table] knows how to copy/move it's delegate's _storage_ into another's, but not the reverse. This should become clear in the [copy](delegate_8hpp_source.html#delegate-copy-ctor-src)/[move](delegate_8hpp_source.html#delegate-move-ctor-src) constructors and [copy](delegate_8hpp_source.html#delegate-copy-assign-src)/[move](delegate_8hpp_source.html#delegate-move-assign-src) assignment operators.

Retreturn type of the delegate 

Argsargument types of the delegate 

## Public Functions Documentation

### function delegate

```cpp
delegate()
```

default constructor. Creates an invalid , i.e. unbound, delegate. 

### function delegate

```cpp
delegate(
    Ret(*)(Args...) free_function
)
```


**Parameters**: 

  * **free_function** pointer to free function 


construct from free function 


### function delegate

```cpp
template <typename T >
delegate(
    T & object,
    Ret(T::*)(Args...) member_func
)
```


**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to member function 


**Template Parameters**: 

  * **T** object type 


construct from object and pointer to member function 


### function delegate

```cpp
template <typename T >
delegate(
    T & object,
    Ret(T::*)(Args...) const member_func
)
```

construct from object and pointer to const member function 

**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to const member function 


**Template Parameters**: 

  * **T** object type 


### function delegate

```cpp
template <typename F ,
std::enable_if_t<!std::is_same_v< std::decay_t< F >, delegate< Ret(Args...)> > > *  =nullptr>
delegate(
    F && f
)
```

construct from function object 

**Parameters**: 

  * **f** function object 


**Template Parameters**: 

  * **F** function object type 


### function delegate

```cpp
delegate(
    const delegate & other
)
```

copy construct from other delegate 

**Parameters**: 

  * **other** delegate to copy 


### function delegate

```cpp
delegate(
    delegate && other
)
```

move construct other delegate 

**Parameters**: 

  * **other** delegate to move 


**Note**: other will be invalid after the move. 

### function operator=

```cpp
delegate & operator=(
    const delegate & other
)
```

copy assignment operator 

**Parameters**: 

  * **other** delegate to copy 


**Return**: delegate<Ret,Args...>& reference to this 

### function operator=

```cpp
delegate & operator=(
    delegate && other
)
```

move assignment operator 

**Parameters**: 

  * **other** delegate to move 


**Return**: delegate<Ret,Args...>& reference to this 

**Note**: other will be invalid after the move. 

### function ~delegate

```cpp
~delegate()
```

destructor 

### function operator()

```cpp
Ret operator()(
    Args... args
)
```

invoke the delegate. Executes the bound callable. 

**Parameters**: 

  * **args** arguments 


**See**: delegate-invocation 

**Return**: Ret return type 

**Note**: [look here] for a more detailed explanation on invoking invalid delegates. 

### function bind

```cpp
void bind(
    Ret(*)(Args...) free_function
)
```

bind a free function. 

**Parameters**: 

  * **free_function** pointer to free function 


### function bind

```cpp
template <typename T >
void bind(
    T & object,
    Ret(T::*)(Args...) member_func
)
```

bind an object and member function. 

**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to member function to bind 


**Template Parameters**: 

  * **T** object type 


### function bind

```cpp
template <typename T >
void bind(
    T & object,
    Ret(T::*)(Args...) const member_func
)
```

bind an object and const member function. 

**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to const member function. 


**Template Parameters**: 

  * **T** object type 


### function bind

```cpp
template <typename F ,
std::enable_if_t<!std::is_same_v< std::decay_t< F >, delegate< Ret(Args...)> > > *  =nullptr>
void bind(
    F && f
)
```

bind a function object. 

**Parameters**: 

  * **f** function object instance 


**Template Parameters**: 

  * **F** function object type 


### function is_valid

```cpp
bool is_valid() const
```

query if a callable is bound to the delegate. 

**Return**: 

  * true callable is bound to the delegate 
  * false no callable bound to delegate. 


### function reset

```cpp
void reset()
```

reset the delegate. This unbinds the callable from the delegate. 

**Note**: [is_valid()](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md#function-is-valid) returns false after a call to this function. 

-------------------------------

Updated on 2022-03-28 at 10:54:23 +0200