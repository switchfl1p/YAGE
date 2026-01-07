#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

#include <Shader.hpp>
#include <Program.hpp>

// Cube Vertex Data with Colors (Separate Position and Color Sections)
// 8 vertices total (one per corner)
// Format: All positions first, then all colors

float vertex_data[] = {
    // Positions (24 vertices * 3 floats = 72 floats)
    
    // Front face (4 vertices)
    -1.0f, -1.0f,  1.0f,  // 0
     1.0f, -1.0f,  1.0f,  // 1
     1.0f,  1.0f,  1.0f,  // 2
    -1.0f,  1.0f,  1.0f,  // 3
    
    // Back face (4 vertices)
     1.0f, -1.0f, -1.0f,  // 4
    -1.0f, -1.0f, -1.0f,  // 5
    -1.0f,  1.0f, -1.0f,  // 6
     1.0f,  1.0f, -1.0f,  // 7
    
    // Top face (4 vertices)
    -1.0f,  1.0f,  1.0f,  // 8
     1.0f,  1.0f,  1.0f,  // 9
     1.0f,  1.0f, -1.0f,  // 10
    -1.0f,  1.0f, -1.0f,  // 11
    
    // Bottom face (4 vertices)
    -1.0f, -1.0f, -1.0f,  // 12
     1.0f, -1.0f, -1.0f,  // 13
     1.0f, -1.0f,  1.0f,  // 14
    -1.0f, -1.0f,  1.0f,  // 15
    
    // Right face (4 vertices)
     1.0f, -1.0f,  1.0f,  // 16
     1.0f, -1.0f, -1.0f,  // 17
     1.0f,  1.0f, -1.0f,  // 18
     1.0f,  1.0f,  1.0f,  // 19
    
    // Left face (4 vertices)
    -1.0f, -1.0f, -1.0f,  // 20
    -1.0f, -1.0f,  1.0f,  // 21
    -1.0f,  1.0f,  1.0f,  // 22
    -1.0f,  1.0f, -1.0f,  // 23
    
    // Colors (24 vertices * 4 floats RGBA = 96 floats)
    
    // Front face - Red
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 0.0f, 1.0f,
    
    // Back face - Green
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    
    // Top face - Blue
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    
    // Bottom face - Yellow
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 0.0f, 1.0f,
    
    // Right face - Magenta
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 1.0f,
    
    // Left face - Cyan
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
    0.0f, 1.0f, 1.0f, 1.0f,
};

// Index data for drawing triangles
unsigned short index_data[] = {
    // Front face
    0, 1, 2,   0, 2, 3,
    // Back face
    4, 5, 6,   4, 6, 7,
    // Top face
    8, 9, 10,  8, 10, 11,
    // Bottom face
    12, 13, 14, 12, 14, 15,
    // Right face
    16, 17, 18, 16, 18, 19,
    // Left face
    20, 21, 22, 20, 22, 23,
};

float delta_time = 0.0f;
float last_frame = 0.0f;

float fov = 45.0f;

class Camera{
    public:
        Camera(){}
        ~Camera(){}

        float camera_speed = 30.0f;

