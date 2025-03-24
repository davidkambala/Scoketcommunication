/*
To compile run: g++ webcam.cpp -o client `pkg-config --cflags opencv4` `pkg-config --libs opencv4`
*/
#include <opencv4/opencv2/opencv.hpp>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>
#include <iostream>

int main() {
    // Open the default webcam (device 0) for live inference
    cv::VideoCapture cap(0);
    if (!cap.isOpened()) {
        std::cerr << "Error opening webcam" << std::endl;
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
        // Capture a frame from the webcam
        if (!cap.read(frame)) {
            std::cerr << "Failed to capture frame" << std::endl;
            break;
        }

        // Encode the frame as JPEG
        std::vector<uchar> buf;
        cv::imencode(".jpg", frame, buf);
        int size = buf.size();

        // First, send the size of the frame
        if (send(sock, &size, sizeof(int), 0) < 0) {
            std::cerr << "Error sending frame size" << std::endl;
            break;
        }
        // Then send the actual frame data
        if (send(sock, buf.data(), size, 0) < 0) {
            std::cerr << "Error sending frame data" << std::endl;
            break;
        }

        // Optionally display the frame locally (for debugging/inference)
        cv::imshow("Webcam", frame);
        // Exit loop if 'q' is pressed
        if (cv::waitKey(1) == 'q') {
            break;
        }
    }

    close(sock);
    return 0;
}
