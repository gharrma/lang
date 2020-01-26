gharrma/lang
============

A programming language built from scratch to experiment with language features, static analyses, and LLVM.


Getting started
---

### Prerequisites

- You will need [LLVM](http://llvm.org) (e.g. `brew install llvm`)

- You will need [CMake](https://cmake.org) (e.g. `brew install cmake`)

- [Optional] For faster builds you can install [Ninja](https://ninja-build.org) (e.g. `brew install ninja`), then set the environment variable `CMAKE_GENERATOR="Ninja"`

This project has been tested with
- `LLVM 9.0.0`
- `CMake 3.16.2`
- `Ninja 1.9.0`

### Download and build

```bash
$ git clone https://github.com/gharrma/lang.git
$ cd lang
$ ./build.sh
```

Then find the compiler executable at `out/langc`.

Note that the `build.sh` script just does the following.

```bash
$ cmake -B out # Configures a new build tree.
$ cmake --build out # Builds the compiler.
```

If you would like a release build, specify `-DCMAKE_BUILD_TYPE=Release` in that first `cmake` command above.


REPL
---

Invoking the compiler with no arguments launches the REPL, which compiles expressions as you type them. Use a semicolon to tell the REPL you are finished typing an expression. Here is an example.

```
$ ./run.sh
>>> fn f(a i32, b i32) = a + b;

[parse]
fn f(a i32, b i32) = (a + b)

[ir]
define void @f(i32 %a, i32 %b) {
entry:
  %plus = add i32 %a, %b
  ret void
}
```


Style Guide
---

For simplicity we use the [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html), though only loosely.


Contributing
---

Contributions are welcome! Make a pull request or reach out to me at gharrma@gmail.com, and we'll make something happen.
