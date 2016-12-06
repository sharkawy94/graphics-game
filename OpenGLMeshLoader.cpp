#include "Model_3DS.h"
#include "GLTexture.h"
#include <conio.h>
#include <iostream>
#include <time.h>
#include <stdlib.h> 
#include <sstream>
#include <string>
#include <stdio.h>
#include <cstring>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <glut.h>
#include <MMSystem.h>
#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <fstream>
#include <conio.h>
using namespace std;

int WIDTH = 1000;
int HEIGHT = 670;
float moveTheme = 0;
float pos = 0;
float maxPos = 0;
float obs1 = 0;
float moveValue = 1.6;
float pauseMoveValue = 0;
float pauseTurnValue = 0;
float turnValue = 0.8;
float themeCenter = 0;
float randZ[40];
float updateRandZ[40];
float randX[40];
float coinPosX[80];
float coinPosZ[80];
float copyCoinPosZ[80];
float height = 1;
float heightValue = -sqrt(2);
float maxHeight = 3;
int camera = 0;
int coinsCollected = 0;
int timer = 0;
int counter = 3;
bool obs = false;
bool Moveright = false;
bool pause = false;
bool Moveleft = false;
bool up = false;
bool down = false;
bool gameOver = false;
bool createObstacles = true;
bool collision = false;
bool jump = false;
bool level1 = true;
bool level2 = false;
bool allowRestart = true;
float animCount = 0;
float levelCount = 5000/33;
float nightValue = 0;
int score = 0;
#define KEY_UP 72
//#define GLUT_BITMAP_HELVETICA_18

void createRandomCoins(void);
void createRandomObstacles(void);
void restart(void);

char title[] = "3D Model Loader Sample";

// 3D Projection Options
GLdouble fovy = 45.0;
GLdouble aspectRatio = (GLdouble)WIDTH / (GLdouble)HEIGHT;
GLdouble zNear = 0.1;
GLdouble zFar = 100;

class Vector
{
public:
	GLdouble x, y, z;
	Vector() {}
	Vector(GLdouble _x, GLdouble _y, GLdouble _z) : x(_x), y(_y), z(_z) {}
	//================================================================================================//
	// Operator Overloading; In C++ you can override the behavior of operators for you class objects. //
	// Here we are overloading the += operator to add a given value to all vector coordinates.        //
	//================================================================================================//
	void operator +=(float value)
	{
		x += value;
		y += value;
		z += value;
	}
};

Vector Eye(0, 8, 20);
Vector At(0, 0, 0);
Vector Up(0, 1, 0);

int cameraZoom = 0;

// Model Variables
Model_3DS model_house;
Model_3DS model_tree;
Model_3DS toiletPaper;
Model_3DS car;
//Model_3DS coin;

// Textures
GLTexture tex_asphalt;
GLTexture side_sidewalk;
GLTexture top_sidewalk;
GLTexture tex_grass;
GLTexture tex_oil;
GLTexture tex_sky;
GLTexture tex_coin;

void drawText(const char *text, int x, int y){

	int size = (int)strlen(text);
	glRasterPos2i(x, y);
	for (int i = 0; i < size; i++) {
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]);
		//glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, text[i]);
	}
}

//=======================================================================
// Lighting Configuration Function
//=======================================================================
void InitLightSource()
{
	// Enable Lighting for this OpenGL Program
	glEnable(GL_LIGHTING);

	// Enable Light Source number 0
	// OpengL has 8 light sources
	glEnable(GL_LIGHT0);

	// Define Light source 0 ambient light
	GLfloat ambient[] = { 0.1f, 0.1f, 0.1, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);

	// Define Light source 0 diffuse light
	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

	// Define Light source 0 Specular light
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	// Finally, define light source 0 position in World Space
	GLfloat light_position[] = { 0.0f, 10.0f, -5.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
}

//=======================================================================
// Material Configuration Function
//======================================================================
void InitMaterial()
{
	// Enable Material Tracking
	glEnable(GL_COLOR_MATERIAL);

	// Sich will be assigneet Material Properties whd by glColor
	glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

	// Set Material's Specular Color
	// Will be applied to all objects
	GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

	// Set Material's Shine value (0->128)
	GLfloat shininess[] = { 128.0f };
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
}

//=======================================================================
// OpengGL Configuration Function
//=======================================================================
void myInit(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);

	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);
	//*******************************************************************************************//
	// fovy:			Angle between the bottom and top of the projectors, in degrees.			 //
	// aspectRatio:		Ratio of width to height of the clipping plane.							 //
	// zNear and zFar:	Specify the front and back clipping planes distances from camera.		 //
	//*******************************************************************************************//

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
	//*******************************************************************************************//
	// EYE (ex, ey, ez): defines the location of the camera.									 //
	// AT (ax, ay, az):	 denotes the direction where the camera is aiming at.					 //
	// UP (ux, uy, uz):  denotes the upward orientation of the camera.							 //
	//*******************************************************************************************//

	InitLightSource();

	InitMaterial();

	glEnable(GL_DEPTH_TEST);

	glEnable(GL_NORMALIZE);
}

