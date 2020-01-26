gharrma/lang
============

A programming language built from scratch to experiment with language features,
static analyses, and LLVM.


Quick Start Guide
-----------------

You will need [LLVM](http://llvm.org) (e.g., `brew install llvm`).
I've recently tested with version `9.0.0`.

You will also need [CMake](https://cmake.org) (e.g., `brew install cmake`).
I've recently tested with version `3.16.2`.

```
$ git clone https://github.com/gharrma/lang.git
$ cd lang
$ mkdir build && cd build
$ cmake .. && make
```

If you prefer [Ninja](https://ninja-build.org) (recommended) over Unix makefiles, you can instead run
```
cmake -G Ninja .. && ninja
```

If you would like a debug build rather than a release build, use
the `--config Debug` flag for `cmake`.


REPL
----

Invoking the compiler with no arguments launches the REPL, which
evaluates expressions as you type them. Use a semicolon to tell the REPL
you are finished typing an expression. Below is example usage.

```
$ cd build && ./c
c> 1 + 2 * 3 * (4 + 5) * 6;
[parse] (1 + (((2 * 3) * (4 + 5)) * 6))
[value] i64 325
c> 1 + 2 + 3);
2:10 [parse] Unexpected token ')'.
```

Style Guide
-----------

For simplicity we use the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html),
though only loosely.


Contributing
------------

Any contributions are welcome! Make a pull request
or reach out to me at gharrma@gmail.com, and we'll make something happen.
