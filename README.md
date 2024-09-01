# Concurrent Structs

## Locking Implementations:

#### Stacks:

###### ``SGLStack.hpp``:

Single Global Lock stack. A standard thread-safe stack using mutexes to block

###### ``FCStack.hpp``:

SGL Stack that implements flat-combining as an optimization

###### ``ElimSGLStack.hpp``:

SGL Stack that implements elimination as an optimization

###### ``ElimTStack.hpp``:

Treiber Stack that implements elimintation as an optimization


#### Queues:

###### ``SGLQueue.hpp``:

    Single Global Lock queue. A standard thread-safe queue using mutexes to block

###### ``FCQueue.hpp``:

SGL Queue that implements flat-combining as an optimization


## Lock-Free Implementations:

#### Stacks:

###### ``TStack.hpp``:

Treiber Stack implementation. A thread-safe, lock-free stack


#### Queues:

###### ``MSQueue.hpp``:

Michael & Scott queue implementation. A thread-safe, lock-free queue.


# TODO:

- Test suite
- Templatize all of the structs and make sure they're returning T on pop() instead of something like int, etc
- C implementations
- Comparison using perf (cache hit % )
- Padded versions for cache efficiency (minimize false sharing)
- C vs C++ impl (using int)
