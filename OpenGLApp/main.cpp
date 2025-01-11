#include <stdio.h> //InputOutput - ex. erorrs to user
#include <string.h>
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

//Window Dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, EBO, shader, uniformXMove;

//Object Movement valuses
bool direction = true;
float triOffset = 0.0f;
float triMaxoffset = 0.7f;
float triIncrement = 0.001f;

//Vertex Shader
static const char* vShader = "                                             \n\
#version 330                                                               \n\
                                                                           \n\
layout (location = 0) in vec3 pos;                                         \n\
layout (location = 1) in vec3 color;                                       \n\
                                                                           \n\
out vec3 fragColor;                                                        \n\
uniform float xMove;                                                       \n\
                                                                           \n\
void main()                                                                \n\
{                                                                          \n\
         gl_Position = vec4(0.4 * pos.x + xMove,0.4 * pos.y, pos.z, 1.0);  \n\
         fragColor = color;                                                \n\
}";

//Fragment Shader
static const char* fShader = "                                     \n\
#version 330                                                       \n\
                                                                   \n\
in vec3 fragColor;                                                 \n\
out vec4 color;                                                    \n\
                                                                   \n\
void main()                                                        \n\
{                                                                  \n\
         color = vec4(fragColor, 1.0);                             \n\
}";

void CreateHouse() {
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

    GLfloat colors[] = {
        // Green base
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f,

        // Red roof
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f
    };

    GLuint indices[] = {
        // Base (square)
        0, 1, 2,
        2, 3, 0,
        // Roof (triangle)
        4, 5, 6
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

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

    glValidateProgram(shader);
    glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
        printf("Error validating program: '%s'\n", eLog);
        return;
    }

    uniformXMove = glGetUniformLocation(shader, "xMove");
}

int main() {
    if (!glfwInit()) {
        printf("GLFW Initialisation failed!");
        glfwTerminate();
        return 1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test Window", NULL, NULL);
    if (!mainWindow) {
        printf("GLFW window creation failed!");
        glfwTerminate();
        return 1;
    }

    int bufferWidth, bufferHeight;
    glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);
    glfwMakeContextCurrent(mainWindow);
    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK) {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(mainWindow);
        glfwTerminate();
        return 1;
    }

    glViewport(0, 0, bufferWidth, bufferHeight);

    CreateHouse();  
    CompileShaders();

    while (!glfwWindowShouldClose(mainWindow)) {
        glfwPollEvents();

        if (direction) {
            triOffset += triIncrement;
        }
        else {
            triOffset -= triIncrement;
        }

        if (abs(triOffset) >= triMaxoffset) {
            direction = !direction;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glUniform1f(uniformXMove, triOffset);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 9, GL_UNSIGNED_INT, 0);  
        glBindVertexArray(0);

        glUseProgram(0);
        glfwSwapBuffers(mainWindow);
    }

    return 0;
}
