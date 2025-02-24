#pragma once

#include <texture.h>
#include <filesystem>
#include <vector>
#include <string>
using namespace std;
using namespace filesystem;


// Should flip through textures
class Animation2D {
public:
	void load_frames(string path) {
		for (directory_entry file : directory_iterator(path)) {
			Texture2D new_tex = Texture2D();
			new_tex.Load((const char*)file.path().c_str());
			frames.push_back(new_tex);
		}
		
	}

	void next() {

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
	unsigned int texture_num;

};