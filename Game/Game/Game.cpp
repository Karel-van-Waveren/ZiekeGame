#define _USE_MATH_DEFINES
#define STB_IMAGE_IMPLEMENTATION
#define GLEW_STATIC
#include <GL\glew.h>
#include <GL/freeglut.h>

#include "stb_image.h"
#include <vector>
#include <cmath>
#include "Particles.h"
#include "Game.h"
#include "Shader.h"
#include "Skybox.h"
#include "ObjectLoader.h"

//using namespace std;

float rotate = 0;


GLuint VBO, VAO;
Shader* ourShader;

GLuint SkyboxVBO, SkyboxVAO;
Shader* skyboxShader;

ObjectLoader* rose;

float lastFrameTime = 0;

int width, height;
bool jumping = false;
float upwardMomentum;
const float gravity = 20.00f;

unsigned int textureId;

int ***arr3D;
const int X3d = 16;
const int Y3d = 16;
const int Z3d = 16;

GLfloat skyboxVertices[] = {
	// Positions          
	-1.0f,  1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f, -1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,

	-1.0f, -1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f, -1.0f,  1.0f,
	-1.0f, -1.0f,  1.0f,

	-1.0f,  1.0f, -1.0f,
	1.0f,  1.0f, -1.0f,
	1.0f,  1.0f,  1.0f,
	1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f,  1.0f,
	-1.0f,  1.0f, -1.0f,

	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f,  1.0f,
	1.0f, -1.0f,  1.0f
};

struct Camera
{
	float posX = 0;
	float posY = 0;
	float posZ = -4;
	float rotX = 0;
	float rotY = 0;
} camera;

bool keys[255];


