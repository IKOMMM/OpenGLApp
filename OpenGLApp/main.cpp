#include <stdio.h> //InputOutput - ex. erorrs to user
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//I have added STEP to show the included elements from the task

//Window Dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader;

//Vertex Shader
static const char* vShader = R"(
#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 vertexColor;

out vec3 fragColor;

void main() {
    gl_Position = vec4(0.4 * pos.x, 0.4 * pos.y, pos.z, 1.0);
    fragColor = vertexColor;
}
)";

//Fragment Shader
static const char* fShader = R"(
#version 330

in vec3 fragColor;
out vec4 color;

void main() {
    color = vec4(fragColor, 1.0);
}
)";


void CreateHouse() {
    // Vertices for the rectangle (base) and triangle (roof)
    GLfloat vertices[] = {
        // Base (square)
        -0.5f, -0.5f, 0.0f,  // Lower left
         0.5f, -0.5f, 0.0f,  // Bottom right
         0.5f,  0.0f, 0.0f,  // Top right
        -0.5f,  0.0f, 0.0f,  // Top left

        // Roof (triangle)
        -0.5f,  0.0f, 0.0f,  // Left
         0.5f,  0.0f, 0.0f,  // Right
         0.0f,  0.5f, 0.0f   // Top
    };

    // Colours for the vertices
    GLfloat colors[] = {
        // Green Base
        0.0f, 1.0f, 0.0f,  // Green
        0.0f, 1.0f, 0.0f,  // Green
        1.0f, 0.0f, 0.0f,  // Green
        1.0f, 0.0f, 0.0f,  // Green

        // Red roof
        1.0f, 0.0f, 0.0f,  // Red
        1.0f, 0.0f, 0.0f,  // Red
        1.0f, 0.0f, 0.0f   // Red
    };

    //STEP 01
    GLuint indices[] = {
        // Base (square)
        0, 1, 2,
        2, 3, 0,
        // Roof (triangle)
        4, 5, 6
    };

    GLuint EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //STEP 02
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vertices), sizeof(colors), colors);
        
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Vertices
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    // Colors
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)sizeof(vertices));
    glEnableVertexAttribArray(1);

    //STEP 03
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
    GLuint theShader = glCreateShader(shaderType);

    const GLchar* theCode[1];
    theCode[0] = shaderCode;

    GLint codeLength[1];
    codeLength[0] = strlen(shaderCode);

    glShaderSource(theShader, 1, theCode, codeLength);
    glCompileShader(theShader);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(theShader, sizeof(eLog), NULL, eLog);
        printf("Error compiling the %d shader: '%s'\n", shaderType, eLog);
        return;
    }

    glAttachShader(theProgram, theShader);
}

void CompileShaders() {
    shader = glCreateProgram();

    if (!shader) {
        printf("Error creating shader program!");
        return;
    }

    AddShader(shader, vShader, GL_VERTEX_SHADER);
    AddShader(shader, fShader, GL_FRAGMENT_SHADER);

    GLint result = 0;
    GLchar eLog[1024] = { 0 };

    glLinkProgram(shader);
    glGetProgramiv(shader, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error linking program: '%s'\n", eLog);
        return;
    }

    //Validate if shader which we've now created is in the curent context that OpenGL is working in
    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }

}

int main()
{
    // Initialise GLFW
    if (!glfwInit()) {
        printf("GLFW Initialisation failed!");
        glfwTerminate();
        return 1;
    }

    // Setup GLFW window properties
    //OpenGL verison
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Core Profile = No Backwards Compatibility
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Allow forward compatibility
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW window creation failed!");
        glfwTerminate();
        return 1;
    }

    //Get Buffer size information
    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

    //Set contex for GLEW to use
    glfwMakeContextCurrent(mainWindow);

    //Allow modern extension features
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    //Create Viewport
    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateHouse();
    CompileShaders();

    //Loop until window close +/- Update/Tick
    while (!glfwWindowShouldClose(mainWindow)) {
        // Get & Handle user input events
        glfwPollEvents();

        //Clear Window
        glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glBindVertexArray(VAO);

        //STEP 04
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);

        glUseProgram(0);

        glfwSwapBuffers(mainWindow);
    }

    return 0;
}