#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <math.h>

unsigned int g_windowWidth = 800;
unsigned int g_windowHeight = 600;
char* g_windowName = "HW1-OpenGL-Basics";

GLFWwindow* g_window;

// model data
std::vector<float> g_meshVertices; //X1Y1Z1 coordinates of the vertices

std::vector<float> g_meshNormals; //Normals for each vertex

std::vector<float> g_meshCount; //Triangle Count

std::vector<unsigned int> g_meshIndices; //A1B1C1 are the vertices from meshVertices used for the triangle


GLfloat g_modelViewMatrix[16];

void computeNormals()
{
	g_meshNormals.resize(g_meshVertices.size());

	// the code below sets all normals to point in the z-axis, so we get a boring constant gray color
	// the following should be replaced with your code for normal computation (Task 1)

	/*
	for (int v = 0; v < g_meshNormals.size() / 3; ++v)
	{
	g_meshNormals[3 * v + 2] = 1.0;
	}
	*/

	
	for (int v = 0; v < g_meshIndices.size() / 3; v++)
	{
		float ba_X = g_meshVertices[3 * g_meshIndices[3 * v + 1]] - g_meshVertices[3 * g_meshIndices[3 * v]];
		float ba_Y = g_meshVertices[3 * g_meshIndices[3 * v + 1] + 1] - g_meshVertices[3 * g_meshIndices[3 * v] + 1];
		float ba_Z = g_meshVertices[3 * g_meshIndices[3 * v + 1] + 2] - g_meshVertices[3 * g_meshIndices[3 * v] + 2];

		float ca_X = g_meshVertices[3 * g_meshIndices[3 * v + 2]] - g_meshVertices[3 * g_meshIndices[3 * v]];
		float ca_Y = g_meshVertices[3 * g_meshIndices[3 * v + 2] + 1] - g_meshVertices[3 * g_meshIndices[3 * v] + 1];
		float ca_Z = g_meshVertices[3 * g_meshIndices[3 * v + 2] + 2] - g_meshVertices[3 * g_meshIndices[3 * v] + 2];

		float cp_X = ((ba_Y * ca_Z) - (ba_Z * ca_Y));
		float cp_Y = ((ba_Z * ca_X) - (ba_X * ca_Z));
		float cp_Z = ((ba_X * ca_Y) - (ba_Y * ca_X));

		g_meshNormals[3 * g_meshIndices[3 * v]] += cp_X;
		g_meshNormals[3 * g_meshIndices[3 * v] + 1] += cp_Y;
		g_meshNormals[3 * g_meshIndices[3 * v] + 2] += cp_Z;

		g_meshNormals[3 * g_meshIndices[3 * v + 1]] += cp_X;
		g_meshNormals[3 * g_meshIndices[3 * v + 1] + 1] += cp_Y;
		g_meshNormals[3 * g_meshIndices[3 * v + 1] + 2] += cp_Z;

		g_meshNormals[3 * g_meshIndices[3 * v + 2]] += cp_X;
		g_meshNormals[3 * g_meshIndices[3 * v + 2] + 1] += cp_Y;
		g_meshNormals[3 * g_meshIndices[3 * v + 2] + 2] += cp_Z;
	}
	
	for (int v = 0; v < g_meshNormals.size() / 3; v++)
	{
		float mag = sqrt(pow(g_meshNormals[3 * v], 2) + pow(g_meshNormals[3 * v + 1], 2) + pow(g_meshNormals[3 * v + 2], 2));

		g_meshNormals[3 * v] = g_meshNormals[3 * v] / mag;
		g_meshNormals[3 * v + 1] = g_meshNormals[3 * v + 1] / mag;
		g_meshNormals[3 * v + 2] = g_meshNormals[3 * v + 2] / mag;
	}

}


