#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Shader.h"
#include "Camera.h"

#include <Model.h>
#include "Terrain.h"
#include "FrameBuffer.h"

#include <string>
#include <iostream>
#include <numeric>



// settings
const unsigned int SCR_WIDTH = 1200;
const unsigned int SCR_HEIGHT = 900;
const float NEAR_PLANE = 0.1f;
const float FAR_PLANE = 1200.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
void updateMVP(Shader shader);
unsigned int loadTexture(char const * path);
//unsigned int createQuad();

// camera
Camera camera(glm::vec3(75,74,574));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int terrainVAO;
unsigned int waterVAO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "IMAT3907", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// simple vertex and fragment shader - add your own tess and geo shader
	Shader shader("..\\shaders\\plainVert.vs", "..\\shaders\\plainFrag.fs", "..\\shaders\\tessControlShader.tcs", "..\\shaders\\tessEvaluationShader.tes", "..\\shaders\\flatNanoGeo.gs");
	Shader waterShader("..\\shaders\\waterVert.vs", "..\\shaders\\waterFrag.fs");

	unsigned int heightMap = loadTexture("..\\resources\\heightMap.jpg");
	unsigned int rockTexture = loadTexture("..\\resources\\rock\\diffuse.jpg");
	unsigned int mossTexture = loadTexture("..\\resources\\moss\\diffuse.jpg");
	unsigned int dudv = loadTexture("..\\resources\\waterDUDV.png");
	unsigned int waterNormals = loadTexture("..\\resources\\waterNormals.png");

	//Terrain Constructor ; number of grids in width, number of grids in height, gridSize, y position
	Terrain terrain(50, 50, 10, 0.f);
	terrainVAO = terrain.getVAO();

	float waterLevel = 50.f;
	Terrain water(50, 50, 10, waterLevel);
	waterVAO = water.getVAO();

	glm::vec4 skyColour = glm::vec4(0.53, 0.81, 0.92, 1.0);

	glClearColor(skyColour.r, skyColour.g, skyColour.b, skyColour.a);

	FrameBuffer refraction(SCR_WIDTH, SCR_HEIGHT);
	FrameBuffer reflection(SCR_WIDTH, SCR_HEIGHT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	while (!glfwWindowShouldClose(window))
	{
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		//std::cout << camera.Position.x << " "  << camera.Position.y << " " << camera.Position.z << std::endl;

		// REFRACTION
		glEnable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, refraction.getBuffer());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	    shader.use();
		updateMVP(shader);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec4("skyColour", skyColour);
		shader.setFloat("waterLevel", waterLevel);

		// textures
		shader.setInt("heightMap", 0);
		shader.setInt("rockTexture", 1);
		shader.setInt("mossTexture", 2);

		shader.setFloat("scale", 75.f);
		shader.setVec4("plane", glm::vec4(0.0, -1.0, 0.0, waterLevel));
	
		glBindVertexArray(terrainVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rockTexture);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mossTexture);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_PATCHES, 0, terrain.getSize());

		// REFLECTION

		float distance = 2 * (camera.Position.y - waterLevel);
		//std::cout << distance << std::endl;
		camera.Position.y -= distance;
		camera.invertPitch();
		shader.setVec4("plane", glm::vec4(0.0, 1.0, 0.0, -waterLevel));

		updateMVP(shader);

		glBindFramebuffer(GL_FRAMEBUFFER, reflection.getBuffer());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDrawArrays(GL_PATCHES, 0, terrain.getSize());
		camera.Position.y += distance;
		camera.invertPitch();

		// TERRAIN AND WATER

		glDisable(GL_CLIP_DISTANCE0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		updateMVP(shader);
		glDrawArrays(GL_PATCHES, 0, terrain.getSize());

		waterShader.use();
		updateMVP(waterShader);
		waterShader.setInt("refraction", 3);
		waterShader.setInt("reflection", 4);
		waterShader.setInt("dudv", 5);
		waterShader.setInt("waterNormals", 6);
		waterShader.setInt("depthMap", 7);
		waterShader.setInt("screenW", SCR_WIDTH);
		waterShader.setInt("screenH", SCR_HEIGHT);
		waterShader.setFloat("time", currentFrame);
		waterShader.setFloat("near", NEAR_PLANE);
		waterShader.setFloat("far", FAR_PLANE);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, refraction.getTexture());

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, reflection.getTexture());
		
		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, dudv);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, waterNormals);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, refraction.getDepthMap());

		glBindVertexArray(waterVAO);
		glDrawArrays(GL_TRIANGLES, 0, water.getSize());
		//glDrawElements(GL_PATCHES, terrain.getSize(), GL_UNSIGNED_INT, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//fboShader.use();
		//fboShader.setInt("screen", 4);

		//glBindVertexArray(createQuad());

		//glActiveTexture(GL_TEXTURE4);
		//glBindTexture(GL_TEXTURE_2D, colourAttachment);
		//glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
	float speed = deltaTime * 10;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, speed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, speed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, speed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, speed);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		stbi_image_free(data);
		std::cout << "Loaded texture at path: " << path << " width " << width << " id " << textureID <<  std::endl;

	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
		
	}

	return textureID;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	//{
	//	linearTessOn = false;
	//	expoTessOn = false;

	//	stepTessOn = !stepTessOn;
	//	std::cout << "STEP TESS toggled! " << stepTessOn << std::endl;
	//}

	//if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	//{
	//	stepTessOn = false;
	//	expoTessOn = false;

	//	linearTessOn = !linearTessOn;
	//	std::cout << "LINEAR TESS toggled! " << linearTessOn << std::endl;
	//}

	//if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	//{
	//	stepTessOn = false;
	//	linearTessOn = false;

	//	expoTessOn = !expoTessOn;
	//	std::cout << "EXPO TESS toggled! " << expoTessOn << std::endl;
	//}
}

//unsigned int createQuad() {
//	unsigned int quadVAO;
//	unsigned int quadVBO;
//
//	float quadVerticies[] = {
//		//pos			  // texture coords
//		-1.f, 1.f, 0.f,   0.f,1.f,
//		-1.f, -1.f, 0.f,  0.f,0.f,
//		 1.f, 1.f, 0.f,   1.f,1.f,
//		 1.f, -1.f, 0.f,  1.f,0.f
//	};
//
//	glGenVertexArrays(1, &quadVAO);
//	glGenBuffers(1, &quadVBO);
//	glBindVertexArray(quadVAO);
//	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticies), &quadVerticies, GL_STATIC_DRAW);
//	glEnableVertexAttribArray(0);
//	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
//	glEnableVertexAttribArray(1);
//	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
//
//	return quadVAO;
//}


void updateMVP(Shader shader) {
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, NEAR_PLANE, FAR_PLANE);
	glm::mat4 view = camera.GetViewMatrix();
	glm::mat4 model = glm::mat4(1.0f);

	shader.setMat4("projection", projection);
	shader.setMat4("view", view);
	shader.setMat4("model", model);
	shader.setVec3("viewPos", camera.Position);
}

