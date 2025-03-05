#pragma once

#include "shader.h"
#include "animation.h"
#include "texture.h"

#include <glm/glm.hpp>
using namespace glm;

#include <vector>
using namespace std;

typedef struct {
	vec2 pos;
	vec2 size;
} Placement;

typedef int ScreenItemFlags;
enum ScreenItemFlags_ {
	ScreenItemFlags_Draggable = 0,
	ScreenItemFlags_Clickable = 1 << 0,
	ScreenItemFlags_Visible = 1 << 1
};

static vector<ScreenItem> screen_space;

class ScreenItem {
	Placement pos;

	Shader* program;
	int VAO;


	void render() {

	}
};