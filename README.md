gharrma/lang
============

A programming language built from scratch to experiment with language features,
static analyses, and LLVM.


Quick Start Guide
-----------------

The compiler should build with any C++ compiler supporting C++14 features;
I am using Clang.

You will also need LLVM, which you can often get through a package
packager (e.g., `brew install llvm`). Verify that `llvm-config --version`
outputs something close to `4.0.1`.

A simple Makefile builds the compiler.

```
$ git clone https://github.com/gharrma/lang.git
$ cd lang
$ make
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

For simplicity we use the
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html),
though only loosely.


Contributing
------------

Any contributions are welcome! Make a pull request
or reach out to me at gharrma@gmail.com, and we'll make something happen.
