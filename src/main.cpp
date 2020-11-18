// Assignment 03: Raytracing

#include "imgui_setup.h"
#include "camera.h"
#include "renderengine.h"
#include "world.h"
#include "lightsource.h"
#include "pointlightsource.h"
#include "volume.h"
#include "transformcolor.h"
#include "TransferFunctionControlPoint.h"

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/glm.hpp>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../depends/stb/stb_image.h"
#include "../depends/stb/stb_image_write.h"

#define RENDER_BATCH_COLUMNS 20 // Number of columns to render in a single go. Increase to gain some display/render speed!

Camera *camera;
RenderEngine *engine;

int screen_width = 800, screen_height = 600; // This is window size, used to display scaled raytraced image.
int image_width = 1920, image_height = 1080; // This is raytraced image size. Change it if needed.
GLuint texImage;

#include <iostream>
TransformColor *getTransformFunctions()
{
    TransformColor *tr=new TransformColor();
    vector<TransferFunctionControlPoint> colorKnots = {
        // TransferFunctionControlPoint(.91f, .7f, .61f, 0),
        // TransferFunctionControlPoint(.91f, .7f, .61f, 80),
        // TransferFunctionControlPoint(1.0f, 1.0f, .85f, 82),
        // TransferFunctionControlPoint(1.0f, 1.0f, .85f, 256)
        TransferFunctionControlPoint(0.0f, 0.0f, 0.0f, 0),
        TransferFunctionControlPoint(0.05f, 0.05f, 0.05f, 80),
        TransferFunctionControlPoint(0.9f, 0.9f, 0.9f, 100),
        TransferFunctionControlPoint(1.0f, 1.0f, 1.0f, 256)
    };

    vector<TransferFunctionControlPoint> alphaKnots = {
        TransferFunctionControlPoint(0.0f, 0),
        TransferFunctionControlPoint(0.0f, 40),
        TransferFunctionControlPoint(0.2f, 60),
        TransferFunctionControlPoint(0.05f, 63),
        TransferFunctionControlPoint(0.0f, 80),
        TransferFunctionControlPoint(0.9f, 82),
        TransferFunctionControlPoint(1.0f, 256)
    };
    tr->computeTransferFunction(colorKnots,alphaKnots);
    return tr;
}

World *scene()
{
    World *world = new World();
    world->setBackground(Color(0.1, 0.3, 0.6));

    TransformColor *transformer= getTransformFunctions();
    transformer->printAlphaTransform();
    transformer->printColorTransform();

    Volume *volume = new Volume(transformer, "volume_file_ctscan");

    LightSource *light = new PointLightSource(world, Vector3D(3, -3, 10), Color(1, 1, 1));
    world->setVolume(volume);
    world->setLight(light);

    return world;       
}



int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Setup raytracer camera. This is used to spawn rays.
    //Vector3D camera_position(128/2, 256/2, 256 + 300);
    // Vector3D camera_target(128/2, 256/2, 0); //Looking down -Z axis
    Vector3D camera_position(128 + 300, 256/2,256/2);
    Vector3D camera_target(0, 256/2, 256/2);
    Vector3D camera_up(0, 1, 0);
    float camera_fovy =  45;
    camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, image_width, image_height);

    //Create a world
    World *world = scene();

    engine = new RenderEngine(world, camera);

    //Initialise texture
    glGenTextures(1, &texImage);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texImage);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool render_status;
        for(int i=0; i<RENDER_BATCH_COLUMNS; i++) 
            render_status = engine->renderLoop(); // RenderLoop() raytraces 1 column of pixels at a time.
        if(!render_status)
        {
            // Update texture
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texImage);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image_width, image_height, GL_RGB, GL_UNSIGNED_BYTE, camera->getBitmap());
        } 

        ImGui::Begin("Lumina", NULL, ImGuiWindowFlags_AlwaysAutoResize);
        ImGui::Text("Size: %d x %d", image_width, image_height);
        if(ImGui::Button("Save")){
          char filename[] = "img.png";
          stbi_write_png(filename, image_width, image_height, 3, camera->getBitmap(),0);        
        }
        //Display render view - fit to width
        int win_w, win_h;
        glfwGetWindowSize(window, &win_w, &win_h);
        float image_aspect = (float)image_width/(float)image_height;
        float frac = 0.95; // ensure no horizontal scrolling
        ImGui::Image((void*)(intptr_t)texImage, ImVec2(frac*win_w, frac*win_w/image_aspect), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f));

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clearColor.x, clearColor.y, clearColor.z, clearColor.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    glDeleteTextures(1, &texImage);

    cleanup(window);

    return 0;
}
