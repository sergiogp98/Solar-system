// Lab 4, terrain generation

#ifdef __APPLE__
	#include <OpenGL/gl3.h>
	// Linking hint for Lightweight IDE
	// uses framework Cocoa
#endif
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

mat4 projectionMatrix;

//Keyboard events variables
GLfloat cameraPositionX = 0, 
		cameraPositionZ = 0, 
		cameraRotation = 0, 
		viewPositionX = 0, 
		viewPositionZ = -3;

//Generate terrain function
Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;
	
	GLfloat *vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 30.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
		}
	
	for(x = 0; x < tex->width; x++)
		for(z = 0; z < tex->height; z++)
		{
// Normal vectors. You need to calculate these.
			vec3 leftVertex = {0, 1, 0};
			vec3 upVertex = {0, 1, 0};
			vec3 rightVertex = {0, 1, 0};
			vec3 downVertex = {0, 1, 0};

			if(x - 1 >= 0){
				leftVertex = (vec3){vertexArray[(x-1 + z * tex->width)*3 + 0],
                      		  		vertexArray[(x-1 + z * tex->width)*3 + 1],
                      		  		vertexArray[(x-1 + z * tex->width)*3 + 2]};
			}

			if(x + 1 <= tex->width){
				rightVertex = (vec3){vertexArray[(x+1 + z * tex->width)*3 + 0],
									 vertexArray[(x+1 + z * tex->width)*3 + 1],
									 vertexArray[(x+1 + z * tex->width)*3 + 2]};
			}

			if (z - 1 >= 0){
				upVertex = (vec3){vertexArray[(x + (z-1) * tex->width)*3 + 0],
								  vertexArray[(x + (z-1) * tex->width)*3 + 1],
								  vertexArray[(x + (z-1) * tex->width)*3 + 2]};
			}

			if(z + 1 <= tex->height){
				downVertex = (vec3){vertexArray[(x + (z+1) * tex->width)*3 + 0],
									vertexArray[(x + (z+1) * tex->width)*3 + 1],
									vertexArray[(x + (z+1) * tex->width)*3 + 2]};
			}

			vec3 leftRight = VectorSub(leftVertex, rightVertex);
			vec3 upDown = VectorSub(upVertex, downVertex);
			vec3 normalVector = Normalize(CrossProduct(leftRight, upDown));

			normalArray[(x + z * tex->width)*3 + 0] = normalVector.x;
			normalArray[(x + z * tex->width)*3 + 1] = normalVector.y;
			normalArray[(x + z * tex->width)*3 + 2] = normalVector.z;
		}

	for (x = 0; x < tex->width-1; x++)
		for (z = 0; z < tex->height-1; z++)
		{
		// Triangle 1
			indexArray[(x + z * (tex->width-1))*6 + 0] = x + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 1] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 2] = x+1 + z * tex->width;
		// Triangle 2
			indexArray[(x + z * (tex->width-1))*6 + 3] = x+1 + z * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 4] = x + (z+1) * tex->width;
			indexArray[(x + z * (tex->width-1))*6 + 5] = x+1 + (z+1) * tex->width;
		}
	
	// End of terrain generation
	
	// Create Model and upload to GPU:

	Model* model = LoadDataToModel(
			vertexArray,
			normalArray,
			texCoordArray,
			NULL,
			indexArray,
			vertexCount,
			triangleCount*3);

	return model;
}

// vertex array object
Model *m, *m2, *tm;

// Reference to shader program
GLuint program;
GLuint tex1, tex2;
TextureData ttex; // terrain

vec3 lightSourcesColorsArr[] = {{1.0f, 0.0f, 0.0f}, // Red light
                                {0.0f, 1.0f, 0.0f}, // Green light
                                {0.0f, 0.0f, 1.0f}, // Blue light
                                {1.0f, 1.0f, 1.0f}};  // White light
                               

GLint isDirectional[] = {0,0,1,1};


vec3 lightSourcesDirectionsPositions[] = {{10.0f, 5.0f, 0.0f}, // Red light, positional
                                          {0.0f, 5.0f, 10.0f}, // Green light, positional
                                          {-1.0f, 0.0f, 0.0f}, // Blue light along X
                                          {0.0f, 0.0f, -1.0f}};  // White light along Z
                                         
GLfloat specularExponent[] = {100.0, 200.0, 60.0, 50.0, 300.0, 150.0};

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

	// Load and compile shader
	program = loadShaders("lab4-3.vert", "lab4-3.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 0); // Texture unit 0
	LoadTGATextureSimple("maskros512.tga", &tex1);
	
	//Upload light sources to the shaders
	glUniform3fv(glGetUniformLocation(program, "lightSourcesDirPosArr"), 4, &lightSourcesDirectionsPositions[0].x);
	glUniform3fv(glGetUniformLocation(program, "lightSourcesColorArr"), 4, &lightSourcesColorsArr[0].x);
	glUniform1fv(glGetUniformLocation(program, "specularExponent"), 4, specularExponent);
	glUniform1iv(glGetUniformLocation(program, "isDirectional"), 4, isDirectional);

// Load terrain data

	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    //Keyboar events
	GLfloat speedFrontBack = 0.5;
	GLfloat speedLeftRight = 0.05;
	GLfloat cameraPositionY = 10;
	GLfloat viewPositionY = 10;
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

	//Upload camera vector to shader
    glUniform3fv(glGetUniformLocation(program, "cameraVector"), 1, cameraVector);

	mat4 total, modelView, camMatrix;
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	
	camMatrix = lookAt(cameraVector[0], cameraVector[1], cameraVector[2],
				       viewVector[0], viewVector[1], viewVector[2],
				       upVector[0], upVector[1], upVector[2]);
	modelView = IdentityMatrix();
	total = Mult(camMatrix, modelView);
	glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
	
	glBindTexture(GL_TEXTURE_2D, tex1);		// Bind Our Texture tex1
	DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");
	printError("display 2");
	
	glutSwapBuffers();
}

void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

void mouse(int x, int y)
{
	printf("%d %d\n", x, y);
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitWindowSize (600, 600);
	glutCreateWindow ("TSBK07 Lab 4");
	glutDisplayFunc(display);
	init ();
	glutTimerFunc(20, &timer, 0);

	glutPassiveMotionFunc(mouse);

	glutMainLoop();
	exit(0);
}
