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


const int _MAX_ROWS_ = 3000;
struct materialValues
{
	char color[20];
	float ka[3];// = { 0.1f, 0.1f, 0.1f };// Default value as per specs
	float kd[3];// = { 0.9f, 0.9f, 0.9f };// Default value as per specs

};
struct materialValues materialFileValues[100];
int materialFileValuesIndex = 0;


GLuint  vertexIndex = 0, facesValue = 0, normalValue = 0, verticesNormalsIndex = 0;
GLuint program;
//enum VAO_IDs { Triangles, NumVAOs };
const GLuint Triangles = 0, NumVAOs = 1;
//enum Buffer_IDs { ArrayBuffer, NumBuffers };
const GLuint ArrayBuffer = 0, NumBuffers = 1;
//enum Attrib_IDs { vPosition = 0 };
const GLuint vPosition = 0, vertexColor = 3;

GLuint NumVertices = 0;
GLfloat vertices[_MAX_ROWS_][3];
GLfloat verticesNormal[_MAX_ROWS_][3];
GLfloat finalVertices[_MAX_ROWS_][3], finalVerticesNormals[_MAX_ROWS_][3];
GLuint faces[_MAX_ROWS_];	GLuint facesBuffer[1];


GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
GLuint ColorBuffer[1];




GLint uniform_mvp;
glm::mat4 model, view, projection, mvp,trans;
char * delimiter = " ,'\n'";
float maxXRange = 0.0f, minXRange = 999.0f, maxYRange = 0.0f, minYRange = 999.0f, maxZRange = 0.0f, minZRange = 999.0f;
GLfloat cameraX = 6.0f, cameraY = 6.0f, cameraZ = 0.0f;


/////////////////////////////////////////////////////
//  int
/////////////////////////////////////////////////////
void init(void)
{
	
	glMatrixMode(GL_PROJECTION);
	ShaderInfo  shaders[] = {{ GL_VERTEX_SHADER, "triangles.vert" },{ GL_FRAGMENT_SHADER, "triangles.frag" },{ GL_NONE, NULL }};
	program = LoadShaders(shaders);
	glUseProgram(program);
	
	// By now we have parsed the .obj for all vertices
	glGenVertexArrays(NumVAOs, VAOs);
	glBindVertexArray(VAOs[Triangles]);
	glGenBuffers(NumBuffers, Buffers); // Create 1 Buffer and put id in VAO's
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[ArrayBuffer]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(finalVertices), finalVertices, GL_DYNAMIC_DRAW);
	// End of vertices
	
	// Colors also should have been parsed by Now
	GLfloat verticesColor[48][3] = {
		{ 1.0f, 0.0f, 0.0f },	// Triangle 1
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },	// Triangle 1
		{ 1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 1.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 0.0f, 1.0f, 1.0f },
		{ 0.5f, 0.5f, 0.5f },	// Triangle 1
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 0.5f, 0.5f },
		{ 0.5f, 1.5f, 0.5f },
		{ 0.5f, 1.5f, 0.5f },
		{ 0.5f, 1.5f, 0.5f },
		{ 0.5f, 1.5f, 0.5f },
		{ 1.0f, 0.0f, 0.0f },	// Triangle 1
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },	// Triangle 1
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f }
	};

	glGenBuffers(1, ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColor), verticesColor, GL_STATIC_DRAW);
	// End Of Color
	
	// Face values 
	/*glGenBuffers(1, facesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesBuffer[0]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*facesValue, faces, GL_STATIC_DRAW);*/
	// End of Face
	 

	uniform_mvp = glGetUniformLocation(program, "mvp");

}

////////////////////////////////////////////////////////////////////
//	display
////////////////////////////////////////////////////////////////////
void display(void)
{
	float screen_width = 512.0f, screen_height = 512.0f;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Transformation
	model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0,-4.0));// T from obj file else just identity mat
	//trans = glm::scale(trans, glm::vec3(1.5, 1.5, 1.5));
	//trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	//view = glm::lookAt(glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	/*Focal Point is at the Geometric center MAX+MIN/2*/
	
	float focalX, focalY, focalZ;
	focalX = (maxXRange + minXRange) / 2;
	focalY = (maxYRange + minYRange) / 2;
	focalZ = (maxZRange + minZRange) / 2;
	//cout << "Focal :: " << focalX <<" " <<  focalY <<" " << focalZ;
	//view = glm::lookAt(glm::vec3(maxXRange, maxYRange, maxZRange), glm::vec3(focalX, focalY, focalZ), glm::vec3(0.0, 1.0, 0.0));
	view = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(0.0, 0.0, -4.0), glm::vec3(0.0, 0.0, 1.0));
	projection = glm::perspective(45.0f, 1.0f*screen_width / screen_height, 0.1f, 100.0f);
	mvp = projection * view * model * trans;
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
	/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, facesBuffer[0]);
	int size;  glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &size);
	cout << endl << " Faces Size :: " << facesValue << " size" << size << endl;
	glDrawElements(GL_TRIANGLES, size / sizeof(GLuint), GL_UNSIGNED_INT, 0);*/
	glDrawArrays(GL_TRIANGLES, 0, facesValue);
	glDisableVertexAttribArray(vPosition);
	glDisableVertexAttribArray(vertexColor);
	glutSwapBuffers();

	glFlush();
}

