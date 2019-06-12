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
//PI value
#define PI 3.14159265358979323846

// Globals
// Data would normally be read from files

Model *bladeModel, //Wing model
			*balconyModel, //Balcony model
			*roofModel, //Roof model
			*wallsModel; //Walls model

GLuint program;
GLuint bladeTexture, roofTexture, wallTexture, balconyTexture; //Texture reference

//Keyboard events variables
GLfloat cameraPositionX = 0, 
			  cameraPositionZ = 0, 
				cameraRotation = 0, 
				viewPositionX = 0, 
				viewPositionZ = -3;

//World-to-view matrix
mat4 worldToViewMatrix;

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

    bladeModel = LoadModelPlus("./windmill/blade.obj"); //Load wing model
		balconyModel = LoadModelPlus("./windmill/windmill-balcony.obj"); //Load balcony model
		roofModel = LoadModelPlus("./windmill/windmill-roof.obj"); //Load roof model
		wallsModel = LoadModelPlus("./windmill/windmill-walls.obj"); //Load walls model
		LoadTGATextureSimple("./windmill/blade_texture.tga", &bladeTexture); //Load wing texture TGA image
		LoadTGATextureSimple("./windmill/roof_texture.tga", &roofTexture); //Load roof texture TGA image
		LoadTGATextureSimple("./windmill/wall_texture.tga", &wallTexture); //Load wall texture TGA image  
		LoadTGATextureSimple("./windmill/balcony_texture.tga", &balconyTexture); //Load balcony texture TGA image
		    
		// GL inits
    glClearColor(0.2,0.2,0.5,0);
    glEnable(GL_DEPTH_TEST);
    printError("GL inits");

    // Load and compile shader
    program = loadShaders("lab3-2.vert", "lab3-2.frag");
    printError("init shader");

    // Upload geometry to the GPU:

    // Allocate and activate Vertex Array Object
    glGenVertexArrays(1, &vertexArrayObjID);
		glBindVertexArray(vertexArrayObjID);

		//Active and bind diferent textures
			//Texture unit 0 = blade texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, bladeTexture); 
			glUniform1i(glGetUniformLocation(program, "texUnit"), 0); 

			//Texture unit 1 = roof texture
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, roofTexture); 
			glUniform1i(glGetUniformLocation(program, "texUnit"), 1);

			//Texture unit 2 = wall texture
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, wallTexture); 
			glUniform1i(glGetUniformLocation(program, "texUnit"), 2);

			//Texture unit 3 = balcony texture
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, balconyTexture); 
			glUniform1i(glGetUniformLocation(program, "texUnit"), 3);

    // End of upload of geometry

	printError("init arrays");
}


void display(void)
{
  //Add time-based rotation/translation
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	//Keyboar events
	GLfloat speedFrontBack = 0.5;
	GLfloat speedLeftRight = 0.05;
	GLfloat cameraPositionY = 3;
	GLfloat viewPositionY = 3;

	if(glutKeyIsDown('a')){
		cameraRotation += speedLeftRight; 		
	}
	if(glutKeyIsDown('w')){
		cameraPositionZ += speedFrontBack*cos(cameraRotation); 
		cameraPositionX += speedFrontBack*sin(cameraRotation);
	}
	if(glutKeyIsDown('s')){
		cameraPositionZ -= speedFrontBack*cos(cameraRotation); 
		cameraPositionX -= speedFrontBack*sin(cameraRotation);
	}
	if(glutKeyIsDown('d')){
		cameraRotation -= speedLeftRight; 
	}
	viewPositionZ = cameraPositionZ + cos(cameraRotation); 
	viewPositionX = cameraPositionX + sin(cameraRotation);

	GLfloat cameraVector[] = {cameraPositionX, cameraPositionY, cameraPositionZ};
	GLfloat viewVector[] = {viewPositionX, viewPositionY, viewPositionZ};
	GLfloat upVector[] = {0, 1, 0};

	//World to view matrix
	worldToViewMatrix = lookAt(cameraVector[0], cameraVector[1], cameraVector[2], viewVector[0], viewVector[1], viewVector[2], upVector[0], upVector[1], upVector[2]);

	//Proyection matrix
	mat4 proyectionMatrix = frustum(left, right, bottom, top, near, far);

	//World to projection matrix
	mat4 worldToProjectionMatrix = Mult(proyectionMatrix, worldToViewMatrix);

  //Send world to projection matrix to shader
  glUniformMatrix4fv(glGetUniformLocation(program, "worldToProjectionMatrix"), 1, GL_TRUE, worldToProjectionMatrix.m);

	//Erase Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //Select VAO
	glBindVertexArray(vertexArrayObjID);

	//Model to world matrix
	mat4 modelToWorldMatrix = T(0, 0, 0);

	//Drawing elements
		//Wall
		glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrix.m);
		glUniform1i(glGetUniformLocation(program, "texUnit"), 2);
		DrawModel(wallsModel, program, "inPosition", "inNormal", "inTexCoord");

		//Roof
		glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrix.m);
		glUniform1i(glGetUniformLocation(program, "texUnit"), 1);
		DrawModel(roofModel, program, "inPosition", "inNormal", "inTexCoord");

		//Balcony	
		glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrix.m);
		glUniform1i(glGetUniformLocation(program, "texUnit"), 3);
		DrawModel(balconyModel, program, "inPosition", "inNormal", "inTexCoord");

		//Blades
			//Blade rotation matrix
			mat4 bladesRotationMatrix = Rx(0.001*t);

			//Blade translation matrix
			mat4 bladesTranslationMatrix = T(5, 9, 0);
		
			//Bottom blade (looks -Y axes by default))
			mat4 modelToWorldMatrixBladeBottom = Mult(bladesTranslationMatrix, bladesRotationMatrix);  
			glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixBladeBottom.m);
			glUniform1i(glGetUniformLocation(program, "texUnit"), 0); 
			DrawModel(bladeModel, program, "inPosition", "inNormal", "inTexCoord");

			//Right blade
			mat4 modelToWorldMatrixBladeRight = Mult(Mult(bladesTranslationMatrix, bladesRotationMatrix) , Rx(PI/2));
			glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixBladeRight.m);
			DrawModel(bladeModel, program, "inPosition", "inNormal", "inTexCoord");
			
			//Up blade
			mat4 modelToWorldMatrixBladeUp = Mult(Mult(bladesTranslationMatrix, bladesRotationMatrix) , Rx(PI));
			glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixBladeUp.m);
			DrawModel(bladeModel, program, "inPosition", "inNormal", "inTexCoord");

			//Left blade
			mat4 modelToWorldMatrixBladeLeft = Mult(Mult(bladesTranslationMatrix, bladesRotationMatrix) , Rx(3*PI/2));
			glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixBladeLeft.m);
			DrawModel(bladeModel, program, "inPosition", "inNormal", "inTexCoord");	

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