        glm::vec3 camera_pos = glm::vec3(0.0f, 1.5f, 10.0f);
        glm::vec3 camera_front = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 camera_up = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);

        void updateView(){
            view = glm::lookAt(camera_pos, camera_pos + camera_front, camera_up);
        }

        void updateCameraSpeed(){
            camera_speed = 10.0f * delta_time;
        }
        
        void cameraControllsCallback(GLFWwindow* window){
            updateCameraSpeed();

            if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                camera_pos += camera_speed * camera_front;

            if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                camera_pos -= camera_speed * camera_front;

            if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                camera_pos -= glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;

            if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                camera_pos += glm::normalize(glm::cross(camera_front, camera_up)) * camera_speed;

            if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
                camera_pos += camera_speed * camera_up;

            if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
                camera_pos -= camera_speed * camera_up;
        }

        void mouseCameraCallback(GLFWwindow* window, double xpos, double ypos){
            if(first_mouse){
                last_x = xpos;
                last_y = ypos;
                first_mouse = false;
            }

            float x_offset = xpos - last_x;
            float y_offset = last_y - ypos;
            last_x = xpos;
            last_y = ypos;

            float sensitivity = 0.025f;
            x_offset *= sensitivity;
            y_offset *= sensitivity;

            yaw += x_offset;
            pitch += y_offset;

            if(pitch > 89.0f)
                pitch = 89.0f;
            if(pitch < -89.0f)
                pitch = -89.0f;

            updateDirection();
        }

        void mouseZoomCallback(GLFWwindow* window, double x_offset, double y_offset)
        {
            fov -= (float)y_offset * 2.0;
            if (fov < 1.0f)
                fov = 1.0f;
            if (fov > 45.0f)
                fov = 45.0f; 
        }


        float yaw = -90.0f;
        float pitch = 0.0f;

        glm::vec3 direction;

        void updateDirection(){
            direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
            direction.y = sin(glm::radians(pitch));
            direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
            camera_front = glm::normalize(direction);
        }

        float last_x = 320, last_y = 240;

        bool first_mouse = true;

};

Camera cam;

GLuint program_uint;

GLuint model_mat_unif;
GLuint camera_mat_unif;
GLuint projection_mat_unif;

glm::mat4 perspective_mat;
int width, height;

void initalizeProgram(GLFWwindow* window){
    // Initialize shaders and programs here
    // Example shader loading:

    glfwGetFramebufferSize(window, &width, &height);

    std::vector<GLuint> shaders;
    Shader vertex_shader("controlled_camera.vert");
    Shader fragment_shader("controlled_camera.frag");
    shaders.push_back(vertex_shader.getShaderUint());
    shaders.push_back(fragment_shader.getShaderUint());

    Program the_program(shaders);
    program_uint = the_program.getProgramUint();

    model_mat_unif = glGetUniformLocation(program_uint, "model_matrix");
    camera_mat_unif = glGetUniformLocation(program_uint, "camera_matrix");
    projection_mat_unif = glGetUniformLocation(program_uint, "perspective_matrix");

    glm::mat4 model_mat(1);

    glUseProgram(program_uint);
    glUniformMatrix4fv(model_mat_unif, 1, GL_FALSE, glm::value_ptr(model_mat));
	glUseProgram(0);
}

GLuint vertex_buffer_object;
GLuint index_buffer_object;
GLuint vao;

void initalizeVertexBuffer(){
    // Example usage:

    //VBO
    glGenBuffers(1, &vertex_buffer_object);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    //IBO
    glGenBuffers(1, &index_buffer_object);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void initializeVertexArrayObjects(){
    glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    size_t color_offset = 24 * 3 * sizeof(float);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)color_offset);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object);
	glBindVertexArray(0);
}

void init(GLFWwindow* window){
    initalizeProgram(window);
    initalizeVertexBuffer();
    initializeVertexArrayObjects();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

void display(GLFWwindow* window){
    // Rendering code here
    // Called every frame

    float current_frame = glfwGetTime();
    delta_time = current_frame - last_frame;
    last_frame = current_frame;

    cam.cameraControllsCallback(window);
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program_uint);
    
    cam.updateView();

    perspective_mat = glm::perspective(glm::radians(fov), float(width)/float(height), 0.1f, 100.0f);
    glUniformMatrix4fv(projection_mat_unif, 1, GL_FALSE, glm::value_ptr(perspective_mat));

    glUniformMatrix4fv(camera_mat_unif, 1, GL_FALSE, glm::value_ptr(cam.view));
    glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

	glBindVertexArray(0);
	glUseProgram(0);

	glfwSwapBuffers(window);
    glfwPollEvents();
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    perspective_mat = glm::perspective(glm::radians(45.0f), float(width)/float(height), 0.1f, 100.0f);
    glUseProgram(program_uint);
    glUniformMatrix4fv(projection_mat_unif, 1, GL_FALSE, glm::value_ptr(perspective_mat));
    glUseProgram(0);

    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
    cam.mouseCameraCallback(window, xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double x_offset, double y_offset)
{
    cam.mouseZoomCallback(window, x_offset, y_offset);
}
