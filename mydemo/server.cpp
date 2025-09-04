#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <memory>
#include <thread>
#include <vector>
#include <atomic>

class SocketServer {
private:
    int server_fd;
    std::atomic<bool> running;
    std::vector<std::thread> client_threads;

    void handle_client(int client_socket) {
        char buffer[1024] = {0};
        
        while (running) {
            // 读取客户端数据
            ssize_t bytes_read = read(client_socket, buffer, sizeof(buffer));
            if (bytes_read <= 0) {
                std::cerr << "Client disconnected or error occurred" << std::endl;
                break;
            }

            std::cout << "Received: " << buffer << std::endl;

            // 回显数据给客户端
            send(client_socket, buffer, strlen(buffer), 0);
            
            // 清空缓冲区
            memset(buffer, 0, sizeof(buffer));
        }
        
        close(client_socket);
    }

public:
    SocketServer() : server_fd(-1), running(false) {}

    ~SocketServer() {
        stop();
    }

    bool start(int port) {
        // 创建socket文件描述符
        server_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd == -1) {
            std::cerr << "Socket creation failed" << std::endl;
            return false;
        }

        // 设置socket选项，避免地址占用错误
        int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
            std::cerr << "Set socket options failed" << std::endl;
            return false;
        }

        // 绑定socket到端口
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (sockaddr*)&address, sizeof(address)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            return false;
        }

        // 开始监听
        if (listen(server_fd, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            return false;
        }

        running = true;
        std::cout << "Server started on port " << port << std::endl;

        // 接受客户端连接
        while (running) {
            sockaddr_in client_addr{};
            socklen_t client_addr_len = sizeof(client_addr);
            
            int client_socket = accept(server_fd, (sockaddr*)&client_addr, &client_addr_len);
            if (client_socket < 0) {
                if (running) {
                    std::cerr << "Accept failed" << std::endl;
                }
                continue;
            }

            std::cout << "New client connected" << std::endl;

            // 为每个客户端创建新线程
            client_threads.emplace_back([this, client_socket]() {
                handle_client(client_socket);
            });
        }

        return true;
    }

    void stop() {
        running = false;
        
        // 关闭服务器socket
        if (server_fd != -1) {
            close(server_fd);
            server_fd = -1;
        }

        // 等待所有客户端线程结束
        for (auto& thread : client_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
        
        client_threads.clear();
        std::cout << "Server stopped" << std::endl;
    }
};

int main() {
    SocketServer server;
    
    // 启动服务器在端口8080
    if (!server.start(8080)) {
        std::cerr << "Failed to start server" << std::endl;
        return 1;
    }

    // 等待用户输入来停止服务器
    std::cout << "Press Enter to stop the server..." << std::endl;
    std::cin.get();
    
    server.stop();
    return 0;
}
