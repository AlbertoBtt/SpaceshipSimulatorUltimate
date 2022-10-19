#include "glew.h"
#include "freeglut.h"
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Camera.h"
#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <irrKlang.h>
#include "main.h"

bool finish = false;
static const int NUM_RINGS =10;
glm::vec3 ringsPositions[NUM_RINGS];
bool drawRings[NUM_RINGS];

struct PlanetData {
	glm::vec3 position;
	float scale;

};


int screenW = 800;
int screenH = 800;
const float ASPECT = float(screenW) / screenH;


Core::Shader_Loader shaderLoader;
GLuint programSun;
GLuint programTex;
GLuint programSkybox;
GLuint programCrystal;
GLuint programShip;

GLuint texture;
GLuint textureplanet1;
GLuint textureplanet2;
GLuint textureplanet3;
GLuint textureplanet4;
GLuint sun;
GLuint textureAsteroid;
GLuint textureAsteroidnormal;
GLuint textureSpaceShip;
GLuint textureSpaceShipNormal;
GLuint textureRing;
GLuint texturenepturn;
GLuint texturepluton;


obj::Model shipModel;
obj::Model sphereModel;
obj::Model ringModel;
obj::Model diamondModel;
obj::Model asteroidModel;

Core::RenderContext asteroidContext;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext ringContext;
Core::RenderContext diamondContext;


glm::vec3 lightDir = glm::vec3(-1, 0, 0);
glm::vec3 lightColor(0.33f, 0.21f, 0.06f);
float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-30, 0, 0);
glm::vec3 cameraDir;
glm::vec3 cameraSide;
bool lightDirUse = false;

bool HDR = true;
float exposure = 3.0f;

glm::mat4 cameraMatrix, perspectiveMatrix;

const int SKY_FRONT = 0;
const int SKY_RIGHT = 1;
const int SKY_LEFT = 2;
const int SKY_BACK = 3;
const int SKY_UP = 4;
const int SKY_DOWN = 5;
float white[] = { 1,1,1,1 };
GLuint m_texture;

unsigned int m_vao, m_vbo;
irrklang::ISoundEngine* SoundEngine = irrklang::createIrrKlangDevice();

float old_x, old_y = -1;
float delta_x, delta_y = 0;
glm::quat rotation = glm::quat(1, 0, 0, 0);
glm::quat rotation_y = glm::normalize(glm::angleAxis(209 * 0.03f, glm::vec3(1, 0, 0)));
glm::quat rotation_x = glm::normalize(glm::angleAxis(307 * 0.03f, glm::vec3(0, 1, 0)));
float dy = 0;
float dx = 0;
int score = 0;

float getVecDistance(glm::vec3 v1, glm::vec3 v2)
{
	return (float)(sqrt((v2.x - v1.x) * (v2.x - v1.x) + (v2.y - v1.y) * (v2.y - v1.y) + (v2.z - v1.z) * (v2.z - v1.z)));
};

void collisionOnCrystal() {
	for (int i = 0; i < NUM_RINGS; i++) {
		float distance = getVecDistance(ringsPositions[i], cameraPos);
		if (distance < 1.f && drawRings[i]) {
			drawRings[i] = false;
			score++;
		}
	}
}

void reset() {
	for (int i = 0; i < NUM_RINGS; i++)
	{

		ringsPositions[i] = glm::ballRand(11.0 + float(rand() % 14));
		drawRings[i] = true;
	}
	cameraPos = glm::vec3(-30, 0, 0);
	score = 0;
}

void collisionOnPlanets(std::vector<PlanetData> planetPositions) {
	for (int i = 0; i < planetPositions.size(); i++) {
		float distance = getVecDistance(planetPositions[i].position, cameraPos);
		if (distance < 1.6f * planetPositions[i].scale) {
			reset();
		}
	}
}

void collisionOnAsteroids(std::vector<glm::vec3> asteroidPositions) {
	for (int i = 0; i < asteroidPositions.size(); i++) {
		float distance = getVecDistance(asteroidPositions[i], cameraPos);
		if (distance < 1.f ) {
			reset();


		}
	}
}

