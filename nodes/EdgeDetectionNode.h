#pragma once
#include "Node.h"
#include <opencv2/imgproc.hpp>

class EdgeDetectionNode : public Node {
public:
    enum Method {
        SOBEL,
        CANNY
    };

    EdgeDetectionNode(Method method = CANNY, int kernelSize = 3, double thresh1 = 100, double thresh2 = 200, bool overlay = false);

    void setParameters(Method method, int kernelSize, double thresh1, double thresh2, bool overlay);
    void process() override;
    cv::Mat getOutput() override;

private:
    Method method;
    int kernelSize;
    double threshold1;
    double threshold2;
    bool overlayEdges;

    cv::Mat output;
};
