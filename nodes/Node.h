#pragma once
#include "opencv2/opencv.hpp"
#include <vector>
#include <string>
using namespace std;

class Node{
    public :
        string name;
        vector<Node*> inputs;

        // Virtual function: must be implemented by derived (child) classes
        virtual void process() = 0;

        // Virtual function to get the result of this node
        virtual cv::Mat getOutput() = 0;
    
        // Virtual destructor for safe cleanup
        virtual ~Node() = default;
};
