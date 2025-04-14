#pragma once
#include "Node.h"
#include "opencv2/opencv.hpp"  // For cv::Mat

class ImageInputNode : public Node {
private:
    cv::Mat image;  // The image data

public:
    // Constructor: takes the file path and loads the image
    ImageInputNode(const std::string& filename) {
        name = "ImageInput";
        loadImage(filename);  // Load image from disk
    }

    // Reload the image from the specified filename
    void reload(const std::string& filename) {
        loadImage(filename);
    }

    // Helper method to load the image from the given filename
    void loadImage(const std::string& filename) {
        image = cv::imread(filename);  // Load image from disk
        if (image.empty()) {
            std::cerr << "Error: Unable to load image at " << filename << std::endl;
        }
    }

    // Override process, but we don't need to do anything here (input node just loads)
    void process() override {
        // No processing needed for this node
    }

    // Get the image (output of the node)
    cv::Mat getOutput() override {
        return image;
    }

    // Get the current filename of the loaded image (if needed in the GUI)
    std::string getFilename() const {
        return image.empty() ? "No image loaded" : "Image Loaded";
    }
};
