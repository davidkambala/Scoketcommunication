/*
To compile run: g++ feedback.cpp -o feedback `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
*/
#include <opencv4/opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>

int main() {
    // Open the video file instead of the camera
    cv::VideoCapture cap("/home/david/Videos/conversation.mp4");  // Replace with your video file path
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file" << std::endl;
        return -1;
    }

    // Create a TCP socket
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(5000);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return -1;
    }

    while (true) {
        cv::Mat frame;
        // Read a frame from the video file; if it fails, break out of the loop
        if (!cap.read(frame)) {
            break;
        }

        // Encode the frame as JPEG
        std::vector<uchar> buf;
        cv::imencode(".jpg", frame, buf);
        int size = buf.size();

        // First, send the size of the frame
        send(sock, &size, sizeof(int), 0);
        // Then send the actual frame data
        send(sock, buf.data(), size, 0);

        // Receive the size of the JSON response (4 bytes)
        int json_size;
        if (recv(sock, &json_size, sizeof(int), 0) <= 0) {
            std::cerr << "Failed to receive JSON size" << std::endl;
            break;
        }

        // Receive the actual JSON response
        std::vector<char> json_data(json_size);
        int received = 0;
        while (received < json_size) {
            int bytes = recv(sock, json_data.data() + received, json_size - received, 0);
            if (bytes <= 0) {
                std::cerr << "Failed to receive JSON data" << std::endl;
                break;
            }
            received += bytes;
        }

        // Convert to string and print
        std::string json_response(json_data.begin(), json_data.end());
        std::cout << "Received JSON: " << json_response << std::endl;
    }

    close(sock);
    return 0;
}
