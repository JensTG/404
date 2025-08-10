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
int last_mouse_button_state[GLFW_MOUSE_BUTTON_LAST];
vec2 last_mouse_pos;
vec2 last_click_pos;
static vector<ScreenItem*> screen_space;
ScreenItem* clicked_item = NULL;

steady_clock::time_point prev_time = steady_clock::now();
duration<double, milli> tick_dur(1000/10);

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void window_resize_callback(GLFWwindow* window, int width, int height);
ScreenItem* identify_stuff(double x, double y);
void tick();

// ---------------------------------------------------------- MAIN ----------------------------------------------------------
int main(int argc, char * argv[]) {
	// ------------------------------------ Load GLFW and Create a Window
	// Basic
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Flavour
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	//glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

	glfwWindowHint(GLFW_POSITION_X, 0);
	glfwWindowHint(GLFW_POSITION_Y, 0);

	auto mWindow = glfwCreateWindow(200, 200, "OpenGL", NULL, NULL);

	// Check for Valid Context
	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

	// Create Context and Load OpenGL Functions
	glfwMakeContextCurrent(mWindow);
	gladLoadGL();
	fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

	glfwSetCursorPosCallback(mWindow, cursor_pos_callback);
	glfwSetMouseButtonCallback(mWindow, mouse_button_callback);
	glfwSetKeyCallback(mWindow, key_callback);
	glfwSetWindowSizeCallback(mWindow, window_resize_callback);
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

	// ------------------------------------ Setup shaders
	Shader anim_program("C:\\404\\Glitter\\Shaders\\anim.v", "C:\\404\\Glitter\\Shaders\\anim.f");
	Shader solid_program("C:\\404\\Glitter\\Shaders\\solid.v", "C:\\404\\Glitter\\Shaders\\solid.f");

	// ------------------------------------ ScreenItems
	Animation2D small_head(0);
	Animation2D body(1);
	WindowBorderRight wbr(mWindow);

	body.program = &anim_program;
	body.VAO = VAO;
	body.parent = mWindow;

	body.load_frames("C:\\404\\Glitter\\Frames\\body", true);
	
	small_head.program = &anim_program;
	small_head.VAO = VAO;
	small_head.parent = mWindow;

	small_head.load_frames("C:\\404\\Glitter\\Frames\\small_head", true);

	body.pos.y += 100;

	wbr.program = &solid_program;
	wbr.VAO = VAO;

	solid_program.setVec4("color", vec4(0.0f, 0.0f, 0.7f, 1.0f));
	
	//screen_space.push_back(&body);
	//screen_space.push_back(&small_head);
	screen_space.push_back(&wbr);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// ------------------------------------ Rendering Loop
	while (glfwWindowShouldClose(mWindow) == false) {
		// Background Fill Color
		glClear(GL_COLOR_BUFFER_BIT);

		// Render (backwards to match priority)
		for (int i = screen_space.size() - 1; i >= 0; i--)
			if (screen_space[i]->visible)
				screen_space[i]->render();

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

// ---------------------------------------------------------- CALLBACKS ----------------------------------------------------------
// Register input and single-click funcs
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key == GLFW_KEY_Q && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
		glfwSetWindowShouldClose(window, GL_TRUE);

	last_key_state[key] = action;
}

static void window_resize_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	vec2 current_mouse_pos = vec2(xpos, ypos);
	if (current_mouse_pos != last_mouse_pos && clicked_item != NULL) {
		clicked_item->dragged(current_mouse_pos.x - last_mouse_pos.x, current_mouse_pos.y - last_mouse_pos.y);
	}
	
	last_mouse_pos = vec2(xpos, ypos);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		last_click_pos = vec2(xpos, ypos);
		clicked_item = identify_stuff(xpos, ypos);
	}
	else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		if (clicked_item != NULL && distance(last_click_pos, vec2(xpos, ypos)) < 10) {
			clicked_item->pos += last_click_pos - vec2(xpos, ypos);
			clicked_item->clicked(xpos, ypos);
		}
		clicked_item = NULL;
	}

	last_mouse_button_state[button] == action;
}

// ---------------------------------------------------------- OTHER FUNCS ----------------------------------------------------------
// Held keys
void processInput(GLFWwindow* window) {
}

// Make things happen
void tick() {
	// Figure this out
}

// Find the box
ScreenItem* identify_stuff(double x, double y) {
	for (ScreenItem* item : screen_space) {
		if (x >= item->pos.x && x <= (item->pos.x + item->size.x) && y >= item->pos.y && y <= (item->pos.y + item->size.y))
			return item;
	}
	return NULL;
}