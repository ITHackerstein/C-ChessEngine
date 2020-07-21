# Chess Engine
---

**DISCLAIMER**: This chess engine is built from a person that has absolutely no knowledge about chess engines or chess strategies and things like that. This was only an approach to a new topic that I wanted to explore.

**TL;DR**: This chess engine probably sucks, so don't expect anything from it.

---

The chess engine is built on SDL2 to provide simple rendering.

## Building

To compile the project you have to download the SDL2 library and put everything except the DLLs in a new folder called *libsdl*, then take the DLLs and put them in a new folder called *build*. Once you have done that you should be able to compile it by running:

```
$ make
```

OR

```
$ make run
```
that will automatically run the executable, located at `build/main.exe`.
