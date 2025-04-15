#pragma once
#include "Node.h"
#include "opencv2/opencv.hpp"
#include <vector>

class ColorChannelSplitterNode : public Node {
private:
    std::vector<cv::Mat> channels; // To store the split channels
    bool grayscaleOutput;          // Whether to output grayscale versions of the channels or not

public:
    // Constructor: initializes with the option to output grayscale
    ColorChannelSplitterNode(bool grayscale = true)
        : grayscaleOutput(grayscale) {
        name = "ColorChannelSplitter";
    }

    // Setter for the grayscale output option
    void setGrayscaleOutput(bool value) {
        grayscaleOutput = value;
    }

    // Getter for the grayscale output option
    bool getGrayscaleOutput() const {
        return grayscaleOutput;
    }

    // Process method: splits the input image into channels and optionally normalizes to grayscale
    void process() override {
        if (inputs.empty() || !inputs[0]) {
            std::cerr << "[!] Splitter: No input!\n";
            return;
        }

        cv::Mat input = inputs[0]->getOutput();
        if (input.empty()) {
            std::cerr << "[!] Splitter: Empty input!\n";
            return;
        }

        cv::split(input, channels);  // Split the input into individual channels

        // If grayscale output is selected, normalize each channel to grayscale
        if (grayscaleOutput) {
            for (auto& ch : channels) {
                cv::normalize(ch, ch, 0, 255, cv::NORM_MINMAX);
            }
        }
    }

    // Access individual channel (by index)
    cv::Mat getChannel(int idx) {
        if (idx >= 0 && idx < channels.size()) {
            return channels[idx];
        }
        return cv::Mat();
    }

    // Override getOutput to return the first channel (default)
    cv::Mat getOutput() override {
        if (grayscaleOutput) {
            return getChannel(0);  // Default grayscale channel
        } else {
            if (channels.size() == 3) {
                cv::Mat merged;
                cv::merge(channels, merged);
                return merged;  // Return full color image
            }
            return cv::Mat();
        }
    }
    
};