//=======================================================================
// Render Ground Function
//=======================================================================
void sidewalkSide(){
	for (float z = -9; z <= 10; z++){
		glPushMatrix();
		glTranslatef(8, 0, z);
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(0, 0.1, -1);
		glTexCoord2f(1, 0);
		glVertex3f(0, 0.1, 1);
		glTexCoord2f(1, 1);
		glVertex3f(0,0.7, 1);
		glTexCoord2f(0, 1);
		glVertex3f(0, 0.7, -1);
		glEnd();
		glPopMatrix();
	}
	for (float z = -9; z <= 10; z++){
		glPushMatrix();
		glTranslatef(-8, 0, z);
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(0, 0.1, -1);
		glTexCoord2f(1, 0);
		glVertex3f(0, 0.1, 1);
		glTexCoord2f(1, 1);
		glVertex3f(0, 0.7, 1);
		glTexCoord2f(0, 1);
		glVertex3f(0, 0.7, -1);
		glEnd();
		glPopMatrix();
	}
}

void sidewalkTop(){
	for (float z = -9; z <= 10; z++){
		glPushMatrix();
		glTranslatef(8.9, 0.7, z);
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(-1, 0, -1);
		glTexCoord2f(1, 0);
		glVertex3f(1, 0, -1);
		glTexCoord2f(1, 1);
		glVertex3f(1, 0, 1);
		glTexCoord2f(0, 1);
		glVertex3f(-1, 0, 1);
		glEnd();
		glPopMatrix();
	}
	for (float z = -9; z <= 10; z++){
		glPushMatrix();
		glTranslatef(-8.9, 0.7, z);
		glBegin(GL_QUADS);
		glNormal3f(0, 1, 0);	// Set quad normal direction.
		glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
		glVertex3f(-1, 0, -1);
		glTexCoord2f(1, 0);
		glVertex3f(1, 0, -1);
		glTexCoord2f(1, 1);
		glVertex3f(1, 0, 1);
		glTexCoord2f(0, 1);
		glVertex3f(-1, 0, 1);
		glEnd();
		glPopMatrix();
	}
}

void grass(){
	glPushMatrix();
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-60, 0, -10);
	glTexCoord2f(20, 0);
	glVertex3f(60, 0, -10);
	glTexCoord2f(20, 5);
	glVertex3f(60, 0, 10);
	glTexCoord2f(0, 5);
	glVertex3f(-60, 0, 10);
	glEnd();
	glPopMatrix();
}

void asphalt(){
	glPushMatrix();
	glTranslatef(0, 0.1, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-10, 0, -10);
	glTexCoord2f(4, 0);
	glVertex3f(10, 0, -10);
	glTexCoord2f(4, 5);
	glVertex3f(10, 0, 10);
	glTexCoord2f(0, 5);
	glVertex3f(-10, 0, 10);
	glEnd();
	glPopMatrix();
}

void theme(float center){
	glPushMatrix();
	//glScalef(1, 1, 0.2);
	glTranslatef(0, 0, center);
	
	//glDisable(GL_LIGHTING);
	//glColor3f(0.9, 0.9, 0.9); //bulbs
	//glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
	
	glBindTexture(GL_TEXTURE_2D, tex_asphalt.texture[0]);	// Bind the ground texture

	asphalt();
	//glEnable(GL_LIGHTING);

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, side_sidewalk.texture[0]);	// Bind the ground texture

	//glRotated(90, 0, 1, 1);
	//glRotatef(90, 0, 0, 1);

	sidewalkSide();

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing
	
	glBindTexture(GL_TEXTURE_2D, top_sidewalk.texture[0]);

	sidewalkTop();

	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_grass.texture[0]);	// Bind the ground texture

	grass();
	//glColor3f(1, 1, 1);
	glPopMatrix();
}