void collisionOnSun() {
		float distance = getVecDistance(glm::vec3(0,0,0), cameraPos);
		if (distance < 7.f) {
			reset();
		}
	}

float vertices[] = {
	//                    vertex1            vertex2            vertex3            vertex4
	/* xpos */		+1, -1, +1,			+1, +1, +1,		+1, +1, -1,		+1, -1, -1,
	/* xneg */		-1, -1, -1,			-1, +1, -1,		-1, +1, +1,		-1, -1, +1,
	/* ypos */		-1, +1, -1,			+1, +1, -1,		+1, +1, +1,		-1, +1, +1,
	/* yneg */		-1, -1, +1,			+1, -1, +1,		+1, -1, -1,		-1, -1, -1,
	/* zpos */		+1, -1, +1,			-1, -1, +1,		-1, +1, +1,		+1, +1, +1,
	/* zneg */		-1, -1, -1,			+1, -1, -1,		+1, +1, -1,		-1, +1, -1,
};

void mouse(int x, int y)
{
	if (old_x >= 0) {
		delta_x = x - old_x;
		delta_y = y - old_y;
	}
	old_x = x;
	old_y = y;
}

void drawText(std::string text, void* font, glm::vec3 color, int x, int y)
{
	glColor3f(color.x, color.y, color.z);
	glPushMatrix();
	glRasterPos2i(x, y);
	for (std::string::iterator i = text.begin(); i != text.end(); ++i)
	{
		char c = *i;
		glutBitmapCharacter(font, c);
	}
	glPopMatrix();
}

void drawScreenText()
{
	drawText("Crystals Collected : " + std::to_string(score) + "/" + std::to_string(NUM_RINGS), GLUT_BITMAP_TIMES_ROMAN_24, glm::vec3{ 0.3, 0.6, 71.516 }, 50, 50);
	drawText("Ship Position X : " + std::to_string(cameraPos.x), GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1.0, .0, 0.0 }, 50, 100);
	drawText("Ship Position Y : " + std::to_string(cameraPos.y), GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1.0, 1.0, 0.0 }, 50, 125);
	drawText("Ship Position Z : " + std::to_string(cameraPos.z), GLUT_BITMAP_HELVETICA_12, glm::vec3{ 0.0, 1.0, 1.0 }, 50, 150);
	drawText("Rotation X : " + std::to_string(cameraDir.x), GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1.0, 1.0, 1.0 }, 50, 200);
	drawText("Rotation Y : " + std::to_string(cameraDir.y), GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1.0, 1.0, 1.0 }, 50, 225);
	drawText("Rotation Z : " + std::to_string(cameraDir.z), GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1.0, 1.0, 1.0 }, 50, 250);
}

void winGame() {
	if (score == 10) {
		drawText("CONGRATULATIONS , YOU WIN . PRESS R TO RESTART ", GLUT_BITMAP_TIMES_ROMAN_24, glm::vec3{ 1, 1, 0 }, 100, 400);
		finish = true;
	}
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void initSkybox()
{
	std::vector<std::string> textures{ 
		"textures/bkg1_right.png",
		"textures/bkg1_left.png",
		"textures/bkg1_top.png",
		"textures/bkg1_bot.png",
		"textures/bkg1_front.png",
		"textures/bkg1_back.png"
	};
	m_texture = loadCubemap(textures);
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)(sizeof(float) * 0));
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}
void drawSkybox() {
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glDepthMask(GL_FALSE);

	glUseProgram(programSkybox);
	glm::mat4 transformation =perspectiveMatrix*cameraMatrix*glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0.f, -0.25f, 0));
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "Transformation"), 1, false, (float*)&transformation);

	glUniform1i(glGetUniformLocation(programSkybox, "skybox"), 0);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);

	glBindVertexArray(m_vao);
	glDrawArrays(GL_QUADS, 0, 24);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glUseProgram(0);

	glDepthMask(GL_TRUE);
}

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.5f;
	float moveSpeed = 0.3f;
	switch (key)
	{
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'h': if (HDR) HDR = false; else HDR = true; break;
	case 'f': if (lightDirUse) lightDirUse = false; else lightDirUse = true; break;
	case 'm': if(exposure<10) exposure += 1.0; break;
	case 'n': if(exposure>1) exposure -= 1.0; break;
	case 'r': if (finish) {
					reset();
					finish = false;
				} 
			break;
	}
}

