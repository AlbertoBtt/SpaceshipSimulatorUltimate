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

#include "Box.cpp"

GLuint program;
Core::Shader_Loader shaderLoader;
GLuint programSun;
GLuint programTex;
GLuint programProcTex;

GLuint texture;
GLuint textureMercury;
//glm::vec3 colorTexture = glm::vec3(1, 0, 0);
obj::Model shipModel;
obj::Model sphereModel;
obj::Model naveTocha;
Core::RenderContext shipContext;
Core::RenderContext sphereContext;
glm::vec3 lightDir = glm::vec3(1, 0, 0);
glm::vec3 lightColor(0.33f, 0.21f, 0.06f);
float cameraAngle = 0;
glm::vec3 cameraPos = glm::vec3(-5, 0, 0);
glm::vec3 cameraDir;

glm::mat4 cameraMatrix, perspectiveMatrix;

void keyboard(unsigned char key, int x, int y)
{
	float angleSpeed = 0.1f;
	float moveSpeed = 0.1f;
	switch (key)
	{
	case 'z': cameraAngle -= angleSpeed; break;
	case 'x': cameraAngle += angleSpeed; break;
	case 'w': cameraPos += cameraDir * moveSpeed; break;
	case 's': cameraPos -= cameraDir * moveSpeed; break;
	case 'd': cameraPos += glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'a': cameraPos -= glm::cross(cameraDir, glm::vec3(0, 1, 0)) * moveSpeed; break;
	case 'e': cameraPos += glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	case 'q': cameraPos -= glm::cross(cameraDir, glm::vec3(1, 0, 0)) * moveSpeed; break;
	}
}

glm::mat4 createCameraMatrix()
{
	// Obliczanie kierunku patrzenia kamery (w plaszczyznie x-z) przy uzyciu zmiennej cameraAngle kontrolowanej przez klawisze.
	cameraDir = glm::vec3(cosf(cameraAngle), 0.0f, sinf(cameraAngle));
	glm::vec3 up = glm::vec3(0, 1, 0);
	
	return Core::createViewMatrix(cameraPos, cameraDir, up);
}

void drawObject(Core::RenderContext context, glm::mat4 modelMatrix, glm::vec3 color, GLuint program )
{
	glUseProgram(program);
	
	glUniform3f(glGetUniformLocation(program, "objectColor"), color.x, color.y, color.z);


	
	glUniform3f(glGetUniformLocation(program, "lightDir"),lightDir.x,lightDir.y,lightDir.z);
	

	
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);

	//glUseProgram(program);
	//glUniform3f(glGetUniformLocation(program, "modelmatrix"),modelMatrix[1][2], modelMatrix[1][3], modelMatrix[2][1]);


	
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);


	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	
	

	Core::DrawContext(context);


	glUseProgram(0);
}

void drawObjectTexture(Core::RenderContext context, glm::mat4 modelMatrix, GLuint texture, GLuint program)
{
	glUseProgram(program);

	//glUniform3f(glGetUniformLocation(program, "objectColor"),1,0,0);
	glUniform3f(glGetUniformLocation(program, "lightDir"), lightDir.x, lightDir.y, lightDir.z);
	glUniform3f(glGetUniformLocation(program, "lightPos"), 0, 0, 0);
	//glUseProgram(program);
	//glUniform3f(glGetUniformLocation(program, "modelmatrix"),modelMatrix[1][2], modelMatrix[1][3], modelMatrix[2][1]);
	glUniform3f(glGetUniformLocation(program, "colorTexture"), 1, 0, 0);
	glUseProgram(program);
	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);
	glUseProgram(program);
	glm::mat4 transformation = perspectiveMatrix * cameraMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	
	Core::SetActiveTexture(texture,"colorTexture",programTex,0);
	

	Core::DrawContext(context);


	glUseProgram(0);
}

