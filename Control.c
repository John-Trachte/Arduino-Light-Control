#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char* readShader(const char* file)
{
    FILE* fp; 
    long size = 0;
    char* shader;

    fp = fopen(file, "rb");
    if(fp == NULL)
      return "";

    fseek(fp, 0L, SEEK_END);
    size = ftell(fp) + 1;
    fclose(fp);

    fp = fopen(file, "r");
    shader = memset(malloc(size), '\0', size);
    fread(shader, 1, size - 1, fp);
    fclose(fp);

    return shader;
}

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


    const char* vertex_shader = readShader("Vertex.glsl");

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertex_shader, NULL);
    glCompileShader(vs);

    const char* fragment_shader = readShader("Fragment.glsl");
    
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