## Core Library

### Camera and CameraController classes
Camera handles only the data and methods necessary to perform view and perspective transformations. Any data and methods related to movement and input are implemented in CameraController.

Usage:

```c++
std::unique_ptr<Camera> cam = nullptr;
std::unique_ptr<CameraController> cam_controler = nullptr;

int width;
int height;

void initalizeCameras(GLFWwindow* window){
    glfwGetFramebufferSize(window, &width, &height);
    cam = std::make_unique<Camera>(width,height);
    cam_controler = std::make_unique<CameraController>(*cam);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
}

void init(GLFWwindow* window){
    //...
    initalizeCameras(window);
    //...
}

float delta_time = 0.0f;
float last_frame = 0.0f;

//Called every frame
void display(GLFWwindow* window){
    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    cam_controler->processCameraInput(window, delta_time);
    cam->updateCamera();  
    //...
}

//On window resize
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    cam->viewport_w = width;
    cam->viewport_h = height;
    cam->updatePerspMat();
    //...
}

//Mouse movement
void mouse_callback(GLFWwindow* window, double x_pos, double y_pos){
    cam_controler->mouseCameraController(window, x_pos, y_pos);
}

//Scroll-wheel movement
void scroll_callback(GLFWwindow* window, double x_offset, double y_offset){
    cam_controler->mouseZoomController(window, x_offset, y_offset);
}
```
