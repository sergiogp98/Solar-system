// Lab 1-1.
// This is the same as the first simple example in the course book,
// but with a few error checks.
// Remember to copy your file to a new on appropriate places during the lab so you keep old results.
// Note that the files "lab1-1.frag", "lab1-1.vert" are required.

// Should work as is on Linux and Mac. MS Windows needs GLEW or glee.
// See separate Visual Studio version of my demos.
#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include <math.h>
#include <stdio.h>
#include "loadobj.h"
#include "VectorUtils3.h"
//Frustum dimensions
#define near 1.0
#define far 30.0
#define right 0.5
#define left -0.5
#define top 0.5
#define bottom -0.5

// Globals
// Data would normally be read from files

Model *m1, *m2;

GLuint program;
GLuint myTex1, myTex2; //Texture reference

//Render new images repeatedly
void OnTimer(int value){
  glutPostRedisplay();
  glutTimerFunc(20, &OnTimer, value);
}

// vertex array object
unsigned int vertexArrayObjID;

void init(void)
{
	dumpInfo();

  m1 = LoadModelPlus("bunnyplus.obj"); //Load new bunny model
  m2 = LoadModelPlus("cubeplus.obj"); //Load cube model
	LoadTGATextureSimple("maskros512.tga", &myTex1); //Load TGA image to texture 1
  LoadTGATextureSimple("dirt.tga", &myTex2); //Load TGA image to texture 2

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab2-7.vert", "lab2-7.frag");
	printError("init shader");

	// Upload geometry to the GPU:

	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);

	//Activate texture object
  glActiveTexture(GL_TEXTURE0);
  glActiveTexture(GL_TEXTURE1);

  // End of upload of geometry

	printError("init arrays");
}


void display(void)
{
  //Add time-based rotation/translation
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	//Transformation and proyection matrices
  mat4 cameraRotation, proyectionMatrix;
  cameraRotation = Ry(-0.001*t);
  proyectionMatrix = frustum(left, right, bottom, top, near, far);

  //Model-to-view matrix
  mat4 modelToWorldMatrixBunny = T(1, 0, 0);
  mat4 modelToWorldMatrixCube = T(-1, 0, 0);

  //World-to-view matrix
	mat4 worldToViewMatrix = lookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);

  //World to projection matrix
  mat4 worldToProjectionMatrix = Mult(proyectionMatrix, Mult(worldToViewMatrix, cameraRotation));

  //Send world to projection matrix to shader
  glUniformMatrix4fv(glGetUniformLocation(program, "worldToProjectionMatrix"), 1, GL_TRUE, worldToProjectionMatrix.m);

	//Erase Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Select VAO
	glBindVertexArray(vertexArrayObjID);

  //First model
      //Bind texture object to a texture unit
      glBindTexture(GL_TEXTURE_2D, myTex1);
      glUniform1i(glGetUniformLocation(program, "texUnit"), 0); //Set texture unit to 0 and send it to the sampler in the vertex shader

      //Send model to world matrix to shader
      glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixBunny.m);

      //Draw the first model
      DrawModel(m1, program, "inPosition", "inNormal", "inTexCoord");

  //Second model
      //Bind texture object to a texture unit
      glBindTexture(GL_TEXTURE_2D, myTex2);
      glUniform1i(glGetUniformLocation(program, "texUnit"), 0); //Set texture unit to 0 and send it to the sampler in the vertex shader

      //Send model to world matrix to shader
      glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixCube.m);

      //Draw the second model
      DrawModel(m2, program, "inPosition", "inNormal", "inTexCoord");

  printError("display");
  glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitContextVersion(3, 2);
	glutCreateWindow ("GL3 white triangle example");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &OnTimer, 0);
	glutMainLoop();
	return 0;
}
