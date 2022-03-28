---
title: pc::impl::ret_val< Ret & >

---

# pc::impl::ret_val< Ret & >



 [More...](#detailed-description)


`#include <multicast_delegate.hpp>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::reference_wrapper< Ret > | **[type](structpc_1_1impl_1_1ret__val_3_01_ret_01_6_01_4.md#using-type)** <br>result of meta function  |

## Detailed Description

```cpp
template <typename Ret >
struct pc::impl::ret_val< Ret & >;
```


**Template Parameters**: 

  * **Ret** multicast_delegate return type. 


specialization for (possibly const) case where Ret is a reference type. 

## Public Types Documentation

### using type

```cpp
using type =  std::reference_wrapper<Ret>;
```

result of meta function 

-------------------------------

Updated on 2022-03-28 at 10:25:43 +0200