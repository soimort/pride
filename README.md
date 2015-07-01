# Pride

[![Rainbow](https://github.com/soimort/pride/raw/gh-pages/images/rainbow.png)](http://www.soimort.org/pride)

## What?

**Pride** is a lightweight **unit testing framework** as well as a simple **logging library** for C.

It is:

* *[Header-only](https://en.wikipedia.org/wiki/Header-only)*, like MinUnit and tinytest (so that it can be easily reused and compiled with your source)
* *Standalone* (no other dependency than the C standard library)
* *Multi-threaded* using [POSIX Threads](https://en.wikipedia.org/wiki/POSIX_Threads) (so that threading will enable faster testing)
* *Colorful* (if you are on an ANSI terminal)

## Why?

Finding a well-maintained lightweight C library for testing and colorful logging was not easy. And it's never too hard to write one from scratch with pride.

`pride.h` is a self-contained header file, conformed to the C99 standard, and released into the public domain.

## How?

Prerequisites:

* a C99-compliant compiler
* a C library that implements POSIX Threads (pthreads), e.g.,  GNU C Library (glibc)
