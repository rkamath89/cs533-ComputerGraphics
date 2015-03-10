// ch01.cpp : Defines the entry point for the console application.
//
//////////////////////////////////////////////////////////
//  triangles.cpp  from the OpenGL Red Book   Chapter 1
//////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <numeric>
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
#include <glm/glm/gtx/transform.hpp>
#include <glm/glm/gtx/rotate_vector.hpp>
#include <glm/glm/gtc/type_ptr.hpp>
using namespace std;
using namespace glm;


const int _MAX_ROWS_ = 1000000;
const int numberOfObjects = 10;
int mtlNumber = 1;// Just to see if initialized

struct materialValues
{
	int materilNumber;
	char color[20];
	float ka[3];// = { 0.1f, 0.1f, 0.1f };// Default value as per specs
	float kd[3];// = { 0.9f, 0.9f, 0.9f };// Default value as per specs

};
struct materialValues materialFileValues[100];
int materialFileValuesIndex = 0;


GLuint  vertexIndex = 0, facesValue = 0, finalNormalValue = 0, verticesNormalsIndex = 0;
GLuint program;

//enum Attrib_IDs { vPosition = 0 };
const GLuint vPosition = 0, vertexColor = 1;

GLuint NumVertices = 0;
GLfloat vertices[_MAX_ROWS_][3];
GLfloat verticesNormal[_MAX_ROWS_][3];
GLfloat finalVertices[_MAX_ROWS_][3], finalVerticesNormals[_MAX_ROWS_][3];
GLuint faces[_MAX_ROWS_];	
GLfloat verticesColor[_MAX_ROWS_][3];

GLuint VAOs[numberOfObjects];
GLuint Buffers[numberOfObjects];
GLuint ColorBuffer[numberOfObjects];

float screen_width = 512.0f, screen_height = 512.0f;

struct translationObject
{
	int objNumber;
	char path[200];
	int numberOfFaces;// Number of vertices actually
	char fileName[200];
	float rotationDegree = 0;
	glm::vec3 rotationValueForObject;
	int scalingEnabled = 0;
	glm::vec3 scalingValueForObject;
	int translationEnabled = 0;
	glm::vec3 transationValueForObject;
	glm::mat4 transformation = mat4(1.0f);
};
int objectNumber = -1;
struct translationObject objectInformation[numberOfObjects];


GLint uniform_mvp;
glm::mat4 model, view, projection, mvp;

float maxXRange = 0.0f, minXRange = 999.0f, maxYRange = 0.0f, minYRange = 999.0f, maxZRange = 0.0f, minZRange = 999.0f;
float sumOfX = 0.0f, sumOfY = 0.0f, sumOfZ = 0.0f;
int numberOfX = 0, numberOfY = 0, numberOfZ = 0;

// Default camera values
GLfloat cameraX = 0.0f, cameraY = 0.0f, cameraZ = 0.0f;
GLfloat focalX = 0.0f, focalY = 0.0f, focalZ = 0.0f;
GLfloat lookupX = 0.0f, lookupY = 0.0f, lookupZ = 1.0f;
glm::vec3 viewUpVector = { lookupX, lookupY, lookupZ };
GLfloat nearDist = 0.1f, farDist = 100.0f;
float degree = 0, degree1 = 0;
// Default camera values End


vec3 lightSource = { 11.0f, 11.0f, 11.0f };


