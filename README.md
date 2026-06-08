# MyTerminal
## What is This?

This is a custom terminal that I am coding using C++ to get familiar with low-level systems and how terminals work under the hood.

I know C++ simplifies string input and many other features that make this an easy project. For that reason, I am avoiding strings and instead opting for char pointers and char arrays.

This change allows me to learn more about how to deal with memory and scale using dynamic arrays.

## Dev Log

### 5/28-5/31: 
- I spent the whole time trying to figure out how to input using C arrays and break down the sentence into words
- I created my custom `carr` (char-array) `struct` that I coded custom operator functionalities to behave like a char pointer:
  - `carr myvar = new char[1024]`
- I overwrote `[]`, `=`, `>>` (for cout)
- Inside of `carr` I created function `reset()`, `resize()`, `create()`
  - `reset()`: Deletes the memory location of the char pointer, and creates another with the same `n`
  - `resize(new_size)`: Creates a new array with the `new_size`, passes over each value from the previous char pointer to the new one, then deletes the memory location of the previous, and replaces it with the new one
  - `create(new_size)`: does updates the `n` with `new_size` then does `reset()`

### 5/31- 6/8:
- I decided to use more native system calls to get more familiar with how actual terminals navigate, so I am not going to use `std::filesystem`
- I am going to use `dirent.h` to navigate directories and `unistd.h` to get the current working directory
- Since using these native system calls only work in Unix systems and I have a Windows machine, I am using WSL to run the terminal.
- Created a `go` command to navigate directories similar to `cd` in bash, `here` to show the current directory even though it shows the path in the prompt.
- Decided to switch to using C for an absolute challenge and to get more familiar with how C works. Since the code is already C-like it wouldn't be like coding from scratch. And I would have absolute control over everything and how memory is handled.
- The reason to moving on with C-like structure is also because of how chaotic everything got with trying to use C++ and C features together. For example in carr the destructor helped with cleaning to a certain extent but becase of how structs pass by value it was destructing when the function came to an end creating issues with memory management and also free space errors.
- I also wanted to go all in if I am doing an application that is supposed ot be low-level.
