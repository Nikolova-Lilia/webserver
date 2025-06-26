/*  What we’ll do:
Use only C++98 features (no C++11+ stuff like std::string::ends_with or auto)

Implement the socket lifecycle (socket(), bind(), listen(), accept(), etc.) as before

Parse HTTP requests manually with std::string and std::getline

Support serving static files: HTML, CSS, JSON, favicon, etc.

Properly detect MIME types (with a basic helper function)

Handle errors (404, 400, etc.)

Keep the code clean and modular to fit the 42 style

Avoid using STL containers that are not allowed (usually std::vector and std::string are fine in 42 C++98)
*/

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <csignal>
#include <map>

// Global server socket fd for cleanup
int g_server_fd = -1;

void sigint_handler(int signo) {
    std::cout << "\nClosing server...\n";
    if (g_server_fd != -1)
        close(g_server_fd);
    exit(0);
}

std::string get_mime_type(const std::string &filename) {
    // Basic MIME type mapping
    if (filename.size() >= 5 &&
        filename.compare(filename.size() -5, 5, ".html") == 0)
        return ("text/html");
    if (filename.size() >= 4 &&
        filename.compare(filename.size() - 4, 4, ".css") == 0)
        return ("text/css");
    if (filename.size() >= 3 &&
        filename.compare(filename.size() - 3, 3, ".js") == 0)
        return "application/javasctipt";
    if (filename.size() == 5 &&
        filename.compare(filename.size() - 5, 5, ".json") == 0)
        return "application/json";
    if (filename.size() == 4 &&
        filename.compare(filename.size() - 4, 4, ".ico") == 0)
        return ("image/x-icon");
    return "text/plain";  
}

int main() {
    signal(SIGINT, sigint_handler);

    // 1. Create socket
    g_server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (g_server_fd == -1) {
        perror("socket");
        return (1);
    }

    // 2. Allow address reuse
    int opt = 1;
    if (setsockopt(g_server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        return (1);
    }

    // 3. Bind
    struct sockaddr_in addr;
    std::memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    if (bind(g_server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return (1);
    }

    // 4. Listen
    if (listen(g_server_fd, 10) < 0) {
        perror("listen");
        return (1);
    }

    std::cout << "Server listening on port 8080\n";

    while (true) {
        // 5. Accept connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(g_server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        //6. Read HTTP request (simple read, no loop)
        char buffer[4096];
        int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read < 0) {
            perror("read");
            close(client_fd);
            continue;
        }
        buffer[bytes_read] = '\0';

        // 7. Parse request line (method, path, version)
        std::istringstream request_stream(buffer);
        std::string request_line;
        if (!std::getline(request_stream, request_line)) {
            // Bad request
            close(client_fd);
            continue;
        }

        std::istringstream line_stream(request_line);
        std::string method, path, version;
        line_stream >> method >> path >> version;

        // Only handle Get for now
        if (method != "GET") {
            // Send 501 Not Implemented
            std::string response = "HTTP/1.1 501 Not Implemented\r\n\r\n";
            send(client_fd, response.c_str(), response.size(), 0);
            close(client_fd);
            continue;
        }

        // Default to index.html if "/"
        std::string filename = (path == "/") ? "index.html" : path.substr(1);

        // Try to open file
        std::ifstream file(filename.c_str());
        if (!file.good()) {
            // 404 Not Found
            std::string not_found = "<h1>404 Not Found</h1>";
            std::ostringstream resp;
            resp << "HTTP/1.1 404 Not Found\r\n";
            resp << "Content-Type: text/html\r\n";
            resp << "Content-Length: " << not_found.size() << "\r\n";
            resp << "\r\n";
            resp << not_found;

            std::string response = resp.str();
            send(client_fd, response.c_str(), response.size(), 0);
            close(client_fd);
            continue;
        }

        // Read file content
        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());

        std::ostringstream resp;
        resp << "HTTP/1.1 200 OK\r\n";
        resp << "Content-Type: " << get_mime_type(filename) << "\r\n";
        resp << "Content-Length: " << content.size() << "\r\n";
        resp << "\r\n";
        resp << content;

        std::string response = resp.str();
        send(client_fd, response.c_str(), response.size(), 0);

        close(client_fd);
    }

    return (0);
}