void RenderGround()
{
	//if (!night){
	//	glDisable(GL_LIGHTING);	// Disable lighting 
	//}
	//else{
	//	glEnable(GL_LIGHTING);
	//}
	glEnable(GL_LIGHTING);

	glColor3f(2+nightValue, 2+nightValue, 2+nightValue);	// Dim the ground texture a bit

	/*for (int i = 0; i <= 400; i += 20){
		theme(moveTheme - i);
	}*/
	if (moveTheme-25 >= themeCenter){
		themeCenter += 20;
	}
	//glPushMatrix();
	//glScalef(0, 0, 0.5);
	theme(moveTheme - themeCenter + 40);
	theme(moveTheme - themeCenter +20);
	theme(moveTheme - themeCenter);
	//glPopMatrix();
	float j = 0.02;
	for (int i = 20; i <= 100; i += 20){
		//glColor3f(1-j, 1-j, 1-j);
		theme(moveTheme - i - themeCenter);
		//j += 0.02;
	}
	glColor3f(1, 1, 1);
	//theme(moveTheme - 40 - themeCenter);

	//glPushMatrix();
	//glBegin(GL_QUADS);
	//glNormal3f(0, 1, 0);	// Set quad normal direction.
	//glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	//glVertex3f(0, 0, -5);
	//glTexCoord2f(1, 0);
	//glVertex3f(1, 0, -5);
	//glTexCoord2f(1, 1);
	//glVertex3f(1, 0, 5);
	//glTexCoord2f(0, 1);
	//glVertex3f(0, 0, 5);
	//glEnd();
	//glPopMatrix();

	glEnable(GL_LIGHTING);	// Enable lighting again for other entites coming throung the pipeline.

}

//=======================================================================
// Display Function
//=======================================================================

void oilTrap(float PosX , float PosZ){
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_oil.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glTranslatef(PosX, 0.2, PosZ);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-1.5, 0, -1.5);
	glTexCoord2f(1, 0);
	glVertex3f(1.5, 0, -1.5);
	glTexCoord2f(1, 1);
	glVertex3f(1.5, 0, 1.5);
	glTexCoord2f(0, 1);
	glVertex3f(-1.5, 0, 1.5);
	glEnd();
	glPopMatrix();
}

void createObstacle(){
	for (int i = 0; i < 40; i++){
		//if (randZ[i] + moveTheme < 15){
			oilTrap(randX[i], randZ[i] + moveTheme);
		//}
	}
	
}

void cam2d(){
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(- WIDTH/2, WIDTH/2, - HEIGHT/2, HEIGHT/2, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 1, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	glDisable(GL_COLOR_MATERIAL);
}

void cam3d() {
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	gluPerspective(fovy, aspectRatio, zNear, zFar);

	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
}

void drawsky() {
	glNormal3f(0, 0, -1);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_sky.texture[0]);	// Bind the ground texture

	//glColor3b(100, 20, 20);

	glPushMatrix();
	//glRotated(5, 1, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(20, 0);
	glVertex3d(500, 335, 0);
	glTexCoord2f(0, 0);
	glVertex3d(-500, 335, 0);
	glTexCoord2f(0, 0.9);
	glVertex3d(-500, 225, 0);
	glTexCoord2f(20, 0.9);
	glVertex3d(500, 225, 0);
	glEnd();
	glPopMatrix();

	//glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	//glBindTexture(GL_TEXTURE_2D, tex_oil.texture[0]);	// Bind the ground texture

	//glPushMatrix();
	//glTranslatef(PosX, 0.2, 0);
	//glBegin(GL_QUADS);
	//glNormal3f(0, 1, 0);	// Set quad normal direction.
	//glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	//glVertex3f(-1.5, 0, -1.5);
	//glTexCoord2f(1, 0);
	//glVertex3f(1.5, 0, -1.5);
	//glTexCoord2f(1, 1);
	//glVertex3f(1.5, 0, 1.5);
	//glTexCoord2f(0, 1);
	//glVertex3f(-1.5, 0, 1.5);
	//glEnd();
	//glPopMatrix();
}

