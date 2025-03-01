#pragma once

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
	ScreenItemFlags_
};

class ScreenItem {
	static vector<Placement> screen
};