// ch01.cpp : Defines the entry point for the console application.
//
//////////////////////////////////////////////////////////
//  triangles.cpp  from the OpenGL Red Book   Chapter 1
//////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>

#include "vgl.h"
#include "LoadShaders.h"
#include<fstream>
#include <string>
#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
#include <GL/freeglut.h>
/* GLM */
// #define GLM_MESSAGES
#define GLM_FORCE_RADIANS
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;

GLuint  vertexIndex = 0, facesValue = 0;
GLuint program;
//enum VAO_IDs { Triangles, NumVAOs };
const GLuint Triangles = 0, NumVAOs = 1;
//enum Buffer_IDs { ArrayBuffer, NumBuffers };
const GLuint ArrayBuffer = 0, NumBuffers = 1;
//enum Attrib_IDs { vPosition = 0 };
const GLuint vPosition = 0, vertexColor = 3;

GLuint NumVertices = 0;
GLfloat vertices[300][3];


GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint ColorBuffer[1];


GLuint faces[300];
GLuint facesBuffer[1];

float materialFileValues[12][3][3];


GLint uniform_mvp;
glm::mat4 model, view, projection, mvp;
char * delimiter = " ,'\n'";
enum colors
{
	red=0,
	blue=1,
	green=2,
	yellow=3,
	cyan=4,
	magenta = 5,
	white=6,
	gray=7,
	purple=8,
	orange=9,
	turquoise = 10,
	pink=11,
	brown=12
};

/////////////////////////////////////////////////////
//  int
/////////////////////////////////////////////////////
void init(void)
{
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	glMatrixMode(GL_PROJECTION);
	ShaderInfo  shaders[] = {{ GL_VERTEX_SHADER, "triangles.vert" },{ GL_FRAGMENT_SHADER, "triangles.frag" },{ GL_NONE, NULL }};
	program = LoadShaders(shaders);
	glUseProgram(program);
	
	// By now we have parsed the .obj for all vertices
	glGenBuffers(NumBuffers, Buffers); // Create 1 Buffer and put id in VAO's
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// End of vertices
	
	// Colors also should have been parsed by Now
	GLfloat verticesColor[8][3] = {
		{ 1.0f, 0.0f, 0.0f },	// Triangle 1
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }

	};

	glGenBuffers(1, ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColor), verticesColor, GL_STATIC_DRAW);
	// End Of Color
	
	// Face values 
	glGenBuffers(1, facesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesBuffer[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*facesValue, faces, GL_STATIC_DRAW);
	// End of Face
	 

	uniform_mvp = glGetUniformLocation(program, "mvp");

}

////////////////////////////////////////////////////////////////////
//	display
////////////////////////////////////////////////////////////////////
void display(void)
{
	int screen_width = 512, screen_height = 512;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Transformation
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, -4.0));
	view = glm::lookAt(glm::vec3(6.0, 6.0, 0.0), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 1.0, 0.0));
	projection = glm::perspective(45.0f, 1.0f*screen_width / screen_height, 0.1f, 10.0f);
	mvp = projection * view * model;
	// End Transformation
	glUseProgram(program);
	glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));
	


	glEnableVertexAttribArray(vPosition);
	glBindVertexArray(VAOs[Triangles]);
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glVertexAttribPointer(
		vPosition, // attribute
		3,                 // number of elements per vertex, here (x,y,z)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
		);

	glEnableVertexAttribArray(vertexColor);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer[0]);
	glVertexAttribPointer(
		vertexColor, // attribute
		3,                 // number of elements per vertex, here (R,G,B)
		GL_FLOAT,          // the type of each element
		GL_FALSE,          // take our values as-is
		0,                 // no extra data between each position
		0                  // offset of first element
		);

	/* Push each element in buffer_vertices to the vertex shader */
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesBuffer[0]);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	cout << endl << " Faces Size :: " << facesValue << " size" << size << endl;
	glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vertexColor);
	glutSwapBuffers();

	glFlush();
}
int getColorPosition(char *color)
{
	if (strcmp(color, "red") == 0)
	{
		return red;
	}
	else if (strcmp(color, "blue") == 0)
	{
		return blue;
	}
	else if (strcmp(color, "green") == 0)
	{
		return green;
	}
	else if (strcmp(color, "yellow") == 0)
	{
		return yellow;
	}
	else if (strcmp(color, "cyan") == 0)
	{
		return cyan;
	}
	else if (strcmp(color, "magenta") == 0)
	{
		return magenta;
	}
	else if (strcmp(color, "white") == 0)
	{
		return white;
	}
	else if (strcmp(color, "gray") == 0)
	{
		return gray;
	}
	else if (strcmp(color, "purple") == 0)
	{
		return purple;
	}
	else if (strcmp(color, "orange") == 0)
	{
		return orange;
	}
	else if (strcmp(color, "turquoise") == 0)
	{
		return turquoise;
	}
	else if (strcmp(color, "pink") == 0)
	{
		return pink;
	}
	else if (strcmp(color, "brown") == 0)
	{
		return brown;
	}
}
void readMaterialFile(char* fileName)
{
	FILE * fp;
	char *token;
	char *keyWord;
	char * delimiter = " ";
	char *lineRead = (char*)malloc(100);
	int colorPosition = 0;
	//fileName = "master.mtl";
	if ((fp = fopen(fileName, "r")) != NULL)
	{
		while (!feof(fp))
		{
			fgets(lineRead, 50, fp);
			{
				keyWord = strtok(lineRead, " ");
				while (keyWord != NULL)
				{
					if (strcmp(keyWord, "newmtl") == 0)
					{
						char *colorName = strtok(NULL,"\n");
						//cout << endl << "Color ::" << colorName;
						int pos = getColorPosition(colorName);
						if (pos >= 0 && pos <= 12)
						{
							colorPosition = pos;
						}
						else
						{
							cout << " Invalid Color " << colorName;// NO ENUM
						}
					}
					else if (strcmp(keyWord, "Ka") == 0)
					{
						//materialFileValues[colorPosition][1][0]
						//cout << endl;
						materialFileValues[colorPosition][0][0] = atof(strtok(NULL, delimiter));
						materialFileValues[colorPosition][0][1] = atof(strtok(NULL, delimiter));
						materialFileValues[colorPosition][0][2] = atof(strtok(NULL,"\n"));
					}
					else if (strcmp(keyWord, "Kd") == 0)
					{

						materialFileValues[colorPosition][1][0] = atof(strtok(NULL, delimiter));
						materialFileValues[colorPosition][1][1] = atof(strtok(NULL, delimiter));
						materialFileValues[colorPosition][1][2] = atof(strtok(NULL, "\n"));
					}
					else if (strcmp(keyWord, "Ks") == 0)
					{
						materialFileValues[colorPosition][2][0] = atof(strtok(NULL, delimiter));
						materialFileValues[colorPosition][2][1] = atof(strtok(NULL, delimiter));
						materialFileValues[colorPosition][2][2] = atof(strtok(NULL, "\n"));
					}
					keyWord = strtok(NULL, delimiter);
				}
			}
		}
	}
}
void printMaterialFile()
{
	for (int i = 0; i <= 12; i++)
	{
		cout << endl << "Color ::" << i << endl;
		for (int j = 0; j < 3; j++)
		{

			for (int k = 0; k < 3; k++)
			{
				cout <<"  "<<  materialFileValues[i][j][k];
			}
			cout << endl;
		}
	}
}
void printVertexValues()
{
	for (int i = 0; i < vertexIndex; i++)
	{
		cout << endl << " Vertex :: " << i << endl;
		for (int j = 0; j < 3; j++)
		{
			cout << vertices[i][j] << "  " ;
			
		}
	}
}
void printFacesValues()
{
	cout << endl;
	int printNewLine=0;
	for (int i = 0; i < facesValue; i++)
	{
		if (printNewLine > 2)
		{
			cout << endl;
			printNewLine = 0;
		}
		printNewLine++;
		cout << faces[i] << " ";
	}
}