glm::mat4 createCameraMatrix()
{
	glm::quat xrotation = glm::quat(glm::angleAxis(delta_x*0.01f, glm::vec3(0, 1, 0)));
	glm::quat yrotation = glm::quat(glm::angleAxis(delta_y * 0.01f, glm::vec3(1, 0, 0)));
	glm::quat rotationChange = xrotation * yrotation;
	delta_x = 0;
	delta_y = 0;
	rotation = rotationChange * rotation;
	rotation = glm::normalize(rotation);
	cameraDir = glm::inverse(rotation) * glm::vec3(0, 0, -1);
	glm::vec3 up = glm::inverse(rotation) * glm::vec3(0, 1, 0);
	cameraSide = glm::cross(cameraDir, up);

	return Core::createViewMatrixQuat(cameraPos, rotation);

}

void drawCrystal(Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color, GLuint program )
{
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);
	glUniform3f(glGetUniformLocation(program, "lightDir"), cameraDir.x,cameraDir.y,cameraDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform1i(glGetUniformLocation(programSkybox, "skybox"), 0);
	glUniform1i(glGetUniformLocation(program, "lightDirUse"), lightDirUse);
	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	Core::DrawContext(context);


	glUseProgram(0);
}

void drawObjectTexture(Core::RenderContext context, glm::mat4 modelMatrix, GLuint texture, GLuint program)
{
	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), cameraDir.x, cameraDir.y, cameraDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(program, "colorTexture"), 1, 0, 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniform1i(glGetUniformLocation(program, "HDR"), HDR);
	glUniform1f(glGetUniformLocation(program, "exposure"), exposure);

	Core::SetActiveTexture(texture,"colorTexture",program,0);

	Core::DrawContext(context);


	glUseProgram(0);
}

