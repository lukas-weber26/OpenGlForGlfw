#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

const char * vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char * fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(1.0,0.5,0.2,1.0);\n"
    "}\0";

void checkLink(unsigned int shaderProgram);
void checkSuccess(unsigned int vertexShader);
void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void processInput(GLFWwindow * window);
int main()
{

    //these seem like pretty normal c library things! 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //fortunately glew does not require too much crap
    

    GLFWwindow * window = glfwCreateWindow(800, 800, "First Window", NULL, NULL);
    if (window == NULL) {
	printf("GLFW Window fail.");
	glfwTerminate();
	exit(1);
    }

    //binds gl functions to the current window
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwPollEvents(); //this seems to be an issue with wayland 
    
    glViewport(0, 0, 800, 800);
    
    //YEAH TURNS OUT THE POSITION OF THIS MATTERS A LOT. HAS TO BE AFTER THE WINDOW HAS BEEN CREATED!
    glewExperimental = GL_TRUE; 
    glewInit();

    float vertices[] = {
	0.5, 0.5,0.0,
	0.5, -0.5, 0.0,
	-0.5,-0.5,0.0,
	-0.5,0.5, 0.0
    };
    unsigned int ind[] = {0,1,3, 1,2,3};
   
    //tell gl to generate vertex arrays and bind them
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);
     
    unsigned int VBO;
    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO); //you have to bind buffers to make the following gl operations affect said buffer
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices), vertices, GL_STATIC_DRAW); //note that GL_STATIC_DRAW means that this is not expected to change much
   
    //notice that this is an identical method for loading an array onto the gpu. up until now gl does not differentiate between EBO and VBO
    unsigned int EBO;
    glGenBuffers(1,&EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(ind),ind,GL_STATIC_DRAW);
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER); 
    glShaderSource(vertexShader,1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkSuccess(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkSuccess(fragmentShader);

    unsigned int shaderProgram; 
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    checkLink(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    //final step here: Make the gpu interpret the shader program in a meaningful way!
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);


    while(!glfwWindowShouldClose(window)) {
	processInput(window);
	
	glClearColor(0.2f, 0.5f, 0.1f, 0.5f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glfwSwapBuffers(window);
	glfwPollEvents(); //this seems to be an issue with wayland 

    }
    
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow * window, int width, int height) { 
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow * window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);	
    }
}

void checkSuccess(unsigned int vertexShader) {
    int success;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success) {
	char infoLog[512];
	glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
	printf("%s\n",infoLog);
	exit(0);
    }
}

void checkLink(unsigned int shaderProgram) {
    int success;
    glGetProgramiv(shaderProgram,GL_LINK_STATUS, &success);
    if (!success) {
	char out[512];
	glGetProgramInfoLog(shaderProgram,512,NULL,out);
	printf("%s",out);
	exit(0);
    }
}
