#pragma once

#include "shader.h"
#include "animation.h"
#include "texture.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;

#include <vector>
using namespace std;

typedef int ScreenItemFlags;
enum ScreenItemFlags_ {
	ScreenItemFlags_Nothing = 0,
	ScreenItemFlags_Visible = 1 << 1,
	ScreenItemFlags_Clickable = 1 << 2,
	ScreenItemFlags_Dragable = 1 << 3
};

class ScreenItem {
public:
	vec2 pos = vec2(0.0f); // Pixels, from upper left
	vec2 size = vec2(0.0f); // In pixels

	bool visible = true;
	Shader* program;
	int VAO;
	int indices = 6;

	int flags = 0;

	virtual void render(GLFWwindow* window) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		mat4 transform(1.0f);
		transform = translate(transform, vec3((2 * pos.x + size.x) / width - 1, -(2 * pos.y + size.y) / height + 1, 0.0f));
		transform = scale(transform, vec3((float)(size.x / width), (float)(size.y / height), 0.0f));
		program->setMatrix("transform", transform);

		program->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
	}

	void clicked(GLFWwindow* window, double x, double y) {
		pos.x += 20;
		
		/*
		if ((flags & ScreenItemFlags_Clickable) == ScreenItemFlags_Clickable)
			click_func(window, x, y);
		*/
	}

	void dragged(GLFWwindow* window, double dx, double dy) {
		if ((flags & ScreenItemFlags_Dragable) == ScreenItemFlags_Dragable)
			click_func(window, dx, dy);
	}

	void assign_click_func(void (*func)(GLFWwindow* window, double x, double y)) {
		click_func == func;
	}

private:
	void (*click_func)(GLFWwindow* window, double x, double y) = NULL;
	void (*drag_func)(GLFWwindow* window, double dx, double dy) = NULL;
};