void checkCollision(){
	for (int i = 0; i < 40; i++){
		updateRandZ[i] += moveValue;
		collision = randZ[i] + moveTheme - 1.5 - 30 <= 9 && randZ[i] + moveTheme + 1.5 - 30 >= 6 && randX[i] >= pos - 0.2 && randX[i] <= pos + 0.2 &&height == 1;
		if (collision){
			gameOver = true;
			cout << updateRandZ[i] << endl;
			cout << randZ[i] + moveTheme - 1.5 << endl;
			cout << randX[i] << endl;
			break;
		}
	}
}
void coin(float posX , float posZ){
	glEnable(GL_TEXTURE_2D);	// Enable 2D texturing

	glBindTexture(GL_TEXTURE_2D, tex_coin.texture[0]);	// Bind the ground texture

	glPushMatrix();
	glTranslatef(posX, 2, posZ);
	glRotatef(-90, 1, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);	// Set quad normal direction.
	glTexCoord2f(0, 0);		// Set tex coordinates ( Using (0,0) -> (5,5) with texture wrapping set to GL_REPEAT to simulate the ground repeated grass texture).
	glVertex3f(-0.5, 0, -0.5);
	glTexCoord2f(1, 0);
	glVertex3f(0.5, 0, -0.5);
	glTexCoord2f(1, 1);
	glVertex3f(0.5, 0, 0.5);
	glTexCoord2f(0, 1);
	glVertex3f(-0.5, 0, 0.5);
	glEnd();
	glPopMatrix();
}
void createCoins(){
	
	for (int i = 0; i < 80; i++){
		if (height >= 1.7 && height <= 3 && coinPosZ[i] + moveTheme-30 >= 4.5 && coinPosZ[i] + moveTheme-30 <= 9.5 && coinPosX[i] == pos){
			coinPosZ[i] = 20;
			//&& coinPosZ[i] >= 6 && coinPosZ[i] <= 8 && coinPosX[i] == pos
			coinsCollected++;
		}
		coin(coinPosX[i], coinPosZ[i] + moveTheme);
	}
}

