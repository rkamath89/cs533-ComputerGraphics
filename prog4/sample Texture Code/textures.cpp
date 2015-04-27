//program2-triangle.cpp
//display a trigangle in 3-D space

#include <iostream>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

#include "vgl.h"
#include "LoadShaders.h"

using namespace std;

// switched from vmath.h to glm
//#include "vmath.h"
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "SOIL.h"

using namespace glm;

vec3 viewUp( 0., 0., 1. );
vec3 eyePoint( 0.75f, 0.5f, 0.5f );
vec3 focalPoint( 0.f, 0.5f, 0.5f);
vec3 eyePointSave = eyePoint;
vec3 focalPointSave = focalPoint;
vec3 viewUpSave = viewUp;

float aspect = 1.f;			// aspect ratio of y over x
float farDistance = 200.f;		// default depth, distance to far clipping plane
float nearDistance = .5f;		// distance to near clipping plane

GLint modelMatrixLoc;
GLint viewMatrixLoc;
GLint projectionMatrixLoc;
GLint MVPMatrixLoc;

GLuint shaderProgram;			// shader program for Gouraud shading

GLuint vaoRectangle;
GLuint vboRectangle;
GLuint vboTexIndices;
GLuint vboRectangleColors;

const GLuint NumVertices = 6;

GLint texLoc;
GLuint checkTex;	// id for checkboard texture

GLint texUnit = 0;

const float SCALE = 0.2f;
const float sceneRadius = 0.2f;
vec3 gaze;
mat4 transform;
const float DEG2RAD = 3.14159f / 180.f;
vec4 tempV4;


bool wireframeRendering = false;

GLfloat red = 0.f, green = 0.f, blue = 1.f;	// colors for two triangles and circle

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

/////////////////////////////////////////////////////
// init
/////////////////////////////////////////////////////
void init (void )
{

	ShaderInfo textureShaders[] = {
		{ GL_VERTEX_SHADER, "tex.vert" },
		{ GL_FRAGMENT_SHADER, "tex.frag" },
		{ GL_NONE, NULL }
	};

	shaderProgram = LoadShaders(textureShaders);

	GLfloat rectangle[6][3] = {
		{ 0.f, 0.f, 0.f } ,    
		{ 0.f, 1.f, 0.f },
		{ 0.f, 1.f, 1.f },
		{ 0.f, 0.f, 0.f },
		{ 0.f, 1.f, 1.f },
		{ 0.f, 0.f, 1.f }
	};

	GLfloat rectangleColors[6][3] = {
		{ 1.f, 0.f, 0.f },
		{ 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f },
		{ 1.f, 0.f, 0.f },
		{ 0.f, 0.f, 1.f },
		{ 0.f, 1.f, 0.f }
	};

	GLfloat rectangleTex[6][2] = {
		{0.f, 0.f},
		{1.f, 0.f},
		{1.f, 1.f},
		{0.f, 0.f},
		{1.f, 1.f},
		{0.f, 1.f}
	};

	glUseProgram( shaderProgram );
	//modelMatrixLoc = glGetUniformLocation( shaderProgram, "modelMatrix" );
	//viewMatrixLoc  = glGetUniformLocation( shaderProgram, "viewMatrix" );
	//projectionMatrixLoc = glGetUniformLocation( shaderProgram, "projectionMatrix" );

	MVPMatrixLoc = glGetUniformLocation(shaderProgram, "MVPMatrix");
	texLoc = glGetUniformLocation(shaderProgram, "tex");

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);	

	// set up the single triangle
	glGenVertexArrays( 1, &vaoRectangle );
	glBindVertexArray(vaoRectangle);

	glGenBuffers( 1, &vboRectangle );
	glBindBuffer(GL_ARRAY_BUFFER, vboRectangle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangle), rectangle, GL_STATIC_DRAW);
	glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray( 0 );

	glGenBuffers( 1, &vboRectangleColors );
	glBindBuffer(GL_ARRAY_BUFFER, vboRectangleColors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleColors), rectangleColors, GL_STATIC_DRAW);
	glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray( 1 );

	glGenBuffers(1, &vboTexIndices);
	glBindBuffer(GL_ARRAY_BUFFER, vboTexIndices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleTex), rectangleTex, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	// ***************** procedural texture - checkerboard ****************************
	// set up checkerboard texture map  adapted from http://www.glprogramming.com/red/chapter09.html
	const int checkImageWidth = 64;
	const int checkImageHeight = 64;
	static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

	int i, j, c;

	for (i = 0; i < checkImageHeight; i++) 
	{
		for (j = 0; j < checkImageWidth; j++) 
		{
			c = ((((i & 0x8) == 0) ^ ((j & 0x8)) == 0)) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &checkTex);
	glBindTexture(GL_TEXTURE_2D, checkTex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
		checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		checkImage);

	//*********************** using SOIL to load texture from image
	bool success;
	glActiveTexture(GL_TEXTURE0 + 1);

	//imageTex = SOIL_load_OGL_texture(
	//	"mandrill.png",
	//	SOIL_LOAD_AUTO,
	//	SOIL_CREATE_NEW_ID,
	//	SOIL_FLAG_INVERT_Y
	//	);

	success = loadTexture("mandrill.png", true);
	if (!success)
		cerr << "error reading mandrill.png" << endl;

	//*********************** load monarch
	glActiveTexture(GL_TEXTURE0 + 2);
	success = loadTexture("monarch.png", true);
	if (!success)
		cerr << "error reading monarch.png" << endl;

	//*********************** load fern
	glActiveTexture(GL_TEXTURE0 + 3);
	success = loadTexture("TRU_Fern_Frond_01.png", false);
	if (!success)
		cerr << "error reading TRU_Fern_Frond_01.png" << endl;

	//*********************** load earth
	glActiveTexture(GL_TEXTURE0 + 4);
	success = loadTexture("earth apollo 17.png", true);
	if (!success)
		cerr << "error reading earth apollo 17.png" << endl;

	//*********************** load test_1.jpb	
	glActiveTexture(GL_TEXTURE0 + 5);
	success = loadTexture("test_1.jpg", true);
	if (!success)
		cerr << "error reading test_1.jpg" << endl;



	glLineWidth( 5.f );		// thick lines for wireframe
}

