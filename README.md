# MyTerminal
## What is This?
This is a basic terminal application that I am coding to get familiar with low-level systesms and how terminals work. I started coding in C++ but later switched to C for more control and familiarity with the language. Below you can find a Dev Log where I included my development process and roadblocks. There is also future development plans where I listed some features I want to add. You can find all the commands I coded until now in `COMMANDS.md`

Here is a very basic demo where I try out some of the commands here:
[Watch the demo](https://youtu.be/1ABnPnfbyJc)

## Tech Stack
- C++
  - I was limitting myself to only primary features but since I switched to C, I don't know if this is applicable, since my current version doesn't use c++
- C
  - The programming language I am using to code the terminal
- WSL
  - I am using Windows which I heard causes trouble when trying to navigate directories with C since it is not a Unix system. So WSL allows me to get around that without having to switch to a linux machine.

## System Requirements and Installation
### System req
- OS: Windows 11 (10 also should work, but not tested)
- Environment: Powershell, Windows Terminal, Command Prompt
- Dep: none

### First run
- Extract the `.zip` file that is in the release
- Double click on the `.exe` file, when the blue screen pops up, click **More info** then **Run Anyway**. This should pop up a terminal that would allow you to try out.

## AI use
- This project doesn't have code that is directly written by AI
- Through the creation of this project AI is only used to search concepts and specific methods for me to code as I am not that experienced with C. Think of it like using AI instead of Stack Overflow.

## Motivation
I have recently gotten curious about low level systems and how terminals work. So I decided to code my own terminal application to get familiar with these concepts. I also see this as a challenge since this is the first time I am working with this much memory management.

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
- To implement allowing regular bash commands that I did not implemented I added `fork()` to copy the current process and `exec()` to run the command. One challenge I ran into, though, is with waiting for the child process to finish. Since Windows doesn't have `waitpid()` function I will try to implement a similar logic in my `#if`, `#endif` structure. 
- I had to a lot of digging and found that I can use `WaitForSingleObject()` to wait for child process in windows, however when I use `fork()` I get a pid number which I cannot directly plug into `WaitForSingleObject()`, then I found `OpenProcess()` which takes pid and gives an handle that I can plug into. After gluing bunch of code together that I found online I was able to get it working.
- I realized I was wrong with my previous comment, and `fork()` also doesn't exist in windows, so I had to do more digging. Then I figured with the scope of this project this addition would be too much work, so I will only give support for running regular bash commands in linux/mac. In order to do this i had to create a macro for the entire `run_ext_command()` function.
- I also added a `history` command to show all previously written commands in the terminal, I am using a global variable to keep track of the history, which may not be the best approach but it works.
- To be able to have a good demo I used `emscripten` to convert the c code to `.wasm`, but didn't work as expected

### 7/6-7/12:
- Implemented a new error printing format including: type of error, the function/location the error occured in and error mmessage. This way I am aiming to have cleaner and easier to debug errors.
- I added reading the contents of files usign read command, I made it into a command that stores the read values so that in future I can implement searching for specific words and other features directly using `read_file()` function.
- Read command also prints the contents of the file chosen.
- I was trying to overload my `print_err()` function, but then I just learned it doesn't work in C. There are some alternative ways, but I will probably have a different name for each variation.
- When trying to implement searching for a keyword, I figured 2 options. One if to search within a file (using `read_file()` I previously created), and searching directly in the input. Since searching in the terminal input would require quotation marks ("") which is not currently supported, I will implement it once I change my input system.
- My algorithm for finding the searched keyword is to go through each line and try to match the word char by char and abort each time it fails and moves to the next char as the start point.
- I learned that it is possible to color the output stream which would allow to print searched keyword to be in different color. Just an idea, might be developed in future

### 7/28-7/29
- The search command was not working over and over again and I was confused. After I did some search on better debugging methods, I took the notes that are in [./Notes.md](./Notes.md). With that debugging method I was able to find my for loop trying to access 1025th element of a size 1024 `found_lines` array. And the reason was my program searching for "\0" to stop the for loop but I forgot to add that to my last used element. So it was comparint it to garbage data.
- The use of these debugging methods also allowed to cover memory leaks that I had from day 1. I found a memory leak in my `text` varible that is the data from the console.
- I spent great amount time fixing all my memory leaks thanks to the debugging flags I added that showed bunch of leaks. I had a leak where I forgot a 1024 sentence each 1024 chars which is rougly 1MB in memory.
- The previous point also made me realize that I should not directly capture that much space in the memory and rather dynamically allocate space as I need. This will require some in depth changes. Maybe rewriting half the code but necessery to keep the development going.
- Something that makes my code actually great is using pointers for the char arrays of each sentence. This way instead of capturing a sequential memory space, each sentence can fit in to the holes in the memory. I don't know if this is factually true, just FYI.
- There is a lot to improve. But according to debugging there are no more memory leaks, I tried every command that I yet coded.
- I realized that using a linked list for the `history` would remove the limit on how many commands that can be stored, as each command can just be added as a node at the start. But for now I will just clear the memory leak that `valgrind` found
- I used valgrind and did a full test, I found 3MB of memory leak which I fixed them all

### 7/29-
- Decided to change the `carr.n` and the newly added `carr.capacity` to be in size_t, for better storage for size. They don't have a negative sizing, and adaptive to the bit system of the computer.
- Added `carr.capacity` to hold the capacity as outlines previously.
- I changed my `carr_copy(carr c1, carr c2)` logic to work even if the text pased down is too big. I made it so if the c2.used is too large for c1.capacity, then I delete c1 and then reallocate space to be c2.used.
- Addded `carr_list` struct to know when a sentence is not allocated yet, and more dynamically allocate new sentences. This way would allow us to manage memory better.
- Added `carr_copy_char` to standarize carr operations and add extra protection. This was also needed to automatically manage `.used` and `.capacity`
- As the usage of `carr` grew and `carr_list` was added, I decided to seperate them from rest of the code for better organization purposes. Which led me to creating `carr.h` and `carr.c`.
- I realized, I was already doing something similar to how `carr_list.used` would work with different variables. This makes the transition smoothers as well as clear coding usage.
- Since, in a sense, I am rebuilding my whole code base I decided to change my way of doing some of these things. For example now history's capacity doubles if it reaches its cap, and each command stored in history has the exact capacity of the information that they store. This could easily bring down memory from 100x1024bytes to something around 100x50bytes which is a great improvement. (50 is the approximate char count in a command)
- I learned that I can actually add colors to my terminal using some escape characters, this has great potential in the future, for now I will sprinkle it to a few places in my code just for fun.
- Added a `Makefile` to make running code easier, especially the new `carr.c` and `carr.h` requires extra steps or very long code to run. The Makefile is taken from an online resource and slightly modified to fulfill my needs.


## Future development plans
- [x] Allowing other misc commands to be directed to `exec()` which would run any command that is possible to run in a regular bash, mostly useful for running stuff like `python file.py` or compile a file with `gcc` (it might be too much to hand-code all, i would rather focus on more unique features)
- [x] Implementing child processes for certain commands to protect the main process
- [ ] Adding bunch of error protection and edge cases as the code is NOT SAFE at all. I just disregarded all the error handling for the sake of development speed.
- [x] Adding a feature to read files and show their contents
- [x] Adding a feature to search for keywords in files (similar to `grep` in bash)
- [ ] Adding a feature to delete files an dirs
- [ ] Change the content of files (tricky, since I need to have some kind of text editor in the terminal)
- [x] A `history` command to show previous commands
- [ ] up/down arrow keys showing previous commands (need to implement `history` first)
- [ ] Adding `<` and `>` for input and output redirection, I likely need to change teh current way commands are implemented which is a lot of work, but it would be cool
- [ ] Adding `|` for piping commands, same issue as the previous one
- [ ] Adding `&` for running commands in the background, same issue as the previous one, likely can be done with `fixed()` and `exec()` though
- [ ] Adding color to the terminal output, like when `search` is used it can color the keyword
- [ ] instead of using `get_line()`, process each input as they are typed ot have functionality such as up or down arrow key copying previous input. Very fundamental change, and might be needed to add certain features
- [ ] Implement a linked list for `history`