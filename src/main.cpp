#include <iostream>
#include <opencv2/opencv.hpp>

#ifndef GL_BGR
#define GL_BGR 0x80E0
#endif

#include "../nodes/ImageInputNode.h"
#include "../nodes/BrightnessContrastNode.h"
#include "../nodes/OutputNode.h"
#include "../nodes/ColorChannelSplitterNode.h" // Include the new node
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

    ColorChannelSplitterNode* splitter = new ColorChannelSplitterNode(true);
    splitter->inputs.push_back(bcNode);

    OutputNode* outputFull = new OutputNode("output_full", "jpg", 90);
    outputFull->inputs.push_back(bcNode);

    OutputNode* outputChannel = new OutputNode("output_channel", "jpg", 90);
    outputChannel->inputs.push_back(splitter);

    GraphEngine engine;

    // UI State
    float brightness = 0.0f;
    float contrast = 1.0f;
    bool useChannelOutput = false;

    GLuint texID = 0;
    cv::Mat processed;

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // === Node Control Panel ===
        ImGui::Begin("Node Control");

        ImGui::SliderFloat("Brightness", &brightness, -100.0f, 100.0f);
        ImGui::SliderFloat("Contrast", &contrast, 0.0f, 3.0f);
        ImGui::Checkbox("Show Channel Output", &useChannelOutput);

        if (ImGui::Button("Process Image")) {
            bcNode->setParameters(contrast, brightness);
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
            inputNode->reload(inputNode->getFilename().c_str());  // You can implement reload or re-assign logic
        }
        ImGui::Text("Loaded: %s", inputNode->getFilename().c_str());
        ImGui::End();

        // === Brightness & Contrast Node UI ===
        ImGui::Begin("🌓 Brightness & Contrast");
        ImGui::SliderFloat("Brightness", &brightness, -100.0f, 100.0f);
        ImGui::SliderFloat("Contrast", &contrast, 0.0f, 3.0f);
        ImGui::End();

        // === Color Channel Splitter Node UI ===
        ImGui::Begin("🎨 Channel Splitter");
        ImGui::Text("Outputs RGB/RGBA channels as grayscale");
        // Display checkbox to enable/disable grayscale output
        bool grayscale = splitter->getGrayscaleOutput();  // Get the current state of grayscaleOutput
        if (ImGui::Checkbox("Grayscale Output", &grayscale)) {  // Check if checkbox is toggled
            splitter->setGrayscaleOutput(grayscale);  // Update the node with the new value
        }
        ImGui::End();

        // === Output Node UI ===
        ImGui::Begin("💾 Output");
        if (ImGui::Button("Process Image")) {
            bcNode->setParameters(contrast, brightness);

            std::unordered_set<Node*> visited;
            if (useChannelOutput)
                engine.execute(outputChannel, visited);
            else
                engine.execute(outputFull, visited);

            processed = (useChannelOutput) ? outputChannel->getOutput() : outputFull->getOutput();

            if (!processed.empty()) {
                texID = matToTexture(processed);
                (useChannelOutput ? outputChannel : outputFull)->showPreview();
            }
        }

        if (ImGui::Button("Save Output")) {
            (useChannelOutput ? outputChannel : outputFull)->save();
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }

    // Cleanup
    delete inputNode;
    delete bcNode;
    delete splitter;
    delete outputFull;
    delete outputChannel;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