////////////////////////////////////////////////////////////////////
// display
////////////////////////////////////////////////////////////////////
void display (void )
{
	glClearColor(0.0f, 0.25f, 0.3f, 1.0f);
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	glUseProgram( shaderProgram );
	
	// set up all the viewing matrices
	mat4 modelMatrix = mat4(1.f);

	mat4 viewMatrix = lookAt( eyePoint, focalPoint, viewUp );

	mat4 projectionMatrix = frustum( -1.0f, 1.0f, -aspect, aspect, nearDistance, farDistance );

	mat4 MVPMatrix = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(MVPMatrixLoc, 1, GL_FALSE, value_ptr(MVPMatrix));

	glUniform1i(texLoc, texUnit);

	glBindVertexArray(vaoRectangle);	// single triangle
	
	glDrawArrays( GL_TRIANGLES, 0, 6 );
		
	glFlush();
}
//////////////////////////////////////////////////////////
// reshape
//////////////////////////////////////////////////////////
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);

	aspect = float(height) / float(width);
}

//////////////////////////////////////////////////////////
// keys
//////////////////////////////////////////////////////////
void keys ( unsigned char key, int x, int y )
{
	bool success = false;
	float angleInc;

	switch (key ) {
		case 27:
		case 'q':
			exit( 99 );
			break;	
		case 's':	// set solid surface rasterization
			glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
			glutPostRedisplay();
			break;
		case 'w':
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			glutPostRedisplay();
			break;

		case 'c':	// move the camera in the opposite direction of the view up vector
			eyePoint[0] -= (SCALE * sceneRadius) * viewUp[0];
			eyePoint[1] -= (SCALE * sceneRadius) * viewUp[1];
			eyePoint[2] -= (SCALE * sceneRadius) * viewUp[2];
			glutPostRedisplay();
			break;
		case 'v':	// move the camera in the direction of the view up vector
			eyePoint[0] += (SCALE * sceneRadius) * viewUp[0];
			eyePoint[1] += (SCALE * sceneRadius) * viewUp[1];
			eyePoint[2] += (SCALE * sceneRadius) * viewUp[2];
			glutPostRedisplay();
			break;
		case 'd':  // move focal point in the direction of the view up vector by a positive step
			focalPoint[0] -= (SCALE * sceneRadius) * viewUp[0];
			focalPoint[1] -= (SCALE * sceneRadius) * viewUp[1];
			focalPoint[2] -= (SCALE * sceneRadius) * viewUp[2];
			glutPostRedisplay();
			break;
		case 'f':	// move the focal point along the direction of the view up vector by a negative step
			focalPoint[0] += (SCALE * sceneRadius) * viewUp[0];
			focalPoint[1] += (SCALE * sceneRadius) * viewUp[1];
			focalPoint[2] += (SCALE * sceneRadius) * viewUp[2];
			glutPostRedisplay();
			break;
		case 'x':	// rotate view up 1 degree around the view up vector
			gaze = focalPoint - eyePoint;
			gaze = normalize(gaze);
			transform = rotate(mat4(1.f), 1.0f * DEG2RAD, gaze);
			tempV4.x = viewUp.x; tempV4.y = viewUp.y; tempV4.z = viewUp.z; tempV4.w = 1.0f;
			tempV4 = transform * tempV4;
			viewUp = vec3(tempV4);
			viewUp = normalize(viewUp);
			glutPostRedisplay();
			break;
		case 'z':	// rotate view up -1 degree around the view up vector
			gaze = focalPoint - eyePoint;
			gaze = normalize(gaze);
			transform = rotate(mat4(1.0f), -1.0f * DEG2RAD, gaze);
			tempV4.x = viewUp.x; tempV4.y = viewUp.y; tempV4.z = viewUp.z; tempV4.w = 1.0f;
			tempV4 = transform * tempV4;
			viewUp = vec3(tempV4);
			viewUp = normalize(viewUp);
			glutPostRedisplay();
			break;
		case 'r':	// reset viewing
			eyePoint = eyePointSave;
			focalPoint = focalPointSave;
			viewUp = viewUpSave;
			glutPostRedisplay();
			break;
		case 't':	// toggle texture
			texUnit = (texUnit + 1) % 6;
			glutPostRedisplay();
			break;
	}
}

