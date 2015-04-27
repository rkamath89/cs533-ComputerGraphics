// ch01.cpp : Defines the entry point for the console application.
//
//////////////////////////////////////////////////////////
//  triangles.cpp  from the OpenGL Red Book   Chapter 1
//////////////////////////////////////////////////////////
#define _CRT_SECURE_NO_DEPRECATE
#include <iostream>
#include <numeric>
#include <vector>
#include "vgl.h"
#include "LoadShaders.h"
#include<fstream>
#include <string>
//#include <GL/glew.h>
/* Using the GLUT library for the base windowing setup */
//#include <GL/freeglut.h>
/* GLM */
// #define GLM_MESSAGES
#define GLM_FORCE_RADIANS
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtx/transform.hpp"
#include "glm/glm/gtx/rotate_vector.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#include "glm/glm/gtx/quaternion.hpp"
#include "soil/SOIL.h"
using namespace std;
using namespace glm;

// For Assignment 3
const int MAXLIGHTS = 4;
int lightIndex = -1;
struct LightProperties {
	bool isEnabled;
	bool isLocal;
	bool isDirectional;
	bool isSpot;
	vec3 ambient;
	vec3 color;
	vec3 position;
	vec3 halfVector;
	vec3 coneDirection;
	float spotCosCutoff;
	float spotExponent;
	float constantAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;
};
// For Assignment 3

const int _MAX_ROWS_ = 1000000;
const int numberOfObjects = 10;
int mtlNumber = 1;// Just to see if initialized
LightProperties LightsRa[MAXLIGHTS];
struct materialValues
{
	int materilNumber;
	char color[20];
	float ka[3];// = { 0.1f, 0.1f, 0.1f };// Default value as per specs
	float kd[3];// = { 0.9f, 0.9f, 0.9f };// Default value as per specs
	float ks[3];
	string mapKd;
	float shininess;

};
struct materialValues materialFileValues[100];
int materialFileValuesIndex = 0;


GLuint  vertexIndex = 0, facesValue = 0, finalNormalValue = 0, verticesNormalsIndex = 0, finalTextureValue = 0, verticesTextureIndex = 0;
GLuint program;

//enum Attrib_IDs { vPosition = 0 };
const GLuint vPosition = 0, vertexColor = 1, kAPosition = 2, kDPosition = 3, kSPosition = 5, shininessPosition = 6, normalPosition = 4 , vertexTexturePosition =7;

GLuint NumVertices = 0;
GLfloat vertices[_MAX_ROWS_][3];
GLfloat verticesNormal[_MAX_ROWS_][3];
GLfloat verticesTexture[_MAX_ROWS_][3];
GLfloat finalVertices[_MAX_ROWS_][3], finalVerticesNormals[_MAX_ROWS_][3];
GLfloat finalTexture[_MAX_ROWS_][3], finalVerticesTexture[_MAX_ROWS_][3];
GLuint faces[_MAX_ROWS_];
// New COde
vector<vec3> kAValues;
vector<vec3> kDValues;
vector<vec3> kSValues;
vector<GLfloat> shininessValues;
vector<vec3> normalValues;
vector<vec2> textureValues;
vector<string> fileNames;
// End

GLuint VAOs[numberOfObjects];
GLuint textureImages[numberOfObjects];
GLuint Buffers[numberOfObjects];
GLuint kABuffer[numberOfObjects];
GLuint kDBuffer[numberOfObjects];
GLuint kSBuffer[numberOfObjects];
GLuint shininessBuffer[numberOfObjects];
GLuint normalsBuffer[numberOfObjects];
GLuint vertexBuffer[numberOfObjects];
GLuint vertexTextureBuffer[numberOfObjects];

float screen_width = 512.0f, screen_height = 512.0f;

struct textureInfo
{
	string texFileName;
	int startPos;
	int endPos;
	int totalVertices;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shinenesssss;
};
struct translationObject
{
	int objNumber;
	char path[200];
	int numberOfFaces;// Number of vertices actually
	char fileName[200];
	float rotationDegree;
	vec3 ambientValue;
	vec3 diffuseValue;
	vec3 specValue;
	float shineness;
	glm::vec3 rotationValueForObject;
	int scalingEnabled;
	glm::vec3 scalingValueForObject;
	int translationEnabled;
	glm::vec3 transationValueForObject;
	glm::mat4 transformation;// = mat4(1.0f);
	string imageName;
	vector<textureInfo> texInfoForObject;

	translationObject() :rotationDegree(0), scalingEnabled(0), translationEnabled(0), transformation(mat4(1.0f)) {}
};
int objectNumber = -1;
struct translationObject objectInformation[numberOfObjects];

GLint uniformEnable;
GLint uniform_mvp;
GLint uniform_mv;
GLint uniform_model;
GLint uniform_numberOfLisghtSources, ambient_uniform, diffuse_uniform, specular_uniform, shininess_uniform;

glm::mat4 model, view, projection, mvp, mv, normalMatrix;

float maxXRange = 0.0f, minXRange = 999.0f, maxYRange = 0.0f, minYRange = 999.0f, maxZRange = 0.0f, minZRange = 999.0f;
float defaultfocalx = 0.0f, defaultfocaly = 0.0f, defaultfocalz = 0.0f, defaultx = 0.0f, defaulty = 0.0f, defaultz = 0.0f, defaultViewx = 0.0f, defaultViewy = 0.0f, defaultViewz = 0.0f;
float sumOfX = 0.0f, sumOfY = 0.0f, sumOfZ = 0.0f;
int numberOfX = 0, numberOfY = 0, numberOfZ = 0;

