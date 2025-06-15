#include "shapes.h"
#include "array"

VAO load_shape(const char* file_path) {
	ifstream sf;
	stringstream ss;
	
	VAO VAO;
	EBO EBO;
	VBO VBO;

	vector<float> vertices;
	vector<float> indices;

	// Open file
	sf.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		sf.open(file_path);
		ss << sf.rdbuf();
		sf.close();
	}
	catch (exception e) {
		cout << "ERROR::SHAPE::FILE\n" << e.what();
		return -1;
	}

	// Read through lines
	char c;
	float f;
	while (ss.get()) {
		switch (c)
		{
		case 'v':
			break;
		case 'i':
			while (ss >> f)

			break;
		default:
			break;
		}
	}

	return 0;
}