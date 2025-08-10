#pragma once

#include <texture.h>
#include <filesystem>
#include <vector>
#include <map>
#include <string>
#include "registering.h"

#include "shader.h"
using namespace std;
using namespace filesystem;


// Should flip through textures
class Animation2D : public ScreenItem {
public:
	void load_frames(string path, bool resize = false) {
		map<int, string> paths = get_sorted_paths(path);
		for (const auto path : paths) {
			Texture2D new_tex = Texture2D();
			new_tex.Load(path.second.c_str());
			frames.push_back(new_tex);
		}
		if (resize)
			size = vec2(frames[0].Width, frames[0].Height);
	}

	void bind(Shader* program) {
		program->use();
		program->setInt("texture_num", texture_num);
		glActiveTexture(GL_TEXTURE0 + texture_num);
		try {
			frames[current].Bind();
		}
		catch (exception e) {
			fprintf(stderr, e.what());
		}
	}

	void change(int incr = 1) {
		current += incr;
		if (current >= frames.size())
			current = 0;
		else if (current < 0)
			current = frames.size() - 1;
	}

	Animation2D(unsigned int tex_num) {
		texture_num = tex_num;
	}

	Animation2D(unsigned int tex_num, string path) {
		texture_num = tex_num;
		load_frames(path);
	}

	void render() override {
		bind(program);
		ScreenItem::render();
	}

private:
	vector<Texture2D> frames;
	vector<string> frame_paths;
	unsigned int texture_num;
	int current = 0;

	map<int, string> get_sorted_paths(string path) {
		map<int, string> paths;
		for (directory_entry file : directory_iterator(path)) {
			string path = file.path().string();
			int index = stoi(file.path().stem().string());
			paths.insert({ index, path });
		}
		return paths;
	}

};