gharrma/lang
============

A programming language built from scratch to experiment with language features, static analyses, and LLVM.


Quick Start Guide
-----------------

```
$ git clone https://github.com/gharrma/lang.git
$ cd lang
$ make
```

The compiler should build with any C++ compiler supporting C++14 features.
But just in case, here's my current output for `g++ --version`.

```
Apple LLVM version 8.0.0 (clang-800.0.42.1)
Target: x86_64-apple-darwin15.6.0
Thread model: posix
```


REPL
----

Invoking the compiler with no arguments launches the REPL, which
evaluates expressions as you type them. Use a semicolon to tell the REPL
you are finished typing an expression. Below is example usage.

```
$ make clean && make release
$ ./c
c> 1 + 2 * 3 * (4 + 5) * 6;
[parse] (1 + (((2 * 3) * (4 + 5)) * 6))
[value] 325
c> 1. + 2. + x;
[parse] ((1.0 + 2.0) + x)
[value] unknown
c> 1 + 2 + 3);
3:10 [lex] Expected token ';'; instead found ')'.
```

Style Guide
-----------

For simplicity we use the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html), though only loosely.


Contributing
------------

Any contributions are welcome! Make a pull request
or reach out to me at gharrma@gmail.com, and we'll make something happen.
