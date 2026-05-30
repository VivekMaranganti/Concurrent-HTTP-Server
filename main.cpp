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
    int client_fd = accept(fd, nullptr, nullptr);
    int buf_size = 1024;
    char buffer[buf_size];
    read(client_fd, buffer, buf_size);
    string response = "HTTP/1.1 200 OK\r\nContent-Length: 5\r\n\r\nHello";
    //Send HTTP response to client
    write(client_fd, response.c_str(), response.size());
    close(fd); close(client_fd);
}