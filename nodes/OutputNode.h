#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>
#include <direct.h>


class OutputNode : public Node {
    std::string filename;
    std::string format; // jpg, png, bmp
    int jpgQuality; // for .jpg
    cv::Mat output;

public:
    OutputNode(const std::string& file, const std::string& fmt = "jpg", int quality = 95)
        : filename(file), format(fmt), jpgQuality(quality) {
        name = "OutputNode";
    }

    void process() override {
        std::cout << "OutputNode process called\n";
    
        if (inputs.empty()) {
            std::cerr << "No input connected!\n";
            return;
        }
    
        output = inputs[0]->getOutput();
        if (output.empty()) {
            std::cerr << "Empty image received from input node!\n";
            return;
        }
    
        std::vector<int> params;
    
        if (format == "jpg" || format == "jpeg") {
            params.push_back(cv::IMWRITE_JPEG_QUALITY);
            params.push_back(jpgQuality); // 0 to 100
        }
    
        std::string fullFilename = filename + "." + format;
    
        std::cout << "Attempting to save: " << fullFilename << std::endl;
    
        bool success = cv::imwrite(fullFilename, output, params);
        if (success) {
            std::cout << "[✔] Image saved to: " << fullFilename << std::endl;
        } else {
            std::cerr << "[✘] Failed to save image!" << std::endl;
        }
    }
    // OutputNode.h
    void save() {
        if (!output.empty()) {
            std::vector<int> params;
            if (format == "jpg" || format == "jpeg") {
                params.push_back(cv::IMWRITE_JPEG_QUALITY);
                params.push_back(jpgQuality); // Use jpgQuality instead of quality
            }
            cv::imwrite(filename + "." + format, output, params);  // Use filename, not name
        }
    }
    
    

    cv::Mat getOutput() override {
        return output;
    }

    void showPreview() {
        if (!output.empty()) {
            std::cout << "Previewing image: " << output.cols << "x" << output.rows << std::endl;
            cv::imshow("Output Preview", output);
            cv::waitKey(1);  // Allows processing of other events
        } else {
            std::cerr << "Error: Output image is empty!" << std::endl;
        }
    }
    

    void setFilename(const std::string& f) { filename = f; }
    void setFormat(const std::string& f) { format = f; }
    void setQuality(int q) { jpgQuality = q; }
};
