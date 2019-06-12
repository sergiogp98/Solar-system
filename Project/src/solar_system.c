/**
 * @brief Interactive solar system\n
 * Uses the following reusable code:
 * 	- GL_utilities (shader loader)
 * 	- MicroGLUT (GLUT replacement)\n
 * 	- VectorUtils3 (vector/matrix code)
 * 	- loadobj (limited OBJ model loader)
 * 	- LoadTGA (loader for textures saved as TGA files)
 * @file solar_system.c
 * @author Roberto Corrochano Piqueras (robco726)
 * @author Sergio García Prados (serga155)
 * @date 13/05/2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "MicroGlut.h"
#include "GL_utilities.h"
#include "VectorUtils3.h"
#include "loadobj.h"
#include "LoadTGA.h"

#define ROTATION_DEGREE M_PI/100 ///<Rotation degree for rocket movement

GLfloat vertices[6][6*3] = ///<Vertices positons of the skybox
{
	{ // +x
		0.5,-0.5,-0.5,		
		0.5,0.5,-0.5,		
		0.5,0.5,0.5,			
		0.5,-0.5,0.5,		
	},
	{ // -x
		-0.5,-0.5,-0.5,		
		-0.5,-0.5,0.5,		
		-0.5,0.5,0.5,		
		-0.5,0.5,-0.5,		
	},
	{ // +y
		0.5,0.5,-0.5,		
		-0.5,0.5,-0.5,		
		-0.5,0.5,0.5,		
		0.5,0.5,0.5,			
	},
	{ // -y
		-0.5,-0.5,-0.5,		
		0.5,-0.5,-0.5,		
		0.5,-0.5,0.5,		
		-0.5,-0.5,0.5		
	},
	{ // +z
		-0.5,-0.5,0.5,		
		0.5,-0.5,0.5,		
		0.5,0.5,0.5,			
		-0.5,0.5,0.5,		
	},
	{ // -z
		-0.5,-0.5,-0.5,	
		-0.5,0.5,-0.5,		
		0.5,0.5,-0.5,		
		0.5,-0.5,-0.5,		
	}
};

GLfloat texcoord[6][6*2] = ///<Texture coordinate of the skybox
{
	{	//Left
		1.0, 1.0,
		1.0, 0.0, 
		0.0, 0.0,
		0.0, 1.0,
	},
	{	//Right
		0.0, 1.0, 
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
	},
	{	//Top
		1.0, 0.0, 
		0.0, 0.0,
		0.0, 1.0,
		1.0, 1.0,
	},
	{	//Bottom
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0, 
		0.0, 0.0,
	},
	{	//Back
		0.0, 1.0,
		1.0, 1.0,
		1.0, 0.0, 
		0.0, 0.0,
	},
	{	//Front
		1.0, 1.0,
		1.0, 0.0,
		0.0, 0.0,
		0.0, 1.0,
	}
};

GLuint indices[6][6] = ///<Indices of the skybox
{
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2},
	{0, 2, 1, 0, 3, 2}
};

char *cubemapTextureFileName[6] = ///<File names of the 6 cubemap textures
{
	"textures/purplenebula_lf.tga",
	"textures/purplenebula_rt.tga",
	"textures/purplenebula_up.tga",
	"textures/purplenebula_dn.tga",
	"textures/purplenebula_bk.tga",
	"textures/purplenebula_ft.tga"
};

char *modelsTextureFileName[11] = ///<File names of the planets, satellite and rocket textures
{
	"textures/sun_texture.tga",
	"textures/mercury_texture.tga",
	"textures/venus_texture.tga",
	"textures/earth_texture.tga",
	"textures/mars_texture.tga",
	"textures/jupiter_texture.tga",
	"textures/saturn_texture.tga",
	"textures/uranus_texture.tga",
	"textures/neptune_texture.tga",
	"textures/moon_texture.tga",
	"textures/rocket_texture.tga"
};

char *modelsFileName[2] = ///<File names of the 2 models (sphere and rocket)
{
	"obj/Sphere.obj",
	"obj/Rocket.obj"
};

TextureData cubemap_texture[6]; ///<Texture data for the cubemap
TextureData satellites_texture[10]; ///<Planets and satellites texture data for the planets and satellite
TextureData rocket_texture; ///<Rocket texture data
mat4 worldToViewMatrix; ///<World-to-view matrix
mat4 projectionMatrix; ///<Projection matrix 
mat4 modelToWorldMatrixSun; ///<Sun model-to-world matrix
mat4 modelToWorldMatrixPlanet; ///<Planets model-to-world matrix
mat4 modelToWorldMatrixSatellite; ///<Satellites model-to-world matrix
mat4 modelToWorldMatrixRocket; ///<Rocket model-to-world matrix 
GLuint program; ///<cm shaders program
GLuint texprogram; ///<tex shaders program 
Model *model[2]; ///<obj models (sphere and rocket)
Model *box[6]; ///<skybox models (left, right, up, down, back and front)
GLfloat rotation_planet; ///<Rotation speed of the planets
GLfloat translation_planet; ///<Translation speed of the planets
GLfloat translation_satellite; ///<Translation speed of the satellites
vec3 sun_position;  ///<Sun position
/**
 * @brief Relation between index and planets' name\
 * 	- 
 */
