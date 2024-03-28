#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include "shaderr.h"
#include <Windows.h>
#include <math.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
int frameCount = -5;
float horizontalInput, verticalInput, upInput;
float moveSpeed = 5.0f;
float sensitivity = 1.0f;
double oldMouseXpos = 0.0;
double oldMouseYpos = 0.0;
double deltaMouseX, deltaMouseY;
float cameraLocation[3] = { 0.0f, 0.0f, -10.0f };
float cameraDirection[3] = { 0.0f, 0.0f, 1.0f };
float lightLocation[3] = { 0.0f, 5.0f, 10.0f };
int texWidth, texHeight, nrChannels;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	//std::cout << "width: " << width << "height: " << height << std::endl;
}
void crossProduct(const float v1[3], const float v2[3], float result[3]) {
	result[0] = v1[1] * v2[2] - v1[2] * v2[1];
	result[1] = v1[2] * v2[0] - v1[0] * v2[2];
	result[2] = v1[0] * v2[1] - v1[1] * v2[0];
}
void normalize(float v[3]) {
	float length = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] /= length;
	v[1] /= length;
	v[2] /= length;
}
void rotateX(float xyz[3], double angle) {
	// Calculate the axis of rotation
	float axis[3];
	float temp[3] = {0.0f, 1.0f, 0.0f};
	crossProduct(temp, xyz, axis);
	axis[1] = 0.0;
	normalize(axis);

	// Construct the rotation matrix
	float c = cos(angle);
	float s = sin(angle);
	float rotationMatrix[3][3] = {
		{c + (1 - c) * axis[0] * axis[0], (1 - c) * axis[0] * axis[1] - s * axis[2], (1 - c) * axis[0] * axis[2] + s * axis[1]},
		{(1 - c) * axis[1] * axis[0] + s * axis[2], c + (1 - c) * axis[1] * axis[1], (1 - c) * axis[1] * axis[2] - s * axis[0]},
		{(1 - c) * axis[2] * axis[0] - s * axis[1], (1 - c) * axis[2] * axis[1] + s * axis[0], c + (1 - c) * axis[2] * axis[2]}
	};

	// Apply the rotation matrix to xyz
	float rotated_xyz[3];
	rotated_xyz[0] = rotationMatrix[0][0] * xyz[0] + rotationMatrix[0][1] * xyz[1] + rotationMatrix[0][2] * xyz[2];
	rotated_xyz[1] = rotationMatrix[1][0] * xyz[0] + rotationMatrix[1][1] * xyz[1] + rotationMatrix[1][2] * xyz[2];
	rotated_xyz[2] = rotationMatrix[2][0] * xyz[0] + rotationMatrix[2][1] * xyz[1] + rotationMatrix[2][2] * xyz[2];

	// Update xyz with the rotated values
	xyz[0] = rotated_xyz[0];
	xyz[1] = rotated_xyz[1];
	xyz[2] = rotated_xyz[2];
}

// Function to rotate a vector around the Y axis
void rotateY(float xyz[3], double angle) {
	// Calculate the axis of rotation
	float axis_x[3];
	float temp[3] = { 0.0f, 1.0f, 0.0f };
	crossProduct(temp, xyz, axis_x);
	axis_x[1] = 0.0;
	normalize(axis_x);

	float axis_y[3];
	crossProduct(xyz, axis_x, axis_y);
	normalize(axis_y);

	// Construct the rotation matrix
	float c = cos(angle);
	float s = sin(angle);
	float rotationMatrix[3][3] = {
		{c + (1 - c) * axis_y[0] * axis_y[0], (1 - c) * axis_y[0] * axis_y[1] - s * axis_y[2], (1 - c) * axis_y[0] * axis_y[2] + s * axis_y[1]},
		{(1 - c) * axis_y[1] * axis_y[0] + s * axis_y[2], c + (1 - c) * axis_y[1] * axis_y[1], (1 - c) * axis_y[1] * axis_y[2] - s * axis_y[0]},
		{(1 - c) * axis_y[2] * axis_y[0] - s * axis_y[1], (1 - c) * axis_y[2] * axis_y[1] + s * axis_y[0], c + (1 - c) * axis_y[2] * axis_y[2]}
	};

	// Apply the rotation matrix to xyz
	float rotated_xyz[3];
	rotated_xyz[0] = rotationMatrix[0][0] * xyz[0] + rotationMatrix[0][1] * xyz[1] + rotationMatrix[0][2] * xyz[2];
	rotated_xyz[1] = rotationMatrix[1][0] * xyz[0] + rotationMatrix[1][1] * xyz[1] + rotationMatrix[1][2] * xyz[2];
	rotated_xyz[2] = rotationMatrix[2][0] * xyz[0] + rotationMatrix[2][1] * xyz[1] + rotationMatrix[2][2] * xyz[2];

	// Update xyz with the rotated values
	xyz[0] = rotated_xyz[0];
	xyz[1] = rotated_xyz[1];
	xyz[2] = rotated_xyz[2];
}


