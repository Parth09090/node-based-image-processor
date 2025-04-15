#include <iostream>
#include <opencv2/opencv.hpp>

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#include "../nodes/ImageInputNode.h"
#include "../nodes/BrightnessContrastNode.h"
#include "../nodes/OutputNode.h"
#include "../nodes/ColorChannelSplitterNode.h"
#include "../nodes/BlurNode.h"
#include "../nodes/ThresholdNode.h"
#include "../nodes/EdgeDetectionNode.h" // ✅ Edge Detection Node
#include "../GraphEngine.h"

// GUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>
#include <unordered_set>

// OpenGL Texture
GLuint matToTexture(const cv::Mat& mat) {
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    GLint format = mat.channels() == 3 ? GL_BGR : GL_LUMINANCE;

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mat.cols, mat.rows, 0, format, GL_UNSIGNED_BYTE, mat.data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return textureID;
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Node Editor GUI", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    // Load initial node graph
    ImageInputNode* inputNode = new ImageInputNode("images.jpg");
    BrightnessContrastNode* bcNode = new BrightnessContrastNode(1.0, 0);
    bcNode->inputs.push_back(inputNode);

    BlurNode* blurNode = new BlurNode(5, false);
    blurNode->inputs.push_back(bcNode);

    ThresholdNode* thresholdNode = new ThresholdNode(128, ThresholdNode::BINARY);
    thresholdNode->inputs.push_back(blurNode);

    EdgeDetectionNode* edgeNode = new EdgeDetectionNode(EdgeDetectionNode::SOBEL); // ✅
    edgeNode->inputs.push_back(thresholdNode); // ✅

    ColorChannelSplitterNode* splitter = new ColorChannelSplitterNode(true);
    splitter->inputs.push_back(thresholdNode); // optional: could be edgeNode

    OutputNode* outputFull = new OutputNode("output_full", "jpg", 90);
    outputFull->inputs.push_back(edgeNode); // ✅

    OutputNode* outputChannel = new OutputNode("output_channel", "jpg", 90);
    outputChannel->inputs.push_back(splitter);

    GraphEngine engine;

    // UI State
    float brightness = 0.0f;
    float contrast = 1.0f;
    bool useChannelOutput = false;
    int blurRadius = 5;
    bool directionalBlur = false;
    float thresholdValue = 128.0f;
    int thresholdMethod = ThresholdNode::BINARY;

    int edgeMethod = EdgeDetectionNode::SOBEL;
    int sobelKernelSize = 3;
    float cannyThreshold1 = 100.0f, cannyThreshold2 = 200.0f;
    bool overlayEdges = false;

    GLuint texID = 0;
    cv::Mat processed;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // === Node Control Panel ===
        ImGui::Begin("Node Control");
            
        ImGui::SliderFloat("Brightness", &brightness, -100.0f, 100.0f);
        ImGui::SliderFloat("Contrast", &contrast, 0.0f, 3.0f);
        ImGui::Checkbox("Show Channel Output", &useChannelOutput);
            
        // Threshold Controls
        ImGui::SliderFloat("Threshold Value", &thresholdValue, 0.0f, 255.0f);
        const char* thresholdMethods[] = { "Binary", "Adaptive", "Otsu" };
        if (ImGui::Combo("Threshold Method", &thresholdMethod, thresholdMethods, IM_ARRAYSIZE(thresholdMethods))) {
            thresholdNode->setParameters(thresholdValue, thresholdMethod);
        }
        
        // Edge Detection Controls
        const char* edgeMethods[] = { "Sobel", "Canny" };
        if (ImGui::Combo("Edge Method", &edgeMethod, edgeMethods, IM_ARRAYSIZE(edgeMethods))) {
            edgeNode->setParameters(static_cast<EdgeDetectionNode::Method>(edgeMethod), // Cast to enum
                                    sobelKernelSize, cannyThreshold1, cannyThreshold2, overlayEdges);
        }

        // Ensure Sobel kernel size is odd and <= 31
        if (edgeMethod == EdgeDetectionNode::SOBEL) {
            if (sobelKernelSize % 2 == 0) {
                sobelKernelSize++;  // Make it odd if it's even
            }
            if (sobelKernelSize > 31) {
                sobelKernelSize = 31;  // Limit the kernel size to 31
            }
            ImGui::SliderInt("Sobel Kernel Size", &sobelKernelSize, 1, 7);
        } else {
            ImGui::SliderFloat("Canny Threshold 1", &cannyThreshold1, 0.0f, 500.0f);
            ImGui::SliderFloat("Canny Threshold 2", &cannyThreshold2, 0.0f, 500.0f);
        }
        ImGui::Checkbox("Overlay Edges", &overlayEdges);
        
        if (ImGui::Button("Process Image")) {
            bcNode->setParameters(contrast, brightness);
            blurNode->setParameters(blurRadius, directionalBlur);
            thresholdNode->setParameters(thresholdValue, thresholdMethod);
            edgeNode->setParameters(static_cast<EdgeDetectionNode::Method>(edgeMethod), // Cast to enum
                                    sobelKernelSize, cannyThreshold1, cannyThreshold2, overlayEdges);
        
            std::unordered_set<Node*> visited;
        
            if (useChannelOutput) {
                engine.execute(outputChannel, visited);
                processed = outputChannel->getOutput();
            } else {
                engine.execute(outputFull, visited);
                processed = outputFull->getOutput();
            }
        
            if (!processed.empty()) {
                texID = matToTexture(processed);
                if (useChannelOutput)
                    outputChannel->showPreview();
                else
                    outputFull->showPreview();
            }
        }
        
        if (ImGui::Button("Save Output")) {
            if (useChannelOutput)
                outputChannel->save();
            else
                outputFull->save();
        }
        
        ImGui::End();

        // === Image Input Node UI ===
        ImGui::Begin("📷 Image Input");
        if (ImGui::Button("Reload Image")) {
            inputNode->reload(inputNode->getFilename().c_str());
        }
        ImGui::Text("Loaded: %s", inputNode->getFilename().c_str());
        ImGui::End();

        // === Brightness & Contrast Node UI ===
        ImGui::Begin("🌓 Brightness & Contrast");
        ImGui::SliderFloat("Brightness", &brightness, -100.0f, 100.0f);
        ImGui::SliderFloat("Contrast", &contrast, 0.0f, 3.0f);
        ImGui::End();

        // === 🌀 Blur Node UI ===
        ImGui::Begin("🌀 Blur Node");
        ImGui::SliderInt("Radius", &blurRadius, 1, 20);
        ImGui::Checkbox("Directional (Horizontal Only)", &directionalBlur);
        if (ImGui::Button("Preview Kernel")) {
            blurNode->setParameters(blurRadius, directionalBlur);
            blurNode->showKernelPreview();
        }
        ImGui::End();

        // === 🔲 Threshold Node UI ===
        ImGui::Begin("🔲 Threshold Node");
        ImGui::SliderFloat("Threshold Value", &thresholdValue, 0.0f, 255.0f);
        if (ImGui::Combo("Threshold Method", &thresholdMethod, thresholdMethods, IM_ARRAYSIZE(thresholdMethods))) {
            thresholdNode->setParameters(thresholdValue, thresholdMethod);
        }
        ImGui::End();

        // === 🪞 Edge Detection Node UI ===
        ImGui::Begin("🪞 Edge Detection Node");
        if (ImGui::Combo("Edge Method", &edgeMethod, edgeMethods, IM_ARRAYSIZE(edgeMethods))) {
            edgeNode->setParameters(static_cast<EdgeDetectionNode::Method>(edgeMethod), // Cast to enum
                                    sobelKernelSize, cannyThreshold1, cannyThreshold2, overlayEdges);
        }
        if (edgeMethod == EdgeDetectionNode::SOBEL) {
            ImGui::SliderInt("Sobel Kernel Size", &sobelKernelSize, 1, 7);
        } else {
            ImGui::SliderFloat("Canny Threshold 1", &cannyThreshold1, 0.0f, 500.0f);
            ImGui::SliderFloat("Canny Threshold 2", &cannyThreshold2, 0.0f, 500.0f);
        }
        ImGui::Checkbox("Overlay Edges", &overlayEdges);
        ImGui::End();
        
        // === 🎨 Channel Splitter UI ===
        ImGui::Begin("🎨 Channel Splitter");
        ImGui::Text("Outputs RGB/RGBA channels as grayscale");
        bool grayscale = splitter->getGrayscaleOutput();
        if (ImGui::Checkbox("Grayscale Output", &grayscale)) {
            splitter->setGrayscaleOutput(grayscale);
        }
        ImGui::End();

        // === 💾 Output Node UI ===
        ImGui::Begin("💾 Output");
        if (ImGui::Button("Process Image")) {
            bcNode->setParameters(contrast, brightness);
            blurNode->setParameters(blurRadius, directionalBlur);
            thresholdNode->setParameters(thresholdValue, thresholdMethod);
            edgeNode->setParameters(static_cast<EdgeDetectionNode::Method>(edgeMethod), // Cast to enum
                                    sobelKernelSize, cannyThreshold1, cannyThreshold2, overlayEdges);

            std::unordered_set<Node*> visited;
            if (useChannelOutput)
                engine.execute(outputChannel, visited);
            else
                engine.execute(outputFull, visited);
        }
        ImGui::End();

        // === Render and Swap Buffers ===
        glClear(GL_COLOR_BUFFER_BIT);
        if (texID != 0) {
            glBindTexture(GL_TEXTURE_2D, texID);
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex2f(-1.0f, -1.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex2f(1.0f, -1.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex2f(1.0f, 1.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex2f(-1.0f, 1.0f);
            glEnd();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