void drawObjectTextureNormal(Core::RenderContext context, glm::mat4 modelMatrix, GLuint texture, GLuint program, GLuint normalmapId)
{
	glUseProgram(program);

	glUniform3f(glGetUniformLocation(program, "lightDir"), cameraDir.x, cameraDir.y, cameraDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(program, "colorTexture"), 1, 0, 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform1i(glGetUniformLocation(program, "lightDirUse"), lightDirUse);
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::SetActiveTexture(texture, "colorTexture", program, 0);
	Core::SetActiveTexture(normalmapId, "normalSampler", program, 1);

	Core::DrawContext(context);


	glUseProgram(0);
}

void renderScene()
{

	std::vector<PlanetData> planetsPositions;
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.f, 0.f, 1.0f);
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	drawSkybox();

	glm::mat4 rotationmat = glm::mat4_cast(rotation);
	rotationmat = glm::inverse(rotationmat);
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f )*rotationmat * glm::translate(glm::vec3(0, -0.25f, 0)) *glm::rotate( glm::radians(180.f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.001f));

	drawObjectTextureNormal(shipContext, shipModelMatrix, textureSpaceShip, programShip, textureSpaceShipNormal);
	

	//planet1
	glm::mat4 planet1rotation = glm::rotate((time / 50.f) * 2 * 3.14f, glm::vec3(0, -2, 0));
	glm::mat4 planet1translation = glm::translate(glm::vec3(-20, 0, -20));
	glm::mat4 scaling = glm::scale(glm::vec3(1.f, 1.f, 1.f));
	glm::mat4 planet1transformation = planet1rotation * planet1translation * scaling;
	drawObjectTextureNormal(sphereContext, planet1transformation, texture, programTex, textureAsteroidnormal);
	PlanetData planet1;
	planet1.position = planet1transformation[3];
	planet1.scale = 1.f;
	planetsPositions.push_back(planet1);

	//planet2
	glm::mat4 planet2rotation = glm::rotate((time / 30.f) * 2 * 3.14f, glm::vec3(0, 2, 0));
	glm::mat4 planet2translation = glm::translate(glm::vec3(-10, 0, 10));
	glm::mat4 planet2transformation = planet2rotation * planet2translation * glm::scale(glm::vec3(1.f, 1.f, 1.f));
	drawObjectTexture(sphereContext, planet2transformation, texturenepturn, programTex);
	PlanetData planet2;
	planet2.position = planet2transformation[3];
	planet2.scale = 1.f;
	planetsPositions.push_back(planet2);

	//planet3
	glm::mat4 thirdplanetrotate = glm::rotate((time / 20.f) * 2 * 3.14f, glm::vec3(0, -2, 0));
	glm::mat4 thirdplanettranslation = glm::translate(glm::vec3(17, 0, -17));
	glm::mat4 thirdplanettransformation = thirdplanetrotate * thirdplanettranslation * glm::scale(glm::vec3(1, 1, 1));
	drawObjectTextureNormal(sphereContext, thirdplanettransformation, textureplanet1, programTex, textureAsteroidnormal);
	PlanetData planet3;
	planet3.position = thirdplanettransformation[3];
	planet3.scale = 1.f;
	planetsPositions.push_back(planet3);

	//planet4
	glm::mat4 fourthplanetrotate = glm::rotate((time / 35.f) * 2 * 3.14f, glm::vec3(0, 2, 0));
	glm::mat4 fourthplanettranslation = glm::translate(glm::vec3(-25, 0, -25));
	glm::mat4 fourthplanettransformation = fourthplanetrotate * fourthplanettranslation * glm::scale(glm::vec3(1.f, 1.f, 1.f));
	drawObjectTextureNormal(sphereContext, fourthplanettransformation, textureplanet2, programTex, textureAsteroidnormal);
	PlanetData planet4;
	planet4.position = fourthplanettransformation[3];
	planet4.scale = 1.f;
	planetsPositions.push_back(planet4);


	//Planet5
	glm::mat4 planet7rota = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(0, -2, 9));
	glm::mat4 planet7trans = glm::translate(glm::vec3(-20, 9, 6));
	glm::mat4 planet7transformation = planet7rota * planet7trans * glm::scale(glm::vec3(1.f, 1.f, 1.f));
	drawObjectTexture(sphereContext, planet7transformation, textureplanet3, programTex);
	PlanetData planet7;
	planet7.position = planet7transformation[3];
	planet7.scale = 1.f;
	planetsPositions.push_back(planet7);

	//Planet6
	glm::mat4 planet9rota = glm::rotate((time / 5.f) * 2 * 3.14f, glm::vec3(0, -2, -13));
	glm::mat4 planet9trans = glm::translate(glm::vec3(-20, -13, 6));
	glm::mat4 planet9transformation = planet9rota * planet9trans * glm::scale(glm::vec3(1.f, 1.f, 1.f));
	drawObjectTexture(sphereContext, planet9transformation, textureplanet4, programTex);
	PlanetData planet9;
	planet9.position = planet9transformation[3];
	planet9.scale = 1.f;
	planetsPositions.push_back(planet9);


	//Planet7
	glm::mat4 eightplanetrotate = glm::rotate((time / 12.6f) * 2 * 3.14f, glm::vec3(-1, -2, 0));
	glm::mat4 eightplanettranslation = glm::translate(glm::vec3(12, 0, -6));
	glm::mat4 eightplanettransformation = eightplanetrotate * eightplanettranslation * glm::scale(glm::vec3(1.f, 1.f, 1.f));
	drawObjectTextureNormal(sphereContext, eightplanettransformation, texturepluton, programTex, textureAsteroidnormal);
	PlanetData planet8;
	planet8.position = eightplanettransformation[3];
	planet8.scale = 1.f;
	planetsPositions.push_back(planet8);


	//asteroids

	std::vector<glm::vec3> asteroidsPosition;

	glm::mat4 rotateAsteroid1 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(14,1,12));
	glm::mat4 rotateAsteroid2 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(-9, 0, 12));
	glm::mat4 rotateAsteroid3 = glm::rotate((time / 9.f) * 2 * 3.14f, glm::vec3(7, 1,3 ));
	glm::mat4 rotateAsteroid4 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(-10, 1, -6));
	glm::mat4 rotateAsteroid5 = glm::rotate((time / 8.f) * 2 * 3.14f, glm::vec3(11, -2, 4));
	glm::mat4 rotateAsteroid6 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(-8, 3, 12));

	glm::mat4 rotateAsteroid7 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(-3,0,-15));
	glm::mat4 rotateAsteroid8 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(-4, 0, -10));
	glm::mat4 rotateAsteroid9 = glm::rotate((time / 9.f) * 2 * 3.14f, glm::vec3(-9, 1, -15));
	glm::mat4 rotateAsteroid10 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(-16, -4, 7));
	glm::mat4 rotateAsteroid11 = glm::rotate((time / 8.f) * 2 * 3.14f, glm::vec3(14, -6, 5));
	glm::mat4 rotateAsteroid12 = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(-8, -6, 5));



	drawObjectTextureNormal(asteroidContext, rotateAsteroid1 *glm::translate(glm::vec3(14,1,12)) * glm::scale(glm::vec3(0.01,0.01,0.01)), textureAsteroid, programTex,textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(14, 1, 12));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid2* glm::translate(glm::vec3(-9, 0, 12))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-9, 0, 12));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid3* glm::translate(glm::vec3(7, 1, 3))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(7, 1, 3));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid4* glm::translate(glm::vec3(-10, 1, -6))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-10, 1, -6));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid5* glm::translate(glm::vec3(11, -2, 4))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(11, -2, 4));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid6* glm::translate(glm::vec3(-8, 3, 12))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-8, 3, 12));

	drawObjectTextureNormal(asteroidContext, rotateAsteroid7* glm::translate(glm::vec3(-3, 0, -15))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-3, 0, -15));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid8* glm::translate(glm::vec3(-4, 0, -10))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-4, 0, -10));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid9* glm::translate(glm::vec3(-9, 1, -15))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-9, 1, -15));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid10* glm::translate(glm::vec3(-16, -4, 7))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-16, -4, 7));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid11* glm::translate(glm::vec3(14, -6, 5))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(14, -6, 5));
	drawObjectTextureNormal(asteroidContext, rotateAsteroid12* glm::translate(glm::vec3(-8, -6, 5))* glm::scale(glm::vec3(0.01, 0.01, 0.01)), textureAsteroid, programTex, textureAsteroidnormal);
	asteroidsPosition.push_back(glm::vec3(-8, -6, 5));

	// Sun
	drawObjectTexture(sphereContext, (glm::translate(glm::vec3(0, 0, 0)) * glm::scale(glm::vec3(6, 6, 6))), sun, programSun);

	//Rings
	glm::mat4 ringtransformation = glm::scale(glm::vec3(0.7, 0.7, 0.7)) * glm::rotate(1.5708f, glm::vec3(0.f, 0.f, 1.f));

	//diamonds
	glm::mat4 diamondtransformation = glm::scale(glm::vec3(0.2, 0.2, 0.2));
	
	for (int i = 0; i < NUM_RINGS; i++) {
		glm::mat4 diamondrotate = glm::rotate((time / 1.f) * 2 * 3.14f, glm::vec3(ringsPositions[i]));
		drawCrystal(diamondContext, diamondrotate * glm::translate(glm::vec3(ringsPositions[i].x, ringsPositions[i].y - 0.1, ringsPositions[i].z)) * diamondtransformation, glm::vec3(0, 1, 0), programCrystal);

		if (drawRings[i]) {
			glm::mat4 ringrotate = glm::rotate((time / 2.f) * 2 * 3.14f, glm::vec3(ringsPositions[i]));
			
			drawObjectTexture(ringContext, ringrotate * glm::translate(ringsPositions[i]) * ringtransformation, textureRing, programTex);
			
		}
	}

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, screenW, screenH, 0.0, 0.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	drawScreenText();
	if (cameraPos.x == -30 && cameraPos.y == 0 && cameraPos.z == 0) {
		drawText("MISSION : COLLECT ALL THE CRYSTALS", GLUT_BITMAP_HELVETICA_18, glm::vec3{ 0, 1, 0 }, 220, 400);
		drawText("PRESS W/S: TO MOVE THE SPACESHIP", GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1, 0, 0 }, 90, 480);
		drawText("PRESS F: TO TURN ON/OFF SPACESHIP'S LIGHTS", GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1, 0, 0 }, 90, 520);
		drawText("PRESS H: TO TURN ON/OFF THE HDR EFFECT", GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1, 0, 0 }, 90, 560);
		drawText("PRESS M : TO INCREASE GAMMA FROM THE SUN (HDR MUST BE ACTIVATED)", GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1, 0, 0 }, 90, 600);
		drawText("PRESS N : TO DECREASE GAMMA FROM THE SUN (HDR MUST BE ACTIVATED)", GLUT_BITMAP_HELVETICA_12, glm::vec3{ 1, 0, 0 }, 90, 640);

	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	collisionOnCrystal();
	collisionOnPlanets(planetsPositions);
	collisionOnSun();
	collisionOnAsteroids(asteroidsPosition);
	winGame();
	glutSwapBuffers();
}

