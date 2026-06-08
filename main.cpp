#include <iostream>
#include <cstring>
#include <unistd.h>
#include <dirent.h>

// prototypes
void throw_error(const char error_type[], const char error_msg[]);

// For convenience this struct stores the size of the array and some other methods
// Acts like a vector in c++ but more simplified and still low-level
struct carr
{
private:
    char *arr;
    int n;

public:
    // defaults to empty and nullptr
    carr(){
        n = 0;
        arr = nullptr;
    }

    carr(int length){
        n = length;
        arr = new char[n];
    }

    // Destructor to prevent any memory leaks because of the array
    ~carr(){
        delete [] arr;
    }

    // Overwriting the [] to function as a usual *char
    char& operator[](int i){
        if(i < 0 || i >= n){
            throw std::out_of_range("The index is out of range: 000");
        }
        
        return arr[i];
    }

    
    // Gives read-only access for constants
    const char& operator[](int i) const{
        if(i < 0 || i >= n){
            throw std::out_of_range("The index is out of range: 001");
        }
        
        return arr[i];
    }

    carr& operator=(const char input[]){
        n = strlen(input) + 1;
        
        delete [] arr;

        strcpy(arr, input);

        return *this;
    }
    
    carr& operator=(char* ptr){
        delete [] arr;

        n = strlen(ptr);
        arr = new char[n];

        for(int i=0; i<strlen(ptr); ++i){
            arr[i] = ptr[i];
        }

        return *this;
    }

    carr& operator=(const carr& other){
        delete [] arr;

        n = other.n;
        create(n);
        if(other.arr != nullptr){
            for(int i=0; i<n; ++i){
                arr[i] = other[i];
            }
        }
        else{
            arr = nullptr;
        }

        return *this;
    }

    // Handles implicit conversions
    operator char*(){
        return arr;
    }
    operator const char*(){
        return arr;
    }

    // Handles the cout statements
    friend std::ostream& operator<<(std::ostream& os, const carr& obj){
        if(obj.arr != nullptr){
            os << obj.arr;
        }
        else{
            os << "[nullptr]";
        }
        return os;
    }
    
    // Only used for expanding the array, new_size > n
    // Returns whether the resizing worked as expected
    bool resize(int new_size){
        if(new_size < n){
            // throw_error("arr", "Tried to resize an array to a smaller size");
            return false;
        }
        
        if(new_size == n){
            return true;
        }

        char *new_arr = new char[new_size];

        for(int i=0; i<n; ++i){
            new_arr[i] = arr[i];
        }
        
        delete [] arr;
        
        arr = new_arr;
        n = new_size;
        
        return true;
    }

    void reset(){
        delete [] arr;
        arr = new char[n];
    }

    void create(int new_size){
        n = new_size;
        reset();
    }
    
    int size(){
        return n;
    }

    char* get_arr(){
        return arr;
    }

    void destroy(){
        delete [] arr;
        n = 0;
        arr = nullptr;
    }
};

// Prototypes
bool seperate_commands(carr &text, carr cmd[], int& n_cmd);
bool run_commands(carr cmd[], int n_cmd);
carr find_current_loc();

int main(){
    std::cout << "Welcome to the terminal\n";
    std::cout << "Type \"exit\" or \"0\" to exit the terminal\n";
    carr text(1024);
    carr cmd[1024];
    int n_cmd=0;
    
    
    do{
        cmd[0].resize(1024);
        n_cmd = 0;

        std::cout << "coolshell: " << find_current_loc() << "> ";
        
        // To leave space for '\0' needs to input one less
        std::cin.getline(text, 1023);

        // Note to self, can use .get to also have \n which might allow me to detect if exceeding the char limit or not
        
        if(strlen(text) == 0) continue;

        seperate_commands(text, cmd, n_cmd);

        // std::cout << static_cast<char*>(cmd[0]);
        
        run_commands(cmd, n_cmd);

        for(int i=0; i<n_cmd; ++i){
            // This is where I want to put the removal fors
            std::cout << "i= " << i << ", cmd[i]= " << (cmd[i]) << "\n";
            cmd[i].destroy();
        }

    } while(strcmp(text, "exit") && strcmp(text, "0"));

    return 0;
}

