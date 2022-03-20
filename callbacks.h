#ifndef _CALLBACK_H_
#define _CALLBACK_H_

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


void framesizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode);
void cursorCallback(GLFWwindow *window, double mx, double my);
void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

#endif