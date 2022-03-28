---
title: pc::impl::vtable

---

# pc::impl::vtable



 [More...](#detailed-description)


`#include <delegate.hpp>`

## Public Functions

|                | Name           | Brief          |
| -------------- | -------------- | -------------- |
| void | **[inline_copy](structpc_1_1impl_1_1vtable.md#function-inline-copy)**(void * dest , const void * source ) | copies inline function objects.  | 
| void | **[inline_move](structpc_1_1impl_1_1vtable.md#function-inline-move)**(void * dest , void * source ) | moves inline function objects.  | 
| void | **[inline_destroy](structpc_1_1impl_1_1vtable.md#function-inline-destroy)**(void * dest ) | destroys inline function objects.  | 
| void | **[trivial_copy](structpc_1_1impl_1_1vtable.md#function-trivial-copy)**(void * dest , const void * source ) | trivial copy from source to dest.  | 
| void | **[trivial_move](structpc_1_1impl_1_1vtable.md#function-trivial-move)**(void * dest , void * source ) | trivially move from source to dest.  | 
| void | **[heap_copy](structpc_1_1impl_1_1vtable.md#function-heap-copy)**(void * dest , const void * source ) | copies heap allocated function object from source to dest.  | 
| void | **[heap_move](structpc_1_1impl_1_1vtable.md#function-heap-move)**(void * dest , void * source ) | moves functor allocated on the heap into dest.  | 
| void | **[heap_destroy](structpc_1_1impl_1_1vtable.md#function-heap-destroy)**(void * dest ) | destroys heap allocated function object.  | 
| void | **[null_copy](structpc_1_1impl_1_1vtable.md#function-null-copy)**(void *  , const void *  ) | null copy. does nothing.  | 
| void | **[null_move](structpc_1_1impl_1_1vtable.md#function-null-move)**(void *  , void *  ) | null move. Does nothing.  | 
| void | **[null_destroy](structpc_1_1impl_1_1vtable.md#function-null-destroy)**(void *  ) | null destroy. does nothing.  | 
| const [impl::vtable](structpc_1_1impl_1_1vtable.md) * | **[make_inline](structpc_1_1impl_1_1vtable.md#function-make-inline)**() | provides vtable for inline stored function objects of type T  | 
| const [impl::vtable](structpc_1_1impl_1_1vtable.md) * | **[make_heap](structpc_1_1impl_1_1vtable.md#function-make-heap)**() | provides vtable for heap allocated function objects of type T  | 
| const [impl::vtable](structpc_1_1impl_1_1vtable.md) * | **[make_trivial](structpc_1_1impl_1_1vtable.md#function-make-trivial)**() | provides vtable for inline stored memcopyable structures, i.e. mfn_holder_t/const_mfn_holder_t/free function pointer or any other trivially destructible, trivially constructible and trivially movable function object type.  | 
| const [impl::vtable](structpc_1_1impl_1_1vtable.md) * | **[make_null](structpc_1_1impl_1_1vtable.md#function-make-null)**() | provides vtable which does nothing. This exists to simplify the control flow, i.e. the delegate never has to check if table is nullptr  | 

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| void(* | **[copy](structpc_1_1impl_1_1vtable.md#variable-copy)**  |
| void(* | **[move](structpc_1_1impl_1_1vtable.md#variable-move)**  |
| void(* | **[destroy](structpc_1_1impl_1_1vtable.md#variable-destroy)**  |

## Detailed Description

```cpp
struct pc::impl::vtable;
```


vtable type holding a free copy, move and destroy function. The void* parameters will always be the address of a delegates storage member. Usage should become clear in the copy/move constructors/assignment operators of the delegate class. The static member functions implement the functions needed by the delegate class. 

## Public Functions Documentation

### function inline_copy

```cpp
template <typename T >
static void inline_copy(
    void * dest,
    const void * source
)
```

copies inline function objects. 

**Template Parameters**: 

  * **T** function object type 


### function inline_move

```cpp
template <typename T >
static void inline_move(
    void * dest,
    void * source
)
```

moves inline function objects. 

**Template Parameters**: 

  * **T** function object type 


### function inline_destroy

```cpp
template <typename T >
static void inline_destroy(
    void * dest
)
```

destroys inline function objects. 

**Template Parameters**: 

  * **T** function object type 


### function trivial_copy

```cpp
static inline void trivial_copy(
    void * dest,
    const void * source
)
```

trivial copy from source to dest. 

### function trivial_move

```cpp
static inline void trivial_move(
    void * dest,
    void * source
)
```

trivially move from source to dest. 

### function heap_copy

```cpp
template <typename T >
static void heap_copy(
    void * dest,
    const void * source
)
```

copies heap allocated function object from source to dest. 

**Template Parameters**: 

  * **T** function object type 


### function heap_move

```cpp
template <typename T >
static void heap_move(
    void * dest,
    void * source
)
```

moves functor allocated on the heap into dest. 

**Template Parameters**: 

  * **T** function object type 


### function heap_destroy

```cpp
template <typename T >
static void heap_destroy(
    void * dest
)
```

destroys heap allocated function object. 

**Template Parameters**: 

  * **T** function object type 


### function null_copy

```cpp
static inline void null_copy(
    void * ,
    const void * 
)
```

null copy. does nothing. 

### function null_move

```cpp
static inline void null_move(
    void * ,
    void * 
)
```

null move. Does nothing. 

### function null_destroy

```cpp
static inline void null_destroy(
    void * 
)
```

null destroy. does nothing. 

### function make_inline

```cpp
template <typename T >
static const impl::vtable * make_inline()
```

provides vtable for inline stored function objects of type T 

**Template Parameters**: 

  * **T** function object type 


### function make_heap

```cpp
template <typename T >
static const impl::vtable * make_heap()
```

provides vtable for heap allocated function objects of type T 

**Template Parameters**: 

  * **T** function object type 


### function make_trivial

```cpp
static inline const impl::vtable * make_trivial()
```

provides vtable for inline stored memcopyable structures, i.e. mfn_holder_t/const_mfn_holder_t/free function pointer or any other trivially destructible, trivially constructible and trivially movable function object type. 

### function make_null

```cpp
static inline const impl::vtable * make_null()
```

provides vtable which does nothing. This exists to simplify the control flow, i.e. the delegate never has to check if table is nullptr 

## Public Attributes Documentation

### variable copy

```cpp
void(* copy;
```


### variable move

```cpp
void(* move;
```


### variable destroy

```cpp
void(* destroy;
```


-------------------------------

Updated on 2022-03-28 at 10:54:23 +0200