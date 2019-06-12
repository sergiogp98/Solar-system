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
GLfloat translationMatrix[] = { //translation matrix (moves 0.5 in the X axis)
    1.0f, 0.0f, 0.0f, 0.5f,
    0.0f, 1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

GLfloat scaleMatrix[] = { //scale to the half
    0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.5f, 0.0f,
    0.0f, 0.0f, 0.0f, 1.0f
};

Model *m;

GLuint program;
GLuint myTex; //Texture reference

//Render new images repeatedly
void OnTimer(int value){
    glutPostRedisplay();
    glutTimerFunc(20, &OnTimer, value);
}

// vertex array object
unsigned int bunnyVertexArrayObjID;

//Matrix for realistic projection
GLfloat projectionMatrix[] = {    
    2.0f*near/(right-left), 0.0f, (right+left)/(right-left), 0.0f,
    0.0f, 2.0f*near/(top-bottom), (top+bottom)/(top-bottom), 0.0f,
    0.0f, 0.0f, -(far + near)/(far - near), -2*far*near/(far - near),
    0.0f, 0.0f, -1.0f, 0.0f 
};

void init(void)
{
	// vertex buffer object, used for uploading the geometry
	unsigned int bunnyVertexBufferObjID;
    unsigned int bunnyIndexBufferObjID;
	unsigned int bunnyTexCoordBufferObjID;
    unsigned int bunnyNormalBufferObjID;

	// Reference to shader program

	dumpInfo();

    m = LoadModel("bunnyplus.obj"); //Load new bunny model
	LoadTGATextureSimple("maskros512.tga", &myTex); //Load TGA iamge to texture	

	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glDisable(GL_DEPTH_TEST);
	printError("GL inits");

	// Load and compile shader
	program = loadShaders("lab2-6.vert", "lab2-6.frag");
	printError("init shader");
	
	// Upload geometry to the GPU:
    
	// Allocate and activate Vertex Array Object
	glGenVertexArrays(1, &bunnyVertexArrayObjID);
    glBindVertexArray(bunnyVertexArrayObjID);

	// Allocate Vertex Buffer Objects
	glGenBuffers(1, &bunnyIndexBufferObjID);
    glGenBuffers(1, &bunnyVertexBufferObjID);
	glGenBuffers(1, &bunnyTexCoordBufferObjID);
    glGenBuffers(1, &bunnyNormalBufferObjID);

	// VBO for vertex data
    glBindBuffer(GL_ARRAY_BUFFER, bunnyVertexBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->vertexArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "inPosition"), 3, GL_FLOAT, GL_FALSE, 0, 0); 
    glEnableVertexAttribArray(glGetAttribLocation(program, "inPosition"));

    // VBO for normal data
    glBindBuffer(GL_ARRAY_BUFFER, bunnyNormalBufferObjID);
    glBufferData(GL_ARRAY_BUFFER, m->numVertices*3*sizeof(GLfloat), m->normalArray, GL_STATIC_DRAW);
    glVertexAttribPointer(glGetAttribLocation(program, "inNormal"), 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(glGetAttribLocation(program, "inNormal"));
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bunnyIndexBufferObjID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m->numIndices*sizeof(GLuint), m->indexArray, GL_STATIC_DRAW);

	//VBO for texture coordinates
	if (m->texCoordArray != NULL){
		glBindBuffer(GL_ARRAY_BUFFER, bunnyTexCoordBufferObjID);
		glBufferData(GL_ARRAY_BUFFER, m->numVertices*2*sizeof(GLfloat), m->texCoordArray, GL_STATIC_DRAW);
		glVertexAttribPointer(glGetAttribLocation(program, "inTexCoord"), 2, GL_FLOAT, GL_FALSE, 0, 0);
		glEnableVertexAttribArray(glGetAttribLocation(program, "inTexCoord"));
		glActiveTexture(GL_TEXTURE0); //Active texture object
		glBindTexture(GL_TEXTURE_2D, myTex); //Bind texture object to a tecture unit
		glUniform1i(glGetUniformLocation(program, "texUnit"), 0); //Set texture unit to 0 and send it to the sampler in the vertex shader
    }
    
	//Activate texture object
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, myTex);

	// End of upload of geometry
	
	printError("init arrays");
}


void display(void)
{
	//Add time-based rotation/translation
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME); 
    int speed = 800;

	//Transformation and proyection matrices
    mat4 rot, trans, proyect, total;
    trans = T(0, 0, -5);
    rot = Mult(Rx(cos(t/speed)*0.3f), Mult(Ry(sin(t/speed)*0.3f), Rz(cos(t/speed)*0.3f)));
    proyect = frustum(left, right, bottom, top, near, far);
	total = Mult(proyect, Mult(rot, trans));

	//World-to-view matrix
	mat4 worldToViewMatrix = lookAt(sin(t/speed)*0.3f, 0, sin(t/speed)*0.3f, 0, 0, -3, 0, 1, 0);
	
	//Rotation matrices through X, Y and Z axis
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

	GLfloat rotationMatrixZ[] = {
		cos(t/speed), -sin(t/speed), 0.0f, 0.0f,
		sin(t/speed), cos(t/speed), 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};


    //Send matrices to vertex shader
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	glUniformMatrix4fv(glGetUniformLocation(program, "worldToViewMatrix"), 1, GL_TRUE, worldToViewMatrix.m);

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





