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
	ScreenItemFlags_Draggable = 0,
	ScreenItemFlags_Clickable = 1 << 0,
	ScreenItemFlags_Visible = 1 << 1
};

class ScreenItem {
public:
	vec2 pos = vec2(0.0f); // Pixels, from bottom left
	vec2 size = vec2(0.0f); // In pixels

	bool visible = true;
	Shader* program;
	int VAO;
	int indices = 6;

	virtual void render(GLFWwindow* window) {
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		mat4 transform(1.0f);
		transform = scale(transform, vec3((float)(size.x / width), (float)(size.y / height), 0.0f));
		transform = translate(transform, vec3(pos.x / size.x, pos.y / size.y, 0.0f));
		program->setMatrix("transform", transform);

		program->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices, GL_UNSIGNED_INT, 0);
	}
};