void processMouseInput(GLFWwindow* window, double deltaTime) {
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	deltaMouseX =  xpos - oldMouseXpos;
	deltaMouseY =  ypos - oldMouseYpos;
	oldMouseXpos = xpos;
	oldMouseYpos = ypos;
	float yAngle = acos(cameraDirection[1]);
	double rotateYAngle;
	// Convert mouse movements to rotation angles
	double rotateSpeed = 0.0017*sensitivity;
	double rotateXAngle = deltaMouseY * rotateSpeed;
	rotateYAngle = deltaMouseX * rotateSpeed;
	if (frameCount <= 0) {
		rotateXAngle = 0;
		rotateYAngle = 0;
	}
	// Perform rotation around X and Y axes
	
	rotateY(cameraDirection, rotateYAngle);
	
	rotateX(cameraDirection, rotateXAngle);
}
void processKeyboardInput(GLFWwindow* window)
{
	verticalInput = 0.0f;
	horizontalInput = 0.0f;
	upInput = 0.0f;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
	glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		verticalInput = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		horizontalInput = -1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		verticalInput = -1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		horizontalInput = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		upInput = 1.0f;
	}
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
		upInput = -1.0f;
	}
	//std::cout << verticalInput << " " << horizontalInput << std::endl;
}
int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(1920, 1080, "openg", NULL, NULL);
	if (window == NULL) {
		std::cout << "YOU MESSED UP NO WINDOW" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glViewport(0, 0, 1920, 1080);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSwapInterval(0); //turn off vsync
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	//the actually important bit
	glEnable(GL_TEXTURE_2D);
	Shader mainShader("vertex.vs", "alternatefragment.fs");
	unsigned int texture, normaltexture, heighttexture;
	glActiveTexture(GL_TEXTURE0); 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	unsigned char* data = stbi_load("C:\\Users\\sanul\\Documents\\openg\\OpenGtime\\textures\\wave_albedo.png", &texWidth, &texHeight, &nrChannels, 4);

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	
	int normwidth, normheight, channels;
	glActiveTexture(GL_TEXTURE1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenTextures(1, &normaltexture);
	glBindTexture(GL_TEXTURE_2D, normaltexture);
	data = stbi_load("C:\\Users\\sanul\\Documents\\openg\\OpenGtime\\textures\\wave_normal.png", &normwidth, &normheight, &channels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, normwidth, normheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	
	glActiveTexture(GL_TEXTURE2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glGenTextures(1, &heighttexture);
	glBindTexture(GL_TEXTURE_2D, heighttexture);
	data = stbi_load("C:\\Users\\sanul\\Documents\\openg\\OpenGtime\\textures\\wave_height.png", &normwidth, &normheight, &channels, 4);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, normwidth, normheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	float quad[] = {
	-1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f
	};
	unsigned int offsets[]{
		0,1,2,
		1,2,3
	};
	//a VAO stores the VBO attribute stuff so we dont need to redo the below steps every frame
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	unsigned int VBO; //make vertex buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

	//EBO to make a quad	
	unsigned int EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(offsets), offsets, GL_STATIC_DRAW);
	
	//set vertex attribute pointers
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	// 1st argument is the location of the vertex data (see vertex shader, location = 0);
	// 2nd is sizeof data (vector3), 3rd is type of data, 4th is irrelevant, 5th is how far apart each vector is (3 floating point numbers)
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); //color data
	glEnableVertexAttribArray(0);
	//glEnableVertexAttribArray(1);
	GLint m_viewport[4]; //preparation for main loop
	int fpsCounter = 0;
	double fpsTimer = glfwGetTime();
	double prevTime = glfwGetTime();
	
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	//main loop
	while (!glfwWindowShouldClose(window)) {
		//input
		frameCount += 1;
		double _Time = glfwGetTime();
		double deltaTime = _Time - prevTime;
		processMouseInput(window, deltaTime);
		processKeyboardInput(window);
		fpsCounter++;
		if (_Time - fpsTimer >= 1.0) {
			std::cout << "fps: " << fpsCounter << " " << "Time per frame: " << 1000.0 / double(fpsCounter) << "ms "<< std::endl;
			fpsCounter = 0;
			fpsTimer += 1.0;
		}

		//moving camera with input

		float rightAxis[3], upAxis[3];
		float temp[3] = { 0.0f, 1.0f, 0.0f };
		crossProduct(temp, cameraDirection, rightAxis);
		rightAxis[1] = 0.0f;
		normalize(rightAxis);
		crossProduct(cameraDirection, rightAxis, upAxis);

		// move it move it

		cameraLocation[0] = cameraLocation[0] + cameraDirection[0] * deltaTime * verticalInput * moveSpeed;
		cameraLocation[1] = cameraLocation[1] + cameraDirection[1] * deltaTime * verticalInput * moveSpeed;
		cameraLocation[2] = cameraLocation[2] + cameraDirection[2] * deltaTime * verticalInput * moveSpeed;
		cameraLocation[0] = cameraLocation[0] + rightAxis[0] * deltaTime * horizontalInput * moveSpeed;
		cameraLocation[1] = cameraLocation[1] + rightAxis[1] * deltaTime * horizontalInput * moveSpeed;
		cameraLocation[2] = cameraLocation[2] + rightAxis[2] * deltaTime * horizontalInput * moveSpeed;
		cameraLocation[0] = cameraLocation[0] + upAxis[0] * deltaTime * upInput * moveSpeed;
		cameraLocation[1] = cameraLocation[1] + upAxis[1] * deltaTime * upInput * moveSpeed;
		cameraLocation[2] = cameraLocation[2] + upAxis[2] * deltaTime * upInput * moveSpeed;

		//rendering

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, normaltexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, heighttexture);
		mainShader.use();	
		glGetIntegerv(GL_VIEWPORT, m_viewport);
		mainShader.setVector2("windowDimensions", m_viewport[2],m_viewport[3]);
		mainShader.setVector3("cameraLocation", cameraLocation);
		mainShader.setVector3("cameraDirection", cameraDirection);
		mainShader.setInt("SphereTexture", 0);
		mainShader.setInt("NormalTexture", 1);
		mainShader.setInt("HeightTexture", 2);
		float lightSpeed = 0.5;
		lightLocation[0] = 2 * sin(lightSpeed*_Time);
		//lightLocation[0] = 0;
		lightLocation[1] = 2 * sin(1.5*lightSpeed*_Time);
		//lightLocation[1] = 2.5;
		lightLocation[2] = 2 * cos(lightSpeed*_Time) + 10.0f;
		//lightLocation[2] = 10.0;

		mainShader.setVector3("lightLocation", lightLocation);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		//glDrawArrays(GL_TRIANGLES, 0, 3); 
		
		//events and buffers

		glfwSwapBuffers(window);
		glfwPollEvents();
		prevTime = _Time;
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}