void myDisplay(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	cam2d();
	drawsky();
	cam3d();
	//glEnable(GL_LIGHTING);
	glClearColor(1, 1, 1, 1);

	//drawLane(0);

	// Draw Ground
	glPushMatrix();
	glTranslatef(0, 0, - 30);
	RenderGround();

	glPushMatrix();
	//glTranslatef(0, 0, moveTheme);
	createObstacle();
	glPopMatrix();

	//glPushMatrix();
	createCoins();
	glPopMatrix();
	//glPopMatrix();

	/*if (collision == false){
		checkCollision();
	}*/

	//if (createObstacles){
	//	/*createObstacle();
	//	createObstacle();*/
	//	//createObstacle();
	//	createObstacles = false;
	//}
	// Draw House Model
	/*glPushMatrix();
	glTranslatef(0, 0, 10);
	glScalef(0.7, 0.7, 0.7);
	model_tree.Draw();
	glPopMatrix();*/

	//glPushMatrix();
	//glTranslatef(8, 5, 0);
	////glRotatef(90, 0, 0, 1);
	//glScalef(0.02, 0.02, 0.02);
	//toiletPaper.Draw();
	//glPopMatrix();
	/*glPushMatrix();
	glRotatef(180, obs1, 0, obs1);*/

	glPushMatrix();
	glTranslatef(pos, height, 7.5);
	glRotatef(-90, obs1, 1, 0);
	glScalef(0.02, 0.02, 0.02);
	car.Draw();
	glPopMatrix();
	/*glPopMatrix();*/
	drawText("collected:", 6, 7.5);
	drawText("time:", -6, 7.5);

	stringstream ss;
	//int timer = (glutGet(GLUT_ELAPSED_TIME) + 1000);
	//ss << timer / 1000 << "s";
	ss << timer << "s";
	const string tmp = ss.str();
	const char* cstr = tmp.c_str();
	//cstr = tmp.c_str();	
	drawText(cstr, -5.5, 7.5);

	stringstream cc;
	cc << coinsCollected;
	const string tmp2 = cc.str();
	const char* cstr2 = tmp2.c_str();
	//cstr = tmp.c_str();
	drawText(cstr2, 8, 7.5);

	if (coinsCollected == 10 && level1 && !level2){
		gameOver = true;
		drawText("level2 will get started in", -2, 7.5);
		stringstream mm;
		mm << counter;
		const string tmp3 = mm.str();
		const char* cstr3 = tmp3.c_str();
		drawText(cstr3, 3, 7.5);
		allowRestart = false;
	}
	if (counter == 0 && level1){
		animCount = 0;
		level1 = false;
		level2 = true;
		createRandomCoins();
		createRandomObstacles();
		restart();
		allowRestart = true;
	}
	if (coinsCollected >= 10 && level2 && moveTheme >= 635 && timer <= 11){
		gameOver = true;
		drawText("CONGRATS", -1, 7.5);
		
	}
	/*if (coinsCollected == 10){
		gameOver = true;
		drawText("congrats", 0, 7.5);
	}*/
	
	if (collision){
		drawText("OPSSSS", -1, 7.5);
	}
	/*if (randZ[40] + moveTheme - 1.5 - 15 <= 9 && randZ[40] + moveTheme + 1.5 - 15 >= 6){
		drawText("GAMEOVER", 0, 7.5);
		gameOver = true;
		pause = false;
	}*/
	if (moveTheme >= 635 && coinsCollected < 10 && timer<=11){
		drawText("COLLECT MORE COINS", -2, 7.5);
		gameOver = true;
		pause = false;
	}
	if (moveTheme >= 635 && coinsCollected < 10 && timer > 11){
		drawText("YOU SUCK", -1, 7.5);
		gameOver = true;
		pause = false;
	}
	if (moveTheme >= 635 && coinsCollected >= 10 && timer > 11){
		drawText("TOO SLOW", -1, 7.5);
		gameOver = true;
		pause = false;
	}

	if (coinsCollected != 10 && pause){
		drawText("PAUSE", 0, 7.5);
	}

	if (coinsCollected > 0){
		score = coinsCollected * 50 - 10 * timer;
	}
	if (score < 0){
		score = 0;
	}
	stringstream rr;
	rr << score;
	const string tmp4 = rr.str();
	const char* cstr4 = tmp4.c_str();
	drawText(cstr4, -8, 7.5);
	drawText("my score:", -10, 7.5);
	
	// Draw Tree Model
	/*glPushMatrix();
	glRotatef(90.f, 1, 0, 0);
	model_house.Draw();
	glPopMatrix();*/

	glutSwapBuffers();
}

//=======================================================================
// Keyboard Function
//=======================================================================
void restart(){
	moveTheme = 0;
	pos = 0;
	maxPos = 0;
	obs1 = 0;
	moveValue = 1.6;
	pauseMoveValue = 0;
	pauseTurnValue = 0;
	turnValue = 0.8;
	themeCenter = 0;
	height = 1;
	heightValue = -sqrt(2);
	maxHeight = 3;
	coinsCollected = 0;
	timer = 0;
	obs = false;
	Moveright = false;
	pause = false;
	Moveleft = false;
	up = false;
	down = false;
	gameOver = false;
	createObstacles = true;
	collision = false;
	jump = false;
	nightValue = 0;
	animCount = 0;
	score = 0;
	for (int i = 0; i < 80; i++){
		coinPosZ[i] = copyCoinPosZ[i];
	}
}
void myKeyboard(unsigned char button, int x, int y)
{
	/*if (getch() == KEY_UP){
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		moveTheme -=0.3;
	}*/
	/*switch (button)
	{
	case 'd':
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 'r':
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;
	case 27:
		exit(0);
		break;
	default:
		break;
		}*/
	if (button == 'v'){
		if (camera == 0){
			camera = 1;
			Eye.y = 15;
		}
		else{
			camera = 0;
			Eye.y = 8;
		}
	}
	if (button == 'p' && pause == gameOver){
		if (!pause){
			pauseMoveValue = moveValue;
			pauseTurnValue = turnValue;
			gameOver = true;
			pause = true;
		}
		else{
			pause = false;
			gameOver = false;
			moveValue = pauseMoveValue;
			turnValue = pauseTurnValue;
		}
	}
	if (button == 'r' && allowRestart){
		restart();
	}
	if (!gameOver){
		if (button == ' ' && !jump && !Moveleft && !Moveright){
			jump = true;
		}
		if (button == 'd'){
			Moveright = true;
			Moveleft = false;
			if (pos <= 4.8 &&pos >= -1.5){
				maxPos = 4.8;
			}
			else{
				maxPos = 0;
			}
		}
		if (button == 'w' && !jump){
			up = true;
			if (down){
				moveValue = 1.6;
				//heightValue = 0.2;
			}
			else{
				moveValue = 1.9;
				//heightValue = 0.3;
			}
			down = false;
		}
		if (button == 's' && !jump){
			if (up){
				moveValue = 1.6;
				//heightValue = 0.2;
			}
			else{
				moveValue = 1.3;
				//heightValue = 0.1;
			}
			up = false;
			down = true;
		}
		if (button == 'a'){
			Moveright = false;
			Moveleft = true;
			up = false;
			down = false;
			if (pos <= 1.5 &&pos >= -4.8){
				maxPos = -4.8;
			}
			else{
				maxPos = 0;
			}
		}
	}

	glutPostRedisplay();
}

