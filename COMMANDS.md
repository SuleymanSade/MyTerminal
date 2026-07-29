NAVIGATION:
here --> The current location, you are here
go --> Move to a different directory
- param1: the name of the directory to move to, use `out` to move out of the current directory
show --> Shows the contents of the current directory
- param1(optional):  the name of the directory show the contents of, if not specified current directory (`.`)
- param2(optional): number of items to show, if not specified default to 100


CREATION:
create / cr --> Creates a file or directory
- param1: `dir` or `folder` or `directory` for directory, `file` for file
- param2: the name of the file or directory to create
- param3(optional): whether to overwrite file if the file already exists, `yes` or `no`, default to `no`, only used for file creation not for directory

FILE READ/WRITE:
read --> Prints the contents of a file
- param1: the name of the file
search --> Searches a specific phrase in a file
- param1: search phrase
- param2: "in"
- param3: the file name

MISC:
history --> Shows all the previous commands that have been inputted in terminal, numbered
exit / 0 --> Exits the terminal

BASH:
Any other command that is normally supported by console should work, in theory. But it only works for linux devices.