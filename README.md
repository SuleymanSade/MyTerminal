# MyTerminal
## What is This?

This is a custom terminal that I am coding using C++ to get familiar with low-level systems and how terminals work under the hood.

I know C++ simplifies string input and many other features that make this an easy project. For that reason, I am avoiding strings and instead opting for char pointers and char arrays.

This change allows me to learn more about how to deal with memory and scale using dynamic arrays.

## Dev Log

### 5/28-5/13: 
- I spent the whole time trying to figure out how to input using C arrays and break down the sentence into words
- I created my custom `carr` (char-array) `struct` that I coded custom operator functionalities to behave like a char pointer:
  - `carr myvar = new char[1024]`
- I overwrote `[]`, `=`, `>>` (for cout)
- Inside of `carr` I created function `reset()`, `resize()`, `recreate()`
  - `reset()`: Deletes the memory location of the char pointer, and creates another with the same `n`
  - `resize(new_size)`: Creates a new array with the `new_size`, passes over each value from the previous char pointer to the new one, then deletes the memory location of the previous, and replaces it with the new one
  - `recreate(new_size)`: does updates the `n` with `new_size` then does `reset()`
