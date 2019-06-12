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

// Globals
// Data would normally be read from files
GLfloat vertices[] =
{
	-0.5f,-0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	
	0.5f,0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	
	-0.5f,-0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	0.0f,0.0f,0.5f,
	
	-0.5f,-0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.0f,0.0f,0.5f,

	0.5f,0.5f,0.0f,
	0.5f,-0.5f,0.0f,
	0.0f,0.0f,0.5f,

	0.5f,0.5f,0.0f,
	-0.5f,0.5f,0.0f,
	0.0f,0.0f,0.5f,
};

GLfloat colors[] =
{
	0.8f, 0.3f, 0.5f, 
	0.2f, 0.1f, 0.6f,
	0.9f, 0.7f, 0.0f,

	0.8f, 0.3f, 0.5f, 
	0.2f, 0.1f, 0.6f,
	0.9f, 0.7f, 0.0f,

	0.8f, 0.3f, 0.5f, 
	0.2f, 0.1f, 0.6f,
	0.9f, 0.7f, 0.0f,

	0.8f, 0.3f, 0.5f, 
	0.2f, 0.1f, 0.6f,
	0.9f, 0.7f, 0.0f,

	0.8f, 0.3f, 0.5f, 
	0.2f, 0.1f, 0.6f,
	0.9f, 0.7f, 0.0f,

	0.8f, 0.3f, 0.5f, 
	0.2f, 0.1f, 0.6f,
	0.9f, 0.7f, 0.0f,
};

GLuint program;


void OnTimer(int value){
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}

// vertex array object
unsigned int vertexArrayObjID;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int vertexBufferObjID;
    unsigned int vertexBufferObjColor;
	// Reference to shader program

	dumpInfo();

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab1-5.vert", "lab1-5.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
	

	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &vertexArrayObjID);
	glBindVertexArray(vertexArrayObjID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &vertexBufferObjID);
	glGenBuffers(1, &vertexBufferObjColor);

	// VBO for vertex data
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjID);
	glBufferData(GL_ARRAY_BUFFER, 9*6*sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
	glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjColor);
    glBufferData(GL_ARRAY_BUFFER, 9*6*sizeof(GLfloat), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Color"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Color"));

	// End of upload of geometry
	
	printError("init arrays");
}


void display(void)
{
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME); 	

	int speed = 500;

	GLfloat translationMatrix[] = { 
    	1.0f, 0.0f, 0.0f, 0.5f,
    	0.0f, 1.0f, 0.0f, 0.0f,
    	0.0f, 0.0f, 1.0f, 0.0f,
    	0.0f, 0.0f, 0.0f, 1.0f
	};

	GLfloat rotationMatrixZ[] = {
		cos(t/speed), -sin(t/speed), 0.0f, 0.0f,
		sin(t/speed), cos(t/speed), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	GLfloat rotationMatrixX[] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, cos(t/speed), -sin(t/speed), 0.0f,
		0.0f, sin(t/speed), cos(t/speed), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	GLfloat rotationMatrixY[] = {
		cos(t/speed), 0.0f, sin(t/speed), 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		-sin(t/speed), 0.0f, cos(t/speed), 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	GLfloat scaleMatrix[] = {
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 2.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 2.0f
	};

    glUniformMatrix4fv(glGetUniformLocation(program, "translationMatrix"), 1, GL_TRUE, translationMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "scaleMatrix"), 1, GL_TRUE, scaleMatrix);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrixX"), 1, GL_TRUE, rotationMatrixX);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrixY"), 1, GL_TRUE, rotationMatrixY);
	glUniformMatrix4fv(glGetUniformLocation(program, "rotationMatrixZ"), 1, GL_TRUE, rotationMatrixZ);

	//Z buffer
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

	//Active Z buffer and back-face culling
	glEnable(GL_DEPTH_TEST);

	//Erase Z buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vertexArrayObjID);	// Select VAO
	glDrawArrays(GL_TRIANGLES, 0, 18);	// draw object
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





