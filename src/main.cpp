// CSE: Project
#include <sstream>
#include <map>

#include "imgui_setup.h"
#include "camera.h"
#include "renderengine.h"
#include "world.h"
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
    TransformColor *tr=new TransformColor(
        "./TransferFunctions/Average/transformationColorAvg.trn",
        "./TransferFunctions/Average/transformationAlphaAvg.trn");
    return tr;
}

TransformColor *getCompositeTransformFunctions()
{
    TransformColor *tr=new TransformColor(
        "./TransferFunctions/Composite/transformationColorComp.trn",
        "./TransferFunctions/Composite/transformationAlphaComp.trn");
    return tr;
}
TransformColor *getCompositeTransformFunctions2()
{
    TransformColor *tr=new TransformColor(
        "./TransferFunctions/Composite/transformationColorComp2.trn",
        "./TransferFunctions/Composite/transformationAlphaComp2.trn");
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

Vector3D camera_position(128 + 300, 256/2,256/2);
Vector3D camera_target(0, 256/2, 256/2);
Vector3D camera_up(0, 1, 0);
float camera_fovy =  45;

void setCamera(string file)
{
    fstream fin;
    fin.open(file.c_str(), ios::in);
    if (fin.fail())
    {
        cout << "Can't open camera configuration file" << endl;
    }
    int a,b,c;
    fin>>a>>b>>c;
    camera_position= Vector3D(a,b,c);
    fin>>a>>b>>c;
    camera_target= Vector3D(a,b,c);
    fin>>a>>b>>c;
    camera_up= Vector3D(a,b,c);
    fin>>camera_fovy;
    fin.close();

    camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, image_width, image_height);
}
const std::string WHITESPACE = " \n\r\t\f\v";
std::string ltrim(const std::string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == std::string::npos) ? "" : s.substr(start);
}
 
std::string rtrim(const std::string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == std::string::npos) ? "" : s.substr(0, end + 1);
}
 
std::string trim(const std::string& s)
{
    return rtrim(ltrim(s));
}
std::vector<float> string2float(std::string str)
{
    std::vector<float> results;
    std::stringstream str_strm(str);
    float val;
    while(str_strm>>val)
    {
        results.push_back(val);
    }
    return results;
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

    Volume *volume = new Volume(transformer, "volume_file.ctscan", 256);
    // Volume *volume = new Volume(transformer, "brain.png", 176);

    world->setVolume(volume);
    world->setLight(glm::vec3(-1.0f,1.0f,0.0f));
    world->setAssembling(2); //defaulted - 3 : compositing + shading

    setCamera("camera.cfg");

    return world;       
}
World *scene2()
{
    World *world = new World();
    world->setBackground(Color(0.1, 0.3, 0.6));

    // world->setBrightness(8.0);
    // TransformColor *transformer= getAverageTransformFunctions();

    world->setBrightness(4.0); //defaulted - 1.0 : whole brightness of scene
    TransformColor *transformer= getCompositeTransformFunctions2();

    // world->setBrightness(1.0);
    // TransformColor *transformer= getComposite2DTransformFunctions();

    transformer->printAlphaTransform();
    transformer->printColorTransform();

    // Volume *volume = new Volume(transformer, "volume_file.ctscan", 256);
    Volume *volume = new Volume(transformer, "brain.png", 176);
    volume->setInterpolation(1); //defaulted - 1 : trilinear
    volume->setGFilterSize(5);   //defaulted - 5
    volume->setTransformationDimension(1); //defaulted - 1 : (value,value) -> (RGB,A)
    volume->genGradient();


    // LightSource *light = new PointLightSource(world, Vector3D(3, -3, 10), Color(1, 1, 1));
    world->setVolume(volume);
    world->setLight(glm::vec3(-1.0f,1.0f,0.0f)); //defaulted
    world->setSamples(100); //defaulted - 100 : number of samples along ray
    world->setAssembling(3); //defaulted - 3 : compositing + shading


    setCamera("camera2.cfg");

    return world;       
}
World *loadScene(std::string configFile)
{
	std::ifstream fileStream(configFile);
	typedef std::map<std::string, std::string> ConfigInfo;
	ConfigInfo configValues;
	std::string line;
	while (std::getline(fileStream, line))
	{
		std::istringstream is_line(line);
		std::string key;
		if (std::getline(is_line, key, '='))
		{
			std::string value;
			if (key[0] == '#')
			    continue;

			if (std::getline(is_line, value))
			{
			    configValues[key] = trim(value);
			}
		}
	}	

    World *world = new World();
    // world->setBackground(Color(0.1, 0.3, 0.6));

    world->setBrightness(string2float(configValues["brightness"])[0] ); //defaulted - 1.0 : whole brightness of scene
    TransformColor *transformer= new TransformColor(
    configValues["colorTransform"],
    configValues["alphaTransform"]);


    transformer->printAlphaTransform();
    transformer->printColorTransform();

    Volume *volume = new Volume(transformer, configValues["volumeFile"], string2float(configValues["volumeSlices"])[0] , string2float(configValues["scalarFilter"])[0]);
    volume->setInterpolation(string2float(configValues["interpolation"])[0]); //defaulted - 1 : trilinear
    volume->setGFilterSize(string2float(configValues["gradientFilter"])[0]);   //defaulted - 5
    volume->setTransformationDimension(string2float(configValues["modeTransform"])[0]); //defaulted - 1 : (value,value) -> (RGB,A)
    volume->genGradient();


    world->setVolume(volume);
    std::vector<float> lD = string2float(configValues["lightDirection"]);
    world->setLight(glm::vec3(lD[0],lD[1],lD[2])); //defaulted
    world->setSamples(string2float(configValues["samplesRay"])[0]); //defaulted - 100 : number of samples along ray
    world->setAssembling(string2float(configValues["renderingMethod"])[0]); //defaulted - 3 : compositing + shading
    world->setShininess(string2float(configValues["shininess"])[0]); //defaulted - 3.0
    world->setSpec(string2float(configValues["ks"])[0]); //defaulted - 0.5


    lD = string2float(configValues["camera_position"]);
    camera_position= Vector3D(lD[0],lD[1],lD[2]);
    lD = string2float(configValues["camera_target"]);
    camera_target= Vector3D(lD[0],lD[1],lD[2]);
    lD = string2float(configValues["camera_up"]);
    camera_up= Vector3D(lD[0],lD[1],lD[2]);
    camera_fovy= string2float(configValues["camera_fovy"])[0];
    camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, image_width, image_height);
    world->setCamera(camera);
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

int main(int argc, char** argv)
{
    std::string config_file = "config.cfg";
    if(argc>1)
        config_file = std::string(argv[1]);


    // Setup window
    GLFWwindow *window = setupWindow(screen_width, screen_height);
    ImGuiIO& io = ImGui::GetIO(); // Create IO object

    ImVec4 clearColor = ImVec4(1.0f, 1.0f, 1.0f, 1.00f);

    // Setup raytracer camera. This is used to spawn rays.
    //Vector3D camera_position(128/2, 256/2, 256 + 300);
    // Vector3D camera_target(128/2, 256/2, 0); //Looking down -Z axis
    // camera = new Camera(camera_position, camera_target, camera_up, camera_fovy, image_width, image_height);

    //Create a world
    // World *world = scene();
    // World *world = scene2();
    World *world = loadScene(config_file);

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

    config_file.append(".png");
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
          stbi_write_png(config_file.c_str(), image_width, image_height, 3, camera->getBitmap(),0);        
          std::cout<<"Saved "<<config_file<<" File!"<<std::endl;
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
