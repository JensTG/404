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
	GLFWwindow* parent = NULL;
	
	vec2 pos = vec2(0.0f); // Pixels, from upper left
	vec2 size = vec2(0.0f); // In pixels

	bool visible = true;
	Shader* program;
	int VAO;
	int indices = 6;

	int flags = 0;

	virtual void render() {
		if (parent == NULL)
			return;

		int width, height;
		glfwGetWindowSize(parent, &width, &height);

		mat4 transform(1.0f);
		transform = translate(transform, vec3((2 * pos.x + size.x) / width - 1, -(2 * pos.y + size.y) / height + 1, 0.0f));
		transform = scale(transform, vec3((float)(size.x / width), (float)(size.y / height), 0.0f));
		program->setMatrix("transform", transform);

		program->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
	}

	virtual void clicked(int x, int y) { // To be defined

	}

	virtual void dragged(int dx, int dy) { // For dragging self
		pos.x += dx;
		pos.y += dy;
	}
};

class WindowBorder : public ScreenItem { // Make sure nothing else happens when clicked or dragged
public:
	void clicked(int x, int y) override {}
	void dragged(int dx, int dy) override {}
};

class WindowBorderRight : public WindowBorder {
public:
	WindowBorderRight(GLFWwindow* parent_window) {		
		int w, h;
		parent = parent_window;
		glfwGetWindowSize(parent, &w, &h);

		printf("%d, %d\n", w, h);

		size = vec2(2, h);
		pos = vec2(w - 2, 0);

		printf("%f, %f", size.x, size.y);
	}

	void dragged(int dx, int dy) override {
		int w, h;
		pos.x += dx;

		glfwGetWindowSize(parent, &w, &h);
		glfwSetWindowSize(parent, w + dx, h);
	}
};