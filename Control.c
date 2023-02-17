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

    while(!glfwWindowShouldClose(window))
    {
        // placeholder so process can close itself
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}