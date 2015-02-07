/*
Rahul Pradeep Kamath
Assignment 1
CS 433/533
*/

#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <string>
#include "vgl.h"
#include "LoadShaders.h"

using namespace std;

int displaySecondTriangle = 1, displayFirstTriangle = 1;
//enum VAO_IDs { Triangles, NumVAOs };
const GLuint Triangles = 0, NumVAOs = 3;
//enum Buffer_IDs { ArrayBuffer, NumBuffers };
const GLuint ArrayBuffer = 0, NumBuffers = 3;// one for the 2 Triangles , 1 for colourful triangle , 1 for circle
//enum Attrib_IDs { vPosition = 0 };
const GLuint vPosition = 0;
const GLuint vPosition1 = 1;
const GLuint vPosition2 = 2;
const GLuint vertexColor = 3;
GLuint program, program1, program2;
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint BufferColour[1];
// Variables for Circle
GLfloat **verticesCircle;
float numberOfTriangles;
float radius;
GLuint verticesRequired;
const float PI = 3.14159;
//
const GLuint NumVertices = 6;
int wireMode = 1;
int circleEnabled = 0;
GLfloat red = 0.0f, blue = 0.0f, green = 0.0f;
int colorEnabled;
// Dynamic allocation Of Matrix based On user Inputs
void allocateMatrix()
{
	GLfloat *vals;
	vals = (GLfloat*)malloc(verticesRequired * 2 * sizeof(GLfloat));
	verticesCircle = (GLfloat**)malloc(verticesRequired * sizeof(GLfloat*));
	for (int i = 0; i < verticesRequired; i++)
	{
		verticesCircle[i] = &(vals[i * 2]);
	}
	for (int i = 0; i < verticesRequired; i++)
	{
		verticesCircle[i][0] = 0.0f;
		verticesCircle[i][1] = 0.0f;
	}
}

// KeyBoard Callback Fwd Declaration
static void special(unsigned char key, int x_cord, int y_cord);

