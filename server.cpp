#include "server.h"
#include "eval/eval.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <thread>

using namespace std; // Convenience

string performCalculation(string input){
    auto result = evaluate(input);
    return to_string(result);
}

const char LINE_DELIMITER = '\n';

void clientConnectionLoop(int connection){
    char buffer[1024]; // Connection buffer
    string stringBuffer; // Holding data independently of transport

    while(auto bytesRead = read(connection, buffer, sizeof(buffer))){
        // Append to independent buffer
        stringBuffer += string(buffer,0,bytesRead);
        list<string> lines;

        // Split to lines
        int start = 0, end = 0;
        while ((start = stringBuffer.find_first_not_of(LINE_DELIMITER, end)) != string::npos) {
            end = stringBuffer.find(LINE_DELIMITER, start);
            lines.push_back(stringBuffer.substr(start, end - start));
        }
        // Check if last line is possibly incomplete
        bool transferLastToNextBuffer = buffer[bytesRead-1] != '\n';

        if(transferLastToNextBuffer) {
            // last line is incomplete, transfer buffer to next input processing
            stringBuffer = lines.back();
            // do not execute
            lines.pop_back();

        }else {
            // All data ends with newline should be executed, no buffer transferred
           stringBuffer.clear();
        }
        // Perform calculation on complete lines
        for(auto line: lines) {
            string calculationResult =  performCalculation(line);
            send(connection, calculationResult.c_str(), calculationResult.size(), 0);
        }
    }
    close(connection);
}

int serverLoop() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0) {
        std::cout << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    int setsockopt_flag = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                   &setsockopt_flag, sizeof(int)) < 0){
        std::cout << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in sockaddr;
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(1234);

    if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
        std::cout << "Failed to bind to port 1234. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(sockfd, 10) < 0) {
        std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    while (true) {
        // Grab a connection from the queue
        auto addrlen = sizeof(sockaddr);
        int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
        if (connection < 0) {
            std::cout << "Failed to attach connection errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        // Create new thread per client connected
        std::thread new_thread(clientConnectionLoop,connection);

        // Now the thread is responsible for freeing resources
        // see https://en.cppreference.com/w/cpp/thread/thread/detach
        new_thread.detach();
    }
    // close(sockfd);
}