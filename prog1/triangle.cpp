// ch01.cpp : Defines the entry point for the console application.
//
//////////////////////////////////////////////////////////
//  triangles.cpp  from the OpenGL Red Book   Chapter 1
//////////////////////////////////////////////////////////

#include <iostream>
using namespace std;

#include "vgl.h"
#include "LoadShaders.h"

int displaySecondTriangle = 1, displayFirstTriangle=1;
//enum VAO_IDs { Triangles, NumVAOs };
const GLuint Triangles = 0, NumVAOs = 2;
//enum Buffer_IDs { ArrayBuffer, NumBuffers };
const GLuint ArrayBuffer = 0, NumBuffers = 2;
//enum Attrib_IDs { vPosition = 0 };
const GLuint vPosition = 0;
const GLuint vPosition1 = 1;
GLuint program, program1;
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];

const GLuint NumVertices = 6;
int wireMode = 1;

static void special(unsigned char key, int x_cord, int y_cord);
/////////////////////////////////////////////////////
//  int
/////////////////////////////////////////////////////
void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	GLfloat vertices[NumVertices][2] = {
		{ -0.90f, -0.9f },	// Triangle 1
		{ 0.85f, -0.9f },
		{ -0.90f, 0.85f },
		{ 0.90f, -0.85f },	// Triangle 2
		{ 0.90f, 0.90f },
		{ -0.85f, 0.90f } };

	glGenBuffers(NumBuffers, Buffers);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	glUseProgram(program);

	glVertexAttribPointer(vPosition, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition);


	// 2nd Triangle
	glBindVertexArray(VAOs[1]);
	GLfloat vertices2[3][2] = {
		{ 0.0f, 0.5f },	// Triangle 3
		{ -0.5f, -0.5f },
		{ 0.5f, -0.5f }
	};
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW);
	ShaderInfo  shaders1[] = {
		{ GL_VERTEX_SHADER, "triangles1.vert" },
		{ GL_FRAGMENT_SHADER, "triangles1.frag" },
		{ GL_NONE, NULL }
	};

	program1 = LoadShaders(shaders1);
	glUseProgram(program1);

	glVertexAttribPointer(vPosition1, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition1);

}
static void special(unsigned char key, int x_cord, int y_cord){
	cout << key;
	switch (key) {

	case 'x' :
		if (displayFirstTriangle)
		{
			displayFirstTriangle = 0;
		}
		else
		{
			displayFirstTriangle = 1;
		}
	case 's' :
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			break;
	case 'w':
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'y':
		if (displaySecondTriangle)
		{
			displaySecondTriangle = 0;
		}
		else
		{
			displaySecondTriangle = 1;
		}
		break;
	default: return;
	}
	glutPostRedisplay();
}
////////////////////////////////////////////////////////////////////
//	display
////////////////////////////////////////////////////////////////////
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	
	if (displayFirstTriangle)
	{
		glBindVertexArray(VAOs[Triangles]);
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
		glUseProgram(program);
		glEnableVertexAttribArray(vPosition);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}
	if (displaySecondTriangle)
	{
		glBindVertexArray(VAOs[1]);
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
		glUseProgram(program1);
		glEnableVertexAttribArray(vPosition1);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}

	glFlush();
}

////////////////////////////////////////////////////////////////////////
//	main
////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(512, 512);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);// GLUT_COMPATIBILITY_PROFILE );
	glutCreateWindow(argv[0]);

	glewExperimental = GL_TRUE;	// added for glew to work!
	if (glewInit())
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit(EXIT_FAILURE);
	}

	// code from OpenGL 4 Shading Language cookbook, second edition
	const GLubyte *renderer = glGetString(GL_RENDERER);
	const GLubyte *vendor = glGetString(GL_VENDOR);
	const GLubyte *version = glGetString(GL_VERSION);
	const GLubyte *glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	cout << "GL Vendor            :" << vendor << endl;
	cout << "GL Renderer          :" << renderer << endl;
	cout << "GL Version (string)  :" << version << endl;
	cout << "GL Version (integer) :" << major << " " << minor << endl;
	cout << "GLSL Version         :" << glslVersion << endl;
	//
	//GLint nExtensions;
	//glGetIntegerv( GL_NUM_EXTENSIONS, &nExtensions );
	//for ( int i = 0; i < nExtensions; i++ )
	//	cout << glGetStringi( GL_EXTENSIONS, i )  << endl;

	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(special);
	glutMainLoop();

	return 0;
}

