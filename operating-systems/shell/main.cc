#include <cstdlib>
#include <iostream>
#include <fcntl.h>
#include <string>
#include <vector>
#include <set>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;

class Command {
    public:
        string command;
        vector<string> args;
        string input_redirection_file;
        string output_redirection_file;
        bool is_input_redirection;
        bool is_output_redirection;

    void add_argument(string arg){
        this->args.push_back(arg);
    }

    void add_name(string name) {
        this->command = name;
    }

    bool has_arguments(){
        return this->args.size() == 0;
    }

    bool has_name() {
        return this->command != "";
    }

    void add_input_redirection(string filename){
        this->input_redirection_file = filename;
        this->is_input_redirection = true;
    }

    void add_output_redirection(string filename){
        this->output_redirection_file = filename;
        this->is_output_redirection = true;
    }

    string to_string(){
        string return_str = command;
        for (int i = 0; i < (int) this->args.size(); i++){
            return_str += " " + this->args[i];
        }
        if (this->is_input_redirection){
            return_str += " < " + this->input_redirection_file;
        }
        if (this->is_output_redirection){
            return_str += " > " + this->output_redirection_file;
        }
        return return_str;
    }

    Command(){
        command = "";
        is_input_redirection = false;
        is_output_redirection = false;
    }

    ~Command(){
        //free(& this->args);
    }
};

class Pipeline {
    public:
        vector<Command*> command_pointers; //Must stay in Pipeline Order! 
        
        Pipeline(vector<Command*> commands_to_run){
            command_pointers = commands_to_run;
        }

        Pipeline(){
        }
        ~Pipeline(){
            for (int i = 0; i < (int) command_pointers.size()-1; i++){
                delete(command_pointers[i]);
            }
        }
};

void input_redirection(Command* p_to_command, bool* p_to_file_err){
    if (! p_to_command->is_input_redirection) return;

    const char* filename = p_to_command->input_redirection_file.c_str();
    int fd = open(filename, O_RDONLY);
    if (fd < 0) *p_to_file_err = true;
    dup2(fd, STDIN_FILENO); //dup2(from, to). SO second argument will be same as first argument
    int stat = close(fd);
    if (stat == -1){
        cerr << p_to_command->command + " Error occured while closing File. Command: " +  p_to_command->to_string() << endl;
    } 
}

void output_redirection(Command* p_to_command, bool* p_to_file_err){
    if (! p_to_command->is_output_redirection) return;

    const char* filename = p_to_command->output_redirection_file.c_str();
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd < 0) *p_to_file_err = true;
    dup2(fd, STDOUT_FILENO); //STDOUT will be same file discriptor as fd
    int stat = close(fd);
    if (stat == -1) {
        cerr << p_to_command->command + " Error occured while closing file. Command: " +  p_to_command->to_string() << endl;
    }
}

string run_program(Command* p_to_command){
    pid_t pid = fork();
    if (pid < 0){
        cerr << "Error Forking (run_program). Command: " + p_to_command->to_string() << endl;
        return "";
    } else if (pid == 0){
        //In Child Process. Run Command
        //Redirections
        bool file_err = false;
        input_redirection(p_to_command, &file_err);
        output_redirection(p_to_command, &file_err);
        if (file_err) {
            cerr << p_to_command->command + " Error occured while opening file. Command: " +  p_to_command->to_string() << endl;
            exit(EXIT_FAILURE);
        }
        //Format Null terminating String and Null terminating array
        const char* prog_c_str = p_to_command->command.c_str();
        const char *args[100]; //MAX LENGTH OF LINE
        args[0] = prog_c_str;
        for (int i = 0; i < (int) p_to_command->args.size(); i++){
            args[i+1] = p_to_command->args[i].c_str();
        }
        args[(int) p_to_command->args.size()+1] = NULL;
        execv (prog_c_str,(char**) args);
        cerr << p_to_command->command + " Error in Child Process (run_program). Command: " +  p_to_command->to_string() << endl;
        return "";
    } else{
        //In Parent Process
        int status;
        waitpid(pid, &status, 0);
        if (status > 255) status -= 255;

        //format sucesess message
        string return_str = p_to_command->command + " exit status: ";
        return_str += to_string(status);
        return return_str; // Format: Command args exit status: XXX
    }
}