GLfloat distance_to_sun[8] = {10, 30, 50, 70, 90, 110, 120, 150}; ///<Planet's distance to sun (0 = Mercury, 1 = Venus, 2 = Earth, 3 = Mars, 4 = Jupiter, 5 = Saturn, 6 = Uranus, 7 = Neptune)
GLfloat planet_size[8] = {0.2, 0.5, 1, 1, 3, 3, 2, 2}; ///<Planet's size
GLfloat z_deviaton[8] = {-1, 1, -1, 1, -1, 1, -1, 1}; ///<Planet's Z deviation from the sun 
GLfloat x_deviation[8] = {1, -1, 1, -1, 1, -1, 1, -1}; ///<Planet's X deviation from the sun
vec3 position_rocket; ///<Rocket position
vec3 position_camera; ///<Camera position
vec3 cameraVectorGeneral; ///<lookAt camera vector parameter for general view
vec3 viewVectorGeneral; ///<lookAt view vector parameter for general view
vec3 upVectorGeneral; ///<lookAt up vector parameter for general view
GLfloat angle = 0.0; ///<Rotation angle over XZ plane in general view
GLint cam = 0; ///<0 = general view, 1 = rocket view
vec3 forwardVector; ///<Rocket forward vector
vec3 rightVector; ///<Rocket right vector
vec3 upVector; ///<Rocket up vector
mat4 rotation_matrix; ///<Rocket rotation matrix
vec3 cameraDirectionVector; ///<Vector from the rocket to the camera
vec3 sphere_position; ///<Position if the sphere
vec3 sphereForwardVector; ///<New forward vector of the rocket after a collision
GLfloat radius_satellite; ///<Satellite's radius
GLfloat radius_rocket; ///<Rocket's radius
GLfloat radius_sun; ///<Sun's radius
GLfloat radius_planets[8]; ///<Plantes' radius
vec3 normal_vector; ///<Normal vector for sphereForwardVector calculation
vec3 tangent_vector; ///<Tangent vector for sphereForwardVector calculation

/**
 * @brief Return a 4 by 4 matrix taking the arguments as matrix numbers
 * @param p0 0x0 element
 * @param p1 0x1 element
 * @param p2 0x2 element
 * @param p3 0x3 element
 * @param p4 1x0 element
 * @param p5 1x1 element
 * @param p6 1x2 element
 * @param p7 1x3 element
 * @param p8 2x0 element
 * @param p9 2x1 element
 * @param p10 2x2 element
 * @param p11 2x3 element
 * @param p12 3x0 element
 * @param p13 3x1 element
 * @param p14 3x2 element
 * @param p15 3x3 element
 * @return A 4x4 matrix
 */
