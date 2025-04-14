#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>

class BrightnessContrastNode : public Node {
    double alpha;  // Contrast (1.0 = no change)
    int beta;      // Brightness (0 = no change)
    cv::Mat output;

public:
    // Constructor with default contrast and brightness values
    BrightnessContrastNode(double a = 1.0, int b = 0) : alpha(a), beta(b) {
        name = "BrightnessContrast";
    }

    // Override process: adjust brightness and contrast
    void process() override {
        if (inputs.empty()) return;
        cv::Mat input = inputs[0]->getOutput();
        input.convertTo(output, -1, alpha, beta);  // Apply contrast and brightness
    }

    // Get the adjusted image
    cv::Mat getOutput() override {
        return output;
    }

    // Set parameters for brightness and contrast (optional)
    void setParameters(double a, int b) {
        alpha = a;
        beta = b;
    }
};
