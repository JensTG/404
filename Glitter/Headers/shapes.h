#pragma once
// Make VAOs from .s

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
using namespace std;

typedef int VAO;
typedef int EBO;
typedef int VBO;

// Generate a VAO from shape data in a file
VAO load_shape(const char* file_path);