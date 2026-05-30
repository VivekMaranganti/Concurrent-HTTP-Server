#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;
int main() {
    //Create TCP socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    
    //Create and define the port address
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(8080);

    //Link the port number to the socket
    if (::bind(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        cout << "bind failed" << endl;
        return 1;
    }

    //Wait for, then process requests from the client
    listen(fd, 10);
    //Accept loop
    while (true) {
        //Accept client request and read buffer
        int client_fd = accept(fd, nullptr, nullptr);
        int buf_size = 1024;
        char buffer[buf_size];
        read(client_fd, buffer, buf_size);

        //Take first line to isolate method and path
        string request(buffer);
        string first_line = request.substr(0, request.find("\r\n"));

        //Get the method
        size_t methodEnd = first_line.find(" ");
        string method = first_line.substr(0, methodEnd);

        //Get the path
        size_t pathStart = methodEnd+1;
        size_t pathEndLen = first_line.find(" ", pathStart)-(pathStart);
        string path = first_line.substr(pathStart, pathEndLen);

        //Select response based on path
        string response;
        if (path == "/") {
            response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
        } else {
            response = "HTTP/1.1 404 Not Found\r\nContent-Length: 9\r\n\r\nNot Found";
        }

        //Send HTTP response to client
        write(client_fd, response.c_str(), response.size());
        close(client_fd);
    }
    close(fd);
}