void renderScene()
{
	// Aktualizacja macierzy widoku i rzutowania. Macierze sa przechowywane w zmiennych globalnych, bo uzywa ich funkcja drawObject.
	// (Bardziej elegancko byloby przekazac je jako argumenty do funkcji, ale robimy tak dla uproszczenia kodu.
	//  Jest to mozliwe dzieki temu, ze macierze widoku i rzutowania sa takie same dla wszystkich obiektow!)

	
	cameraMatrix = createCameraMatrix();
	perspectiveMatrix = Core::createPerspectiveMatrix();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.3f, 0.3f, 1.0f);
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	// Macierz statku "przyczepia" go do kamery. Warto przeanalizowac te linijke i zrozumiec jak to dziala.
	glm::mat4 shipModelMatrix = glm::translate(cameraPos + cameraDir * 0.5f + glm::vec3(0.f, -0.25f, 0)) * glm::rotate(-cameraAngle + glm::radians(90.0f), glm::vec3(0, 1, 0)) * glm::scale(glm::vec3(0.001f));

	drawObject(shipContext, shipModelMatrix, glm::vec3(0.6f), program);






	//first planet
	glm::mat4 firstboxrotation = glm::rotate((time / 34.f) * 2 * 3.14f, glm::vec3(0,-2, 0));
	glm::mat4 firstboxtranslation = glm::translate(glm::vec3(2, 0, -2));
	glm::mat4 scaling =glm::scale(glm::vec3(0.1f, 0.1f, 0.1f));
	glm::mat4 firstboxtransformation =  firstboxrotation * firstboxtranslation * scaling;
	drawObject(sphereContext, firstboxtransformation, glm::vec3(1.0f, 0.5f, 0.2f),program);

	//second planet
	glm::mat4 marterota = glm::rotate((time / 200.f) * 2 * 3.14f, glm::vec3(0, -2, 0));
	glm::mat4 martetrans = glm::translate(glm::vec3(2, 0, -6));
	glm::mat4 martetransformation = marterota * martetrans * glm::scale(glm::vec3(0.2,0.2,0.2)) ;
	drawObject(sphereContext, martetransformation, glm::vec3(0.0f, 0.6f, 0.9f), programProcTex);

	//third planet
	glm::mat4 thirdplanetrotate = glm::rotate((time / 15.f) * 2 * 3.14f, glm::vec3(0, -2, 0));
	glm::mat4 thirdplanettranslation = glm::translate(glm::vec3(2, 0, -5));
	glm::mat4 thirdplanettransformation = thirdplanetrotate * thirdplanettranslation * glm::scale(glm::vec3(0.3, 0.3, 0.3));
	drawObjectTexture(sphereContext, thirdplanettransformation, textureMercury, programTex);


	//fourth planet
	glm::mat4 fourthplanetrotate = glm::rotate((time / 10.f) * 2 * 3.14f, glm::vec3(0, -2, 0));
	glm::mat4 fourthplanettranslation = glm::translate(glm::vec3(2, 0, -1.5));
	glm::mat4 fourthplanettransformation = fourthplanetrotate * fourthplanettranslation * glm::scale(glm::vec3(0.15, 0.15, 0.15));
	//drawObject(sphereContext, fourthplanettransformation, glm::vec3(1.0f, 0.6f, 0.9f), program);
	drawObjectTexture(sphereContext, fourthplanettransformation, texture, programTex);

	//first moon 
	glm::mat4 firstmoonrotate = glm::rotate((time / 20.f) * 2 * 3.14f, glm::vec3(2, 0.5, -1.5));
	glm::mat4 firstmoonsecondrotate = fourthplanetrotate * glm::rotate((time / 5.f) * 2 * 3.14f, glm::vec3(2, 0, -1.5));
	glm::mat4 firstmoontranslation = glm::translate(glm::vec3(2, 0.5, -1.5));
	glm::mat4 firstmoontransformation =  firstmoonsecondrotate *firstmoontranslation * firstmoonrotate *  glm::scale(glm::vec3(0.05, 0.05, 0.05));
	drawObject(sphereContext, firstmoontransformation, glm::vec3(1.f, 0.f, 0.f), program);

	//second  moon 
	glm::mat4 secondmoonrotate = glm::rotate((time / 5.f) * 2 * 3.14f, glm::vec3(2, 0.5, -1.5));
	glm::mat4 secondmoonsecondrotate = fourthplanetrotate * glm::rotate((time / 3.f) * 2 * 3.14f  , glm::vec3(2, 0, -1.5));
	glm::mat4 secondmoontranslation = glm::translate(glm::vec3(2, 0.70, -1.5));
	glm::mat4 secondmoontransformation = secondmoonsecondrotate * secondmoontranslation * secondmoonrotate * glm::scale(glm::vec3(0.03, 0.03, 0.03));
	drawObject(sphereContext, secondmoontransformation, glm::vec3(1.f, 0.f, 0.f),programProcTex);
	// Sun
	drawObject(sphereContext, glm::translate(glm::vec3(0,0,0)), glm::vec3(1.0f, 0.8f, 0.f),programSun); 



	glutSwapBuffers();
}

void init()
{
	glEnable(GL_DEPTH_TEST);

	program = shaderLoader.CreateProgram("shaders/shader_4_1.vert", "shaders/shader_4_1.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_4_sun.vert", "shaders/shader_4_sun.frag");
	programTex = shaderLoader.CreateProgram("shaders/shader_4_tex.vert", "shaders/shader_4_tex.frag");
	programProcTex = shaderLoader.CreateProgram("shaders/shader_proc_tex.vert", "shaders/shader_proc_tex.frag");
	sphereModel = obj::loadModelFromFile("models/sphere.obj");
	shipModel = obj::loadModelFromFile("models/nave_orion.obj");
	shipContext.initFromOBJ(shipModel);
	sphereContext.initFromOBJ(sphereModel);
	texture = Core::LoadTexture("textures/earth2.png");
	textureMercury = Core::LoadTexture("textures/grid.png");

}

void shutdown()
{
	shaderLoader.DeleteProgram(program);
	shaderLoader.DeleteProgram(programSun);
}

void idle()
{
	glutPostRedisplay();
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(200, 200);
	glutInitWindowSize(600, 600);
	glutCreateWindow("OpenGL Pierwszy Program");
	glewInit();

	init();
	glutKeyboardFunc(keyboard);
	glutDisplayFunc(renderScene);
	glutIdleFunc(idle);

	glutMainLoop();

	shutdown();

	return 0;
}
