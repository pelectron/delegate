---
title: pc::impl::const_mfn_holder_t
summary: holds a pointer to T and a const member function pointer with signature Ret(Args...) const 

---

# pc::impl::const_mfn_holder_t



holds a pointer to T and a const member function pointer with signature Ret(Args...) const  [More...](#detailed-description)


`#include <delegate.hpp>`

## Public Functions

|                | Name           | Brief          |
| -------------- | -------------- | -------------- |
| | **[const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md#function-const-mfn-holder-t)**(T & object , Ret(T::*)(Args...) const member_func ) | Constructor.  | 
| | **[const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md#function-const-mfn-holder-t)**(const [const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md) &  ) =default |  | 
| | **[const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md#function-const-mfn-holder-t)**([const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md) &&  ) =default |  | 

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| T * | **[t](structpc_1_1impl_1_1const__mfn__holder__t.md#variable-t)**  |
| Ret(T::* | **[func](structpc_1_1impl_1_1const__mfn__holder__t.md#variable-func)**  |

## Detailed Description

```cpp
template <typename T ,
typename Ret ,
typename... Args>
struct pc::impl::const_mfn_holder_t;
```

holds a pointer to T and a const member function pointer with signature Ret(Args...) const 

**Template Parameters**: 

  * **T** object type 
  * **Ret** member function return type. 
  * **Args** member function argument types. 

## Public Functions Documentation

### function const_mfn_holder_t

```cpp
inline const_mfn_holder_t(
    T & object,
    Ret(T::*)(Args...) const member_func
)
```

Constructor. 

**Parameters**: 

  * **object** object instance 
  * **member_func** pointer to const member function 


### function const_mfn_holder_t

```cpp
const_mfn_holder_t(
    const const_mfn_holder_t & 
) =default
```


### function const_mfn_holder_t

```cpp
const_mfn_holder_t(
    const_mfn_holder_t && 
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