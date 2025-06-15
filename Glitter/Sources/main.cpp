// Local Headers
#include "glitter.hpp"
#include "animation.h"
#include "shader.h"
#include "registering.h"

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <chrono>
using namespace chrono;

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <filesystem>

float vertices[] = {
	// positions          // colors           // texture coords
	 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
	 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
	-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
	-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left  
};
unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
};

int last_key_state[GLFW_KEY_LAST];
static vector<ScreenItem*> screen_space;

Animation2D small_head(0);
Animation2D body(1);
steady_clock::time_point prev_time = steady_clock::now();
duration<double, milli> tick_dur(1000/10);

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
ScreenItem* identify_stuff(double x, double y);
void tick();

int main(int argc, char * argv[]) {

	// ------------------------------------ Load GLFW and Create a Window ------------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	auto mWindow = glfwCreateWindow(mWidth, mHeight, "OpenGL", nullptr, nullptr);

	// Check for Valid Context
	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

	// Create Context and Load OpenGL Functions
	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
	glfwSetKeyCallback(mWindow, key_callback);
	stbi_set_flip_vertically_on_load(true);

	// ------------------------------------ Setup OpenGL
	unsigned int VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// ------------------------------------ Setup anims and shaders
	Shader program("C:\\404\\Glitter\\Shaders\\anim.v", "C:\\404\\Glitter\\Shaders\\anim.f");

	body.program = &program;
	body.VAO = VAO;

	body.load_frames("C:\\404\\Glitter\\Frames\\body", true);
	screen_space.push_back(&body);

	
	small_head.program = &program;
	small_head.VAO = VAO;

	small_head.load_frames("C:\\404\\Glitter\\Frames\\small_head", true);
	screen_space.push_back(&small_head);

	body.pos.y += 100;
	
	// ------------------------------------ Rendering Loop ------------------------------------
	while (glfwWindowShouldClose(mWindow) == false) {
		// Background Fill Color
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Render (backwards to match priority)
		for (int i = screen_space.size() - 1; i >= 0; i--)
			if (screen_space[i]->visible)
				screen_space[i]->render(mWindow);

		// Tick
		steady_clock::time_point now = steady_clock::now();
		if ((now - prev_time) > tick_dur) {
			prev_time = now;
			tick();
		}

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}   glfwTerminate();
	return EXIT_SUCCESS;
}

// Register input and single-click funcs
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_Q && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		small_head.change(1);

	last_key_state[key] = action;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	cout << "Mouseclick: " << xpos << ", " << ypos << '\n';

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		ScreenItem* item = identify_stuff(xpos, ypos);
		if (item != NULL)
			item->clicked(window, xpos, ypos);
	}
}

// Held keys
void processInput(GLFWwindow* window) {
}

// Make things happen
void tick() {
	small_head.change();
	body.change();
}

// Find the box
ScreenItem* identify_stuff(double x, double y) {
	for (ScreenItem* item : screen_space) {
		if (x >= item->pos.x && x <= (item->pos.x + item->size.x) && y >= item->pos.y && y <= (item->pos.y + item->size.y))
			return item;
	}
	return NULL;
}