void readInputFile(char* fileName)
{
	FILE * fp;
	char *token;
	char *keyWord;
	char *lineRead = (char*)malloc(100);
	ifstream objectFile;
	char* mtlLibName= (char*)malloc(100); // Will be the name of material LIB Fetched from .obj file

	if ((fp = fopen(fileName, "r")) != NULL)
	{
		//vertex = 0;
		facesValue = 0;
		while (!feof(fp))
		{
			fgets(lineRead, 50, fp);
			{
				keyWord = strtok(lineRead, " ");
				{
					
					while (keyWord  != NULL)
					{
						if (strcmp(keyWord,"mtllib") == 0 )
						{
							mtlLibName = strtok(NULL, delimiter);
							cout << endl <<  "Mat lib name ::"  << mtlLibName;
							readMaterialFile(mtlLibName);
							//printMaterialFile(); //For Debugging
						}
						else if (strcmp(keyWord,"v") == 0 )
						{
							vertices[vertexIndex][0] = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][1] = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][2] = atof(strtok(NULL, delimiter));
							vertexIndex++;
						}
						else if (strcmp(keyWord, "vn") == 0)
						{
							cout << endl;
							cout << "Vertex Noraml X :: " << strtok(NULL, delimiter);
							cout << "Vertex Noraml Y :: " << strtok(NULL, delimiter);
							cout << "Vertex Noraml Z :: " << strtok(NULL, delimiter);
						}
						else if (strcmp(keyWord, "vt") == 0)
						{
							cout << endl;
							cout << "vertex Texture 1 :: " << strtok(NULL, delimiter);
							cout << "vertex Texture 2 :: " << strtok(NULL, delimiter);
						}
						else if (strcmp(keyWord, "f") == 0)
						{
							char * value = strtok(NULL, delimiter);
							while (value != NULL)
							{
								faces[facesValue] = atoi(value)-1;
								facesValue++;
								value = strtok(NULL, delimiter);
							}
						}
						keyWord = strtok(NULL, delimiter);
					}
				}
			}
		}
		NumVertices = vertexIndex;// To keep Track of Number of Vertices , default is 0 . array is constructed using 300x3
	
	}
}
static void special(unsigned char key, int x_cord, int y_cord)
{

	string clr;
	switch (key)
	{
	case 's': // Shaded Mode
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 'w': // WireFrame Mode
		glLineWidth(1);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	default: return;
	}
	glutPostRedisplay();
}
////////////////////////////////////////////////////////////////////////
//	main
////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{

	//Code to read data from File
	char* fileName = "dodecahedron.obj";
	readInputFile(fileName);
	//printVertexValues();
	//printFacesValues();
	// End

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
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

	
	init();
	glutDisplayFunc(display);
	glutKeyboardFunc(special);
	glutMainLoop();

	return 0;
}