//=======================================================================
// Motion Function
//=======================================================================
//void myMotion(int x, int y)
//{
//	y = HEIGHT - y;
//
//	if (cameraZoom - y > 0)
//	{
//		Eye.x += -0.1;
//		Eye.z += -0.1;
//	}
//	else
//	{
//		Eye.x += 0.1;
//		Eye.z += 0.1;
//	}
//
//	cameraZoom = y;
//
//	glLoadIdentity();	//Clear Model_View Matrix
//
//	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);	//Setup Camera with modified paramters
//
//	GLfloat light_position[] = { 0.0f, 10.0f, 0.0f, 1.0f };
//	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//
//	glutPostRedisplay();	//Re-draw scene 
//}

//=======================================================================
// Mouse Function
//=======================================================================
void myMouse(int button, int state, int x, int y)
{
	//y = HEIGHT - y;
	//state == GLUT_DOWN

	if (button == GLUT_LEFT_BUTTON && !gameOver)
	{
		if (!jump && !Moveleft && !Moveright){
			jump = true;
		}
		//cameraZoom = y;
	}
}


//=======================================================================
// Reshape Function
//=======================================================================
void myReshape(int w, int h)
{
	if (h == 0) {
		h = 1;
	}

	WIDTH = w;
	HEIGHT = h;

	// set the drawable region of the window
	glViewport(0, 0, w, h);

	// set up the projection matrix 
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLdouble)WIDTH / (GLdouble)HEIGHT, zNear, zFar);

	// go back to modelview matrix so we can move the objects about
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(Eye.x, Eye.y, Eye.z, At.x, At.y, At.z, Up.x, Up.y, Up.z);
}

