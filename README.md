# slug
A (very early toy) bytecode assembler and interpreter.

As of today, it is not event turing complete.
For now, it doesn't do anything fancy except some very basic math.

## Build
slug uses a [rewrite](https://github.com/tsoding/musializer/blob/master/src/nob.h) of [tsoding's nobuild](https://github.com/tsoding/nobuild) as a build tool. 
```sh
cc -o nobuild nobuild.c
./nobuild     
```
```sh
# Available nobuild commands
./nobuild		# Build `slug`
./nobuild clean   	# Clean `slug`
./nobuild re      	# Clean and rebuild
```
There is no Windows port, but it should be pretty easy to port with [MinGW-w64](https://www.mingw-w64.org/).
