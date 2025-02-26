#pragma once

#include <texture.h>
#include <filesystem>
#include <vector>
#include <string>

#include "shader.h"
using namespace std;
using namespace filesystem;


// Should flip through textures
class Animation2D {
public:
	void load_frames(string path) {
		for (directory_entry file : directory_iterator(path)) {
			Texture2D new_tex = Texture2D();
			string filepath = file.path().string();
			new_tex.Load(filepath.c_str());
			frames.push_back(new_tex);
		}
	}

	void bind(Shader program) {
		program.setInt("texture" + to_string(texture_num), texture_num);
		glActiveTexture(GL_TEXTURE0 + texture_num);
		try {
			frames[current].Bind();
		}
		catch (exception e) {
			fprintf(stderr, e.what());
		}
	}

	void change(int incr) {
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

private:
	vector<Texture2D> frames;
	vector<string> frame_paths;
	unsigned int texture_num;
	int current = 0;

	vector<string> get_sorted_paths(string path) {
		vector<string> paths;
		string dir;
		string ext;
		for (directory_entry file : directory_iterator(path)) {
			paths.push_back(file.path());
		}

	}

};