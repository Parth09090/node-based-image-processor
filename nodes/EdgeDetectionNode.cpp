#include "EdgeDetectionNode.h"
#include <opencv2/imgproc.hpp>
#include <iostream>

EdgeDetectionNode::EdgeDetectionNode(Method method, int kernelSize, double thresh1, double thresh2, bool overlay)
    : method(method), kernelSize(kernelSize), threshold1(thresh1), threshold2(thresh2), overlayEdges(overlay) {}

void EdgeDetectionNode::setParameters(Method m, int kSize, double t1, double t2, bool overlay) {
    method = m;
    kernelSize = kSize;
    threshold1 = t1;
    threshold2 = t2;
    overlayEdges = overlay;
}

void EdgeDetectionNode::process() {
    if (inputs.empty() || !inputs[0]) {
        std::cerr << "EdgeDetectionNode: No input connected!\n";
        return;
    }

    inputs[0]->process();
    cv::Mat input = inputs[0]->getOutput();
    if (input.empty()) {
        std::cerr << "EdgeDetectionNode: Input image is empty!\n";
        return;
    }

    cv::Mat gray, edges;
    if (input.channels() == 3)
        cv::cvtColor(input, gray, cv::COLOR_BGR2GRAY);
    else
        gray = input.clone();

    if (method == CANNY) {
        cv::Canny(gray, edges, threshold1, threshold2, kernelSize);
    } else { // SOBEL
        cv::Mat gradX, gradY;
        cv::Sobel(gray, gradX, CV_16S, 1, 0, kernelSize);
        cv::Sobel(gray, gradY, CV_16S, 0, 1, kernelSize);
        cv::Mat absX, absY;
        cv::convertScaleAbs(gradX, absX);
        cv::convertScaleAbs(gradY, absY);
        cv::addWeighted(absX, 0.5, absY, 0.5, 0, edges);
    }

    if (overlayEdges) {
        cv::Mat colorEdges;
        cv::cvtColor(edges, colorEdges, cv::COLOR_GRAY2BGR);
        cv::Mat blended;
        cv::addWeighted(input, 0.8, colorEdges, 0.2, 0, blended);
        output = blended;
    } else {
        output = edges;
    }
}

cv::Mat EdgeDetectionNode::getOutput() {
    return output;
}
