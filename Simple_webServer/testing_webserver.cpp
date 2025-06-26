/*   Simplified version C++ using system calls like socket(), bind(), etc.  

    ✅ Summary: The Socket Lifecycle
    Step	Function	Description
    1	    socket()	Create a socket
    2	    bind()	    Bind it to an IP/port
    3	    listen()	Start listening for connections
    4	    accept()	Accept an incoming client
    5	    read()	    Read the HTTP request
    6	    send()	    Send the HTTP response
    7	    close()	    Close the socket


    Start server:
    Create a socket
    Bind it to a port (e.g., 8080)
    Listen for incoming connections

Loop:
    Accept a connection
    Read the HTTP request from the client
    Process the request:
        - Look for the requested file (like index.html)
        - If found, prepare a "200 OK" response with the file content
        - If not found, prepare a "404 Not Found" response
    Send the HTTP response back to the client
    Close the connection
    */

#include <iostream>
#include <sys/socket.h> // For socket(), bind(), listen(), accept()
#include <netinet/in.h> // For sockaddr_in
#include <unistd.h>     // For close, read(), write()
#include <cstring>      // For strlen()
#include <csignal>      // For signal()
#include <fstream>      // For file handling
#include <sstream>      // For stringstream

int server_fd; // global so it can be accessed in a signal handler

void handle_sigint(int sig) {
    std::cout << "\nCaught signal " << sig << ", exiting...\n";
    close(server_fd); // close the server socket
    exit(0);
}

int main() {
    signal(SIGINT, handle_sigint); // catch CTRL+C
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0); // 1. Create socket
    if (server_fd == -1) {
        perror("socket failed");
        return (1);
    }

    // Allow reuse of address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        return (1);
    }

    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Listen on all network interfaces
    address.sin_port = htons(8080); // Use port 8080

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // 2. Bind socket to port
    if (bind(server_fd,(struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        return (1);
    }

    // 3. Start listening
    if (listen(server_fd, 10) < 0) {
        perror("listen failed");
        return (1);
    }

    std::cout << "Server is listening on port 8080...\n";

    while (true) {
        int addrlen = sizeof(address);
        int client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen); // 4. Accept connection

        if (client_fd < 0) {
            perror("accept failed");
            continue;
        }

        char buffer[3000] = {0};
        read(client_fd, buffer, 3000); // 5. Read HTTP request
        std::cout << "Received request:\n" << buffer << std::endl;

        // Parse the requested path
        std::string request(buffer);
        std::istringstream request_stream(request);
        std::string method, path, version;
        request_stream >> method >> path >> version;

        std:: string filename;

        if (path == "/") {
            filename = "index.html";
        } else {
            // Remove leading
            filename = path.substr(1);
        }

        std::ifstream file(filename); // Try opening the file
        std::stringstream response;

        if (file.good()) {
            std::string content((std::istreambuf_iterator<char>(file)), 
                                std::istreambuf_iterator<char>());
            
            response << "HTTP/1.1 200 OK\r\n";
            response << "Content-Type: text/html\r\n";
            response << "Content-Length: " << content.size() << "\r\n";
            response << "\r\n";
            response << content;
        } else {
            std::string not_found = "<h1>404 Not Found</h1>";
            response << "HTTP/1.1 404 Not Found\r\n";
            response << "Content-type: text/html\r\n";
            response << "Content-Length: " << not_found.size() << "\r\n";
            response << "\r\n";
            response << not_found;
        }

        std::string final_response = response.str();
        send(client_fd, final_response.c_str(), final_response.size(), 0);
        close(client_fd); // close connection

        /*
        // 6. Send HTTP response
        const char* response = 
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, world!";

            send(client_fd, response, strlen(response), 0);

            close(client_fd); // 7. Close connection
            */
    }

    return (0);
}