// Default camera values
GLfloat cameraX = 0.0f, cameraY = 0.0f, cameraZ = 0.0f;
GLfloat focalX = 0.0f, focalY = 0.0f, focalZ = 0.0f;
GLfloat lookupX = 0.0f, lookupY = 0.0f, lookupZ = 1.0f;
glm::vec3 viewUpVector(lookupX, lookupY, lookupZ);
GLfloat nearDist = 0.1f, farDist = 200.0f;
float degree = 0, degree1 = 0;
// Default camera values End


vec3 lightSource(11.0f, 11.0f, 11.0f);


char * delimiter = " ,'\n'";

// Texture mapping
GLint texLoc;
GLuint checkTex;	// id for checkboard texture
GLint texUnit = 0;
GLuint vboTexIndices;
// End


/////////////////////////////////////////////////////
// loadTexture
/////////////////////////////////////////////////////
bool loadTexture(char* fileName, bool invertY)
{
	unsigned char *image;
	int width, height, channels;

	image = SOIL_load_image(fileName, &width, &height, &channels, SOIL_LOAD_AUTO);
	if (image == NULL)
	{
		cerr << "Unable to load image: " << fileName << endl;
		return false;
	}
	else
	{
		bool invert_y = true;
		if (invert_y)
		{
			for (int j = 0; j * 2 < height; ++j)
			{
				int index1 = j * width * channels;
				int index2 = (height - 1 - j) * width * channels;
				for (int i = width * channels; i > 0; --i)
				{
					unsigned char temp = image[index1];
					image[index1] = image[index2];
					image[index2] = temp;
					++index1;
					++index2;
				}
			}
		}
		GLuint tex2;
		glGenTextures(1, &tex2);
		glBindTexture(GL_TEXTURE_2D, tex2);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

		if (channels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0,
			GL_RGB, GL_UNSIGNED_BYTE, image);
		else if (channels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
			GL_RGBA, GL_UNSIGNED_BYTE, image);

		SOIL_free_image_data(image);	// free up memory allocated by SOIL_load_image

		return true;
	}
}
float findMax(float number1, float number2)
{
	if (number1 > number2)
	{
		return number1;
	}
	else
	{
		return number2;
	}
}
void printContentsOfLightObject()
{
	for (int i = 0; i <= lightIndex; i++)
	{
		cout << endl << "isLocaL :" << LightsRa[i].isLocal;
		cout << endl << "isSpot :" << LightsRa[i].isSpot;
		cout << endl << "isDirectional :" << LightsRa[i].isDirectional;
		cout << endl << "ambient :" << LightsRa[i].ambient.x << " " << LightsRa[i].ambient.y << " " << LightsRa[i].ambient.z;
		cout << endl << "color :" << LightsRa[i].color.x << " " << LightsRa[i].color.y << " " << LightsRa[i].color.z;
		cout << endl << "position :" << LightsRa[i].position.x << " " << LightsRa[i].position.y << " " << LightsRa[i].position.z;
		cout << endl << "ConstantAttenuation :" << LightsRa[i].constantAttenuation;
		cout << endl << "linearAttenuation :" << LightsRa[i].linearAttenuation;
		cout << endl << "quadraticAttenuation :" << LightsRa[i].quadraticAttenuation;
		cout << endl << "cone Direction :" << LightsRa[i].coneDirection.x << " " << LightsRa[i].coneDirection.y << " " << LightsRa[i].coneDirection.z;
		cout << endl << "spotCosCutoff :" << LightsRa[i].spotCosCutoff;
		cout << endl << "spotExponent :" << LightsRa[i].spotExponent;
		cout << endl;
	}
}
void printObjectTextureInfo(int objNum)
{
	if (objectInformation[objNum].texInfoForObject.size() > 0)
	{
		for (int i = 0; i < objectInformation[objNum].texInfoForObject.size(); i++)
		{
			cout << endl << "Start :: " << objectInformation[objNum].texInfoForObject[i].startPos
				<< "End :: " << objectInformation[objNum].texInfoForObject[i].endPos
				<< "Total :: " << objectInformation[objNum].texInfoForObject[i].totalVertices
				<< "FileName :: " << objectInformation[objNum].texInfoForObject[i].texFileName;
		}
		cout << endl;
	}
}
void init(void)
{
	degree1 = cameraY / cameraX;
	glMatrixMode(GL_PROJECTION);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	ShaderInfo  shaders[] = { { GL_VERTEX_SHADER, "triangles.vert" }, { GL_FRAGMENT_SHADER, "triangles.frag" }, { GL_NONE, NULL } };
	program = LoadShaders(shaders);
	glUseProgram(program);
	uniform_mvp = glGetUniformLocation(program, "MVPMatrix");
	uniform_mv = glGetUniformLocation(program, "MVMatrix");
	uniform_model = glGetUniformLocation(program, "NormalMatrix");
	uniform_numberOfLisghtSources = glGetUniformLocation(program, "numberOfLightsUsed");
	ambient_uniform = glGetUniformLocation(program, "ambient_uniform");
	diffuse_uniform = glGetUniformLocation(program, "diffuse_uniform");
	specular_uniform = glGetUniformLocation(program, "specular_uniform");
	shininess_uniform = glGetUniformLocation(program, "shininess_uniform");
	texLoc = glGetUniformLocation(program, "tex");
	uniformEnable = glGetUniformLocation(program, "enableColor");

	glGenVertexArrays(numberOfObjects, VAOs);
	glGenBuffers(numberOfObjects, Buffers); // Create 1 Buffer and put id in VAO's
	glGenBuffers(numberOfObjects, kABuffer);
	glGenBuffers(numberOfObjects, kDBuffer);
	glGenBuffers(numberOfObjects, kSBuffer);
	glGenBuffers(numberOfObjects, shininessBuffer);
	glGenBuffers(numberOfObjects, normalsBuffer);
	glGenBuffers(numberOfObjects, vertexBuffer);
	glGenTextures(numberOfObjects, textureImages);
	glGenBuffers(numberOfObjects, vertexTextureBuffer);



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
	//glBindBuffer(GL_ARRAY_BUFFER, ColorBuffer[number]);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(verticesColor), verticesColor, GL_DYNAMIC_DRAW);
	objectInformation[number].ambientValue = vec3(kAValues[0].x, kAValues[0].y, kAValues[0].z);
	glBindBuffer(GL_ARRAY_BUFFER, kABuffer[number]);
	glBufferData(GL_ARRAY_BUFFER, kAValues.size() * sizeof(glm::vec3), &kAValues[0], GL_DYNAMIC_DRAW);

	objectInformation[number].diffuseValue = vec3(kDValues[0].x, kDValues[0].y, kDValues[0].z);
	glBindBuffer(GL_ARRAY_BUFFER, kDBuffer[number]);
	glBufferData(GL_ARRAY_BUFFER, kDValues.size() * sizeof(glm::vec3), &kDValues[0], GL_DYNAMIC_DRAW);

	objectInformation[number].specValue = vec3(kSValues[0].x, kSValues[0].y, kSValues[0].z);
	glBindBuffer(GL_ARRAY_BUFFER, kSBuffer[number]);
	glBufferData(GL_ARRAY_BUFFER, kSValues.size() * sizeof(glm::vec3), &kSValues[0], GL_DYNAMIC_DRAW);

	objectInformation[number].shineness = shininessValues[0];
	glBindBuffer(GL_ARRAY_BUFFER, shininessBuffer[number]);
	glBufferData(GL_ARRAY_BUFFER, shininessValues.size() * sizeof(GLfloat), &shininessValues[0], GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer[number]);
	glBufferData(GL_ARRAY_BUFFER, normalValues.size() * sizeof(glm::vec3), &normalValues[0], GL_DYNAMIC_DRAW);
	// End Of Color
	objectInformation[number].imageName = fileNames[0];

	glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBuffer[number]);
	glBufferData(GL_ARRAY_BUFFER, textureValues.size() * sizeof(glm::vec2), &textureValues[0], GL_DYNAMIC_DRAW);


}
////////////////////////////////////////////////////////////////////
//	display
////////////////////////////////////////////////////////////////////
void display(void)
{
	int displayObject = 0;
	glClearColor(0.0, 0.0, 1.0, 1.0);
	glClearDepth(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mat4 transformedModel;
	for (int i = 0; i < objectNumber; i++)
	{

		model = glm::mat4(1.0f);
		view = glm::lookAt(glm::vec3(cameraX, cameraY, cameraZ), glm::vec3(focalX, focalY, focalZ), viewUpVector);
		projection = glm::frustum(-0.1f, 0.1f, -0.1f, 0.1f, nearDist, farDist);
		mv = view *model;
		mvp = projection * view * model * objectInformation[i].transformation;
		transformedModel = view * model  * objectInformation[i].transformation;

		glUseProgram(program);
		glUniformMatrix4fv(uniform_mvp, 1, GL_FALSE, glm::value_ptr(mvp));

		glUseProgram(program);
		glUniformMatrix4fv(uniform_mv, 1, GL_FALSE, glm::value_ptr(mv));
		glUniform1i(texLoc, 0);
		glUseProgram(program);
		glUniformMatrix4fv(uniform_model, 1, GL_FALSE, glm::value_ptr(transformedModel));
		bool success;

		const char* fileNameTeture = objectInformation[i].imageName.c_str();
		char textureFile[100];
		strcpy(textureFile, fileNameTeture);
		/*glEnable(GL_TEXTURE_2D);
		success = loadTexture(textureFile, true,i);
		if (!success)
			cerr << "error reading File" << endl;
*/

		glUniform3fv(ambient_uniform, 1, &objectInformation[i].ambientValue[0]);
		glUniform3fv(diffuse_uniform, 1, &objectInformation[i].diffuseValue[0]);
		glUniform3fv(specular_uniform, 1, &objectInformation[i].specValue[0]);
		glUniform1f(shininess_uniform, objectInformation[i].shineness);


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


		glEnableVertexAttribArray(kAPosition);
		glBindBuffer(GL_ARRAY_BUFFER, kABuffer[i]);
		glVertexAttribPointer(
			kAPosition, // attribute
			3,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
			);
		glEnableVertexAttribArray(kDPosition);
		glBindBuffer(GL_ARRAY_BUFFER, kDBuffer[i]);
		glVertexAttribPointer(
			kDPosition, // attribute
			3,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
			);
		glEnableVertexAttribArray(kSPosition);
		glBindBuffer(GL_ARRAY_BUFFER, kSBuffer[i]);
		glVertexAttribPointer(
			kSPosition, // attribute
			3,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
			);
		glEnableVertexAttribArray(shininessPosition);
		glBindBuffer(GL_ARRAY_BUFFER, shininessBuffer[i]);
		glVertexAttribPointer(
			shininessPosition, // attribute
			1,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
			);

		glEnableVertexAttribArray(normalPosition);
		glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer[i]);
		glVertexAttribPointer(
			normalPosition, // attribute
			3,                 // number of elements per vertex, here (R,G,B)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is
			0,                 // no extra data between each position
			0                  // offset of first element
			);

		glEnableVertexAttribArray(vertexTexturePosition);
		glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBuffer[i]);
		glVertexAttribPointer(vertexTexturePosition, 2, GL_FLOAT, GL_FALSE, 0, 0);
		
		for (int texInfo = 0; texInfo < objectInformation[i].texInfoForObject.size(); texInfo++)
		{
			const char* fileNameTeture = objectInformation[i].texInfoForObject[texInfo].texFileName.c_str();
			char textureFile[100];
			strcpy(textureFile, fileNameTeture);
			if (strcmp(textureFile, "dummy") != 0)
			{
				glEnable(GL_TEXTURE_2D);
				glActiveTexture(GL_TEXTURE0 + texInfo);
				success = loadTexture(textureFile, true);
				glUniform1i(texLoc, texInfo);
				if (!success)
					cerr << "error reading File" << endl;
			}
			else
			{
				glUniform1i(uniformEnable,1);
				glUniform3fv(ambient_uniform, 1, &objectInformation[i].texInfoForObject[texInfo].ambient[0]);
				glUniform3fv(diffuse_uniform, 1, &objectInformation[i].texInfoForObject[texInfo].diffuse[0]);
				glUniform3fv(specular_uniform, 1, &objectInformation[i].texInfoForObject[texInfo].specular[0]);
				glUniform1f(shininess_uniform, objectInformation[i].texInfoForObject[texInfo].shinenesssss);
			}
			glDrawArrays(GL_TRIANGLES, objectInformation[i].texInfoForObject[texInfo].startPos, objectInformation[i].texInfoForObject[texInfo].totalVertices);
			glUniform1i(uniformEnable, 2);
			glDisable(GL_TEXTURE_2D);
		}


		//glDrawArrays(GL_TRIANGLES, 0, objectInformation[i].numberOfFaces);
		glDisableVertexAttribArray(vPosition);
		glDisableVertexAttribArray(kAPosition);
		glDisableVertexAttribArray(kDPosition);
		glDisableVertexAttribArray(normalPosition);
		glDisableVertexAttribArray(vertexTexturePosition);


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

				//End
				keyWord = strtok(lineRead, " ");
				while (keyWord != NULL)
				{
					if (strcmp(keyWord, "newmtl") == 0)
					{
						newMaterialValue.ka[0] = 0.1f, newMaterialValue.ka[1] = 0.1f, newMaterialValue.ka[2] = 0.1f;
						newMaterialValue.kd[0] = 0.9f, newMaterialValue.kd[1] = 0.9f, newMaterialValue.kd[2] = 0.9f;
						newMaterialValue.ks[0] = 0.0f, newMaterialValue.ks[1] = 0.0f, newMaterialValue.ks[2] = 0.0f;
						newMaterialValue.shininess = 0.0f;
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
					}
					else if (strcmp(keyWord, "Ks") == 0)
					{
						// We do not handle this
						newMaterialValue.ks[0] = atof(strtok(NULL, delimiter));
						newMaterialValue.ks[1] = atof(strtok(NULL, delimiter));
						newMaterialValue.ks[2] = atof(strtok(NULL, "\n"));
					}
					else if (strcmp(keyWord, "Ns") == 0)
					{
						newMaterialValue.shininess = atof(strtok(NULL, "\n"));
						
					}
					else if (strcmp(keyWord, "map_Kd") == 0)
					{
						newMaterialValue.mapKd = strtok(NULL, "\n");
						storeValue = true;
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
			fgets(lineRead, 200, fp);
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
					else if (strcmp(keyWord, "light") == 0)
					{
						lightIndex++;

						char * lightType = strtok(NULL, delimiter);
						if (strcmp(lightType, "local") == 0)
						{
							LightsRa[lightIndex].isEnabled = true;
							LightsRa[lightIndex].isLocal = true;

						}
						else if (strcmp(lightType, "spot") == 0)
						{
							LightsRa[lightIndex].isEnabled = true;
							LightsRa[lightIndex].isLocal = true;
							LightsRa[lightIndex].isSpot = true;

						}
						else if (strcmp(lightType, "directional") == 0)
						{
							LightsRa[lightIndex].isEnabled = true;
							LightsRa[lightIndex].isDirectional = true;

						}


					}
					else if (strcmp(keyWord, "ambient") == 0)
					{
						float x = atof(strtok(NULL, delimiter));
						float y = atof(strtok(NULL, delimiter));
						float z = atof(strtok(NULL, delimiter));
						LightsRa[lightIndex].ambient.x = x;
						LightsRa[lightIndex].ambient.y = y;
						LightsRa[lightIndex].ambient.z = z;

					}
					else if (strcmp(keyWord, "color") == 0)
					{
						float x = atof(strtok(NULL, delimiter));
						float y = atof(strtok(NULL, delimiter));
						float z = atof(strtok(NULL, delimiter));
						LightsRa[lightIndex].color.x = x;
						LightsRa[lightIndex].color.y = y;
						LightsRa[lightIndex].color.z = z;

					}
					else if (strcmp(keyWord, "position") == 0)
					{
						float x = atof(strtok(NULL, delimiter));
						float y = atof(strtok(NULL, delimiter));
						float z = atof(strtok(NULL, delimiter));
						LightsRa[lightIndex].position.x = x;
						LightsRa[lightIndex].position.y = y;
						LightsRa[lightIndex].position.z = z;

					}
					else if (strcmp(keyWord, "constAtt") == 0)
					{
						LightsRa[lightIndex].constantAttenuation = atof(strtok(NULL, delimiter));

					}
					else if (strcmp(keyWord, "linearAtt") == 0)
					{
						LightsRa[lightIndex].linearAttenuation = atof(strtok(NULL, delimiter));

					}
					else if (strcmp(keyWord, "quadAtt") == 0)
					{
						LightsRa[lightIndex].quadraticAttenuation = atof(strtok(NULL, delimiter));

					}
					else if (strcmp(keyWord, "coneDirection") == 0)
					{
						float x = atof(strtok(NULL, delimiter));
						float y = atof(strtok(NULL, delimiter));
						float z = atof(strtok(NULL, delimiter));
						LightsRa[lightIndex].coneDirection.x = x;
						LightsRa[lightIndex].coneDirection.y = y;
						LightsRa[lightIndex].coneDirection.z = z;

					}
					else if (strcmp(keyWord, "spotCosCutoff") == 0)
					{
						LightsRa[lightIndex].spotCosCutoff = atof(strtok(NULL, delimiter));

					}
					else if (strcmp(keyWord, "spotExponent") == 0)
					{
						LightsRa[lightIndex].spotExponent = atof(strtok(NULL, delimiter));

					}
					else if (strcmp(keyWord, "camera") == 0)
					{
						float x = atof(strtok(NULL, delimiter));
						float y = atof(strtok(NULL, delimiter));
						float z = atof(strtok(NULL, delimiter));
						defaultx = cameraX = x;
						defaulty = cameraY = y;
						defaultz = cameraZ = z;

					}
					else if (strcmp(keyWord, "focal") == 0)
					{
						float x = atof(strtok(NULL, delimiter));
						float y = atof(strtok(NULL, delimiter));
						float z = atof(strtok(NULL, delimiter));
						defaultfocalx = focalX = x;
						defaultfocaly = focalY = y;
						defaultfocalz = focalZ = z;
					}
					else if (strcmp(keyWord, "viewup") == 0)
					{
						float x = atof(strtok(NULL, delimiter));
						float y = atof(strtok(NULL, delimiter));
						float z = atof(strtok(NULL, delimiter));
						defaultViewx = x; defaultViewy = y; defaultViewz = z;
						viewUpVector = { x, y, z };

					}
					keyWord = strtok(NULL, delimiter);
					if (keyWord == NULL && rotationApplied == true)
					{
						rotationApplied = false;
						quat rotationInformation;
						rotationInformation = angleAxis(radians(objectInformation[objectNumber].rotationDegree),
							vec3(objectInformation[objectNumber].rotationValueForObject.x, objectInformation[objectNumber].rotationValueForObject.y,
							objectInformation[objectNumber].rotationValueForObject.z));
						objectInformation[objectNumber].transformation = mat4_cast(rotationInformation);


						//	glm::rotate(objectInformation[objectNumber].transformation,
						//	objectInformation[objectNumber].rotationDegree,
						//	vec3(objectInformation[objectNumber].rotationValueForObject.x, objectInformation[objectNumber].rotationValueForObject.y, objectInformation[objectNumber].rotationValueForObject.z));
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
		cout << " Color :: " << newMaterialValue.color << endl;
		cout << " Ka :: " << newMaterialValue.ka[0] << " " << newMaterialValue.ka[1] << " " << newMaterialValue.ka[2];
		cout << " Kd :: " << newMaterialValue.kd[0] << " " << newMaterialValue.kd[1] << " " << newMaterialValue.kd[2];
		cout << " Ks :: " << newMaterialValue.ks[0] << " " << newMaterialValue.ks[1] << " " << newMaterialValue.ks[2];
		cout << " Shininess :: " << newMaterialValue.shininess;
		cout << " Map_Kd" << newMaterialValue.mapKd;
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
			//cout << verticesColor[i][j] << "  ";

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
void printTextureValues()
{
	cout << endl << "Total Size ::" << textureValues.size();
	for (int i = 0; i < textureValues.size(); i++)
	{
		cout << endl << "Texture ::" << i;
		cout << endl << "X ::" << textureValues[i].x;
		cout << endl << "Y ::" << textureValues[i].y;
		cout << endl;
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
void fillNormalArrayAndComputeColor(int size, struct materialValues fetchedMaterialValues, vec3 calculatedNormal)
{
	for (int i = finalNormalValue; i < finalNormalValue + 3; i++)
	{
		normalValues.push_back(vec3(calculatedNormal.x, calculatedNormal.y, calculatedNormal.z));
		kAValues.push_back(vec3(fetchedMaterialValues.ka[0], fetchedMaterialValues.ka[1], fetchedMaterialValues.ka[2]));
		kDValues.push_back(vec3(fetchedMaterialValues.kd[0], fetchedMaterialValues.kd[1], fetchedMaterialValues.kd[2]));
		kSValues.push_back(vec3(fetchedMaterialValues.ks[0], fetchedMaterialValues.ks[1], fetchedMaterialValues.ks[2]));
		shininessValues.push_back(fetchedMaterialValues.shininess);
	}

	finalNormalValue = finalNormalValue + 3;

}
vec3 computeNormalValues(int readFaces, struct materialValues fetchedMaterialValues)
{

	vec3 edge1;
	vec3 edge2;
	vec3 calculatedNormal;

	edge1.x = finalVertices[readFaces][0] - finalVertices[readFaces - 2][0];
	edge1.y = finalVertices[readFaces][1] - finalVertices[readFaces - 2][1];
	edge1.z = finalVertices[readFaces][2] - finalVertices[readFaces - 2][2];

	edge2.x = finalVertices[readFaces - 1][0] - finalVertices[readFaces - 2][0];
	edge2.y = finalVertices[readFaces - 1][1] - finalVertices[readFaces - 2][1];
	edge2.z = finalVertices[readFaces - 1][2] - finalVertices[readFaces - 2][2];

	calculatedNormal = cross(edge2, edge1);

	fillNormalArrayAndComputeColor(readFaces, fetchedMaterialValues, calculatedNormal);


	return calculatedNormal;
}

void readInputFile(char* fileName, char* filePath, int objNumber)
{
	FILE * fp;
	bool materialValueFound = false;
	int countOfSlashes = 0, lengthOfSlashes = 0;
	char *token;
	char *keyWord;
	char *lineRead = NULL;
	lineRead = (char*)malloc(100);
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
		int startTex = -1, endTex = 0;
		bool firstTime = true;
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

							mtlLibName = strtok(NULL, delimiter);
							strcat(filePath, mtlLibName);
							//puts(filePath);
							//cout << endl << "Mat lib name ::" << filePath;
							readMaterialFile(mtlLibName);						// CHANGE TO FILE PATH ON LINUX ENV
							//printMaterialFile(); //For Debugging
						}
						else if (strcmp(keyWord, "usemtl") == 0)
						{
							if (startTex != -1)
							{
								struct textureInfo tempInfo;
								tempInfo.startPos = startTex;
								tempInfo.endPos = endTex;
								tempInfo.totalVertices = endTex - startTex;
								tempInfo.texFileName = foundMaterialValue.mapKd;
								tempInfo.shinenesssss = foundMaterialValue.shininess;
								tempInfo.ambient = vec3(foundMaterialValue.ka[0], foundMaterialValue.ka[1], foundMaterialValue.ka[2]);
								tempInfo.diffuse = vec3(foundMaterialValue.kd[0], foundMaterialValue.kd[1], foundMaterialValue.kd[2]);
								tempInfo.specular = vec3(foundMaterialValue.ks[0], foundMaterialValue.ks[1], foundMaterialValue.ks[2]);
								tempInfo.shinenesssss = foundMaterialValue.shininess;

								objectInformation[objNumber].texInfoForObject.push_back(tempInfo);
								startTex = endTex;
							}
							if (firstTime)
							{
								firstTime = false;
								startTex = 0;
							}
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
							if (xValue == -9.9999999999999995e-007 || xValue == 9.9999999999999995e-007 || xValue == 9.99999997e-007 || xValue == -9.99999997e-007)
							{
								xValue = 0;
							}
							verticesNormal[verticesNormalsIndex][0] = xValue;
							float yValue = atof(strtok(NULL, delimiter));
							if (yValue == -9.9999999999999995e-007 || yValue == 9.9999999999999995e-007 || yValue == 9.99999997e-007 || yValue == -9.99999997e-007)
							{
								yValue = 0;
							}
							verticesNormal[verticesNormalsIndex][1] = yValue;
							float zValue = atof(strtok(NULL, delimiter));
							if (zValue == -9.9999999999999995e-007 || zValue == 9.9999999999999995e-007 || zValue == 9.99999997e-007 || zValue == -9.99999997e-007)
							{
								zValue = 0;
							}
							verticesNormal[verticesNormalsIndex][2] = zValue;
							verticesNormalsIndex++;
						}
						else if (strcmp(keyWord, "vt") == 0)
						{

							float xValue = atof(strtok(NULL, delimiter));
							if (xValue == -9.9999999999999995e-007 || xValue == 9.9999999999999995e-007 || xValue == 9.99999997e-007 || xValue == -9.99999997e-007)
							{
								xValue = 0;
							}
							verticesTexture[verticesTextureIndex][0] = xValue;
							float yValue = atof(strtok(NULL, delimiter));
							if (yValue == -9.9999999999999995e-007 || yValue == 9.9999999999999995e-007 || yValue == 9.99999997e-007 || yValue == -9.99999997e-007)
							{
								yValue = 0;
							}
							verticesTexture[verticesTextureIndex][1] = yValue;
							verticesTextureIndex++;
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
														foundMaterialValue.ks[0] = 0.0f, foundMaterialValue.ks[1] = 0.0f, foundMaterialValue.ks[2] = 0.0f;
														foundMaterialValue.shininess = 0.0f;
													}
													computeNormalValues(facesValue, foundMaterialValue);
												}
											}
											facesValue++;
											endTex++;
											i = i + length;
										}
										else if (positionOfSlash == 1)
										{
											
											// Handle face Texture here
											string result = getValueTillSlash(i, value);
											int vertexTextureIndex = atoi(result.c_str()) - 1;
											int length = result.length();
											// WRONG WAY FOR MULTIPLE TEXTURES
											if (fileNames.size() == 0)
											{
												fileNames.push_back(foundMaterialValue.mapKd);
											}
											textureValues.push_back(vec2(verticesTexture[vertexTextureIndex][0], verticesTexture[vertexTextureIndex][1]));
											i = i + length;
										}
										else if (positionOfSlash == 2)
										{
											// Handle face normal here
											string result = getValueTillSlash(i, value);
											int vertexIndex = atoi(result.c_str()) - 1;
											int length = result.length();

											normalValues.push_back(vec3(verticesNormal[vertexIndex][0], verticesNormal[vertexIndex][1], verticesNormal[vertexIndex][2]));

											if (materialValueFound == false)
											{
												//cout << endl << " No color Use Default";
												strcpy(foundMaterialValue.color, "default");
												foundMaterialValue.ka[0] = 0.1f, foundMaterialValue.ka[1] = 0.1f, foundMaterialValue.ka[2] = 0.1f;
												foundMaterialValue.kd[0] = 0.9f, foundMaterialValue.kd[1] = 0.9f, foundMaterialValue.kd[2] = 0.9f;
												foundMaterialValue.ks[0] = 0.0f, foundMaterialValue.ks[1] = 0.0f, foundMaterialValue.ks[2] = 0.0f;
												foundMaterialValue.shininess = 0.0f;

											}
											kAValues.push_back(vec3(foundMaterialValue.ka[0], foundMaterialValue.ka[1], foundMaterialValue.ka[2]));
											kDValues.push_back(vec3(foundMaterialValue.kd[0], foundMaterialValue.kd[1], foundMaterialValue.kd[2]));
											kSValues.push_back(vec3(foundMaterialValue.ks[0], foundMaterialValue.ks[1], foundMaterialValue.ks[2]));
											shininessValues.push_back(foundMaterialValue.shininess);

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
											foundMaterialValue.ks[0] = 0.0f, foundMaterialValue.ks[1] = 0.0f, foundMaterialValue.ks[2] = 0.0f;
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
		if (startTex != -1)
		{
			struct textureInfo tempInfo;
			tempInfo.startPos = startTex;
			tempInfo.endPos = endTex;
			tempInfo.totalVertices = endTex - startTex;
			tempInfo.texFileName = foundMaterialValue.mapKd;
			tempInfo.shinenesssss = foundMaterialValue.shininess;
			tempInfo.ambient = vec3(foundMaterialValue.ka[0], foundMaterialValue.ka[1], foundMaterialValue.ka[2]);
			tempInfo.diffuse = vec3(foundMaterialValue.kd[0], foundMaterialValue.kd[1], foundMaterialValue.kd[2]);
			tempInfo.specular = vec3(foundMaterialValue.ks[0], foundMaterialValue.ks[1], foundMaterialValue.ks[2]);

			objectInformation[objNumber].texInfoForObject.push_back(tempInfo);
			startTex = endTex;
		}
		if (firstTime)
		{
			firstTime = false;
			startTex = 0;
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
	//cout << " kEY :: " << key << endl;
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

		//cameraX = 3 * maxXRange, cameraY = 3 * maxYRange, cameraZ = maxZRange;
		//focalX = (int)((maxXRange - minXRange) / 2);
		//focalY = (int)((maxYRange - minYRange) / 2);
		//focalZ = (int)((maxZRange - minZRange) / 2);
		//focalX = 0.0f, focalY = 0.0f, focalZ = 0.0f;
		//lookupX = 0.0f, lookupY = 0.0f, lookupZ = 1.0f;
		//viewUpVector = { lookupX, lookupY, lookupZ };
		cameraX = defaultx;
		cameraY = defaulty;
		cameraZ = defaultz;
		focalX = defaultfocalx;
		focalY = defaultfocaly;
		focalZ = defaultfocalz;
		viewUpVector = { defaultViewx, defaultViewy, defaultViewz };
		nearDist = 0.1f, farDist = 200.0f;
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
void reshape(int width, int height)
{

	//Declare HEIGHT and WIDTH of the window as global variables and use them in glutInitWindowSize

	const float ar_origin = (float)screen_width / (float)screen_height;
	const float ar_new = (float)width / (float)height;

	float scale_w = (float)width / (float)screen_width;
	float scale_h = (float)height / (float)screen_height;
	if (ar_new > ar_origin) {
		scale_w = scale_h;
	}
	else {
		scale_h = scale_w;
	}

	float margin_x = (width - screen_width * scale_w) / 2;
	float margin_y = (height - screen_height * scale_h) / 2;

	glViewport(margin_x, margin_y, screen_width * scale_w, screen_height * scale_h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, screen_width / ar_origin, 0, screen_height / ar_origin, 0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
float getRadius()
{
	return sqrt(pow((cameraX - focalX), 2) + pow((cameraY - focalY), 2));
}
void handleSpecialKeypress(int key, int x, int y)
{
	float radius;
	//cout << " special kEY :: " << key << endl;
	string clr;
	switch (key)
	{
	case GLUT_KEY_UP:
		nearDist = nearDist + 0.1f;
		break;
	case GLUT_KEY_DOWN:
		nearDist = nearDist - 0.1f;
		break;
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

	default: return;
	}
	glutPostRedisplay();

}
void setUniformVariables()
{


	glUniform1i(uniform_numberOfLisghtSources, lightIndex);
	for (int i = 0; i <= lightIndex; i++)
	{
		glUseProgram(program);
		GLint myLoc;
		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].isEnabled");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].isEnabled");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].isEnabled");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].isEnabled");
		glUniform1i(myLoc, LightsRa[i].isEnabled);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].isLocal");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].isLocal");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].isLocal");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].isLocal");
		glUniform1i(myLoc, LightsRa[i].isLocal);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].isSpot");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].isSpot");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].isSpot");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].isSpot");
		glUniform1i(myLoc, LightsRa[i].isSpot);

		/*if (i == 0)
		myLoc = glGetUniformLocation(program, "Lights[0].isDirectional");
		else if (i == 1)
		myLoc = glGetUniformLocation(program, "Lights[1].isDirectional");
		else if (i == 2)
		myLoc = glGetUniformLocation(program, "Lights[2].isDirectional");
		else if (i == 3)
		myLoc = glGetUniformLocation(program, "Lights[3].isDirectional");
		glUniform1i(myLoc, LightsRa[i].isDirectional);*/

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].ambient");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].ambient");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].ambient");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].ambient");
		glUniform3fv(myLoc, 1, &LightsRa[i].ambient[0]);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].color");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].color");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].color");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].color");
		glUniform3fv(myLoc, 1, &LightsRa[i].color[0]);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].position");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].position");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].position");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].position");
		glUniform3fv(myLoc, 1, &LightsRa[i].position[0]);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].constantAttenuation");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].constantAttenuation");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].constantAttenuation");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].constantAttenuation");
		glUniform1f(myLoc, LightsRa[lightIndex].constantAttenuation);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].linearAttenuation");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].linearAttenuation");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].linearAttenuation");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].linearAttenuation");
		glUniform1f(myLoc, LightsRa[lightIndex].linearAttenuation);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].quadraticAttenuation");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].quadraticAttenuation");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].quadraticAttenuation");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].quadraticAttenuation");
		glUniform1f(myLoc, LightsRa[lightIndex].quadraticAttenuation);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].coneDirection");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].coneDirection");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].coneDirection");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].coneDirection");
		glUniform3fv(myLoc, 1, &LightsRa[i].coneDirection[0]);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].spotCosCutoff");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].spotCosCutoff");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].spotCosCutoff");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].spotCosCutoff");
		glUniform1f(myLoc, LightsRa[lightIndex].spotCosCutoff);

		if (i == 0)
			myLoc = glGetUniformLocation(program, "Lights[0].spotExponent");
		else if (i == 1)
			myLoc = glGetUniformLocation(program, "Lights[1].spotExponent");
		else if (i == 2)
			myLoc = glGetUniformLocation(program, "Lights[2].spotExponent");
		else if (i == 3)
			myLoc = glGetUniformLocation(program, "Lights[3].spotExponent");
		glUniform1f(myLoc, LightsRa[lightIndex].spotExponent);
	}
}
////////////////////////////////////////////////////////////////////////
//	main
////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{

	cout << " HEllo";
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
	init();
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
		setUniformVariables();
		//printContentsOfLightObject();
		//printContentsOfObject();
	}



	//printContentsOfObject();
	// Code to read everything from the files specified by control file
	for (int i = 0; i < objectNumber; i++)
	{
		strcpy(fileName, objectInformation[i].fileName);
		strcpy(filePath, objectInformation[i].path);
		//cout << "\nPath from Object :: " << objectInformation[i].path;
		readInputFile(fileName, filePath,i);
		objectInformation[i].numberOfFaces = facesValue;
		printObjectTextureInfo(i);
		//printTextureValues();
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
		verticesTextureIndex = 0;
		facesValue = 0;
		finalNormalValue = 0;
		NumVertices = 0;


		// Clear Out all arrays and reset the Variables and set it up for next iteration 
		memset(vertices, 0, sizeof(vertices[0][0]) * _MAX_ROWS_ * 3);
		memset(verticesNormal, 0, sizeof(verticesNormal[0][0]) * _MAX_ROWS_ * 3);
		memset(finalVertices, 0, sizeof(finalVertices[0][0]) * _MAX_ROWS_ * 3);
		memset(finalVerticesNormals, 0, sizeof(finalVerticesNormals[0][0]) * _MAX_ROWS_ * 3);
		memset(faces, 0, sizeof(faces[0]) * _MAX_ROWS_);

		kAValues.clear();
		kDValues.clear();
		kSValues.clear();
		shininessValues.clear();
		normalValues.clear();
		textureValues.clear();
		fileNames.clear();
	}
	//printMaxAndMinAndCenter();
	//cameraX = 3 * maxXRange;
	//cameraY = 3 * maxYRange;
	//cameraZ = maxZRange;
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