void drawCube(int i) {
	glBindTexture(GL_TEXTURE_2D, textureId);
	if (i == -1)
		return;
	i = i % 256;

	float x1 = i % 16 * (1.0f / 16.0f);
	float y1 = i / 16 * (1.0f / 16.0f);

	float x2 = x1 + (1.0f / 16.0f);
	float y2 = y1 + (1.0f / 16.0f);

	glBegin(GL_QUADS);
	glColor3f(1, 1, 1);
	glTexCoord2f(x1, y1); glVertex3f(-1, -1, -1);
	glTexCoord2f(x1, y2); glVertex3f(1, -1, -1);
	glTexCoord2f(x2, y2); glVertex3f(1, 1, -1);
	glTexCoord2f(x2, y1); glVertex3f(-1, 1, -1);

	glTexCoord2f(x1, y1); glVertex3f(-1, -1, 1);
	glTexCoord2f(x1, y2); glVertex3f(1, -1, 1);
	glTexCoord2f(x2, y2); glVertex3f(1, 1, 1);
	glTexCoord2f(x2, y1); glVertex3f(-1, 1, 1);

	glTexCoord2f(x1, y1); glVertex3f(-1, -1, -1);
	glTexCoord2f(x1, y2); glVertex3f(-1, 1, -1);
	glTexCoord2f(x2, y2); glVertex3f(-1, 1, 1);
	glTexCoord2f(x2, y1); glVertex3f(-1, -1, 1);

	glTexCoord2f(x1, y1); glVertex3f(1, -1, -1);
	glTexCoord2f(x1, y2); glVertex3f(1, 1, -1);
	glTexCoord2f(x2, y2); glVertex3f(1, 1, 1);
	glTexCoord2f(x2, y1); glVertex3f(1, -1, 1);

	glTexCoord2f(x1, y1); glVertex3f(-1, -1, -1);
	glTexCoord2f(x1, y2); glVertex3f(1, -1, -1);
	glTexCoord2f(x2, y2); glVertex3f(1, -1, 1);
	glTexCoord2f(x2, y1); glVertex3f(-1, -1, 1);

	glTexCoord2f(x1, y1); glVertex3f(-1, 1, -1);
	glTexCoord2f(x1, y2); glVertex3f(1, 1, -1);
	glTexCoord2f(x2, y2); glVertex3f(1, 1, 1);
	glTexCoord2f(x2, y1); glVertex3f(-1, 1, 1);
	glEnd();
}
void drawCube()
{
	glBindTexture(GL_TEXTURE_2D, textureId);
	glBegin(GL_QUADS);
	glColor3f(1, 0, 0);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(-1, 1, -1);

	glColor3f(1, 1, 0);
	glVertex3f(-1, -1, 1);
	glVertex3f(1, -1, 1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);

	glColor3f(0, 0, 1);
	glVertex3f(-1, -1, -1);
	glVertex3f(-1, 1, -1);
	glVertex3f(-1, 1, 1);
	glVertex3f(-1, -1, 1);

	glColor3f(1, -1, 1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	glVertex3f(1, -1, 1);

	glColor3f(0, 1, 0);
	glVertex3f(-1, -1, -1);
	glVertex3f(1, -1, -1);
	glVertex3f(1, -1, 1);
	glVertex3f(-1, -1, 1);

	glColor3f(1, 1, 0);
	glVertex3f(-1, 1, -1);
	glVertex3f(1, 1, -1);
	glVertex3f(1, 1, 1);
	glVertex3f(-1, 1, 1);
	glEnd();
}


void display()
{

	//glClearColor(0.6f, 0.6f, 1, 1);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(90.0f, (float)width / height, 1, 300);



	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(camera.rotX, 1, 0, 0);
	glRotatef(camera.rotY, 0, 1, 0);
	glTranslatef(camera.posX, camera.posZ, camera.posY);
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	//glColor3f(.5f, .5f, .5f);
	glDisable(GL_TEXTURE_2D);

	// static object
	glPushMatrix();
	glTranslatef(2, 2, -4);
	glutSolidSphere(2, 100, 100);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(8, 2, -4);
	glRotatef(rotate, 90, 0,0);
	glutSolidCube(4);
	glPopMatrix();


	//ourShader->Use();
	//glBindVertexArray(VAO);
	//glDrawArrays(GL_TRIANGLES, 0, 3);
	//glBindVertexArray(0);

	//glPushMatrix();
	//glTranslatef(-2, 2, 2);
	drawParticles();
	//glPopMatrix();

	glutSolidSphere(20, 10, 10);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0);									glVertex3f(-15, -1, -15);
	glTexCoord2f(0, 1.0f / 16.0f);						glVertex3f(15, -1, -15);
	glTexCoord2f(1.0f / 16.0f, 1.0f / 16.0f);			glVertex3f(15, -1, 15);
	glTexCoord2f(1.0f / 16.0f, 0);						glVertex3f(-15, -1, 15);
	glEnd();

	//for (int x = -10; x <= 10; x += 5)
	//{
	//	for (int y = -10; y <= 10; y += 5)
	//	{
	//		glPushMatrix();
	//		glTranslatef((float)x, 0.0f, (float)y);
	//		drawCube();
	//		glPopMatrix();
	//	}
	//}

	//for (int i = 0; i < X3d; i++) {
	//	for (int j = 0; j < Y3d; j++) {
	//		for (int k = 0; k < Z3d; k++) {
	//			int	m = arr3D[i][j][k];
	//			glPushMatrix();
	//			glTranslatef(i * 2, -15 * 2 + k * 2, j * 2);
	//			drawCube(m);
	//			glPopMatrix();
	//		}
	//	}
	//}

	//glDepthMask(GL_FALSE);
	//skyboxShader->Use();

	//glBindVertexArray(SkyboxVAO);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	//glDrawArrays(GL_TRIANGLES, 0, 36);
	//glBindVertexArray(0);
	//glDepthMask(GL_TRUE);

	//glEnable(GL_DEPTH_TEST);

	glutSwapBuffers();
}

void move(float angle, float fac)
{
	camera.posX += (float)cos((camera.rotY + angle) / 180 * M_PI) * fac;
	camera.posY += (float)sin((camera.rotY + angle) / 180 * M_PI) * fac;
}

void jump() {
	jumping = true;
	upwardMomentum = 10;
}

void falling(float dT) {
	camera.posZ -= upwardMomentum*dT;
	upwardMomentum -= gravity*dT;

	if (camera.posZ > -4) {
		upwardMomentum = 0;
		jumping = false;
	}
}

void idle()
{
	float frameTime = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	float deltaTime = frameTime - lastFrameTime;
	lastFrameTime = frameTime;

	const float speed = 10;
	if (keys['a']) move(0, deltaTime*speed);
	if (keys['d']) move(180, deltaTime*speed);
	if (keys['w']) move(90, deltaTime*speed);
	if (keys['s']) move(270, deltaTime*speed);


	if (camera.posZ >= -4)
		if (keys[' ']) jump();
	if (jumping)
		falling(deltaTime);
	rotate++;

	glutPostRedisplay();
}

