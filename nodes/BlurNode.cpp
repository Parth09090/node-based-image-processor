#include "BlurNode.h"
#include <iostream>
#include <algorithm>

BlurNode::BlurNode(int r, bool dir) : radius(r), directional(dir) {}

void BlurNode::setParameters(int r, bool dir) {
    radius = std::clamp(r, 1, 20);
    directional = dir;
}

cv::Mat BlurNode::getOutput() {
    return output;
}

void BlurNode::showKernelPreview() {
    int ksize = 2 * radius + 1;
    cv::Mat kernelX = cv::getGaussianKernel(ksize, -1, CV_32F);

    if (!directional) {
        cv::Mat kernelY = kernelX.clone();
        cv::Mat kernel2D = kernelX * kernelY.t();
        std::cout << "2D Gaussian Kernel (Uniform):\n" << kernel2D << std::endl;
    } else {
        std::cout << "1D Gaussian Kernel (Directional):\n" << kernelX << std::endl;
    }
}

void BlurNode::process() {
    if (inputs.empty() || !inputs[0]) {
        std::cerr << "[BlurNode] No input connected!\n";
        output = cv::Mat();
        return;
    }

    inputs[0]->process();
    cv::Mat inputImage = inputs[0]->getOutput();

    if (inputImage.empty()) {
        std::cerr << "[BlurNode] Input image is empty!\n";
        output = cv::Mat();
        return;
    }

    int ksize = 2 * radius + 1;
    cv::Mat result;

    if (directional) {
        cv::GaussianBlur(inputImage, result, cv::Size(ksize, 1), 0);
    } else {
        cv::GaussianBlur(inputImage, result, cv::Size(ksize, ksize), 0);
    }

    output = result;
}