void loadObj(std::string p_path)
{
	std::ifstream nfile;
	nfile.open(p_path);
	std::string s;

	while (nfile >> s)
	{
		if (s.compare("v") == 0)
		{
			float x, y, z;
			nfile >> x >> y >> z;
			g_meshVertices.push_back(x);
			g_meshVertices.push_back(y);
			g_meshVertices.push_back(z);
		}		
		else if (s.compare("f") == 0)
		{
			std::string sa, sb, sc;
			unsigned int a, b, c;
			nfile >> sa >> sb >> sc;

			a = std::stoi(sa);
			b = std::stoi(sb);
			c = std::stoi(sc);

			g_meshIndices.push_back(a - 1);
			g_meshIndices.push_back(b - 1);
			g_meshIndices.push_back(c - 1);
		}
		else
		{
			std::getline(nfile, s);
		}
	}

	computeNormals();

	std::cout << p_path << " loaded. Vertices: " << g_meshVertices.size() / 3 << " Triangles: " << g_meshIndices.size() / 3 << std::endl;
}

double getTime()
{
	return glfwGetTime();
}

void glfwErrorCallback(int error, const char* description)
{
	std::cerr << "GLFW Error " << error << ": " << description << std::endl;
	exit(1);
}

void glfwKeyCallback(GLFWwindow* p_window, int p_key, int p_scancode, int p_action, int p_mods)
{
	if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(g_window, GL_TRUE);
	}
}

void initWindow()
{
	// initialize GLFW
	glfwSetErrorCallback(glfwErrorCallback);
	if (!glfwInit())
	{
		std::cerr << "GLFW Error: Could not initialize GLFW library" << std::endl;
		exit(1);
	}

	g_window = glfwCreateWindow(g_windowWidth, g_windowHeight, g_windowName, NULL, NULL);
	if (!g_window)
	{
		glfwTerminate();
		std::cerr << "GLFW Error: Could not initialize window" << std::endl;
		exit(1);
	}

	// callbacks
	glfwSetKeyCallback(g_window, glfwKeyCallback);

	// Make the window's context current
	glfwMakeContextCurrent(g_window);

	// turn on VSYNC
	glfwSwapInterval(1);
}

void initGL()
{
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLfloat) g_windowWidth / (GLfloat)g_windowHeight, 0.1f, 10.0f);
}

void clearModelViewMatrix()
{
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 4; ++j)
		{
			g_modelViewMatrix[4 * i + j] = 0.0f;
		}
	}
}

void updateModelViewMatrix()
{	
	clearModelViewMatrix();


	// the following code sets a static modelView matrix
	// this should be replaced with code implementing Task 2 (teapot rotation)

	g_modelViewMatrix[0] = cos(getTime());
	g_modelViewMatrix[2] = sin(getTime());
	g_modelViewMatrix[5] = 1.0f;
	g_modelViewMatrix[8] = -sin(getTime());
	g_modelViewMatrix[10] = cos(getTime());

	g_modelViewMatrix[14] = -5.0f;
	g_modelViewMatrix[15] = 1.0f;

}

void setModelViewMatrix()
{
	glMatrixMode(GL_MODELVIEW);
	updateModelViewMatrix();
	glLoadMatrixf(g_modelViewMatrix);
}

void render()
{
	setModelViewMatrix();

	glBegin(GL_TRIANGLES);

	for (size_t f = 0; f < g_meshIndices.size(); ++f)
	{
		const float scale = 0.1f;
		const unsigned int idx = g_meshIndices[f];
		const float x = scale * g_meshVertices[3 * idx + 0];
		const float y = scale * g_meshVertices[3 * idx + 1];
		const float z = scale * g_meshVertices[3 * idx + 2];

		const float nx = g_meshNormals[3 * idx + 0];
		const float ny = g_meshNormals[3 * idx + 1];
		const float nz = g_meshNormals[3 * idx + 2];
				
		glNormal3f(nx, ny, nz);
		glVertex3f(x, y, z);
	}

	glEnd();
	
}

void renderLoop()
{
	while (!glfwWindowShouldClose(g_window))
	{
		// clear buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		render();

		// Swap front and back buffers
		glfwSwapBuffers(g_window);

		// Poll for and process events
		glfwPollEvents();
	}
}

int main()
{
	initWindow();
	initGL();
	loadObj("data/teapot.obj");
	renderLoop();
}