char * delimiter = " ,'\n'";
/////////////////////////////////////////////////////
//  int
/////////////////////////////////////////////////////
void init(void)
{
	degree1 = cameraY / cameraX;
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	ShaderInfo  shaders[] = { { GL_VERTEX_SHADER, "triangles.vert" }, { GL_FRAGMENT_SHADER, "triangles.frag" }, { GL_NONE, NULL } };
	program = LoadShaders(shaders);
	glUseProgram(program);
	uniform_mvp = glGetUniformLocation(program, "mvp");
	
	glGenVertexArrays(numberOfObjects, VAOs);
	glGenBuffers(numberOfObjects, Buffers); // Create 1 Buffer and put id in VAO's
	glGenBuffers(numberOfObjects, ColorBuffer);


}
void storeValuesInBuffer(int number)
{
	glMatrixMode(GL_PROJECTION);
	// By now we have parsed the .obj for all vertices
	//glGenVertexArrays(9, VAOs);
	glBindVertexArray(VAOs[number]);
	//glGenBuffers(NumBuffers, Buffers); // Create 1 Buffer and put id in VAO's
	glBindBuffer(GL_ARRAY_BUFFER, Buffers[number]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(finalVertices), finalVertices, GL_DYNAMIC_DRAW);
	// End of vertices
	//glGenBuffers(9, ColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer[number]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColor), verticesColor, GL_DYNAMIC_DRAW);
	// End Of Color


}
////////////////////////////////////////////////////////////////////
//	display
////////////////////////////////////////////////////////////////////
void display(void)
{
	int displayObject = 0;
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClearDepth(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (int i = 0; i < objectNumber; i++)
	{

		/*Focal Point is at the Geometric center MAX+MIN/2*/
		/*float focalX, focalY, focalZ;
		focalX = (maxXRange + minXRange) / 2;
		focalY = (maxYRange + minYRange) / 2;
		focalZ = (maxZRange + minZRange) / 2;*/
		//cout << "Focal :: " << focalX <<" " <<  focalY <<" " << focalZ;

		model = glm::mat4(1.0f);
		view = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(focalX, focalY, focalZ), viewUpVector);
		//projection = glm::perspective(45.0f, 1.0f*screen_width / screen_height, nearDist, farDist);
		projection = glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, nearDist, farDist);
		mvp = projection * view * model * objectInformation[i].transformation;
		
		glUseProgram(program);
		glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

		glEnableVertexAttribArray(vPosition);
		glBindVertexArray(VAOs[i]);
		glBindBuffer(GL_ARRAY_BUFFER, Buffers[i]);
		glVertexAttribPointer(
			vPosition, // attribute
			3,                 // number of elements per vertex, here (x,y,z)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
			);

		glEnableVertexAttribArray(vertexColor);
		glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer[i]);
		glVertexAttribPointer(
			vertexColor, // attribute
			3,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
			);

		glDrawArrays(GL_TRIANGLES, 0, objectInformation[i].numberOfFaces);
		glDisableVertexAttribArray(vPosition);
		glDisableVertexAttribArray(vertexColor);
		
		
	}
	glutSwapBuffers();
	glFlush();
	

}