void run_pipeline(Pipeline* pipeline){
    const int PIPELINE_LENGTH = (int) pipeline->command_pointers.size();
    vector<pid_t> pids;

    //All pipeline file discriptors
    int input_fd = -1;
    int trailing_input_fd = -1;
    int trailing_write_fd = -1;

    for (int i = 0; i < PIPELINE_LENGTH; i++){
        Command* cur_command = pipeline->command_pointers[i];
        int pipe_fds[2];
        int pipe_read = -1;
        int pipe_write = -1;
        //Setup Pipe
        if (i != PIPELINE_LENGTH-1){ //No Pipeline from Z to nothing
            if(pipe(pipe_fds) < 0) {
                cerr << "Error Setting Up Pipe (run_pipeline). Command: " + cur_command->to_string() << endl;
                exit(EXIT_FAILURE);
            }
            pipe_read = pipe_fds[0];
            pipe_write = pipe_fds[1];
        }

        pid_t pid = fork();
        if (pid < 0){
            cerr << "Error Forking (run_pipeline). Command: " + cur_command->to_string() << endl;
            exit(EXIT_FAILURE);
        } else if (pid == 0){

            if (i != PIPELINE_LENGTH-1) dup2(pipe_write, STDOUT_FILENO); //Set write end of pipe
            if (i != 0) dup2(input_fd, STDIN_FILENO); //Set read end of pipe
            close(pipe_read); close(pipe_write);//KEEP THIS!!

            //In Child Process. Run Command
            //Redirections
            bool file_err = false;
            input_redirection(cur_command, &file_err);
            output_redirection(cur_command, &file_err);
            if (file_err) {
                cerr << cur_command->command + " Error occured while opening file. Command: " +  cur_command->to_string() << endl;
                exit(EXIT_FAILURE);
            }
            //Format Null terminating String and Null terminating array
            const char* prog_c_str = cur_command->command.c_str();
            const char *args[100]; //MAX LENGTH OF LINE
            args[0] = prog_c_str;
            for (int i = 0; i < (int) cur_command->args.size(); i++){
                args[i+1] = cur_command->args[i].c_str();
            }
            args[(int) cur_command->args.size()+1] = NULL;
            execv (prog_c_str,(char**) args);
            cerr << cur_command->command + " Error in Child Process (run_program). Command: " +  cur_command->to_string() << endl;
            exit(EXIT_FAILURE);
        } else{
            //In Parent Process
            pids.push_back(pid);

            trailing_input_fd = input_fd;
            input_fd = pipe_read;
            trailing_write_fd = pipe_write; //Setting allows pipe to stay open
    
            if (trailing_input_fd != STDIN_FILENO && trailing_input_fd != STDOUT_FILENO) close(trailing_input_fd);
            if (trailing_write_fd != STDIN_FILENO && trailing_write_fd != STDOUT_FILENO) close(trailing_write_fd);
            //could be extra
            if (i == PIPELINE_LENGTH-1 && pipe_write != STDIN_FILENO && pipe_write != STDOUT_FILENO) close(pipe_write);
            if (i == PIPELINE_LENGTH-1 && pipe_read != STDIN_FILENO && pipe_read != STDOUT_FILENO) close(pipe_read);
        }
    }

    //could be extra
    if (input_fd != STDIN_FILENO && input_fd != STDOUT_FILENO) close(input_fd);
    if (trailing_input_fd != STDIN_FILENO && trailing_input_fd != STDOUT_FILENO) close(trailing_input_fd);
    if (trailing_write_fd != STDIN_FILENO && trailing_write_fd != STDOUT_FILENO) close(trailing_write_fd);

    for(int i = 0; i < (int) pids.size(); i++){
        int status;
        waitpid(pids[i], &status, 0);
        if (status > 255) status -= 255;

        //format sucesess message
        string return_str = pipeline->command_pointers[i]->command + " exit status: ";
        return_str += to_string(status);
        cout << return_str << endl; // Format: Command args exit status: XXX
    }
    delete(pipeline);
}

