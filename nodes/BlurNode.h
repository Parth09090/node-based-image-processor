#pragma once
#include "Node.h"
#include <opencv2/opencv.hpp>

class BlurNode : public Node {
private:
    int radius;
    bool directional;
    cv::Mat output;

public:
    BlurNode(int r = 5, bool dir = false);

    void setParameters(int r, bool dir);
    void showKernelPreview();
    void process() override;
    cv::Mat getOutput() override;
};
