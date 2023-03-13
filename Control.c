#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct location {
    double xpos, ypos;
} current;

int serialOpen()
{
    char* device = "";

    int port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    return port;
}

void serialClose(int* port)
{
    close(*port);
}

void* sendMessage(void* ptr)
{
    struct location *current = (struct location*) ptr;
    printf("(%.0lf,%.0lf)\n", current->xpos, current->ypos);
}

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

static void cursorPosCallback(GLFWwindow* window, double x, double y)
{
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    current.xpos = xpos;
    current.ypos = ypos;

    pthread_t messenger;
    int status;

    status = pthread_create(&messenger, NULL, sendMessage, &current);
    if(status != 0)
    {
        printf("thread failed\n");
    } else {
        pthread_join(messenger, NULL);
    }
}

void escapeCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
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

    // int port = serialOpen();
    // if(port == -1)
    // {
    //     printf("Serial open failed\n");
    //     glfwTerminate();
    //     return 1;
    // } else if(!isatty(port)) 
    // {
    //     printf("Port not a tty\n");
    //     glfwTerminate();
    //     return 1; 
    // }

    // change built-in functions
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetKeyCallback(window, escapeCallback);

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

    float colours[] = {
        1.0f, 0.0f,  0.0f, // red
        0.0f, 1.0f,  0.0f, // green
        0.0f, 0.0f,  1.0f, // blue
        0.0f, 0.0f,  1.0f, // blue
        0.0f, 1.0f,  0.0f, // green
        1.0f, 0.0f,  0.0f  // red
    };

    // points vertex boundary object
    GLuint pvbo = 0;
    glGenBuffers(1, &pvbo);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW); // 18 float buffer

    // colors vertex boundary object
    GLuint cvbo = 0;
    glGenBuffers(1, &cvbo);
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glBufferData(GL_ARRAY_BUFFER, 18 * sizeof(float), points, GL_STATIC_DRAW);

    // data for drawn object
    GLuint vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, pvbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL); // variables made from 3 floats
    glBindBuffer(GL_ARRAY_BUFFER, cvbo);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL); // variables made from 3 floats
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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

    printf("Move the mouse to a part of the window to change the color of the LEDs connected to the Arduino.\n ** use the esc key to close the window **\n\n");    

    while(!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6); // draw 6 points
        
        glfwPollEvents();
        
        glfwSwapBuffers(window);
    }

    // serialClose(&port);
    glfwTerminate();

    return 0;
}