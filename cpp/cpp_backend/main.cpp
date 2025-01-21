#include <iostream>
#include <cstudio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <features.h>
#include <unistd.h> // For pipe, fork, and exec
#include <sys/wait.h> // For waitpd

int main() {
    // Pipes for communication: C++ -> Python and Python -> C++
    int pipe_in[2];   // Pipe for input to python
    int pipe_out[2];   // Pipe for output from python

    // Create pipes
    if (pipe(pipe_in) == -1 || pipe(pipe_out) == -1) {
        std::cer << "Failed to create pipes." << std::endl;
        return 1;
    }

    // Fork the process to c create a child for python script
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork process" :: std::endl;
        return 1;
    }

    /* If fork return value (pid) is 0, it is child process.
     * The child process handles communication with the python script
     */
    if (pid == 0) {
        
        // Redirect stdin and stdout to the pipes
        dup2(pipe_in[0], STDIN_FILENO);    // Redirect stdin to pipe_in
        dup2(pipe_out[1], STDOUT_FILENO);  // Redirect stdout to pipe_out (write end)

        // Close pipes as they are no longer needed
        close (pipe_in[1]);
        close (pipe_out[0]);

        // Execute the python script
        execlp("python3", "python3", "python_tools/ai_handler.py", NULL);

        // If execlp fails
        perror("Failed to start python script");
        exit(1);
    }
    /* Parent process. Communicates with python script */
    else {
        // Close unused pipe ends in the parent process
        close(pipe_in[0]);
        close (pipe_out[1]);

        // Send JSON data to the Python script
        std::string json_input = R"({"message": "Hello from C++!"})";
        write(pipe_in[1], json_input.c_str(), json_input.size());
        write(pipe_in[1], "\n", 1); // Add newline to signal end of input

        // Read response from Python
        char buffer[256];
        ssize_t bytes_read = read(pipe_out[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0'; // Null terminate
            std::cout << "Python response: " << buffer << std::endl;
        }

        // Wait for the child process to finish 
        int status;
        waitpid(pid, &status, 0);
    }

    return 0;
}