//Return a command object if well formed. Return a NULL if malformed
Command* parse_command(string command){
    command += " "; //ensures no whitespace tokens
    //Setup Whitespace set
    std::set<char> whitespace_chars;
    whitespace_chars.insert(' ');
    whitespace_chars.insert('\f');
    whitespace_chars.insert('\n');
    whitespace_chars.insert('\r');
    whitespace_chars.insert('\t');
    whitespace_chars.insert('\v');

    Command* p_to_new_command = new Command();

    vector<string> tokens; //temp store all parsed tokens

    int token_length = 0;
    set<char>::iterator it;
    for(int i = 0; i < (int) command.length(); i++){
        it = whitespace_chars.find(command[i]);
        if (it != whitespace_chars.end()){
            //Is a whitespace char
            if (token_length != 0){
                //Valid Token
                string token = command.substr(i-token_length, token_length);
                tokens.push_back(token);
                token_length = 0;
            } else{
                //Invalid Token
                continue;
            }
        }
        else {
            //Is a char in a token
            token_length++;
        }
    }

    //Create command obj from tokens
    for(int i = 0; i < (int) tokens.size(); i++){
        string cur_token = tokens[i];
        if (cur_token == "<"){
            //input redirection

            //check redirection not already implimented and can be implimented
            if(p_to_new_command->is_input_redirection || i == (int) tokens.size()-1 
            || tokens[i+1] == "<" || tokens[i+1] == ">") { //cannot redirect to redirect
                delete(p_to_new_command);
                return NULL;
            }
            i++; //next argument is file
            string file = tokens[i];
            p_to_new_command->add_input_redirection(file);
        } else if (cur_token == ">"){
            //Output redirection

            //check redirection not already implimented and can be implimented
            if(p_to_new_command->is_output_redirection || i == (int) tokens.size()-1 
            || tokens[i+1] == "<" || tokens[i+1] == ">") {
                delete(p_to_new_command);
                return NULL;
            }

            i++; //next argument is file
            string file = tokens[i];
            p_to_new_command->add_output_redirection(file);
        } else if (p_to_new_command->has_name()){
            //Add arg
            p_to_new_command->add_argument(cur_token);
        } else {
            //Add Command name
            p_to_new_command->add_name(cur_token);
        }
    }
    //if new command has a name it is well formed
    if (p_to_new_command->has_name()) {
        return p_to_new_command;
    } else {
        delete(p_to_new_command);
        return NULL;
    }
}

//Return True if line contains a pipeline. False if just 1 command
bool parse_line(Command* p_to_command, vector<Command*>* p_to_command_list, bool* p_to_is_malformed, string command){
    set<char> pipe_char;
    pipe_char.insert('|');
    int index_of_last_pipe = 0;
    bool is_pipe = false;
    set<char>::iterator it;
    for(int i = 0; i < (int) command.length(); i++){
        it = pipe_char.find(command[i]);
        if (it != pipe_char.end()){
            //Is a pipe char
            is_pipe = true;
            p_to_command = NULL;
            Command* p_to_new_command = parse_command(command.substr(index_of_last_pipe, i-index_of_last_pipe));
            if (p_to_new_command == NULL) {
                *p_to_is_malformed = true;
                delete(p_to_new_command);
                return true;
            } else{
                p_to_command_list->push_back(p_to_new_command);
            }
            index_of_last_pipe = ++i;
        }
    }

    Command* p_to_new_command = parse_command(command.substr(index_of_last_pipe, command.length()-index_of_last_pipe));

    if (p_to_new_command == NULL) {
        *p_to_is_malformed = true;
    }
    else if (index_of_last_pipe == 0) {
        //No pipelines
        *p_to_command = *p_to_new_command;
        delete(p_to_new_command);
    } else {
        //At least 1 pipeline
        p_to_command_list->push_back(p_to_new_command);
    }
    return is_pipe;
}

void parse_and_run_command(string command) {
    Command* command_to_run = new Command();
    Pipeline* p_to_pipeline = new Pipeline();
    bool is_malformed = false;
    bool has_pipeline = parse_line(command_to_run, &p_to_pipeline->command_pointers, &is_malformed, command);

    //todo Handle how to proceed after parsing
    if (command_to_run->command == "exit") {
        exit(0);
    }
    else if (is_malformed) {
        cerr << "Invalid command.\n";
    }
    else if (has_pipeline){
        //Handle setting up pipelines and running commands in pipelines
        run_pipeline(p_to_pipeline);
    } else {
        //Run a single command
        cout << run_program(command_to_run) << endl;
    }
    //todo deallocate memeory
    delete(command_to_run);
}

int main(void) {
    string command;
    cout << "> ";
    while (getline(cin, command)) {
        parse_and_run_command(command);
        cout << "> ";
    }
    return 0;
}
