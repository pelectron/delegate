---
title: Classes

---

# Classes




* **namespace [pc](namespacepc.md)** 
    * **class [delegate< Ret(Args...)>](classpc_1_1delegate_3_01_ret_07_args_8_8_8_08_4.md)** <br>This class can be used to execute free functions, member functions and functors/ function objects, as long as they have a call signature of Ret(Args...). 
    * **namespace [impl](namespacepc_1_1impl.md)** 
        * **struct [const_mfn_holder_t](structpc_1_1impl_1_1const__mfn__holder__t.md)** <br>holds a pointer to T and a const member function pointer with signature Ret(Args...) const 
        * **struct [mfn_holder_t](structpc_1_1impl_1_1mfn__holder__t.md)** <br>holds a pointer to T and a member function pointer with signature Ret(Args...) 
        * **struct [ret_val](structpc_1_1impl_1_1ret__val.md)** 
        * **struct [ret_val< Ret & >](structpc_1_1impl_1_1ret__val_3_01_ret_01_6_01_4.md)** 
        * **struct [ret_val< Ret && >](structpc_1_1impl_1_1ret__val_3_01_ret_01_6_6_01_4.md)** 
        * **struct [value_collector](structpc_1_1impl_1_1value__collector.md)** 
        * **struct [value_collector< void >](structpc_1_1impl_1_1value__collector_3_01void_01_4.md)** <br>specialization for Ret = void. 
        * **struct [vtable](structpc_1_1impl_1_1vtable.md)** 
    * **class [multicast_delegate< Ret(Args...)>](classpc_1_1multicast__delegate_3_01_ret_07_args_8_8_8_08_4.md)** <br>the multicast_delegate class can bind any amount of callables, execute them and collect the returned values. 
* **namespace [std](namespacestd.md)** <br>STL namespace. 



-------------------------------

Updated on 2022-03-28 at 10:25:43 +0200
