---
title: pc::impl::mfn_holder_t
summary: holds a pointer to T and a member function pointer with signature Ret(Args...) 

---

# pc::impl::mfn_holder_t



holds a pointer to T and a member function pointer with signature Ret(Args...)  [More...](#detailed-description)


`#include <delegate.hpp>`

## Public Functions

|                | Name           | Brief          |
| -------------- | -------------- | -------------- |
| | **[mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md#function-mfn-holder-t)**(T & object , Ret(T::*)(Args...) member_func ) | Constructor.  | 
| | **[mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md#function-mfn-holder-t)**(const [mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md) &  ) =default |  | 
| | **[mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md#function-mfn-holder-t)**([mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md) &&  ) =default |  | 

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| T * | **[t](structpc_1_1impl_1_1mfn__holder__t.md#variable-t)**  |
| Ret(T::* | **[func](structpc_1_1impl_1_1mfn__holder__t.md#variable-func)**  |

## Detailed Description

```cpp
template <typename T ,
typename Ret ,
typename... Args>
struct pc::impl::mfn_holder_t;
```

holds a pointer to T and a member function pointer with signature Ret(Args...) 

**Template Parameters**: 

  * **T** object type 
  * **Ret** member function return type. 
  * **Args** member function argument types. 

## Public Functions Documentation

### function mfn_holder_t

```cpp
inline mfn_holder_t(
    T & object,
    Ret(T::*)(Args...) member_func
)
```

Constructor. 

**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to member function 


### function mfn_holder_t

```cpp
mfn_holder_t(
    const mfn_holder_t & 
) =default
```


### function mfn_holder_t

```cpp
mfn_holder_t(
    mfn_holder_t && 
) =default
```


## Public Attributes Documentation

### variable t

```cpp
T * t;
```


### variable func

```cpp
Ret(T::* func;
```


-------------------------------

Updated on 2022-03-28 at 10:25:43 +0200