void readMaterialFile(char* fileName)
{
	FILE * fp;
	char *token;
	char *keyWord;
	char * delimiter = " ";
	char *lineRead = (char*)malloc(100);
	int colorPosition = 0;
	bool storeValue = false;
	//fileName = "master.mtl";
	if ((fp = fopen(fileName, "r")) != NULL)
	{
		while (!feof(fp))
		{
			fgets(lineRead, 50, fp);
			{
				struct materialValues  newMaterialValue;
				//Initialize Default values
				newMaterialValue.ka[0] = 0.1f, newMaterialValue.ka[1] = 0.1f, newMaterialValue.ka[2] = 0.1f;
				newMaterialValue.kd[0] = 0.9f, newMaterialValue.kd[1] = 0.9f, newMaterialValue.kd[2] = 0.9f;
				//End
				keyWord = strtok(lineRead, " ");
				while (keyWord != NULL)
				{
					if (strcmp(keyWord, "newmtl") == 0)
					{
						char *colorName = strtok(NULL,"\n");
						strcpy(newMaterialValue.color, colorName);
						
					}
					else if (strcmp(keyWord, "Ka") == 0)
					{
						//materialFileValues[colorPosition][1][0]
						//cout << endl;
						newMaterialValue.ka[0] = atof(strtok(NULL, delimiter));
						newMaterialValue.ka[1] = atof(strtok(NULL, delimiter));
						newMaterialValue.ka[2] = atof(strtok(NULL, "\n"));
					}
					else if (strcmp(keyWord, "Kd") == 0)
					{
						newMaterialValue.kd[0] = atof(strtok(NULL, delimiter));
						newMaterialValue.kd[1] = atof(strtok(NULL, delimiter));
						newMaterialValue.kd[2] = atof(strtok(NULL, "\n"));
						storeValue = true;
					}
					else if (strcmp(keyWord, "Ks") == 0)
					{
						// We do not handle this
						//materialFileValues[colorPosition][2][0] = atof(strtok(NULL, delimiter));
						//materialFileValues[colorPosition][2][1] = atof(strtok(NULL, delimiter));
						//materialFileValues[colorPosition][2][2] = atof(strtok(NULL, "\n"));
					}
					keyWord = strtok(NULL, delimiter);
				}
				if (storeValue)
				{
					storeValue = false;
					materialFileValues[materialFileValuesIndex++] = newMaterialValue;
				}
			}
		}
	}
}
void printMaterialFile()
{
	struct materialValues  newMaterialValue;
	for (int i = 0; i < materialFileValuesIndex; i++)
	{
		newMaterialValue = materialFileValues[i];
		cout << endl;
		cout << i << " Color :: " << newMaterialValue.color << endl;
		cout << " Ka :: " << newMaterialValue.ka[0] << " " << newMaterialValue.ka[1] << " " << newMaterialValue.ka[2];
		cout << " Kd :: " << newMaterialValue.kd[0] << " " << newMaterialValue.kd[1] << " " << newMaterialValue.kd[2];
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
void printVertexNormalValues()
{
	for (int i = 0; i < verticesNormalsIndex; i++)
	{
		cout << endl << " Vertex Normals:: " << i << endl;
		for (int j = 0; j < 3; j++)
		{
			cout << vertices[i][j] << "  ";

		}
	}
}
void printFinalVertexValues()
{
	cout << "\nFinal Vertex entry has ::" << facesValue;
	for (int i = 0; i < facesValue; i++)
	{
		cout << endl << " Final Vertex :: " << i << endl;
		for (int j = 0; j < 3; j++)
		{
			cout << finalVertices[i][j] << "  ";

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
void computeTheMaxAndMin(float x, float y, float z)
{
	if (x > maxXRange)
	{
		maxXRange = x;
	}
	else if (x < minXRange)
	{
		minXRange = x;
	}
	if (y > maxYRange)
	{
		maxYRange = y;
	}
	else if (y < minYRange)
	{
		minYRange = y;
	}
	if (z > maxZRange)
	{
		maxZRange = z;
	}
	else if (z < minZRange)
	{
		minZRange = z;
	}
}
void printMaxAndMin()
{
	cout << endl << "MAX ::" << maxXRange << " " << maxYRange << " " << maxZRange << endl;
	cout << endl << "MIN ::" << minXRange << " " << minYRange << " " << minZRange << endl;
}
string getValueTillSlash(int start, char* value)
{
	string result;
	while (value != NULL && value[start] != '\0')
	{
		if (value[start] != '/')
		{
			result = result + value[start];
			start++;
		}
		else
		{
			break;
		}
		
	}
	return result;

}
void readInputFile(char* fileName)
{
	FILE * fp;
	int countOfSlashes = 0, lengthOfSlashes = 0;
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
							float xValue = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][0] = xValue;
							float yValue = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][1] = yValue;
							float zValue = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][2] = zValue;
							computeTheMaxAndMin(xValue, yValue, zValue);
							vertexIndex++;
						}
						else if (strcmp(keyWord, "vn") == 0)
						{
							/*cout << endl;
							cout << "Vertex Noraml X :: " << strtok(NULL, delimiter);
							cout << "Vertex Noraml Y :: " << strtok(NULL, delimiter);
							cout << "Vertex Noraml Z :: " << strtok(NULL, delimiter);*/
							float xValue = atof(strtok(NULL, delimiter));
							verticesNormal[verticesNormalsIndex][0] = xValue;
							float yValue = atof(strtok(NULL, delimiter));
							verticesNormal[verticesNormalsIndex][1] = yValue;
							float zValue = atof(strtok(NULL, delimiter));
							verticesNormal[verticesNormalsIndex][2] = zValue;
							verticesNormalsIndex++;
						}
						else if (strcmp(keyWord, "vt") == 0)
						{
							//cout << endl;
							//cout << "vertex Texture 1 :: " << strtok(NULL, delimiter);
							//cout << "vertex Texture 2 :: " << strtok(NULL, delimiter);
						}
						else if (strcmp(keyWord, "f") == 0)
						{
							bool slashFormat = false;
							lengthOfSlashes = 0, countOfSlashes=0; // Reset the values per iteration
							char * value = strtok(NULL, delimiter);
							/* Check if format is v/vt/vn*/
							
							int i = 0;
							while (value != NULL && value[i] != '\0')
							{
								if (value[i] == '/')
								{
									slashFormat = true;
									countOfSlashes++;
									//break;
								}
								lengthOfSlashes++;
								i++;
							}
							// End
							
							if (slashFormat == true)
							{
								while (value != NULL)
								{
									int j = 0;
									lengthOfSlashes = 0;
									while (value != NULL && value[j] != '\0')
									{
										lengthOfSlashes++;
										j++;
									}
									i = 0;
									int positionOfSlash = 0; // is it 1st 2nd or 3rd slash
									while (i < lengthOfSlashes+ 1 && value != NULL && value[i] != NULL && value[i] != '\n' && value[i] != '\0')
									{
										if (value[i] == '/')
										{
											positionOfSlash++;
											i++;
										}
										if (positionOfSlash == 0)// 1st slash is for vertex info
										{
											//int val = value[i] - '0';
											string result= getValueTillSlash(i,value);
											int vertexIndex = atoi(result.c_str()) - 1;
											//cout << endl << result;
											int length = result.length();
											finalVertices[facesValue][0] = vertices[vertexIndex][0];
											finalVertices[facesValue][1] = vertices[vertexIndex][1];
											finalVertices[facesValue][2] = vertices[vertexIndex][2];
											facesValue++;
											i = i + length;
										}
										else if (positionOfSlash == 1)
										{
											// Do nothing we do not handle Vt
											string result = getValueTillSlash(i, value);
											//cout << "/" << result;
											int length = result.length();
											//positionOfSlash++;
											i = i + length;
										}
										else if (positionOfSlash == 2)
										{
											// Handle face normal here
											string result = getValueTillSlash(i, value);
											int vertexIndex = atoi(result.c_str()) - 1;
											int length = result.length();
											//int val = value[i] - '0';
											//int vertexIndex = val - 1;
											finalVerticesNormals[normalValue][0] = verticesNormal[vertexIndex][0];
											finalVerticesNormals[normalValue][1] = verticesNormal[vertexIndex][1];
											finalVerticesNormals[normalValue][2] = verticesNormal[vertexIndex][2];
											normalValue++;
											i = i + length;
										}

									}
									value = strtok(NULL, delimiter);
								}
							}
							else if (slashFormat == false)
							{
								int j = 0;
								while (value != NULL)
								{
									int vertexIndex = atoi(value) - 1;
									finalVertices[facesValue][0] = vertices[vertexIndex][0];
									finalVertices[facesValue][1] = vertices[vertexIndex][1];
									finalVertices[facesValue][2] = vertices[vertexIndex][2];
									facesValue++;
									value = strtok(NULL, delimiter);
								}
							}
						}
						keyWord = strtok(NULL, delimiter);
					}
				}
			}
		}
		NumVertices = vertexIndex;// To keep Track of Number of Vertices , default is 0 . array is constructed using 300x3
	
	}
	else
	{
		cout << endl <<" File Does not exist";
	}
}
static void special(unsigned char key, int x_cord, int y_cord)
{
	cout << " kEY :: " << key << endl;
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
	case 'r':
		//GLfloat radius = 10.0f;
		//GLfloat camX = 1;//sin(glfwGetTime()) * radius;
		//GLfloat camZ = 1;//cos(glfwGetTime()) * radius;
		//glm::mat4 view;
		//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		break;	
	case 'u' :
		cameraY = cameraY + 0.05f;
		cameraX = cameraX + 0.05f;
	case 'i':
		cameraZ = cameraZ - 0.05f;
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
	char* fileName = "cube_multicolor2.obj";
	readInputFile(fileName);
	//printVertexValues();
	//printFacesValues();
	//printFinalVertexValues();
	//printMaxAndMin();
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

