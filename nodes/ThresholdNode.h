#ifndef THRESHOLDNODE_H
#define THRESHOLDNODE_H

#include "Node.h"
#include <opencv2/opencv.hpp>

class ThresholdNode : public Node {
public:
    ThresholdNode(double tValue = 128, int method = BINARY);  // Default to BINARY
    void setParameters(double tValue, int method);
    void showHistogram();
    void process() override;
    cv::Mat getOutput() override;

    // Constants to represent different thresholding methods
    static const int BINARY = 0;
    static const int ADAPTIVE = 1;
    static const int OTSU = 2;

private:
    double thresholdValue;
    int thresholdMethod;
    cv::Mat output;
};

#endif
