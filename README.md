# MyTerminal
## What is This?


## Tech Stack
- C++
  - I was limitting myself to only primary features but since I switched to C, I don't know if this is applicable
- C
  - The programming language I am using to code the terminal
- WSL
  - I am using Windows which I heard causes trouble when trying to navigate directories with C since it is not a Unix system. So WSL allows me to get around that without having to switch to a linux machine.

## Motivation
I have recently gotten curious about low level systems and 

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

### 6/8-6/10:
- I thought about making this terminal more of a game, so that people can navigate through the directories, open files and search through things. For now this is just an idea but would be cool to see how it turns out.
- I learned that actual terminals use child processes to ensure that the main terminal process doesn't crash if something goes wrong. So I also want to imlement that in my terminal.
- I was able to implement `show` command back with C, it is a very similar code I just had to play around with how I can manage memory with C.
- I also completed `show` command with the option to specify target directory and number of items to show.
- I attempted to implement the child process with `fork()` but reached an interesting roadblock. When I use `fork()` the function occurs in the child process meaning when I use `go` command it only moves the child process to the new directory but the parent process is still the original directory. So the next `show` command show the contents of the original directory rather than the newly moved one. To get around this I can use `fork()` only for certain commands, though I am worried about the scalibility of this solution, I decided to put a halt on this idea for now and will likely come back again.

### 6/10-6/11:
- Added a new command called `create` to create files and directories. Unlike the standard approach in back where you use `mkdir` for directories and `touch` for files, I wanted to make it simple and sound so I combined them into one command. But whether to create a `file` or `dir` is determined by the first parameter.
- I implemented the file creation which was pretty straightforward using `fopen()`, which also allows to whether to overwrite the file if it already exists.
- When working with the directory creation, I ran into another roadblock. Apparently, creating directory differs between windows and linux/mac, so after doing some research I found that I can check whether the program is running on windows or linux/mac and use the appropriate calls.
- To get around the previously mentioned issue I used `#if` and `#endif` preprocessors, these are checked before the compilor runs. Normally in C the compilar compiles the code for both options (both if and else) but since one side is not suitable for os it will break since it doesn't know what that function is. But with preprocessors only the code that is suitable for os will run and the other will be ignored so it won't break. I used this to set their function of _mkdir() for windows and mkdir() for linux/mac to my `create_dir()` function which works great.

## Future development plans
- [ ] Allowing other misc commands to be directed to `exec()` which would run any command that is possible to run in a regular bash, mostly useful for running stuff like `python file.py` or compile a file with `gcc` (it might be too much to hand-code all, i would rather focus on more unique features)
- [ ] Implementing child processes for certain commands to protect the main process
- [ ] Adding bunch of error protection and edge cases as the code is NOT SAFE at all. I just disregarded all the error handling for the sake of development speed.
- [ ] Adding a feature to read files and show their contents
- [ ] Adding a feature to search for keywords in files (similar to `grep` in bash)
- [ ] Adding a feature to delete files an dirs
- [ ] Change the content of files (tricky, since I need to have some kind of text editor in the terminal)
- [ ] A `history` command to show previous commands
- [ ] up/down arrow keys showing previous commands (need to implement `history` first)
- [ ] Adding `<` and `>` for input and output redirection, I likely need to change teh current way commands are implemented which is a lot of work, but it would be cool
- [ ] Adding `|` for piping commands, same issue as the previous one
- [ ] Adding `&` for running commands in the background, same issue as the previous one, likely can be done with `fixed()` and `exec()` though