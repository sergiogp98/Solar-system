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

#define MIN_Y_HEIGHT = 10;

mat4 projectionMatrix;

//Vertex array
GLfloat *vertexArray;

//Keyboard events variables
GLfloat cameraPositionX = 0, 
		cameraPositionZ = 0,
        cameraPositionY = 0, 
		cameraRotation = 0, 
		viewPositionX = 0, 
        viewPositionY = 0,
		viewPositionZ = -3;

//Generate terrain function
Model* GenerateTerrain(TextureData *tex)
{
	int vertexCount = tex->width * tex->height;
	int triangleCount = (tex->width-1) * (tex->height-1) * 2;
	int x, z;
	
	vertexArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *normalArray = malloc(sizeof(GLfloat) * 3 * vertexCount);
	GLfloat *texCoordArray = malloc(sizeof(GLfloat) * 2 * vertexCount);
	GLuint *indexArray = malloc(sizeof(GLuint) * triangleCount*3);
	
	printf("bpp %d\n", tex->bpp);
	for (x = 0; x < tex->width; x++)
		for (z = 0; z < tex->height; z++)
		{
// Vertex array. You need to scale this properly
			vertexArray[(x + z * tex->width)*3 + 0] = x / 1.0;
			vertexArray[(x + z * tex->width)*3 + 1] = tex->imageData[(x + z * tex->width) * (tex->bpp/8)] / 100.0;
			vertexArray[(x + z * tex->width)*3 + 2] = z / 1.0;
// Normal vectors. You need to calculate these.
			vec3 leftVertex = {0, 1, 0};
			vec3 mainVertex = {vertexArray[(x + z * tex->width)*3 + 0],
					           vertexArray[(x + z * tex->width)*3 + 1],
							   vertexArray[(x + z * tex->width)*3 + 2]};
			vec3 upVertex = {0, 1, 0};

			if(x - 1 >= 0){
				leftVertex = (vec3){vertexArray[(x-1 + z * tex->width)*3 + 0],
                      		  		vertexArray[(x-1 + z * tex->width)*3 + 1],
                      		  		vertexArray[(x-1 + z * tex->width)*3 + 2]};
			}

			if (z - 1 >= 0){
				upVertex = (vec3){vertexArray[(x + (z-1) * tex->width)*3 + 0],
								  vertexArray[(x + (z-1) * tex->width)*3 + 1],
								  vertexArray[(x + (z-1) * tex->width)*3 + 2]};
			}

			vec3 normalVector = CalcNormalVector(mainVertex, leftVertex, upVertex);

			normalArray[(x + z * tex->width)*3 + 0] = normalVector.x;
			normalArray[(x + z * tex->width)*3 + 1] = normalVector.y;
			normalArray[(x + z * tex->width)*3 + 2] = normalVector.z;			
// Texture coordinates. You may want to scale them.
			texCoordArray[(x + z * tex->width)*2 + 0] = x; // (float)x / tex->width;
			texCoordArray[(x + z * tex->width)*2 + 1] = z; // (float)z / tex->height;
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

	//Paint blue pixel for the lake

	
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

//Height point calculation function
GLfloat MapHeight(TextureData *tex, GLfloat xPos, GLfloat zPos) {
	int xDown = floor(xPos);
	int zDown = floor(zPos);
	int xUp = xDown + 1;
	int zUp = zDown + 1;
	GLfloat lengthPosToDown = sqrt(pow(xPos - xDown,2)+pow(zPos - zDown,2));
	GLfloat lengthPosToUp = sqrt(pow(xPos - xUp,2)+pow(zPos - zUp,2));
	GLfloat vertex2 = vertexArray[(xUp + zDown * tex->width)*3 + 1];
	GLfloat vertex3 = vertexArray[(xDown + zUp * tex->width)*3 + 1];

	GLfloat vertex1, xHeight, zHeight, heighestPos;

	if(lengthPosToDown < lengthPosToUp)
	{
		vertex1 = vertexArray[(xDown + zDown * tex->width)*3 + 1];
		xHeight = (xPos - xDown)*(vertex2-vertex1);
		zHeight = (zPos - zDown)*(vertex3-vertex1);
	}
	else
	{
		vertex1 = vertexArray[(xUp + zUp * tex->width)*3 + 1];
		xHeight = (xUp - xPos)*(vertex3-vertex1);
		zHeight = (zUp - zPos)*(vertex2-vertex1);			
	}

	heighestPos = vertex1 + xHeight + zHeight;
	
	return heighestPos;
}

// vertex array object
Model *m, *m2, *tm;
// Reference to shader program
GLuint program;
GLuint tex1, tex2, tex3;
TextureData ttex; // terrain

void init(void)
{
	// GL inits
	glClearColor(0.2,0.2,0.5,0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	printError("GL inits");

	projectionMatrix = frustum(-0.1, 0.1, -0.1, 0.1, 0.2, 50.0);

	// Load and compile shader
	program = loadShaders("lab4-6.vert", "lab4-6.frag");
	glUseProgram(program);
	printError("init shader");
	
	glUniformMatrix4fv(glGetUniformLocation(program, "projMatrix"), 1, GL_TRUE, projectionMatrix.m);
	
    //Load textures
	LoadTGATextureSimple("grass.tga", &tex1);
    LoadTGATextureSimple("rock.tga", &tex2);
    LoadTGATextureSimple("water.tga", &tex3);

    //Load models
	m = LoadModelPlus("groundsphere.obj");
	m2 = LoadModelPlus("octagon.obj");
	
// Load terrain data

	LoadTGATextureData("fft-terrain.tga", &ttex);
	tm = GenerateTerrain(&ttex);
	printError("init terrain");
}

void display(void)
{
	// clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    //Add time-based rotation/translation
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

    //Keyboar events
	GLfloat speedFrontBack = 0.5;
	GLfloat speedLeftRight = 0.05;
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
    cameraPositionY = MapHeight(&ttex, cameraPositionX, cameraPositionZ) + 2;
    viewPositionY = cameraPositionY;
	viewPositionZ = cameraPositionZ + cos(cameraRotation); 
	viewPositionX = cameraPositionX + sin(cameraRotation);

	GLfloat cameraVector[] = {cameraPositionX, cameraPositionY, cameraPositionZ};
	GLfloat viewVector[] = {viewPositionX, viewPositionY, viewPositionZ};
	GLfloat upVector[] = {0, 1, 0};

	mat4 total, modelView, camMatrix, translationMatrix;
	
	printError("pre display");
	
	glUseProgram(program);

	// Build matrix
	
	camMatrix = lookAt(cameraVector[0], cameraVector[1], cameraVector[2],
				       viewVector[0], viewVector[1], viewVector[2],
				       upVector[0], upVector[1], upVector[2]);
	modelView = IdentityMatrix();
   	total = Mult(camMatrix, modelView);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);

	//Draw terrain
		//Texture 1
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex1);		
		glUniform1i(glGetUniformLocation(program, "grassTex"), 0); 

		//Texture 2
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, tex2);
		glUniform1i(glGetUniformLocation(program, "rockTex"), 1);

		DrawModel(tm, program, "inPosition", "inNormal", "inTexCoord");

    //Draw sphere model
	translationMatrix = T(40+10*cos(0.0003*t), MapHeight(&ttex, 40+10*cos(0.0003*t), 40+10*sin(0.0003*t)), 40+10*sin(0.0003*t));
    total = Mult(Mult(camMatrix, modelView), translationMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");

    translationMatrix = T(40+10*sin(0.0003*t), MapHeight(&ttex, 40+10*sin(0.0003*t), 40+10*cos(0.0003*t)), 40+10*cos(0.0003*t));
    total = Mult(Mult(camMatrix, modelView), translationMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(m2, program, "inPosition", "inNormal", "inTexCoord");
    
    //Draw cube model
	translationMatrix = T(20+10*cos(0.0003*t), MapHeight(&ttex, 20+10*cos(0.0003*t), 20+10*sin(0.0003*t)), 20+10*sin(0.0003*t));
    total = Mult(Mult(camMatrix, modelView), translationMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(m, program, "inPosition", "inNormal", "inTexCoord");
	
    translationMatrix = T(20+10*sin(0.0003*t), MapHeight(&ttex, 20+10*sin(0.0003*t), 20+10*cos(0.0003*t)), 20+10*cos(0.0003*t));
    total = Mult(Mult(camMatrix, modelView), translationMatrix);
    glUniformMatrix4fv(glGetUniformLocation(program, "mdlMatrix"), 1, GL_TRUE, total.m);
    DrawModel(m2, program, "inPosition", "inNormal", "inTexCoord");

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
