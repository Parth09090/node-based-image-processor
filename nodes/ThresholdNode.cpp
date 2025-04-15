#include "ThresholdNode.h"
#include <iostream>
#include <opencv2/opencv.hpp>

ThresholdNode::ThresholdNode(double tValue, int method) 
    : thresholdValue(tValue), thresholdMethod(method) {}

void ThresholdNode::setParameters(double tValue, int method) {
    thresholdValue = tValue;
    thresholdMethod = method;
}

void ThresholdNode::showHistogram() {
    if (inputs.empty() || !inputs[0]) {
        std::cerr << "[ThresholdNode] No input connected!\n";
        return;
    }

    inputs[0]->process();
    cv::Mat inputImage = inputs[0]->getOutput();

    if (inputImage.empty()) {
        std::cerr << "[ThresholdNode] Input image is empty!\n";
        return;
    }

    // Calculate the histogram
    std::vector<cv::Mat> bgr_planes;
    cv::split(inputImage, bgr_planes);
    cv::Mat hist;
    int histSize = 256; // Number of bins
    float range[] = {0, 256};
    const float* histRange = {range};

    cv::calcHist(&bgr_planes[0], 1, 0, cv::Mat(), hist, 1, &histSize, &histRange);
    
    // Display histogram
    cv::Mat histImage(400, 512, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::normalize(hist, hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat());

    for (int i = 1; i < histSize; i++) {
        cv::line(histImage, 
            cv::Point((i - 1) * 2, histImage.rows - cvRound(hist.at<float>(i - 1))),
            cv::Point(i * 2, histImage.rows - cvRound(hist.at<float>(i))),
            cv::Scalar(255, 0, 0), 2, 8, 0);
    }

    cv::imshow("Histogram", histImage);
    cv::waitKey(0);  // Wait for key press to close histogram window
}

void ThresholdNode::process() {
    if (inputs.empty() || !inputs[0]) {
        std::cerr << "[ThresholdNode] No input connected!\n";
        output = cv::Mat();
        return;
    }

    inputs[0]->process();
    cv::Mat inputImage = inputs[0]->getOutput();

    if (inputImage.empty()) {
        std::cerr << "[ThresholdNode] Input image is empty!\n";
        output = cv::Mat();
        return;
    }

    // Thresholding based on method
    switch (thresholdMethod) {
        case BINARY:
            cv::threshold(inputImage, output, thresholdValue, 255, cv::THRESH_BINARY);
            break;
        case ADAPTIVE:
            cv::adaptiveThreshold(inputImage, output, 255, cv::ADAPTIVE_THRESH_MEAN_C, 
                cv::THRESH_BINARY, 11, 2);
            break;
        case OTSU:
            cv::threshold(inputImage, output, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
            break;
        default:
            std::cerr << "[ThresholdNode] Unknown threshold method\n";
            output = cv::Mat();
            break;
    }
}

cv::Mat ThresholdNode::getOutput() {
    return output;
}