//////////////////////////////////////////////////////////
// specialInput
//////////////////////////////////////////////////////////
void specialInput(int key, int x, int y)
{
	vec3 gaze;
	mat4 transform;
	const float SCALE = 0.01f;
	vec4 tempV4;
	vec3 axis;
	float DEG2RAD = 3.14159f / 180.f;

	switch (key)
	{
	case GLUT_KEY_UP:
		// move eye position along gaze or forward vector
		gaze[0] = focalPoint[0] - eyePoint[0];
		gaze[1] = focalPoint[1] - eyePoint[1];
		gaze[2] = focalPoint[2] - eyePoint[2];
		gaze = normalize(gaze);
		eyePoint[0] += (SCALE * sceneRadius) * gaze[0];
		eyePoint[1] += (SCALE * sceneRadius) * gaze[1];
		eyePoint[2] += (SCALE * sceneRadius) * gaze[2];
		focalPoint[0] += (SCALE * sceneRadius) * gaze[0];
		focalPoint[1] += (SCALE * sceneRadius) * gaze[1];
		focalPoint[2] += (SCALE * sceneRadius) * gaze[2];
		glutPostRedisplay();
		break;

	case GLUT_KEY_DOWN:
		// move eye position back along gaze or forward vector
		gaze[0] = focalPoint[0] - eyePoint[0];
		gaze[1] = focalPoint[1] - eyePoint[1];
		gaze[2] = focalPoint[2] - eyePoint[2];
		gaze = normalize(gaze);
		eyePoint[0] -= (SCALE * sceneRadius) * gaze[0];
		eyePoint[1] -= (SCALE * sceneRadius) * gaze[1];
		eyePoint[2] -= (SCALE * sceneRadius) * gaze[2];
		focalPoint[0] -= (SCALE * sceneRadius) * gaze[0];
		focalPoint[1] -= (SCALE * sceneRadius) * gaze[1];
		focalPoint[2] -= (SCALE * sceneRadius) * gaze[2];
		glutPostRedisplay();
		break;

	case GLUT_KEY_LEFT:
		// rotate the eye point and focal point around the z axis
		axis.x = 0.f; axis.y = 0.f; axis.z = 1.f;
		transform = rotate(mat4(1.0f), -1.0f * DEG2RAD, axis);
		tempV4.x = focalPoint.x; tempV4.y = focalPoint.y; tempV4.z = focalPoint.z; tempV4.w = 1.0f;
		tempV4 = transform * tempV4;
		focalPoint = (vec3)tempV4;
		tempV4.x = eyePoint.x; tempV4.y = eyePoint.y; tempV4.z = eyePoint.z; tempV4.w = 1.0f;
		tempV4 = transform * tempV4;
		eyePoint = vec3(tempV4);

		glutPostRedisplay();
		break;

	case GLUT_KEY_RIGHT:
		// rotate the eye point and focal point around the z axis
		axis.x = 0.f; axis.y = 0.f; axis.z = 1.f;
		transform = rotate(mat4(1.0f), 1.0f * DEG2RAD, axis);
		tempV4.x = eyePoint.x; tempV4.y = eyePoint.y; tempV4.z = eyePoint.z; tempV4.w = 1.0f;
		tempV4 = transform * tempV4;
		eyePoint = vec3(tempV4);
		tempV4.x = focalPoint.x; tempV4.y = focalPoint.y; tempV4.z = focalPoint.z; tempV4.w = 1.0f;
		tempV4 = transform * tempV4;
		focalPoint = vec3(tempV4);
		glutPostRedisplay();
		break;
	}
}
////////////////////////////////////////////////////////////////////////
// main
////////////////////////////////////////////////////////////////////////
int main(int argc, char* argv[])
{
	glutInit( &argc, argv );
	glutInitDisplayMode( GLUT_RGBA );
	glutInitWindowSize( 512, 512 );
	// glutInitContextVersion( 4, 3 );
	// glutInitContextProfile( GLUT_CORE_PROFILE );// GLUT_COMPATIBILITY_PROFILE );
	glutCreateWindow( argv[0] );

	glewExperimental = GL_TRUE;	// added for glew to work!
	if ( glewInit() )
	{
		cerr << "Unable to initialize GLEW ... exiting" << endl;
		exit (EXIT_FAILURE );
	}

	init();
	glutDisplayFunc( display );
	glutKeyboardFunc( keys );
	glutSpecialFunc(specialInput);
	glutReshapeFunc(reshape);		// new with the viewer program
	glutMainLoop();

	return 0;
}