void createCircleData(float radius)
{
	circleEnabled = 1;
	glBindVertexArray(VAOs[2]);
	verticesRequired = 3 + (numberOfTriangles - 1); // 1 Vertex is common [0,0] , the 1st triangle needs 2 Vertices apart from [0,0] and the other triangles need 1 vertice .
	float incrementValue = 360.0f / numberOfTriangles;
	/*cout << endl << " User Entered Radius " << radius << " Triangles " << numberOfTriangles << "Vertices Required " << verticesRequired
	<< " Increment Value " << incrementValue << endl;*/
	allocateMatrix();
	int rowCount = 1;
	for (float i = 0.0f; i <= 361.0f; i = i + incrementValue)
	{
		GLfloat x = radius*sin((i*PI) / 180.0f);
		GLfloat y = radius*cos((i*PI) / 180.0f);
		verticesCircle[rowCount][0] = x;
		verticesCircle[rowCount][1] = y;
		rowCount++;
	}
	/*cout << " X : " << verticesCircle[0][0] << " Y : " << verticesCircle[0][1] << endl;
	rowCount = 1;
	for (int i = 1; i < 360; i = i + incrementValue)
	{
	cout << " X : " << verticesCircle[rowCount][0] << " Y : " << verticesCircle[rowCount][1] << endl;
	rowCount++;
	}*/

	glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, (verticesRequired * 2 * sizeof(GLfloat)), *verticesCircle, GL_DYNAMIC_DRAW);
	ShaderInfo  shaders2[] = {
		{ GL_VERTEX_SHADER, "circles.vert" },
		{ GL_FRAGMENT_SHADER, "circles.frag" },
		{ GL_NONE, NULL }
	};

	program2 = LoadShaders(shaders2);
	/*GLint loc1 = glGetUniformLocation(program2, "customColour");
	glProgramUniform4f(program2, loc1, red, green, blue, 0.0f);*/
	glUseProgram(program2);

	glVertexAttribPointer(vPosition2, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vPosition2);

}
// Initialization
void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	/*for (int i = 0; i < NumVAOs; i++)
	{
	cout << endl << "VAO " << i << " :" << VAOs[i];
	}*/
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

	/*for (int i = 0; i < NumBuffers; i++)
	{
	cout << endl << "Buffer Name " << i << " :" << Buffers[i];
	}*/
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	ShaderInfo  shaders[] = {
		{ GL_VERTEX_SHADER, "triangles.vert" },
		{ GL_FRAGMENT_SHADER, "triangles.frag" },
		{ GL_NONE, NULL }
	};

	program = LoadShaders(shaders);
	/*GLint loc1 = glGetUniformLocation(program, "customColour");
	glProgramUniform4f(program, loc1, 1.0f, 0.0f, 0.0f, 0.0f);*/
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
	GLfloat verticesColour[3][3] = {
		{ 1.0f, 0.0f, 0.0f },	// Triangle 3
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 0.0f, 1.0f }
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

	glGenBuffers(1, BufferColour);
	glBindBuffer(GL_ARRAY_BUFFER, BufferColour[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColour), verticesColour, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexColor, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
	glEnableVertexAttribArray(vertexColor);
}
static void special(unsigned char key, int x_cord, int y_cord)
{

	string clr;
	switch (key)
	{
	case 'x': // Display FirstTriangle
		if (displayFirstTriangle)
		{
			displayFirstTriangle = 0;
		}
		else
		{
			displayFirstTriangle = 1;
		}
		break;
	case 's': // Shaded Mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'w': // WireFrame Mode
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'y': // Toggle Second colorful triangle
		if (displaySecondTriangle)
		{
			displaySecondTriangle = 0;
		}
		else
		{
			displaySecondTriangle = 1;
		}
		break;
	case 'g': // Generate Circle
		cout << " Circle Generation ";
		cout << " \nEnter Radius :: ";
		cin >> radius;
		cout << " \nEnter Number Of Triangle Steps :: ";
		cin >> numberOfTriangles;
		//cin.ignore(NULL, '\n');
		createCircleData(radius);
		break;
	case 'z': //  Toggle Circle
		if (circleEnabled)
		{
			circleEnabled = 0;
		}
		else
		{
			circleEnabled = 1;
		}
		break;

	case 'q': // Q ,  Quit
		exit(0);
		break;
	case 27: // Escape Key
		exit(0);
		break;
	case 'c':
		char colors[30];
		colorEnabled = 1;
		//cin.ignore(10, '\n');
		cout << "\nEnter the colors ";
		cout << "\nEnter Red ";
		cin >> red;
		cout << "\nEnter Green ";
		cin >> green;
		cout << "\nEnter Blue";
		cin >> blue;
		//getline(cin,clr);
		//cin >> clr;

		/*for (int i = 0; i < clr.length(); i++)
		{
		colors[i] = clr[i];
		}
		char *val;
		val = strtok(colors, " ");
		if (val != NULL)
		red = atof(val);
		val = strtok(NULL, " ");
		if (val != NULL)
		blue = atof(val);
		val = strtok(NULL, " ");
		if (val != NULL)
		green = atof(val);
		cout << "\n Values :: " << red << " " << blue << " " << green;*/
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
		if (colorEnabled)
		{
			GLint loc1 = glGetUniformLocation(program, "customColour");
			glProgramUniform4f(program, loc1, red, green, blue, 0.0f);
		}
		glUseProgram(program);
		glEnableVertexAttribArray(vPosition);
		glDrawArrays(GL_TRIANGLES, 0, 6);

	}
	if (displaySecondTriangle)
	{
		glBindVertexArray(VAOs[1]);
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[1]);
		glEnableVertexAttribArray(vPosition1);
		glBindBuffer(GL_ARRAY_BUFFER, BufferColour[0]);
		glEnableVertexAttribArray(vertexColor);
		glUseProgram(program1);

		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
	// Circle
	if (circleEnabled)
	{
		glBindVertexArray(VAOs[2]);
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[2]);
		if (colorEnabled)
		{
			GLint loc1 = glGetUniformLocation(program2, "customColour");
			glProgramUniform4f(program2, loc1, red, green, blue, 0.0f);
		}
		glUseProgram(program2);
		glEnableVertexAttribArray(vPosition2);
		glDrawArrays(GL_TRIANGLE_FAN, 0, verticesRequired);
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

	/*cout << "GL Vendor            :" << vendor << endl;
	cout << "GL Renderer          :" << renderer << endl;
	cout << "GL Version (string)  :" << version << endl;
	cout << "GL Version (integer) :" << major << " " << minor << endl;
	cout << "GLSL Version         :" << glslVersion << endl;*/
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

