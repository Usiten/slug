# slug
A (very early toy) bytecode assembler and interpreter.

As of today, it is not event turing complete.
For now, it doesn't do anything fancy except some very basic math.

It is highly **un**optimized, and leaks memory all over the place.

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
There is no Windows port, but it should be pretty easy to port with [MinGW-w64](https://www.mingw-w64.org/).

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
    -c, --compile  `file.sl` [`out.slx`]   Compile `file.sl` to bytecode assembly `file.slx` (or `out.slx` if provided)
    -x, --execute  `file.slx`              Execute bytecode assembly `file.slx`
    -r, --run  `file.sl`                   Compile and execute `file.sl` without generating bytecode assembly
```

Expect lot of crashes and errors.