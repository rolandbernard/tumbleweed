
<img src="logo.svg" width="48"/> The Tumbleweed Programming Language
===================================
This is the source code repository for the Tumbleweed compiler.

## About

Tumbleweed is a strongly typed programming language.
Just like tumbleweed this programming language is useless and unwanted. 

## Installing from source
```
$ git clone https://github.com/rolandbernard/tumbleweed
$ cd tumbleweed
$ make install
```

## Hello world
The "Hello world"-program for this language looks like this:
```
extern printf(fmt: *i8, ..) i32;

main() i32 {
    printf("Hello world!\n");
    return 0;
}
```
