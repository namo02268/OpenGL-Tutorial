#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "Model.h"
#include "UI_Manager.h"

#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

int main() {
	// initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// create GLFW window 
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
	stbi_set_flip_vertically_on_load(true);

	// configure global opengl state
	glEnable(GL_DEPTH_TEST);

	// build and compile shaders
	Shader lightingShader("color.vert", "color.frag");

	// load models
	std::shared_ptr<Model> Suzanne = std::make_shared <Model>("suzanne", "resources/objects/suzanne/suzanne.obj");
	std::shared_ptr<Model> cube = std::make_shared <Model>("cube", "resources/objects/cube/cube.obj");

	// lightCuve
	Shader lightCubeShader("light_cube.vert", "light_cube.frag");

	float vertices[] = {
	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,

	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,

	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,

	-0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f, -0.5f,
	 0.5f, -0.5f,  0.5f,
	 0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f,  0.5f,
	-0.5f, -0.5f, -0.5f,

	-0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f, -0.5f,
	 0.5f,  0.5f,  0.5f,
	 0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f,  0.5f,
	-0.5f,  0.5f, -0.5f,
	};

	unsigned int lightCubeVBO, lightCubeVAO;
	glGenVertexArrays(1, &lightCubeVAO);
	glGenBuffers(1, &lightCubeVBO);

	glBindBuffer(GL_ARRAY_BUFFER, lightCubeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindVertexArray(lightCubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// positions of the point lights
	auto light1Pos = glm::vec3(0.7f, 0.2f, 2.0f);
	auto light2Pos = glm::vec3(2.3f, -1.0f, -1.0f);
	auto light3Pos = glm::vec3(2.0f, 2.0f, 3.0f);

	// make UIs
	auto UIs = UIManager(window);
	auto modelUI = std::make_shared<ModelUI>(Suzanne);
	auto modelUI2 = std::make_shared<ModelUI>(cube);
	UIs.Add(modelUI);
	UIs.Add(modelUI2);

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		if (deltaTime >= 1.0f / 60.0f) {
			lastFrame = currentFrame;

			// input
			processInput(window);

			// render
			glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// enable shader
			lightingShader.use();
			lightingShader.setVec3("viewPos", camera.Position);
			lightingShader.setFloat("material.shininess", 32.0f);
			// directional light
			lightingShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
			lightingShader.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
			lightingShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
			// point light 1
			lightingShader.setVec3("pointLights[0].position", light1Pos);
			lightingShader.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[0].constant", 1.0f);
			lightingShader.setFloat("pointLights[0].linear", 0.09f);
			lightingShader.setFloat("pointLights[0].quadratic", 0.032f);
			// point light 2
			lightingShader.setVec3("pointLights[1].position", light2Pos);
			lightingShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[1].constant", 1.0f);
			lightingShader.setFloat("pointLights[1].linear", 0.09f);
			lightingShader.setFloat("pointLights[1].quadratic", 0.032f);
			// point light 3
			lightingShader.setVec3("pointLights[2].position", light3Pos);
			lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
			lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
			lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
			lightingShader.setFloat("pointLights[2].constant", 1.0f);
			lightingShader.setFloat("pointLights[2].linear", 0.09f);
			lightingShader.setFloat("pointLights[2].quadratic", 0.032f);

			// view/projection transformations
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 view = camera.GetViewMatrix();
			lightingShader.setMat4("projection", projection);
			lightingShader.setMat4("view", view);

			// render the loaded model
			Suzanne->Update(lightingShader);
			Suzanne->Draw(lightingShader);

			lightCubeShader.use();
			cube->Update(lightCubeShader);
			cube->Draw(lightCubeShader);

			// also draw the lamp object(s)
			lightCubeShader.use();
			lightCubeShader.setMat4("projection", projection);
			lightCubeShader.setMat4("view", view);
			glBindVertexArray(lightCubeVAO);

			glm::mat4 model;
			model = glm::mat4(1.0f);
			model = glm::translate(model, light1Pos);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			model = glm::mat4(1.0f);
			model = glm::translate(model, light2Pos);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			model = glm::mat4(1.0f);
			model = glm::translate(model, light3Pos);
			model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
			lightCubeShader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);

			// UI
			UIs.Update(SCR_WIDTH, SCR_HEIGHT);
			UIs.Draw();

			// swap buffers and poll events
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
		camera.ProcessKeyboard(UP, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.ProcessKeyboard(DOWN, deltaTime);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	SCR_WIDTH = width;
	SCR_HEIGHT = height;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	if (state == GLFW_PRESS)
	{
		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		float xoffset = xpos - lastX;
		float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;

		camera.ProcessMouseMovement(xoffset, yoffset);
	}
	else {
		firstMouse = true;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
