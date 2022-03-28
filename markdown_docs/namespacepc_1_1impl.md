---
title: pc::impl

---

# pc::impl



## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[pc::impl::const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md)** <br>holds a pointer to T and a const member function pointer with signature Ret(Args...) const  |
| struct | **[pc::impl::mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md)** <br>holds a pointer to T and a member function pointer with signature Ret(Args...)  |
| struct | **[pc::impl::ret_val](structpc_1_1impl_1_1ret__val.md)**  |
| struct | **[pc::impl::ret_val< Ret & >](structpc_1_1impl_1_1ret__val_3_01_ret_01_6_01_4.md)**  |
| struct | **[pc::impl::ret_val< Ret && >](structpc_1_1impl_1_1ret__val_3_01_ret_01_6_6_01_4.md)**  |
| struct | **[pc::impl::value_collector](structpc_1_1impl_1_1value__collector.md)**  |
| struct | **[pc::impl::value_collector< void >](structpc_1_1impl_1_1value__collector_3_01void_01_4.md)** <br>specialization for Ret = void.  |
| struct | **[pc::impl::vtable](structpc_1_1impl_1_1vtable.md)**  |

## Types

|                | Name           |
| -------------- | -------------- |
| template <typename Ret \> <br>using typename [ret_val](structpc_1_1impl_1_1ret__val.md)< Ret >::type | **[ret_val_t](namespacepc_1_1impl.md#using-ret-val-t)**  |

## Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr size_t | **[max_storage_size](namespacepc_1_1impl.md#variable-max-storage-size)**  |

## Types Documentation

### using ret_val_t

```cpp
template <typename Ret >
using ret_val_t =  typename ret_val<Ret>::type;
```




## Attributes Documentation

### variable max_storage_size

```cpp
static constexpr size_t max_storage_size = 16u;
```


the maximum size a callable can be before it gets allocated on the heap. 16 was chosen because this is the byte size of two pointers in a 64 bit system, i.e. just what is needed to invoke a member function on an object without allocating with new. 





-------------------------------

Updated on 2022-03-28 at 10:25:43 +0200