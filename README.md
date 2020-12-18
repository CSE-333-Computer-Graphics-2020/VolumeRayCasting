# VolumeRayCasting

Volume visualization is an important tool to view and analyze large amounts of data from various scientific disciplines.Volume rendering involves the projection of a volume dataset onto a 2D image plane. 

#### Dependencies

* [imgui](https://github.com/ocornut/imgui) - Dear ImGui is a bloat-free graphical user interface library for C++. 
* [glm](https://github.com/g-truc/glm) - (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications
* [glfw](https://github.com/glfw/glfw) - (GLFW) is an Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan application development. It provides a simple, platform-independent API for creating windows, contexts and surfaces, reading input, handling events, etc.
> Note this project is derived from assignment 3 code provided by the Instructor

#### Building for source
To generate the executable using Makefile
```sh
$ make
```
Executing the code (Provide configuration file in [more detail here](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting#defining--providing-configuration-file))
```sh
$ ./Project configuration_file_here
```
Example : 
```sh
$ ./Project ./input/config_8.cfg
```
### Usage
##### Transfer Function Usage
Using Transfer Function : Knots needs to be provide and then curve is computed using [CubicSpline Interpolation](https://en.wikiversity.org/wiki/Cubic_Spline_Interpolation).
* Providing any of the Transfer function from [TransferFunction](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/tree/main/TransferFunctions) Folder in configration file (more detail on this in next section).

* Three Transfer functions are provided and could be used by changing [main.cpp line number 106](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/blob/1b1061fb5e08dbf99157eb8e21b1b1c5b9fbf00c/src/main.cpp#L164)
* Custom Transfer functions : needs to provide knots and a CubicSpline in fit into the it. look at this example.
```cpp
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
```
#### Defining / Providing Configuration file
Provide the configuration file to code in this format (fields details are in the comment above them). Some of possible configuration files are provided inside [input](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/tree/main/input) folder
    # Brightness of the whole Scene [float]
    brightness=4.0

    # location of the Color Transform knots file to be used
    colorTransform=./TransferFunctions/CompositePhong/transformationColorPhong4.trn

    # location of the Alpha (Opacity) Transform knots file to be used
    alphaTransform=./TransferFunctions/CompositePhong/transformationAlphaPhong4.trn

    # location of file containing volume
    volumeFile=./volumes/volume_file.ctscan
    # slices of the whole volume
    volumeSlices=256

    # Find value at the location in volume
    # Value (Color (RGBA) and gradient) at a location (float) of the grid is computed as :
    # 1 - for trilinear interpolation with nearest 8 neighbors for the value
    # 2 - for the nearest neighbor
    interpolation=1

    # Scalar Filter Size (needs to be odd number)
    #   apply filter of NXN to make volume smooth
    scalarFilter=1

    # Gradient Computation Filter Size (needs to be odd number)
    #   after computing gradient by central difference apply filter of NXN to make volume smooth
    gradientFilter=1

    # Mode of Transformation 
    # 1 - (Voxel Value, Voxel Value) -> (RGB, A) (Provide Different Objects themeselve)
    # 2 - (Voxel Value, Gradient Magnitude) -> (RGB, A) (Provide Edges i.e. transition details between Objects)
    modeTransform=1

    # Light direction
    # lightDirection=-1.0 1.0 0.0
    lightDirection=0.0 1.0 0.0

    # Shineness Constant
    shininess=3.0

    # Specular Contribution
    ks=0.1

    # number of samples along ray
    samplesRay=100

    # assembling method :
    # 1 - average
    # 2 - compositing
    # 3 - compositing + shading
    renderingMethod=3

    # Camera Configuration
    #   Camera Position
    camera_position=428 128 128
    #   Camera Target
    camera_target=0 128 128
    # Camera Up Vector
    camera_up=0 1 0
    #   Camera Field of Vision
    camera_fovy=45

### Features

- user friendly way to provide transfer function & different transfer functions for both color and opacity.
- support for both Nearest Neighbour and Trilinear Interpolation to find values at the location in the Volume.
- [Box Linear Filter](https://en.wikipedia.org/wiki/Box_blur) for Scalar Values of the Volume and Gradients.
- Customizable number of sample along the ray.
- Different Tranformation Mode to Find the Value of RGBA at each voxel.
  - (Voxel Value, **Voxel Value**) -> (RGB, A) (Provide Different Objects themeselve)
  - (Voxel Value, **Gradient Magnitude**) -> (RGB, A) (Provide Edges i.e. transition details between Objects)
- Customizable light direction
- Different Rendering techniques :
  - average
  - compositing
  - compositing + shading
- Customizable Camera location, View Direction and Orientation.
- Ability to cache the Gradient Computed & filtered applied Volume.

### Output showing different Features
##### Averaging Effect
```sh
./Project ./input/config_3.cfg
```
[![Averaging Render](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/blob/main/input/config_3.cfg.png "Averaging Render")](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/blob/main/input/config_1.cfg "Averaging Render")
#### Compositing Effect
```sh
./Project ./input/config_4.cfg
```
[![Compositing Render](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/blob/main/input/config_2.cfg.png "Compositing Render")](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/blob/1b1061fb5e08dbf99157eb8e21b1b1c5b9fbf00c/input/config_2.cfg#L51 "Compositing Render")
#### Compositing & Shading Effect
```sh
./Project ./input/config_3.cfg
```
[![Compositing & Shading Render](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/blob/main/input/config_8.cfg.png "Compositing & Shading Render")](https://github.com/CSE-333-Computer-Graphics-2020/VolumeRayCasting/blob/1b1061fb5e08dbf99157eb8e21b1b1c5b9fbf00c/input/config_8.cfg#L52 "Compositing & Shading Render")
#### Transition Effect 
Highlighting Transition from internal tissues to bones. Using Gradient Magnitude.
