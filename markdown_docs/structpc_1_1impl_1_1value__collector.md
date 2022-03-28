---
title: pc::impl::value_collector

---

# pc::impl::value_collector



 [More...](#detailed-description)


`#include <multicast_delegate.hpp>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using std::vector< [ret_val_t](namespacepc_1_1impl.md#using-ret-val-t)< Ret > > | **[vector_type](structpc_1_1impl_1_1value__collector.md#using-vector-type)** <br>vector type  |
| using typename vector_type::iterator | **[iterator](structpc_1_1impl_1_1value__collector.md#using-iterator)** <br>iterator type  |
| using typename vector_type::const_iterator | **[const_iterator](structpc_1_1impl_1_1value__collector.md#using-const-iterator)** <br>const iterator type  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| [vector_type](structpc_1_1impl_1_1value__collector.md#using-vector-type) | **[values](structpc_1_1impl_1_1value__collector.md#variable-values)**  |

## Detailed Description

```cpp
template <typename Ret >
struct pc::impl::value_collector;
```


**Template Parameters**: 

  * **Ret** return type of the multicast_delegate. 


This class stores the values returned by the delegates

## Public Types Documentation

### using vector_type

```cpp
using vector_type =  std::vector<ret_val_t<Ret> >;
```

vector type 

### using iterator

```cpp
using iterator =  typename vector_type::iterator;
```

iterator type 

### using const_iterator

```cpp
using const_iterator =  typename vector_type::const_iterator;
```

const iterator type 

## Public Attributes Documentation

### variable values

```cpp
vector_type values;
```


-------------------------------

Updated on 2022-03-28 at 10:54:23 +0200