/*
* Returns: bool
    * true: successfully seperated
    * false: unsuccessful operation
*/
bool seperate_commands(carr &text, carr cmd[], int& n_cmd){
    int i=0, cmd_row=0, cmd_col=0;
    while(text[i] != '\0' && i<1024){
        // Error checks for safety
        if(cmd_col >= 1024){
            throw_error("Out of bounds", "the lenght of a single command exceeds 1024 chars");
            return false;
        }
        if(cmd_row >= 1024){
            throw_error("Out of bounds", "the lenght of number of commands exceeds 1024 chars"); 
            return false;
        }
        if(i >= 1024){
            throw_error("Out of bounds", "the lenght of the entire line exceeds 1024 chars, or missing '\\0'");
            return false;
        }
        
        // Seperates by spaces
        if(text[i] == ' '){
            // The reason for this additional condition check is to disregard double spaces as a single space 
            // so it doesn't impact the command seperation
            if(cmd_col != 0){
                cmd[cmd_row][cmd_col] = '\0'; // End of a char array 
                cmd_row +=1;
                cmd_col = 0;

                // cmd[cmd_row] = new char[1024];
                
                cmd[cmd_row].create(1024);
                n_cmd+=1;
            }
        }
        else{
            cmd[cmd_row][cmd_col] = text[i];
            cmd_col+=1;
        }

        i+=1;
    }

    n_cmd +=1;

    cmd[cmd_row][cmd_col] = '\0'; // End of a char array 


    return true;
}

// An internal error system that doesn't end the program
// Only use for errors that will be handled internally
void throw_error(const char error_type[], const char error_msg[]){
    std::cerr << "A(n) " << error_type << " error occured in terminal:\n";
    std::cerr << "\t" << error_msg << "\n";
}

carr find_current_loc(){
    carr abs_path;
    abs_path.create(1024);

    while(getcwd(abs_path, abs_path.size()) == nullptr && abs_path.size() < 3e4){
        // doubles the size of buffer if it doesn't fit the leght of abs_path
        // hard stop at 3e4 as a safety
        abs_path.create(abs_path.size()*2);
    }

    return abs_path;
}

void change_dir(carr& new_dir){
    // const carr curr = find_current_loc();
    if(strcmp(new_dir.get_arr(), "out") == 0){
        chdir("..");
    }
    else{
        chdir(new_dir.get_arr());
    }

    int i=0;
}
int carr_to_int(carr &carr_var, int def){
    int res = 0, size = 0;

    while(carr_var[size] != '\0') size += 1;

    for(int i=0; i<size; ++i){
        if(carr_var[i] <= '9' && carr_var[i] >= '0'){
            res += (carr_var[i] - '0') * (size - i);
        }
        else{
            throw_error("wrong type", "entered a non-numerical value");
            return def;
        }
    }

    return res;
}

void list_content(carr content[], carr& target_dir, int N){
    DIR* dir = opendir(target_dir);

    // target_dir = (target_dir.get_arr() == nullptr) ? "." : target_dir; 

    struct dirent* entry;
    
    int i=0;
    while(i<N && (entry = readdir(dir)) != nullptr){
        
        
        // Hides the hidden files
        if(entry->d_name[0] == '.'){
            i-=1;
        }
        else{
            content[i] = entry->d_name;
            // char * empty = "\0";
            // strcat(content[i], empty);
        }
        i+=1;
    }

    closedir(dir);
}

bool run_commands(carr cmd[], int n_cmd){
    if(strcmp(cmd[0], "here") == 0){
        std::cout << find_current_loc() << "\n";
    }
    else if(strcmp(cmd[0], "go") == 0){
        change_dir(cmd[1]);
    }
    else if(strcmp(cmd[0], "show") == 0){
        const int N = (n_cmd < 2) ? 100: carr_to_int(cmd[2], 100);
        carr content[N];
        carr empty_dir(2);
        empty_dir[0] = '.'; empty_dir[1] = '\0';

        // list_content(content, (n_cmd < 1) ? empty_dir:cmd[1], N);
        list_content(content, empty_dir, 100);
        for(int i=0; i<N && content[i].get_arr() != nullptr; ++i){
            std::cout << content[i] << "\n";
        }
    }
    return false;
}