//=======================================================================
// Assets Loading Function
//=======================================================================
void LoadAssets()
{
	// Loading Model files
	//model_house.Load("Models/house/house.3ds");
	//model_tree.Load("Models/tree/Tree1.3ds");
	//toiletPaper.Load("Models/barrel/Barrel N281211.3ds");
	//toiletPaper.Load("Models/paper/paper.3ds");
	car.Load("Models/car/Car Lamborghini gallardo 2005 N240211.3ds");
	//coin.Load("Models/coin/Coin 1.3ds");

	// Loading texture files
	tex_asphalt.Load("Textures/asphalt.tga");
	side_sidewalk.Load("Textures/brick2.tga");
	top_sidewalk.Load("Textures/sidewalkTop.tga");
	tex_grass.Load("Textures/grass.tga");
	tex_oil.Load("Textures/oil3.tga");
	tex_sky.Load("Textures/sky7.tga");
	tex_coin.Load("Textures/coin.tga");
}
//=======================================================================
// Anim Function
//=======================================================================
void Anim(int value) {
	
	if (gameOver){
		moveValue = 0;
		turnValue = 0;
	}
	if (collision){
		obs = true;
	}
	if (!gameOver){
		animCount++;
		
		timer = ((animCount * 1000) / 33) / 1000;
		if (jump){
			
			if (height == 1){
				heightValue = -sqrt(2);
			}
			/*if (height == 3){
				heightValue = -0.2;
			}*/
			//height += heightValue;
			height = (heightValue*heightValue*-1) + 3;
			heightValue += 0.1;
			if (heightValue >= sqrt(2)){
				height = 1;
				jump = false;
				Moveleft = false;
				Moveright = false;
			}
			//for (float x = -sqrt(2); x <= sqrt(2);)
			/*if (maxHeight == 3 && height+heightValue >= maxHeight){
				maxHeight = 1; 
				height = 3;
				heightValue *= -1;
			}
			else{
				if (maxHeight == 1 && height-heightValue <= maxHeight){
					maxHeight = 3;
					height = 1;
					heightValue *= -1;
					jump = false;
					Moveleft = false;
					Moveright = false;
				}
			}		*/
		}
		for (float i = 0.1; i <= moveValue && !gameOver; i+= 0.1){
			//moveTheme += moveValue;
			moveTheme += 0.1;
			checkCollision();
		}
		if (!jump){
			if (Moveright && pos <= maxPos){
				if (pos + turnValue >= maxPos){
					pos = maxPos;
					Moveright = false;
				}
				else{
					pos += turnValue;
				}
			}
			if (Moveleft && pos >= maxPos){
				if (pos - turnValue <= maxPos){
					pos = maxPos;
					Moveleft = false;
				}
				else{
					pos -= turnValue;
				}
			}
		}
	}
	if (obs && obs1 <= 6){
		obs1 += 0.3;
	}
	if (moveTheme >= 200 && nightValue>= -0.9 && level2){
		nightValue -= 0.04;
	}
	if (moveTheme >= 400 && nightValue >= -1.5 && level2){
		nightValue -= 0.04;
	}
	if (coinsCollected == 10 && level1 && !level2){
		levelCount--;
		counter = ((levelCount) / 33);
	}
	glutPostRedisplay();
	

	glutTimerFunc(33, Anim, 0);
}
//=======================================================================
// Main Function
//=======================================================================
void createRandomObstacles(){
	srand(time(NULL));
	for (int i = 0; i < 40; i++){
		int randomX = rand() % 10 + 1;
		float randomZ = ((float)rand() / (RAND_MAX));
		randZ[i] = (randomZ + i)*(-15);
		updateRandZ[i] = randZ[i];
		if (randomX > 6){
			//oilTrap(0, randomZ*(i+1));
			randX[i] = 0;
		}
		else{
			if (randomX > 3){
				//oilTrap(4.8, randomZ*i);
				randX[i] = 4.8;
			}
			else{
				randX[i] = -4.8;
				//oilTrap(-4.8, randomZ*i);
			}
		}
	}
}
void createRandomCoins(){
	srand(time(NULL));
	for (int i = 0; i < 80; i++){
		int randomX = rand() % 10 + 1;
		float randomZ = ((float)rand() / (RAND_MAX));
		coinPosZ[i] = (randomZ + i)*(-7.5);
		copyCoinPosZ[i] = coinPosZ[i];
		//updateRandZ[i] = randZ[i];
		if (randomX > 6){
			//oilTrap(0, randomZ*(i+1));
			coinPosX[i] = 0;
		}
		else{
			if (randomX > 3){
				//oilTrap(4.8, randomZ*i);
				coinPosX[i] = 4.8;
			}
			else{
				coinPosX[i] = -4.8;
				//oilTrap(-4.8, randomZ*i);
			}
		}
	}
}

void playMusic(int value){
	PlaySound("highway to hell", NULL, SND_MEMORY);
	//SND_ASYNC
	glutTimerFunc(32000, playMusic, 0);
}

void main(int argc, char** argv)
{
	//PlaySound(TEXT("music/highway to hell.wav"), NULL, SND_ASYNC);
	/*Wave one("highway to hell.WAV");
	one.play();
	getch();*/
	createRandomObstacles();

	createRandomCoins();

	/*for (int i = 0; i < 10; i++){
		cout << randZ[i] << endl;
	}*/

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

	glutInitWindowSize(WIDTH, HEIGHT);

	glutInitWindowPosition(50, 50);

	glutCreateWindow(title);

	glutDisplayFunc(myDisplay);

	//glutIdleFunc(Anim);

	glutTimerFunc(33, Anim, 0);
	//playMusic(5);
	//glutTimerFunc(32000, playMusic, 0);

	
	glutKeyboardFunc(myKeyboard);

	//glutMotionFunc(myMotion);

	glutMouseFunc(myMouse);

	glutReshapeFunc(myReshape);

	myInit();

	LoadAssets();

	glutMainLoop();
	
}