void init()
{
	SoundEngine->play2D("audio/music.mp3", true);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	programSun = shaderLoader.CreateProgram("shaders/shader_4_sun.vert", "shaders/shader_4_sun.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_4_tex.vert", "shaders/shader_4_tex.frag");
	programShip = shaderLoader.CreateProgram("shaders/shader_ship.vert", "shaders/shader_ship.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	programCrystal = shaderLoader.CreateProgram("shaders/shader_crystal.vert", "shaders/shader_crystal.frag");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/nave_orion.obj");
	ringModel = obj::loadModelFromFile("models/cilinder.obj");
	diamondModel = obj::loadModelFromFile("models/diamond.obj");
	asteroidModel = obj::loadModelFromFile("models/asteroid.obj");

	asteroidContext.initFromOBJ(asteroidModel);
	shipContext.initFromOBJ(shipModel);
	sphereContext.initFromOBJ(sphereModel);
	ringContext.initFromOBJ(ringModel);
	diamondContext.initFromOBJ(diamondModel);

	textureAsteroid = Core::LoadTexture("textures/asteroid.png");
	textureAsteroidnormal = Core::LoadTexture("textures/asteroid_normals.png");
	textureSpaceShip = Core::LoadTexture("textures/nave2.png");
	textureSpaceShipNormal = Core::LoadTexture("textures/nave2n.png");
	textureRing = Core::LoadTexture("textures/anillo.png");

	texture = Core::LoadTexture("textures/earth2.png");
	textureplanet1 = Core::LoadTexture("textures/planet1texture.png");
	textureplanet2 = Core::LoadTexture("textures/planet2texture.png");
	textureplanet3 = Core::LoadTexture("textures/planet3texture.png");
	textureplanet4 = Core::LoadTexture("textures/planet4texture.png");
	sun = Core::LoadTexture("textures/sun.png");
	texturenepturn = Core::LoadTexture("textures/nepturn.png");
	texturepluton = Core::LoadTexture("textures/pluton.png");

	for (int i = 0; i < NUM_RINGS; i++)
	{
		glm::vec3 position = glm::ballRand(10.f + float(rand() % 18));
		if (position.x > 0) position.x += 8;
		else position.x -= 8;
		ringsPositions[i] =  position;
		drawRings[i] = true;
	}

	initSkybox();

	cameraAngle = 0;
	cameraDir = glm::vec3(0, 0, 0);
	
}

void shutdown()
{
	shaderLoader.DeleteProgram(programTex);
	shaderLoader.DeleteProgram(programSun);
	shaderLoader.DeleteProgram(programCrystal);
	shaderLoader.DeleteProgram(programSkybox);
	shaderLoader.DeleteProgram(programShip);
}

void idle()
{
	glutPostRedisplay();
}

void reshapeScene(GLint width, GLint height)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	int w = height * ASPECT;
	int left = (width - w) / 2;
	glViewport(left, 0, w, height);
	gluOrtho2D(0, screenW, screenH, 0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	srand((unsigned)time(NULL));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	gluPerspective(55.0f, screenW/screenH, 1.0, 100000.0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);

	glutCreateWindow("Spaceship Ultimate");
	glewInit();
	
	init();
	glutKeyboardFunc(keyboard);
	glutPassiveMotionFunc(mouse);
	glutDisplayFunc(renderScene);
	glutReshapeFunc(reshapeScene);
	glutIdleFunc(idle);
	glutMainLoop();

	shutdown();

	return 0;

}