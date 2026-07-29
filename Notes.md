These are mostly that I wrote for myself, but I am going to make it public if anyone wants to check them out.

## Debugging and memory sec
- Use valgrind
    - This is for when running the program
    - use `valgrind --leak-check=full --track-origins=yes ./terminal`
- Add `-fsanitize=address,undefined -g`
    - address: This insta crashes when invalid memory access
    - undefined: catches NULL pointer reference
- Compiler flags: `-Wall -Wextra`
- Core dump debug
    - `ulimit -c unlimited` to the terminal
    - compile with `-g -O0`
    - after crash load to gdb with `coredumpctl debug`

### When to use
- Compiler flags -> every build
- Address sanitizer -> active testing
- Debugging (like red dots) -> logic breaking
- Core dump debug -> Segmentation fault
- Valgrind -> finalizing the feature

### The whole command to run

`gcc -Wall -Wextra -Wpedantic -g -fsanitize=address,undefined terminal.c -o terminal && ./terminal`

when using valgrind

`gcc -Wall -Wextra -g terminal.c -o terminal && valgrind ./terminal`
