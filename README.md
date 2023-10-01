# slug
A (very early toy) bytecode assembler and interpreter.

As of today, it is not event turing complete.
For now, it doesn't do anything fancy except some very basic math.

It is highly **un**optimized.

## Build
slug uses a [rewrite](https://github.com/tsoding/musializer/blob/master/src/nob.h) of [tsoding's nobuild](https://github.com/tsoding/nobuild) as a build tool. 
```sh
$ cc -o nobuild nobuild.c
$ ./nobuild     
```
```sh
# Available nobuild commands
$ ./nobuild		# Build `slug`
$ ./nobuild clean   	# Clean `slug`
$ ./nobuild re      	# Clean and rebuild
```
~~There is no Windows port, but it should be pretty easy to port with [MinGW-w64](https://www.mingw-w64.org/).~~
It also work on windows, you just need to have `gcc` available

## How to use
```sh
$ ./slug -c `file.sl`     # Compile `file.sl` to `file.slx` bytecode assembly
$ ./slug -x `file.slx`	# Execute `file.slx`
```
Or, one-liner, if you don't need the assembly:
```sh
$ ./slug -r `file.sl`     # Compile and run `file.sl` without saving a bytecode assembly file
```
For more command, you can always just display the usage:
```sh
$ ./slug
Usage:
    [...]
```

Expect lot of crashes and errors.

## Tests
Requires python3
```sh
$ cd tests
$ python3 test.py
```
Ultimately, I plan to add tests to nobuild, as I want all the toolchain to be C, but for now this will do just fine.