void mousePassiveMotion(int x, int y)
{
	int dx = x - width / 2;
	int dy = y - height / 2;
	if ((dx != 0 || dy != 0) && abs(dx) < 400 && abs(dy) < 400)
	{
		camera.rotY += dx / 10.0f;
		camera.rotX += dy / 10.0f;
		glutWarpPointer(width / 2, height / 2);
	}
}

void keyboard(unsigned char key, int, int)
{
	if (key == 27)
		exit(0);
	keys[key] = true;
}

void keyboardUp(unsigned char key, int, int)
{
	keys[key] = false;
}

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height, bbp;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		//image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);

		image = stbi_load(faces[i], &width, &height, &bbp, 3);
		if (image == nullptr)
			throw("Failed to load texture");

		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
		);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void initSkybox() {
	std::vector<const GLchar*> faces;
	faces.push_back("skybox/right.jpg");
	faces.push_back("skybox/left.jpg");
	faces.push_back("skybox/top.jpg");
	faces.push_back("skybox/bottom.jpg");
	faces.push_back("skybox/back.jpg");
	faces.push_back("skybox/front.jpg");
	cubemapTexture = loadCubemap(faces);
}


void initLighting() {
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
}

void init() {
	//arr3D = new int**[X3d];
	//for (int i = 0; i < X3d; i++) {
	//	arr3D[i] = new int*[Y3d];
	//	for (int j = 0; j < Y3d; j++) {
	//		arr3D[i][j] = new int[Z3d];
	//		for (int k = 0; k < Z3d; k++) {
	//			if (k == 15)
	//				arr3D[i][j][k] = 2;
	//			else if (k <= 15)
	//				arr3D[i][j][k] = 1;
	//			else
	//				arr3D[i][j][k] = -1;
	//		}
	//	}
	//}

	//arr3D = new int**[X3d];
	//for (int i = 0; i < X3d; i++) {
	//	arr3D[i] = new int*[Y3d];
	//	for (int j = 0; j < Y3d; j++) {
	//		arr3D[i][j] = new int[Z3d];
	//		float perlin = stb_perlin_noise3(1/i, 1/j, 1);
	//		for (int k = 0; k < perlin; k++) {
	//				arr3D[i][j][k] = 2;
	//		}
	//	}
	//}	

	glewExperimental = GL_TRUE;
	glewInit();

	//ourShader = new Shader("Shaders/default.vs", "Shaders/default.frag");
	skyboxShader = new Shader("Shaders/skybox.vs", "Shaders/skybox.frag");

	//// Set up vertex data (and buffer(s)) and attribute pointers
	//GLfloat vertices[] = {
	//	// Positions         // Colors
	//	0.5f, -0.5f, 0.0f,   1.0f, 0.0f, 0.0f,  // Bottom Right
	//	-0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,  // Bottom Left
	//	0.0f,  0.5f, 0.0f,   0.0f, 0.0f, 1.0f   // Top 
	//};

	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);
	//// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	//glBindVertexArray(VAO);

	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//// Position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	//glEnableVertexAttribArray(0);
	//// Color attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	//glEnableVertexAttribArray(1);

	//glBindVertexArray(0); // Unbind VAO

	initLighting();
	initParticles(0,0,-40);
	initSkybox();
	memset(keys, 0, sizeof(keys));
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_BLEND);
	printf("1");

	rose = new ObjectLoader("models/lowPolyAirplane.obj");
	printf("2");


	int img_width, img_height, bbp;
	unsigned char* imgData = stbi_load("terrain.png", &img_width, &img_height, &bbp, 4);
	if (imgData == nullptr)
		throw("Failed to load texture");
	glGenTextures(1, &textureId);
	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

int main(int argc, char* argv[])
{
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInit(&argc, argv);
	glutCreateWindow("karels zieke game");
	init();

	glutIdleFunc(idle);
	glutDisplayFunc(display);
	glutReshapeFunc([](int w, int h) { width = w; height = h; glViewport(0, 0, w, h); });
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutPassiveMotionFunc(mousePassiveMotion);
	glutWarpPointer(width / 2, height / 2);

	glutMainLoop();

	return 0;
}