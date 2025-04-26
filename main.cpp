#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <netinet/in.h>

const int PORT = 8080;

std::string readFile(const std::string &path) {
    std::ifstream file("." + path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

std::string getContentType(const std::string &path) {
    if (path.ends_with(".html")) return "text/html";
    return "text/plain";
}

void handleClient(int clientSocket) {
    char buffer[4096] = {0};
    read(clientSocket, buffer, 4096);
    std::string request(buffer);

    std::istringstream iss(request);
    std::string method, path, protocol;
    iss >> method >> path >> protocol;

    std::string filePath = "." + (path == "/" ? "/index.html" : path);
    std::string content = readFile(filePath);
    std::string statusLine;
    std::string body;

    if (!content.empty()) {
        statusLine = "HTTP/1.1 200 OK\r\n";
        body = content;
    } else {
        body = readFile("./404.html");
        statusLine = "HTTP/1.1 404 Not Found\r\n";
    }

    std::string headers =
        "Content-Length: " + std::to_string(body.size()) + "\r\n" +
        "Content-Type: " + getContentType(filePath) + "\r\n" +
        "Connection: close\r\n\r\n";

    std::string response = statusLine + headers + body;
    send(clientSocket, response.c_str(), response.size(), 0);
    close(clientSocket);
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 10);

    std::cout << "Server is running on port " << PORT << std::endl;

    while (true) {
        socklen_t addrlen = sizeof(address);
        int client_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        std::thread(handleClient, client_socket).detach();
    }

    close(server_fd);
    return 0;
}
