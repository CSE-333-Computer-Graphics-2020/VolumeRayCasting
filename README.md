# VolumeRayCasting

Volume visualization is an important tool to view and analyze large amounts of data from various scientific disciplines.Volume rendering involves the projection of a volume dataset onto a 2D image plane. 

#### Dependencies

* [imgui](https://github.com/ocornut/imgui) - Dear ImGui is a bloat-free graphical user interface library for C++. 
* [glm](https://github.com/g-truc/glm) - (GLM) is a header only C++ mathematics library for graphics software based on the OpenGL Shading Language (GLSL) specifications
> Note this project is derived from assignment 3 code provided by the Instructor

#### Building for source
To generate the executable using Makefile
```sh
$ make
```
Executing the code
```sh
$ ./Project
```
### Usage
Using Transfer Function :
* Three Transfer functions are provided and could be used by changing [main.cpp line number 104](https://github.com/AvatarWan/VolumeRayCasting/blob/7c47dfa04f0cfa2020d8e9edfd8530d5a9942d9d/src/main.cpp#L104)
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

Using different Rendering Techniques :
* [Averaging](https://github.com/AvatarWan/VolumeRayCasting/blob/7c47dfa04f0cfa2020d8e9edfd8530d5a9942d9d/src/world.cpp#L12)
* [Compositing](https://github.com/AvatarWan/VolumeRayCasting/blob/7c47dfa04f0cfa2020d8e9edfd8530d5a9942d9d/src/world.cpp#L13)
* [Compositing with gradient used for Opacity](https://github.com/AvatarWan/VolumeRayCasting/blob/7c47dfa04f0cfa2020d8e9edfd8530d5a9942d9d/src/world.cpp#L14)

### Todos

 - Phong Shading
 - Optimization

[//]: # (These are reference links used in the body of this note and get stripped out when the markdown processor does its job. There is no need to format nicely because it shouldn't be seen. Thanks SO - http://stackoverflow.com/questions/4823468/store-comments-in-markdown-syntax)


   [dill]: <https://github.com/joemccann/dillinger>
   [git-repo-url]: <https://github.com/joemccann/dillinger.git>
   [john gruber]: <http://daringfireball.net>
   [df1]: <http://daringfireball.net/projects/markdown/>
   [markdown-it]: <https://github.com/markdown-it/markdown-it>
   [Ace Editor]: <http://ace.ajax.org>
   [node.js]: <http://nodejs.org>
   [Twitter Bootstrap]: <http://twitter.github.com/bootstrap/>
   [jQuery]: <http://jquery.com>
   [@tjholowaychuk]: <http://twitter.com/tjholowaychuk>
   [express]: <http://expressjs.com>
   [AngularJS]: <http://angularjs.org>
   [Gulp]: <http://gulpjs.com>

   [PlDb]: <https://github.com/joemccann/dillinger/tree/master/plugins/dropbox/README.md>
   [PlGh]: <https://github.com/joemccann/dillinger/tree/master/plugins/github/README.md>
   [PlGd]: <https://github.com/joemccann/dillinger/tree/master/plugins/googledrive/README.md>
   [PlOd]: <https://github.com/joemccann/dillinger/tree/master/plugins/onedrive/README.md>
   [PlMe]: <https://github.com/joemccann/dillinger/tree/master/plugins/medium/README.md>
   [PlGa]: <https://github.com/RahulHP/dillinger/blob/master/plugins/googleanalytics/README.md>
