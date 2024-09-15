#include <iostream>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "stb_image.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "Light.h"
#include "RigidBody.h"
#include "ShapeMatching.h"
#include "Cloth.h"
#include "SoftBody.h"
#include "Wave.h"

const int screenWidth = 1600;
const int screenHeight = 1200;

Camera camera(glm::vec3(0.f, 2.f, 10.f));
float lastX = screenWidth / 2.f;
float lastY = screenHeight / 2.f;
bool firstMouse = true;

float lastFrame = 0.f, deltaTime = 0.f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double x, double y)
{
    float xPos = static_cast<float>(x);
    float yPos = static_cast<float>(y);

    if (firstMouse)
    {
        lastX = xPos;
        lastY = yPos;
        firstMouse = false;
    }

    float xOffset = xPos - lastX;
    float yOffset = lastY - yPos;
    lastX = xPos, lastY = yPos;

    camera.process_mouse_movement(xOffset, yOffset);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.process_keyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.process_keyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.process_keyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.process_keyboard(RIGHT, deltaTime);
}

int main()
{ 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GL_MULTISAMPLE, 4);

    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_MULTISAMPLE);

    stbi_set_flip_vertically_on_load(true);

    Wave wave;

    std::string modelPath = "D:/Unity project/games103/Assets/bunny.obj";//"D:/Unity project/games103/Assets/bunny.obj";
    Model mModel(modelPath);

    std::string vShaderPath = "D:/OpenGL/Shader/vertexShader.glsl";
    std::string fShaderPath = "D:/OpenGL/Shader/fragmentShader.glsl";
    Shader shader(vShaderPath, fShaderPath);

    DirLight dirLight(glm::vec3(-0.2f, -1.0f, -0.3f), glm::vec3(0.05f), glm::vec3(0.4f), glm::vec3(0.5f));
    // SpotLight spotLight(camera.position, camera.front, glm::vec3(0.f), glm::vec3(1.f), glm::vec3(1.f), 1.f, 0.09f, 0.032f, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(15.f)));

    shader.use();

    dirLight.setup(shader);

    shader.set_float("material.Ns", 225.f);

    std::cout << glGetError() << std::endl;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
            wave.createWave = true;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glDisable(GL_CULL_FACE);

        shader.use();

        glm::mat4 model = glm::mat4(1.f);
        shader.set_mat4("model", model);

        wave.update(deltaTime, shader);

        glm::mat4 view = glm::mat4(1.f);
        view = camera.get_view_matrix();
        shader.set_mat4("view", view);

        glm::mat4 projection = glm::mat4(1.f);
        projection = glm::perspective(glm::radians(camera.fov), screenWidth * 1.0f / screenHeight, 0.1f, 100.f);
        shader.set_mat4("projection", projection);

        shader.set_vec3("viewPos", camera.position);

        // spotLight.position = camera.position;
        // spotLight.direction = camera.front;
        // spotLight.setup(shader);

        wave.mesh->draw(shader);
        
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}