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
    int width, height;
    int serial;
} current;


int serialOpen()
{
    struct termios config;

    char* device = "/dev/ttyACM0";

    int port = open(device, O_RDWR | O_NOCTTY | O_NDELAY);

    if(port == -1)
    {
        printf("Serial open failed\n1) ensure you are in the dialout group for access to serial ports \n  $ sudo usermod -a -G dialout <username>\n  restart computer\n2) check that the device is properly connected\n\n");
        glfwTerminate();
        exit(1);
    } else if(!isatty(port)) 
    {
        printf("Port not a tty\n");
        glfwTerminate();
        exit(1);
    } else if(tcgetattr(port, &config) < 0)
    {
        printf("Could not get terminal attributes\n");
        glfwTerminate();
        exit(1);
    }

    if(cfsetospeed(&config, B9600) < 0)
    {
        printf("Invallid baud rate");
        glfwTerminate();
        exit(1);
    }

    return port;
}


void serialClose(int* port)
{
    close(*port);
}


void* sendMessage(void* ptr)
{
    struct location *current = (struct location*) ptr;
    
    // change coords into a percentage of the screen
    double sendX = (current->xpos / current->width);
    double sendY = current->ypos / current->height;

    char send;
    if(sendX < 0.5)
    {
        if(sendY < 0.5)
        {
            send = 'G';
        } else {
            send = 'B';
        }
    } else {
        if(sendY < 0.5)
        {
            send = 'Y';
        } else {
            send = 'R';
        }
    }

    write(current->serial, &send, sizeof(char));    
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
    int windowWidth = mon->width / 2;
    int windowHeight = mon->height / 2;
    GLFWwindow* window = glfwCreateWindow(mon->width / 2, windowHeight, "Light control", NULL, NULL);
    if(!window) {
        printf("GLFW window failed\n");
        glfwTerminate();
        return 1;
    }

    int port = serialOpen();
    current.serial = port;
    current.width = windowWidth;
    current.height = windowHeight;

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

    serialClose(&port);
    glfwTerminate();

    return 0;
}