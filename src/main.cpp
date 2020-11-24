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
TransformColor *getAverageTransformFunctions()
{
    TransformColor *tr=new TransformColor();
    vector<TransferFunctionControlPoint> colorKnots = {
        TransferFunctionControlPoint(0.0f, 0.0f, 0.0f, 0),
        TransferFunctionControlPoint(0.58f, 0.55f, 0.21f, 90),
        TransferFunctionControlPoint(0.94f, 0.87f, 0.05f, 100),
        TransferFunctionControlPoint(1.0f, 1.0f, 1.0f, 256)

    };

    vector<TransferFunctionControlPoint> alphaKnots = {
        TransferFunctionControlPoint(0.0f, 0),
        TransferFunctionControlPoint(0.1f, 100),
        TransferFunctionControlPoint(0.95f, 130),
        TransferFunctionControlPoint(1.0f, 256)
    };
    tr->computeTransferFunction(colorKnots,alphaKnots);
    return tr;
}

TransformColor *getCompositeTransformFunctions()
{
    TransformColor *tr=new TransformColor();
    vector<TransferFunctionControlPoint> colorKnots = {
        TransferFunctionControlPoint(.91f, .7f, .61f, 0),
        TransferFunctionControlPoint(.91f, .7f, .61f, 80),
        TransferFunctionControlPoint(1.0f, 1.0f, .85f, 82),
        TransferFunctionControlPoint(1.0f, 1.0f, .85f, 256)
    };

    vector<TransferFunctionControlPoint> alphaKnots = {
        TransferFunctionControlPoint(0.0f, 0),
        TransferFunctionControlPoint(0.0f, 40),
        TransferFunctionControlPoint(0.05f, 60),
        TransferFunctionControlPoint(0.01f, 63),
        TransferFunctionControlPoint(0.0f, 80),
        TransferFunctionControlPoint(0.09f, 82),
        TransferFunctionControlPoint(0.1f, 256)    };
    tr->computeTransferFunction(colorKnots,alphaKnots);
    return tr;
}
TransformColor *getComposite2DTransformFunctions()
{
    TransformColor *tr=new TransformColor();
    vector<TransferFunctionControlPoint> colorKnots = {
        TransferFunctionControlPoint(.91f, .7f, .61f, 0),
        TransferFunctionControlPoint(.91f, .7f, .61f, 80),
        TransferFunctionControlPoint(1.0f, 1.0f, .85f, 82),
        TransferFunctionControlPoint(1.0f, 1.0f, .85f, 256)
    };

    vector<TransferFunctionControlPoint> alphaKnots = {
        TransferFunctionControlPoint(0.0f, 0),
        TransferFunctionControlPoint(0.0f, 40),
        TransferFunctionControlPoint(0.02f, 60),
        TransferFunctionControlPoint(0.005f, 63),
        TransferFunctionControlPoint(0.0f, 80),
        TransferFunctionControlPoint(0.09f, 82),
        TransferFunctionControlPoint(0.1f, 256)    };
    tr->computeTransferFunction(colorKnots,alphaKnots);
    return tr;
}
World *scene()
{
    World *world = new World();
    world->setBackground(Color(0.1, 0.3, 0.6));

    // world->setBrightness(8.0);
    // TransformColor *transformer= getAverageTransformFunctions();

    world->setBrightness(1.0);
    TransformColor *transformer= getCompositeTransformFunctions();

    // world->setBrightness(1.0);
    // TransformColor *transformer= getComposite2DTransformFunctions();

    transformer->printAlphaTransform();
    transformer->printColorTransform();

    Volume *volume = new Volume(transformer, "volume_file.ctscan");

    LightSource *light = new PointLightSource(world, Vector3D(3, -3, 10), Color(1, 1, 1));
    world->setVolume(volume);
    world->setLight(light);

    return world;       
}

float rescaleX(const float &x)
{
    return -1.0 + ((1.0*x - 0) / (image_width - 0)) * (1.0 - (-1.0));
}
float rescaleY(const float &y)
{
    return -1.0 + ((1.0*(image_height - y) - 0) / (image_height - 0)) * (1.0 - (-1.0));
}
void mouseLeftClickDrag(ImGuiIO &io)
{
    if(ImGui::IsMouseDragging(0)&& !ImGui::IsAnyItemActive())
    {
        float xo = io.MouseClickedPos[0].x;
        float yo = io.MouseClickedPos[0].y;
        float xd = ImGui::GetMouseDragDelta().x;
        float yd = ImGui::GetMouseDragDelta().y;
        float xn = rescaleX(xo+xd);
        float yn = rescaleY(yo+yd);
        std::cout<<"Dragging!"<<std::endl;
    }
}
    Vector3D camera_position(128 + 300, 256/2,256/2);
    Vector3D camera_target(0, 256/2, 256/2);
    Vector3D camera_up(0, 1, 0);
    float camera_fovy =  45;

void movementSlidersX(float matrix[],float &angle_x)
{
    ImGui::Begin("Window1");
    if(ImGui::SliderFloat("Angle(radians) x", &angle_x, 0.0f, 3.14f)){
            std::cout << "angle_x" << angle_x << std::endl;
            glm::mat4 rotate_mat = glm::rotate(glm::make_mat4(matrix), angle_x, glm::vec3(1,0,0)) ;
            glm::vec4 c_pos = rotate_mat* glm::vec4(camera_position.X(),camera_position.Y(),camera_position.Z(),0.0);
            Vector3D c_posV(c_pos[0],c_pos[1],c_pos[2]);
            Camera *camera = new Camera(c_posV, camera_target, camera_up, camera_fovy, image_width, image_height);
            engine->setCamera(camera);
    };
    ImGui::End();
}

int main(int, char**)
{
    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Setup raytracer camera. This is used to spawn rays.
    //Vector3D camera_position(128/2, 256/2, 256 + 300);
    // Vector3D camera_target(128/2, 256/2, 0); //Looking down -Z axis
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


    float angleX = 0.0f;
    GLfloat matrix[] = {
    1.0f, 0.0f, 0.0f, 0.0f, // first column
    0.0f, 1.0f, 0.0f, 0.0f, // second column
    0.0f, 0.0f, 1.0f, 0.0f,// third column
     0.0f, 0.0f, 0.0f, 1.0f
    };


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

        //Camera Movement
        // movementSlidersX(matrix,angleX);


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
