#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    if(!glfwInit())
    {
        printf("GLFW init failed\n");
        return 1;
    }

    // gets info about current monitor for display
    const GLFWvidmode* mon = glfwGetVideoMode(glfwGetPrimaryMonitor());

    // create window 1/4 screen area
    GLFWwindow* window = glfwCreateWindow(mon->width / 2, mon->height / 2, "Light control", NULL, NULL);
    if(!window) {
        printf("GLFW window failed\n");
        glfwTerminate();
        return 1;
    }

    // allow use of OpenGL API
    glfwMakeContextCurrent(window);

    // start GLEW extension handler
    glewExperimental = GL_TRUE;
    glewInit();

    // DEPTH TESTING NOT INCLUDED, not necessary in the current scope. Can be included with the following:
    // glEnable(GL_DEPTH_TEST);
    // glDepthFunc(GL_LESS);

    float points[] = {
        -1.0f, 1.0f, 0.0f,  // top left
        1.0f, 1.0f, 0.0f,   // top right
        1.0f, -1.0f, 0.0f,  // bottom right
        1.0f, -1.0f, 0.0f,  // bottom right
        -1.0f, -1.0f, 0.0f, // bottom left
        -1.0f, 1.0f, 0.0f   // top left
    };

    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW); // 18 float buffer

    // data for drawn object
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // variables made from 3 floats

    // temporary shaders
    const char* vertex_shader =
    "#version 400\n" 
    "in vec3 vp;"
    "void main() {"
    " gl_Position = vec4(vp, 1.0);"
    "}";

    const char* fragment_shader =
    "#version 400\n"
    "out vec4 frag_colour;"
    "void main() {"
    " frag_colour = vec4(0.5, 0.0, 0.5, 1.0);" 
    "}";

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);
    
    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragment_shader, NULL);
    glCompileShader(fs);

    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, fs);
    glAttachShader(shader_program, vs);
    glLinkProgram(shader_program);

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6); // draw 6 points
        
        glfwPollEvents();
        
        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}