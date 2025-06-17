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

	virtual void clicked(double x, double y) { // To be defined
		pos.x += 20;
	}

	virtual void dragged(double dx, double dy) { // For dragging self
		pos.x += dx;
		pos.y += dy;
	}
};