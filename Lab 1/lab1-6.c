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

// Globals
// Data would normally be read from files
Model *m;

GLuint program;

void OnTimer(int value){
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}

// vertex array object
unsigned int bunnyVertexArrayObjID;

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int bunnyVertexBufferObjID;
    unsigned int bunnyIndexBufferObjID;
    unsigned int bunnyNormalBufferObjID;

	// Reference to shader program

	dumpInfo();

    m = LoadModel("bunny.obj");

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab1-6.vert", "lab1-6.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
    
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &bunnyVertexArrayObjID);
    glBindVertexArray(bunnyVertexArrayObjID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &bunnyIndexBufferObjID);
    glGenBuffers(1, &bunnyNormalBufferObjID);
    glGenBuffers(1, &bunnyVertexBufferObjID);

	// VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bunnyVertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Position"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Position"));

    // VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "in_Normal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "in_Normal"));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);

	// End of upload of geometry
	
	printError("init arrays");
}


void display(void)
{
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME); 
    int speed = 5000;

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

	glBindVertexArray(bunnyVertexArrayObjID);	// Select VAO
	glDrawElements(GL_TRIANGLES, m->numIndices, GL_UNSIGNED_INT, 0L);	// draw object
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