materialValues getDetailsOfMaterial(char *color)
{
	struct materialValues  tempMaterialValues;
	for (int i = 0; i < materialFileValuesIndex; i++)
	{
		tempMaterialValues = materialFileValues[i];
		if (strcmp(tempMaterialValues.color, color) == 0)
		{
			return tempMaterialValues;
		}
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
	bool storeValue = false;
	//fileName = "master.mtl";
	if ((fp = fopen(fileName, "r")) != NULL)
	{
		while (!feof(fp))
		{
			fgets(lineRead, 100, fp);
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
						char *colorName = strtok(NULL, "\n");
						strcpy(newMaterialValue.color, colorName);
						newMaterialValue.materilNumber = mtlNumber++;
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
void readControlFIle(char* fileName)
{
	FILE * fp;
	char *token;
	char *keyWord;
	char * delimiter = " ,'\n','\r','\0'";
	char *lineRead = (char*)malloc(100);

	if ((fp = fopen(fileName, "r")) != NULL)
	{
		int actionNumber = 1;
		while (!feof(fp))
		{
			fgets(lineRead, 100, fp);
			{
				bool rotationApplied = false;
				keyWord = strtok(lineRead, delimiter);
				while (keyWord != NULL)
				{
					if (strcmp(keyWord, "obj") == 0)
					{
						actionNumber = 1;
						objectNumber++;
						objectInformation[objectNumber].objNumber = objectNumber;
						char * fname = strtok(NULL, delimiter);
						strcpy(objectInformation[objectNumber].fileName, fname);	// USE ENTIRE TOKEN IN LINUX BOX


						// UNCOMMENT ON LINUX BOX
						// Finux File Format
						/*cout << endl << " Fetching File path";
						string keyWordString(fname);
						unsigned found = keyWordString.find_last_of("/\\");
						//printf("\nPath ::  %s", keyWordString.substr(0, found + 1).c_str());
						cout << endl << "Done";
						strcpy(objectInformation[objectNumber].path,keyWordString.substr(0, found + 1).c_str());
						//printf("\n %s", keyWordString.substr(found + 1, keyWordString.length()).c_str());
						//objectInformation[objectNumber].
						*/
						// Finux File Format

						
					}
					else if (strcmp(keyWord, "rx") == 0 || strcmp(keyWord, "ry") == 0 || strcmp(keyWord, "rz") == 0)
					{
						rotationApplied = true;
						if (strcmp(keyWord, "rx") == 0)
						{
							float x = atof(strtok(NULL, delimiter));
							if (x > objectInformation[objectNumber].rotationDegree)
							{
								objectInformation[objectNumber].rotationDegree = x;
								objectInformation[objectNumber].rotationValueForObject.x = 1; // Rotate About X
							}
							
						}
						else if (strcmp(keyWord, "ry") == 0)
						{
							float y = atof(strtok(NULL, delimiter));
							if (y > objectInformation[objectNumber].rotationDegree)
							{
								objectInformation[objectNumber].rotationDegree = y;
								objectInformation[objectNumber].rotationValueForObject.y = 1; // Rotate About Y
							}
						}
						else if (strcmp(keyWord, "rz") == 0)
						{
							float z = atof(strtok(NULL, delimiter));
							if (z > objectInformation[objectNumber].rotationDegree)
							{
								objectInformation[objectNumber].rotationDegree = z;
								objectInformation[objectNumber].rotationValueForObject.z = 1; // Rotate About Z
							}
							
						}
					}
					else if (strcmp(keyWord, "t") == 0)
					{
						if (objectInformation[objectNumber].translationEnabled == 0)
						{
							objectInformation[objectNumber].translationEnabled = actionNumber;
							actionNumber++;
						}
						objectInformation[objectNumber].transationValueForObject.x = atof(strtok(NULL, delimiter));
						objectInformation[objectNumber].transationValueForObject.y = atof(strtok(NULL, delimiter));
						objectInformation[objectNumber].transationValueForObject.z = atof(strtok(NULL, delimiter));
						
						objectInformation[objectNumber].transformation = glm::translate(objectInformation[objectNumber].transformation,
							glm::vec3(objectInformation[objectNumber].transationValueForObject.x, objectInformation[objectNumber].transationValueForObject.y, objectInformation[objectNumber].transationValueForObject.z));
					}
					else if (strcmp(keyWord, "s") == 0)
					{
						if (objectInformation[objectNumber].scalingEnabled == 0)
						{
							objectInformation[objectNumber].scalingEnabled = actionNumber;
							actionNumber++;
						}
						objectInformation[objectNumber].scalingValueForObject.x = atof(strtok(NULL, delimiter));
						objectInformation[objectNumber].scalingValueForObject.y = atof(strtok(NULL, delimiter));
						objectInformation[objectNumber].scalingValueForObject.z = atof(strtok(NULL, delimiter));

						objectInformation[objectNumber].transformation = glm::scale(objectInformation[objectNumber].transformation,
							glm::vec3(objectInformation[objectNumber].scalingValueForObject.x, objectInformation[objectNumber].scalingValueForObject.y, objectInformation[objectNumber].scalingValueForObject.z));
					}
					keyWord = strtok(NULL, delimiter);
					if (keyWord == NULL && rotationApplied == true)
					{
						rotationApplied = false;
						objectInformation[objectNumber].transformation = glm::rotate(objectInformation[objectNumber].transformation,
							objectInformation[objectNumber].rotationDegree,
							vec3(objectInformation[objectNumber].rotationValueForObject.x, objectInformation[objectNumber].rotationValueForObject.y, objectInformation[objectNumber].rotationValueForObject.z));
					}
				}
			}
		}objectNumber++;
	}
}
void printContentsOfObject()
{
	for (int i = 0; i < objectNumber; i++)
	{
		cout << endl << " Object Number :: " << objectInformation[i].objNumber;
		cout << endl << " Object Name :: " << objectInformation[i].fileName;
		
		cout << endl << " Object Rotation Degree:: " << objectInformation[i].rotationDegree;
		
		cout << endl << " Rotation :: " << objectInformation[i].rotationValueForObject.x << " " << objectInformation[i].rotationValueForObject.y << " " << objectInformation[i].rotationValueForObject.z;
		
		cout << endl << " Scaling :: " << objectInformation[i].scalingValueForObject.x << " " << objectInformation[i].scalingValueForObject.y << " " << objectInformation[i].scalingValueForObject.z;
		
		cout << endl << " Translation :: " << objectInformation[i].transationValueForObject.x << " " << objectInformation[i].transationValueForObject.y << " " << objectInformation[i].transationValueForObject.z;

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
			cout << vertices[i][j] << "  ";

		}
	}
}
void printColors()
{
	for (int i = 0; i < finalNormalValue; i++)
	{
		cout << endl << " Vertex Color:: " << i << endl;
		for (int j = 0; j < 3; j++)
		{
			cout << verticesColor[i][j] << "  ";

		}
	}
}
void printVertexNormalValues()
{
	for (int i = 0; i < finalNormalValue; i++)
	{
		cout << endl << " Vertex Normals:: " << i << endl;
		for (int j = 0; j < 3; j++)
		{
			cout << finalVerticesNormals[i][j] << "  ";

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
	int printNewLine = 0;
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
void printMaxAndMinAndCenter()
{
	cout << endl << "MAX ::" << maxXRange << " " << maxYRange << " " << maxZRange << endl;
	cout << endl << "MIN ::" << minXRange << " " << minYRange << " " << minZRange << endl;
	cout << endl;
	cout << endl << " Center-X : " << focalX << " Center-Y : " << focalY << " Center-Z : " << focalZ;
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
float getDotProduct(vec3 normalizedLight, vec3 normalizedNormal)
{
	return ((normalizedLight.x * normalizedNormal.x) +
		(normalizedLight.y * normalizedNormal.y) +
		(normalizedLight.z * normalizedNormal.z));
}
void fillNormalArrayAndComputeColor(int size, struct materialValues fetchedMaterialValues,vec3 calculatedNormal)
{
	vec3 normalizedNormal = normalize(calculatedNormal);
	vec3 normalizedLight = normalize(lightSource);
	float diffuseCoefficient = getDotProduct(normalizedLight, normalizedNormal);

	vec3 finalColor = vec3(fetchedMaterialValues.ka[0], fetchedMaterialValues.ka[1], fetchedMaterialValues.ka[2]) +
								( (max(diffuseCoefficient, 0.0f)) * (vec3(fetchedMaterialValues.kd[0], fetchedMaterialValues.kd[1], fetchedMaterialValues.kd[2])));
	for (int i = finalNormalValue; i < finalNormalValue+3; i++)
	{
		finalVerticesNormals[i][0] = calculatedNormal.x;
		finalVerticesNormals[i][1] = calculatedNormal.y;
		finalVerticesNormals[i][2] = calculatedNormal.z;
		verticesColor[i][0] = finalColor.x;// fetchedMaterialValues.kd[0];//finalColor.x;
		verticesColor[i][1] = finalColor.y; //fetchedMaterialValues.kd[1]; //finalColor.y;
		verticesColor[i][2] = finalColor.z; //fetchedMaterialValues.kd[2]; //finalColor.z;
	}
	
	finalNormalValue = finalNormalValue + 3;

}
vec3 computeNormalValues(int readFaces ,struct materialValues fetchedMaterialValues)
{

	vec3 edge1;
	vec3 edge2;
	vec3 calculatedNormal;

	edge1.x = finalVertices[readFaces - 1][0] - finalVertices[readFaces - 3][0];
	edge1.y = finalVertices[readFaces - 1][1] - finalVertices[readFaces - 3][1];
	edge1.z = finalVertices[readFaces - 1][2] - finalVertices[readFaces - 3][2];

	edge2.x = finalVertices[readFaces - 2][0] - finalVertices[readFaces - 3][0];
	edge2.y = finalVertices[readFaces - 2][1] - finalVertices[readFaces - 3][1];
	edge2.z = finalVertices[readFaces - 2][2] - finalVertices[readFaces - 3][2];

	calculatedNormal = cross(edge2, edge1);
	//vec3 normalizedNormal = normalize(calculatedNormal);
	//cout << " \n Calculated Edge1 : " << edge1.x << " " << edge1.y << " " << edge1.z ;
	//cout << " \n Calculated Edge2 : " << edge2.x << " " << edge2.y << " " << edge2.z;
	//cout << "\nNormal ::" << calculatedNormal.x << " " << calculatedNormal.y << " " << calculatedNormal.z;
	//cout << "\n Normalized Normal ::" << normalizedNormal.x << " " << normalizedNormal.y << " " << normalizedNormal.z;
	
	fillNormalArrayAndComputeColor(readFaces, fetchedMaterialValues, calculatedNormal);


	return calculatedNormal;
}

void readInputFile(char* fileName,char* filePath)
{
	FILE * fp;
	bool materialValueFound = false;
	int countOfSlashes = 0, lengthOfSlashes = 0;
	char *token;
	char *keyWord;
	char *lineRead = (char*)malloc(100);
	ifstream objectFile;
	char* mtlLibName = (char*)malloc(100); // Will be the name of material LIB Fetched from .obj file
	if ((fp = fopen(fileName, "r")) != NULL)
	{
		//vertex = 0;
		facesValue = 0;
		bool fileContainsVertexNormalValues = false;
		int  normalForFace = 0, getNormalForFacePos = 0;
		struct materialValues foundMaterialValue;
		char * colorName;
		while (!feof(fp))
		{
			fgets(lineRead, 50, fp);
			{
				int vertexNumber = 0;
				keyWord = strtok(lineRead, " ");
				{

					while (keyWord != NULL)
					{
						if (strcmp(keyWord, "mtllib") == 0)
						{
							
							mtlLibName=strtok(NULL, delimiter);
							strcat(filePath, mtlLibName);
							//puts(filePath);
							cout << endl << "Mat lib name ::" << filePath;
							readMaterialFile(mtlLibName);						// CHANGE TO FILE PATH ON LINUX ENV
							//printMaterialFile(); //For Debugging
						}
						else if (strcmp(keyWord, "usemtl") == 0)
						{
							colorName = strtok(NULL, delimiter);
							foundMaterialValue = getDetailsOfMaterial(colorName);
							materialValueFound = true;
							//cout << endl << "ColorName ::" << foundMaterialValue.color;
						}
						else if (strcmp(keyWord, "v") == 0)
						{
							float xValue = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][0] = xValue;
							sumOfX = sumOfX + xValue;
							float yValue = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][1] = yValue;
							sumOfY = sumOfY + yValue;
							float zValue = atof(strtok(NULL, delimiter));
							vertices[vertexIndex][2] = zValue;
							sumOfZ = sumOfZ + zValue;
							computeTheMaxAndMin(xValue, yValue, zValue);
							vertexIndex++;
						}
						else if (strcmp(keyWord, "vn") == 0)
						{
							fileContainsVertexNormalValues = true;
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
							lengthOfSlashes = 0, countOfSlashes = 0; // Reset the values per iteration
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
									while (i < lengthOfSlashes + 1 && value != NULL && value[i] != NULL && value[i] != '\n' && value[i] != '\0')
									{
										if (value[i] == '/')
										{
											positionOfSlash++;
											i++;
										}
										if (positionOfSlash == 0)// 1st slash is for vertex info
										{
											//int val = value[i] - '0';
											string result = getValueTillSlash(i, value);
											int vertexIndex = atoi(result.c_str()) - 1;
											//cout << endl << result;
											int length = result.length();
											finalVertices[facesValue][0] = vertices[vertexIndex][0];
											finalVertices[facesValue][1] = vertices[vertexIndex][1];
											finalVertices[facesValue][2] = vertices[vertexIndex][2];
											// End 
											if (fileContainsVertexNormalValues == false)
											{
												// Group 3 vertex and get the normal
												vertexNumber++;
												if (vertexNumber > 2)
												{
													vertexNumber = 0;
													if (materialValueFound == false)
													{
														//cout << endl << " No color Use Default";
														strcpy(foundMaterialValue.color, "default");
														foundMaterialValue.ka[0] = 0.1f, foundMaterialValue.ka[1] = 0.1f, foundMaterialValue.ka[2] = 0.1f;
														foundMaterialValue.kd[0] = 0.9f, foundMaterialValue.kd[1] = 0.9f, foundMaterialValue.kd[2] = 0.9f;
													}
													computeNormalValues(facesValue, foundMaterialValue);
												}
											}
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
											finalVerticesNormals[finalNormalValue][0] = verticesNormal[vertexIndex][0];
											finalVerticesNormals[finalNormalValue][1] = verticesNormal[vertexIndex][1];
											finalVerticesNormals[finalNormalValue][2] = verticesNormal[vertexIndex][2];

											//for (int colPos = 0; colPos < 3; colPos++)
											//{
											//	float maxValue = 0.0f;
											//	/*float multipliedValue = dot(finalVerticesNormals[finalNormalValue][colPos], lightSource[colPos]);
											//	if (multipliedValue > maxValue)
											//	{
											//		maxValue = multipliedValue;
											//	}*/
											//	if (materialValueFound == false)
											//	{
											//		//cout << endl << " No color Use Default";
											//		strcpy(foundMaterialValue.color, "default");
											//		foundMaterialValue.ka[0] = 0.1f, foundMaterialValue.ka[1] = 0.1f, foundMaterialValue.ka[2] = 0.1f;
											//		foundMaterialValue.kd[0] = 0.9f, foundMaterialValue.kd[1] = 0.9f, foundMaterialValue.kd[2] = 0.9f;
											//	}
											//	float result = foundMaterialValue.kd[colPos];// foundMaterialValue.ka[colPos] + (maxValue * foundMaterialValue.kd[colPos]);
											//	verticesColor[finalNormalValue][colPos] = result;
											//}
											float maxValue = 0.0f;
											vec3 lightSrcNormalized = normalize(lightSource);
											vec3 normalNormalized = normalize(vec3(finalVerticesNormals[finalNormalValue][0], finalVerticesNormals[finalNormalValue][1], finalVerticesNormals[finalNormalValue][2]));
											float coefficient = getDotProduct(lightSrcNormalized, normalNormalized);
											if (materialValueFound == false)
											{
												//cout << endl << " No color Use Default";
												strcpy(foundMaterialValue.color, "default");
												foundMaterialValue.ka[0] = 0.1f, foundMaterialValue.ka[1] = 0.1f, foundMaterialValue.ka[2] = 0.1f;
												foundMaterialValue.kd[0] = 0.9f, foundMaterialValue.kd[1] = 0.9f, foundMaterialValue.kd[2] = 0.9f;
											}
											vec3 colorComputed = vec3(foundMaterialValue.ka[0], foundMaterialValue.ka[1], foundMaterialValue.ka[2]) +
												((max(coefficient, 0.0f)) * (vec3(foundMaterialValue.kd[0], foundMaterialValue.kd[1], foundMaterialValue.kd[2])));
												//float result = foundMaterialValue.kd[colPos];// foundMaterialValue.ka[colPos] + (maxValue * foundMaterialValue.kd[colPos]);
											verticesColor[finalNormalValue][0] = colorComputed.x;
											verticesColor[finalNormalValue][1] = colorComputed.y;
											verticesColor[finalNormalValue][2] = colorComputed.z;

											finalNormalValue++;
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
									// Group 3 vertex and get the normal
									vertexNumber++;
									if (vertexNumber > 2)
									{
										vertexNumber = 0;
										if (materialValueFound == false)
										{
											//cout << endl << " No color Use Default";
											strcpy(foundMaterialValue.color, "default");
											foundMaterialValue.ka[0] = 0.1f, foundMaterialValue.ka[1] = 0.1f, foundMaterialValue.ka[2] = 0.1f;
											foundMaterialValue.kd[0] = 0.9f, foundMaterialValue.kd[1] = 0.9f, foundMaterialValue.kd[2] = 0.9f;
										}
										computeNormalValues(facesValue, foundMaterialValue);
									}
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
		cout << endl << " File Does not exist";
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
	case 'c':
		cameraZ = cameraZ - 0.1f;
		break;
	case 'v':
		cameraZ = cameraZ + 0.1f;
		break;
	case 'd':
		focalZ = focalZ + 0.1f;
		break;
	case 'f':
		focalZ = focalZ - 0.1f;
		break;
	case 'x':
		degree = 0.1f;//(3.14 / 180);
		viewUpVector = glm::rotate(viewUpVector, degree, glm::vec3(cameraX, cameraY, cameraZ));
		break;
	case 'z':
		degree = 0.1f;
		viewUpVector = glm::rotate(viewUpVector, -degree, glm::vec3(cameraX, cameraY, cameraZ));
		break;
	case 'r':
		
		cameraX = 3 * maxXRange, cameraY = 3 * maxYRange, cameraZ = maxZRange;
		//focalX = (int)((maxXRange - minXRange) / 2);
		//focalY = (int)((maxYRange - minYRange) / 2);
		//focalZ = (int)((maxZRange - minZRange) / 2);
		focalX = 0.0f, focalY = 0.0f, focalZ = 0.0f;
		lookupX = 0.0f, lookupY = 0.0f, lookupZ = 1.0f;
		viewUpVector = { lookupX, lookupY, lookupZ };
		nearDist = 0.1f, farDist = 100.0f;
		degree = 0.0f;
		degree1 = cameraY / cameraX;
		break;
	case 'q':
		exit(0);
		break;
	case 27: // Escape Key
		exit(0);
		break;
		/*case 'x':
		trans = glm::scale(trans, glm::vec3(1.5, 1.5, 1.5));
		break;
		*/
	default: return;
	}
	glutPostRedisplay();
}
void reshape(int screen_width, int screen_height)
{
	
	glViewport(0, 0, (GLsizei)screen_width, (GLsizei)screen_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, nearDist, farDist);
	//glFrustum(-1.0, 1.0, -1.0, 1.0, nearDist, farDist);
	glMatrixMode(GL_MODELVIEW);
}
float getRadius()
{
	return sqrt(pow((cameraX - focalX), 2) + pow((cameraY - focalY), 2));
}
void handleSpecialKeypress(int key, int x, int y)
{
	float radius;
	cout << " special kEY :: " << key << endl;
	string clr;
	switch (key)
	{
	case GLUT_KEY_LEFT:
		radius = getRadius();
		degree1 = degree1 + (3.14 / 180);
		cameraX = radius * cos(degree1);
		cameraY = radius * sin(degree1);
		//cout << endl << radius << " " <<  degree1 << "  " << cameraX << " " << cameraY;
		break;
	case GLUT_KEY_RIGHT:
		radius = getRadius();
		degree1 = degree1 - (3.14 / 180);
		cameraX = radius * cos(degree1);
		cameraY = radius * sin(degree1);
		//cout << endl << degree1 << "  " << cameraX << " " << cameraY;
		break;
	case GLUT_KEY_UP:
		nearDist = nearDist + 0.1f;
		break;
	case GLUT_KEY_DOWN:
		nearDist = nearDist - 0.1f;
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
	char fileName[200];
	char filePath[200];
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_ALPHA | GLUT_DOUBLE | GLUT_DEPTH);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);
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
	if (argc < 2)
	{
		cout << endl << " Wrong usage , pass parametes -c ControlFile.txt";
	}
	else
	{
		char * controlFileName;
		controlFileName = argv[2];
		cout << endl << " Entered File :: " << controlFileName;
		readControlFIle(controlFileName);
		//printContentsOfObject();
	}


	init();
	//printContentsOfObject();
	// Code to read everything from the files specified by control file
	for (int i = 0; i < objectNumber; i++)
	{
		strcpy(fileName, objectInformation[i].fileName);
		strcpy(filePath, objectInformation[i].path);
		cout << "\nPath from Object :: " << objectInformation[i].path;
		readInputFile(fileName, filePath);
		objectInformation[i].numberOfFaces = facesValue;
		//printVertexNormalValues();
		//printColors();
		// Store all values required in buffer and then rreset values in arrays
		storeValuesInBuffer(i);
		numberOfX = numberOfX + (vertexIndex);
		numberOfY = numberOfY + (vertexIndex);
		numberOfZ = numberOfZ + (vertexIndex);
		//Reset the variables
		vertexIndex = 0;
		verticesNormalsIndex = 0;
		facesValue = 0;
		finalNormalValue = 0;
		NumVertices = 0;


		// Clear Out all arrays and reset the Variables and set it up for next iteration 
		memset(vertices, 0, sizeof(vertices[0][0]) * _MAX_ROWS_ * 3);
		memset(verticesNormal, 0, sizeof(verticesNormal[0][0]) * _MAX_ROWS_ * 3);
		memset(finalVertices, 0, sizeof(finalVertices[0][0]) * _MAX_ROWS_ * 3);
		memset(finalVerticesNormals, 0, sizeof(finalVerticesNormals[0][0]) * _MAX_ROWS_ * 3);
		memset(faces, 0, sizeof(faces[0]) * _MAX_ROWS_);
		memset(verticesColor, 0, sizeof(verticesColor[0][0]) * _MAX_ROWS_ * 3);
	}
	//printMaxAndMinAndCenter();
	cameraX = 3 * maxXRange;
	cameraY = 3 * maxYRange;
	cameraZ = maxZRange;
	//focalX = (int)((maxXRange -minXRange ) / 2);
	//focalY = (int)((maxYRange - minYRange) / 2);
	//focalZ = (int)((maxZRange - minZRange) / 2);
	degree1 = cameraY / cameraX;
	//Code to read data from File

	//printColors();
	//printVertexNormalValues();
	//printVertexValues();
	//printFacesValues();
	//printFinalVertexValues();
	//printMaxAndMin();
	// End

	// Handle display seperately , put a loop of objectInformation inside that and store facesValue as part of objecctinformation
	glutDisplayFunc(display);
	glutKeyboardFunc(special);
	glutSpecialFunc(handleSpecialKeypress);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}

