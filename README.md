# alog
A logging library written in ANSI C with (a)synchronous logging support.

Intended to have minimal dependencies and function cross-platform 
(Mac, Windows, Linux, POSIX).

### Current Feature Set
Synchronous logging to:
- `stdout`
- `stderr`
- Arbitrary `FILE` stream
- Arbitrary static filepath
- Custom callbacks

Log severity levels and filtering

Option to automatically add new line characters to the end of logs

Global & user-managed configuration support

### Future Feature Set (ETA: soon™)

- CMake support
- Additional test cases and CI
- Asynchronous logging (using pthreads / winthreads)
- Error handling
- ANSI color support
- Tutorials & examples beyond the test cases
- Log file rotation
- Log timestamps
- Logging to dynamic filepath
- Additional built-in callbacks such as syslog
- Import configuration from files

### Usage

Simply add alog.h and alog.c to your project. 
See the `test` directory for some usage examples.

### Background

I did not originally intend to write a logging library. However,
while working on another project that I wanted to add robust logging to, 
I failed to find an alternative library that was both a simple C library
while also having first-class support for both POSIX and Windows simulatenously.

Admittedly, I did not search that long, but I decided that I
wanted to get some practice working with the compiler using C89. And so 
(alog)ging library was born.

I made this public as the code might be useful to others, but know that I will
probably only make updates and add features as I feel the need for them in any
other projects that I'm working on.

### License

If for whatever reason you'd like to use this in one of your own projects,
this code is licensed under the MIT license, which is quite permissive. See
[LICENSE](LICENSE) for details. 