mat4 SetMatrix4(GLfloat p0, GLfloat p1, GLfloat p2, GLfloat p3,
				GLfloat p4, GLfloat p5, GLfloat p6, GLfloat p7,
				GLfloat p8, GLfloat p9, GLfloat p10, GLfloat p11, 
				GLfloat p12, GLfloat p13, GLfloat p14, GLfloat p15)
{
	mat4 m;
	m.m[0] = p0;
	m.m[1] = p1;
	m.m[2] = p2;
	m.m[3] = p3;
	m.m[4] = p4;
	m.m[5] = p5;
	m.m[6] = p6;
	m.m[7] = p7;
	m.m[8] = p8;
	m.m[9] = p9;
	m.m[10] = p10;
	m.m[11] = p11;
	m.m[12] = p12;
	m.m[13] = p13;
	m.m[14] = p14;
	m.m[15] = p15;
	return m;
}

/**
 * @brief Load and active texture for the cubemap, planets, satellite and rocket 
 */
void loadTextures()
{
	int i;
	
	//CUBE MAP
		//Active texture 0 		
		glActiveTexture(GL_TEXTURE0); 
		
		//Load texture data and create ordinary texture objects (for skybox)
		for (i = 0; i < 6; i++){
			LoadTGATexture(cubemapTextureFileName[i], &cubemap_texture[i]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, cubemap_texture[i].w, cubemap_texture[i].h, 0, GL_RGBA, GL_UNSIGNED_BYTE, cubemap_texture[i].imageData);
		}
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glEnable(GL_TEXTURE_CUBE_MAP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//PLANETS AND SATELLITE
		//Active texture 1
		glActiveTexture(GL_TEXTURE1);

		//Load texture data
		for(i = 0; i < 10; i++){
			LoadTGATexture(modelsTextureFileName[i], &satellites_texture[i]);
		}

	//ROCKET
		//Active texture 1
		glActiveTexture(GL_TEXTURE1);

		//Load texture data 
		LoadTGATexture(modelsTextureFileName[10], &rocket_texture);
}

/**
 * @brief Calculate the radius of a scaled model
 * @param m Model to calculate its radius
 * @param scalar Scalar number for the model
 * @pre m musy exist and be loaded
 * @pre scalar must be greater than 0
 * @return Radius of the scaled model
 */
float RadiusObj(Model *m, float scalar){
	vec3 vertex;
	float radius;
	vertex = (*(vec3 *)(&m->vertexArray[m->indexArray[0]*3]));
	radius = Norm(ScalarMult(vertex, scalar));
	return radius;
}

/**
 * @brief Detect if there is a collision between the rocket and a sphere. In that case, the rocket move arouond the sphere as it was landing on it
 * @param radius_sphere Radius of the sphere which the rocket could collide with
 * @pre radius_sphere must be greater than 0
 */
void CollisionDetection(float radius_sphere){
	vec3 diff;

	diff = VectorSub(position_rocket, sphere_position);

	if(Norm(diff) < radius_rocket + radius_sphere){
		SplitVector(forwardVector, upVectorGeneral, &normal_vector, &tangent_vector);
		sphereForwardVector = Normalize(VectorSub(normal_vector, tangent_vector));
		position_rocket = VectorAdd(position_rocket, sphereForwardVector);
		position_camera = VectorAdd(position_rocket, ScalarMult(cameraDirectionVector, 3));
	}
}

/**
 * @brief Control the camera and rocket movement with the keyboard
 * @param c Key pressed
 * @param x Mouse X location in window relative coordinates when the key was pressed
 * @param y Mouse Y location in window relative coordinates when the key was pressed
 */
void keyboard(unsigned char c, int x, int y)
{
	switch (c)
	{
	case 'w': //GO STRAIGHT
		if(cam == 0){
			cameraVectorGeneral.x += sin(angle);
			cameraVectorGeneral.z += cos(angle);
		}else{
			position_rocket = VectorAdd(position_rocket, forwardVector);
		}	
		break;
	case 's': //GO BACKWARDS
		if(cam == 0){
			cameraVectorGeneral.x -= sin(angle);
			cameraVectorGeneral.z -= cos(angle);
		}else{
			position_rocket = VectorSub(position_rocket, forwardVector);
		}
		break;
	case 'a': //TURN LEFT
		if(cam == 0){
			angle = fmod(angle + 0.03 + 2*M_PI, 2*M_PI);
		}else{
			forwardVector = Normalize(VectorAdd(forwardVector, ScalarMult(rightVector, -ROTATION_DEGREE)));
			rightVector = Normalize(CrossProduct(forwardVector, upVector));
		}
		break;
	case 'd': //TURN RIGHT
		if(cam == 0){
			angle = fmod(angle - 0.03 + 2*M_PI, 2*M_PI);
		}else{
			forwardVector = Normalize(VectorAdd(forwardVector, ScalarMult(rightVector, ROTATION_DEGREE)));
			rightVector = Normalize(CrossProduct(forwardVector, upVector));
		}
		break;
	case '+': //ROTATE UP
		if(cam == 1){
			forwardVector = Normalize(VectorAdd(forwardVector, ScalarMult(upVector, ROTATION_DEGREE)));
			upVector = Normalize(CrossProduct(rightVector, forwardVector));
		}
		break;			
	case '-': //ROTATE DOWN
		if(cam == 1){
			forwardVector = Normalize(VectorAdd(forwardVector, ScalarMult(upVector, -ROTATION_DEGREE)));
			upVector = Normalize(CrossProduct(rightVector, forwardVector));
		}	
		break;
	case 'q': //CHANGE CAMERA VIEW
		cam = fmod(cam + 1, 2);
		break;
	}

	if(cam == 0){
		viewVectorGeneral.x = cameraVectorGeneral.x + sin(angle);
		viewVectorGeneral.z = cameraVectorGeneral.z + cos(angle);
	}else{
		cameraDirectionVector = Normalize(VectorAdd(ScalarMult(forwardVector, -1), ScalarMult(upVector, 0.5)));
		position_camera = VectorAdd(position_rocket, ScalarMult(cameraDirectionVector, 3.0));
		rotation_matrix = Transpose(SetMatrix4(rightVector.x, rightVector.y, rightVector.z, 0,
									  		   upVector.x, upVector.y, upVector.z, 0,
									           forwardVector.x, forwardVector.y, forwardVector.z, 0,
									           0, 0, 0, 1));
	}
	glutPostRedisplay();	
}

/**
 * @brief Orders a new call to the display function plus starts the next round with the timer
 * @param i Value parameter to glutTimerFunc
 */
void timer(int i)
{
	glutTimerFunc(20, &timer, i);
	glutPostRedisplay();
}

/**
 * @brief Initialize some global variables and send projection matrix to the vertex shaders
 */
void init()
{
	int i;

	//Load cubemap shader
	program = loadShaders("shaders/cm.vert", "shaders/cm.frag");
	glUseProgram(program);

	//Load models
	for(i = 0; i < 2; i++){
		model[i] = LoadModelPlus(modelsFileName[i]);
	}
	
	//Load global data 
	for (i = 0; i < 6; i++){
		box[i] = LoadDataToModel(vertices[i], NULL, texcoord[i], NULL, indices[i], 4, 6);
	}

	//Initialize and send to shader projection matrix	
	projectionMatrix = perspective(
		90.0, //field of view in degree
		1.0,  //aspect ratio  
		0.2,  //Z near
		10000.0 //Z far
	);

	//Send projection matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(program, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(program, "tex"), 1); // Texture unit 1

	//Load skybox shader 
	texprogram = loadShaders("shaders/tex.vert", "shaders/tex.frag");

	//Send projection matrix to shader
	glUniformMatrix4fv(glGetUniformLocation(texprogram, "projectionMatrix"), 1, GL_TRUE, projectionMatrix.m);
	glUniform1i(glGetUniformLocation(texprogram, "tex"), 0); // Texture unit 0

	//Initialize roket position
	position_rocket.x = 50;
	position_rocket.y = 0;
	position_rocket.z = 0;
	
	//Initialize forward, right and up vector
	forwardVector = SetVector(1.0, 0.0, 0.0);
	rightVector = SetVector(0.0, 0.0, 1.0);
	upVector = SetVector(0.0, 1.0, 0.0);
	
	//Initialize camera direction vector
	cameraDirectionVector = Normalize(VectorAdd(ScalarMult(forwardVector, -1), upVector));  

	//Intialize camera postion
	position_camera = VectorAdd(position_rocket, ScalarMult(cameraDirectionVector, 3.0));
	
	//Initialize camera, view and up general vector
	cameraVectorGeneral.x = 0;
	cameraVectorGeneral.y = 0;
	cameraVectorGeneral.z = 0;
	viewVectorGeneral.x = 0;
	viewVectorGeneral.y = 0;
	viewVectorGeneral.z = -3;
	upVectorGeneral.x = 0;
	upVectorGeneral.y = 1;
	upVectorGeneral.z = 0;

	//Initialize rotation matrix
	rotation_matrix = SetMatrix4(rightVector.x, rightVector.y, rightVector.z, 0,
								 upVector.x, upVector.y, upVector.z, 0,
								 forwardVector.x, forwardVector.y, forwardVector.z, 0,
								 0, 0, 0, 1);
	//Initialize sun position
	sun_position = SetVector(0, 0, 0);

	//Initialize rocket, planet, sun and satellites radius
	radius_rocket = RadiusObj(model[1], 0.005);
	radius_satellite = RadiusObj(model[0], 0.2);
	radius_sun = RadiusObj(model[0], 5.0);
	for(i = 0; i < 8; i++){
		radius_planets[i] = RadiusObj(model[0], planet_size[i]);
	}

	glEnable(GL_DEPTH_TEST);
	
}

/**
 * @brief Redisplay the current window
 */
void display(void)
{
	int i;

	//Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//Add time-based rotation/translation
	GLfloat t = (GLfloat)glutGet(GLUT_ELAPSED_TIME);

	//Draw box
		glDisable(GL_DEPTH_TEST);
		glUseProgram(texprogram);

		if(cam == 0){ //General view
			worldToViewMatrix = lookAt(cameraVectorGeneral.x, cameraVectorGeneral.y, cameraVectorGeneral.z, 
									   viewVectorGeneral.x, viewVectorGeneral.y, viewVectorGeneral.z, 
									   upVectorGeneral.x, upVectorGeneral.y, upVectorGeneral.z);
		}else{ //Rocket view
			worldToViewMatrix = lookAt(position_camera.x, position_camera.y, position_camera.z,
									   position_rocket.x, position_rocket.y, position_rocket.z,
									   upVector.x, upVector.y, upVector.z);
		}
		worldToViewMatrix.m[3] = 0;
		worldToViewMatrix.m[7] = 0;
		worldToViewMatrix.m[11] = 0;
		glUniformMatrix4fv(glGetUniformLocation(texprogram, "worldToViewMatrix"), 1, GL_TRUE, worldToViewMatrix.m);

		for (i = 0; i < 6; i++){
			glBindTexture(GL_TEXTURE_2D, cubemap_texture[i].texID);
			DrawModel(box[i], texprogram, "inPosition", NULL, "inTexCoord");
		}
		glEnable(GL_DEPTH_TEST);

	translation_planet = 0.00001*t;
	rotation_planet = 0.0005*t;
	translation_satellite = 0.00075*t;

	//Draw planets, satellites and rocket		
		glActiveTexture(GL_TEXTURE1);
		glUseProgram(program);

		//Worlds-to-view matrix
		if(cam == 0){ //General view
			worldToViewMatrix = lookAt(cameraVectorGeneral.x, cameraVectorGeneral.y, cameraVectorGeneral.z, 
									   viewVectorGeneral.x, viewVectorGeneral.y, viewVectorGeneral.z, 
									   upVectorGeneral.x, upVectorGeneral.y, upVectorGeneral.z);
		}else{ //Rocket view
			worldToViewMatrix = lookAt(position_camera.x, position_camera.y, position_camera.z,
									   position_rocket.x, position_rocket.y, position_rocket.z,
									   upVector.x, upVector.y, upVector.z);
		}
		glUniformMatrix4fv(glGetUniformLocation(program, "worldToViewMatrix"), 1, GL_TRUE, worldToViewMatrix.m);

		//Sun
		glUniform1f(glGetUniformLocation(program, "drawSun"), 1);
		glUseProgram(program);
		modelToWorldMatrixSun = Mult(T(sun_position.x, sun_position.y, sun_position.z), S(5, 5, 5));
		glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixSun.m);
		glBindTexture(GL_TEXTURE_2D, satellites_texture[0].texID);
		DrawModel(model[0], program, "inPosition", "inNormal", "inTexCoord");
		glUniform1f(glGetUniformLocation(program, "drawSun"), 0);

		sphere_position = sun_position;
		CollisionDetection(radius_sun);

		for(i = 0; i < 8; i++){
			//Draw planets
			glUseProgram(program);
			modelToWorldMatrixPlanet = Mult(Mult(modelToWorldMatrixSun, Mult(Ry(translation_planet), T((sun_position.x+distance_to_sun[i])*x_deviation[i], 0, (sun_position.z+distance_to_sun[i])*z_deviaton[i]))), Mult(Ry(rotation_planet), S(planet_size[i], planet_size[i], planet_size[i])));
			glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixPlanet.m);
			glBindTexture(GL_TEXTURE_2D, satellites_texture[i+1].texID);
			DrawModel(model[0], program, "inPosition", "inNormal", "inTexCoord");
			sphere_position = SetVector(modelToWorldMatrixPlanet.m[3], modelToWorldMatrixPlanet.m[7], modelToWorldMatrixPlanet.m[11]);
			CollisionDetection(radius_planets[i]*5);

			//Draw satellite
			glUseProgram(program);
			modelToWorldMatrixSatellite = Mult(modelToWorldMatrixPlanet, Mult(Ry(translation_satellite), Mult(T(5, 0, 0), S(0.2, 0.2, 0.2))));
			glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixSatellite.m);
			glBindTexture(GL_TEXTURE_2D, satellites_texture[9].texID);
			DrawModel(model[0], program, "inPosition", "inNormal", "inTexCoord");
			sphere_position = SetVector(modelToWorldMatrixSatellite.m[3], modelToWorldMatrixSatellite.m[7], modelToWorldMatrixSatellite.m[11]);
			CollisionDetection(radius_satellite*1.5);
		}

	//Draw rocket
		glUseProgram(program);
		modelToWorldMatrixRocket = Mult(T(position_rocket.x, position_rocket.y, position_rocket.z), Mult(S(0.005, 0.005, 0.005), Mult(rotation_matrix, Rx(M_PI/2))));
		glUniformMatrix4fv(glGetUniformLocation(program, "modelToWorldMatrix"), 1, GL_TRUE, modelToWorldMatrixRocket.m);
		glBindTexture(GL_TEXTURE_2D, rocket_texture.texID);
		DrawModel(model[1], program, "inPosition", "inNormal", "inTexCoord");

	glutSwapBuffers();
}

/**
 * @brief Main program 
 * @param argc Number of arguments
 * @param argv Pointer to the arguments
 */
int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutCreateWindow("Solar system");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	init();
	loadTextures();
	glutTimerFunc(20, &timer, 0);
	printf("Move forward with 'w'\n");
	printf("Move backward with 's'\n");
	printf("Move left with 'a'\n");
	printf("Move right with 'd'\n");
	printf("Change view with 'q'\n");
	glutMainLoop();
	return 0;
}
