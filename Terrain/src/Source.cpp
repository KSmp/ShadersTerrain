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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);
//unsigned int loadTexture2(char const * path);
void setVAO(vector <float> vertices);
void setFBOcolour();
unsigned int createQuad();

// camera
Camera camera(glm::vec3(260,50,300));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

//arrays
unsigned int terrainVAO;
unsigned int waterVAO;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// flags
bool stepTessOn = true;
bool linearTessOn = false;
bool expoTessOn = false;

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
	Shader shader("..\\shaders\\plainVert.vs", "..\\shaders\\plainFrag.fs", "..\\shaders\\tessControlShader.tcs", "..\\shaders\\tessEvaluationShader.tes");
	Shader waterShader("..\\shaders\\waterVert.vs", "..\\shaders\\waterFrag.fs");
	Shader fboShader("..\\shaders\\fboVert.vs", "..\\shaders\\fboFrag.fs");

	unsigned int heightMap = loadTexture("..\\resources\\heightMap.jpg");
	unsigned int rockTexture = loadTexture("..\\resources\\rock\\diffuse.jpg");
	unsigned int mossTexture = loadTexture("..\\resources\\moss\\diffuse.jpg");

	//Terrain Constructor ; number of grids in width, number of grids in height, gridSize
	Terrain terrain(50, 50, 10);
	terrainVAO = terrain.getVAO();

	Terrain water(50, 50, 10);
	waterVAO = water.getVAO();

	glm::vec4 skyColour = glm::vec4(0.53, 0.81, 0.92, 1.0);

	glClearColor(skyColour.r, skyColour.g, skyColour.b, skyColour.a);
	glEnable(GL_CLIP_DISTANCE0);

	FrameBuffer refraction(50, 50);
	FrameBuffer reflection(50, 50);

	while (!glfwWindowShouldClose(window))
	{

		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		processInput(window);

		glBindFramebuffer(GL_FRAMEBUFFER, refraction.getBuffer());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_DEPTH_TEST);
		
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1200.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);
	    shader.use();
	    shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setMat4("model", model);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec4("skyColour", skyColour);

		// textures
		shader.setInt("heightMap", 0);
		shader.setInt("rockTexture", 1);
		shader.setInt("mossTexture", 2);

		// flags
		shader.setBool("stepTessOn", stepTessOn);
		shader.setBool("linearTessOn", linearTessOn);
		shader.setBool("expoTessOn", expoTessOn);


		shader.setFloat("scale", 75.f);
		shader.setVec4("plane", glm::vec4(0.0, -1.0, 0.0, 0.0));
	
		glBindVertexArray(terrainVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, heightMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, rockTexture);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, mossTexture);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDrawArrays(GL_PATCHES, 0, terrain.getSize());

		waterShader.use();
		waterShader.setMat4("projection", projection);
		waterShader.setMat4("view", view);
		glBindVertexArray(waterVAO);
		glDrawArrays(GL_TRIANGLES, 0, water.getSize());
		//glDrawElements(GL_PATCHES, terrain.getSize(), GL_UNSIGNED_INT, 0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);

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
	if (glfwGetKey(window, GLFW_KEY_KP_1) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
	{
		linearTessOn = false;
		expoTessOn = false;

		stepTessOn = !stepTessOn;
		std::cout << "STEP TESS toggled! " << stepTessOn << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_2) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
	{
		stepTessOn = false;
		expoTessOn = false;

		linearTessOn = !linearTessOn;
		std::cout << "LINEAR TESS toggled! " << linearTessOn << std::endl;
	}

	if (glfwGetKey(window, GLFW_KEY_KP_3) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
	{
		stepTessOn = false;
		linearTessOn = false;

		expoTessOn = !expoTessOn;
		std::cout << "EXPO TESS toggled! " << expoTessOn << std::endl;
	}
}

unsigned int createQuad() {
	unsigned int quadVAO;
	unsigned int quadVBO;

	float quadVerticies[] = {
		//pos			  // texture coords
		-1.f, 1.f, 0.f,   0.f,1.f,
		-1.f, -1.f, 0.f,  0.f,0.f,
		 1.f, 1.f, 0.f,   1.f,1.f,
		 1.f, -1.f, 0.f,  1.f,0.f
	};

	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerticies), &quadVerticies, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

	return quadVAO;
}




