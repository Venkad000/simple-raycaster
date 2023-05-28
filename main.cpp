#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include <iostream>
#include <vector>
#include <cmath>

#define PI 3.141592
#define DR 0.0174533

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 512;

//float fov = 45.0f;

const char* vertexShaderSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
"}\0";

const char* fragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);\n"
"}\n\0";

const char* vertexShaderMapSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
"}\0";

const char* fragmentShaderMapSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);\n"
"}\n\0"; 

const char* vertexShaderVerticalSource = "#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main()\n"
"{\n"
"   gl_Position = projection * view * model * vec4(aPos, 1.0f);\n"
"}\0";

const char* fragmentShaderVerticalSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"   FragColor = vec4(0.98f, 0.95f, 0.011f, 1.0f);\n"
"}\n\0";

float vertices[] = {
       0.0f, 0.0f, 0.0f,
       20.0f, 0.0f, 0.0f,
       0.0f,0.0f,0.0f,
       0.0f,0.0f,0.0f,
};

unsigned int indices[] = { 
        0
};

glm::vec3 playerPos(96.0f, 96.0f, 0.0f);
GLfloat playerRot = 0.0f;

float pdx, pdy;

float deltaTime = 0.0f;
float lastFrameTime = 0.0f;

int mapX = 8, mapY = 8, mapS = 64;

int map[] = {
    1,1,1,1,1,1,1,1,
    1,0,0,0,0,0,0,1,
    1,0,1,0,0,0,0,1,
    1,0,1,0,0,1,0,1,
    1,0,1,1,1,0,0,1,
    1,0,0,0,0,0,0,1,
    1,0,0,0,0,1,0,1,
    1,1,1,1,1,1,1,1,
};

std::vector<glm::vec3>obstaclesPositions;

void bulidMap() {
    for (int x = 0; x < mapX; x++) {
        for (int y = 0; y < mapY; y++) {
            if (map[y * mapX + x] == 1) {
                obstaclesPositions.push_back(glm::vec3(float(x) * mapS,  float(y)  * mapS,0));
            }
        }
    }
}

std::vector<float> rays;
std::vector<float>verticalsXPos;
std::vector<float>verticalsScale;

float dist(float ax, float ay, float bx, float by) {
    return sqrt((bx - ax) * (bx - ax) + (by - ay) * (by - ay));
}

void calculateRays() {
    verticalsScale.clear();
    verticalsXPos.clear();

    int r, mx, my, mp, dof;
    double rx, ry, ra, xo, yo;
    double distT = 0;
    ra = playerRot - DR * 45; if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
    for (r = 0; r < 90; r++) {
        dof = 0;
        float disH = 10000000;
        float hx = playerPos.x;
        float hy = playerPos.y;

        float aTan = -1 / tan(ra);
        if (ra > PI) {
            int temp = (int)(playerPos.y) >> 6;
            ry = ( temp << 6) - 0.0001;
            rx = (playerPos.y - ry) * aTan + playerPos.x;
            yo = -64;
            xo = -yo * aTan;
        }
        if (ra < PI) {
            int temp = (int)(playerPos.y) >> 6;
            ry = ( temp << 6) + 64 ;
            rx = (playerPos.y - ry) * aTan + playerPos.x;
            yo = 64;
            xo = -yo * aTan;
        }
        if (ra == 0 || ra == PI) {
            rx = playerPos.x;
            ry = playerPos.y;
            dof = 8;
        }
        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                hx = rx;
                hy = ry;
                disH = dist(playerPos.x, playerPos.y, hx, hy);
                dof = 8;
            }
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        
        dof = 0;
        float disV = 10000000;
        float vx = playerPos.x;
        float vy = playerPos.y;
        float anTan = -tan(ra);
        if (ra > PI/2 && ra < 3 * PI/2) {
            int temp = (int)(playerPos.x) >> 6;
            rx = (temp << 6) - 0.0001;
            ry = (playerPos.x - rx) * anTan + playerPos.y;
            xo = -64;
            yo = -xo * anTan;
        }
        if (ra < PI/2  || ra > 3 * PI / 2) {
            int temp = (int)(playerPos.x) >> 6;
            rx = (temp << 6) + 64;
            ry = (playerPos.x - rx) * anTan + playerPos.y;
            xo = 64;
            yo = -xo * anTan;
        }
        if (ra == 0 || ra == PI) {
            rx = playerPos.x;
            ry = playerPos.y;
            dof = 8;
        }
        while (dof < 8) {
            mx = (int)(rx) >> 6;
            my = (int)(ry) >> 6;
            mp = my * mapX + mx;
            if (mp > 0 && mp < mapX * mapY && map[mp] == 1) {
                vx = rx;
                vy = ry;
                disV = dist(playerPos.x, playerPos.y, vx, vy);
                dof = 8;
            }
            else {
                rx += xo;
                ry += yo;
                dof += 1;
            }
        }
        if (disV < disH) {
            rx = vx;
            ry = vy;
            distT = disV;
        }
        if (disH < disV) {
            rx = hx;
            ry = hy;
            distT = disH;
        }
        float lineH = (mapS * 320) / distT;
        if (lineH > 320) { lineH = 320; }
        //std::cout << lineH << std::endl;
        verticalsScale.push_back(lineH);
        verticalsXPos.push_back(r * 5 + 530);
        ra += DR; if (ra < 0) { ra += 2 * PI; } if (ra > 2 * PI) { ra -= 2 * PI; }
    }
}



int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "RayCaster", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    calculateRays();

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    unsigned int vertexShader1 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader1, 1, &vertexShaderMapSource, NULL);
    glCompileShader(vertexShader1);
    glGetShaderiv(vertexShader1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int fragmentShader1 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader1, 1, &fragmentShaderMapSource, NULL);
    glCompileShader(fragmentShader1);
    glGetShaderiv(fragmentShader1, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int shaderProgram1 = glCreateProgram();
    glAttachShader(shaderProgram1, vertexShader1);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);
    glGetProgramiv(shaderProgram1, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram1, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader1);
    glDeleteShader(fragmentShader1);

    float verticesx[] = {
         1.0f, 1.0f, 0.0f,
         63.0f, 1.0f, 0.0f,
         1.0f, 63.0f, 0.0f,
         63.0f,  63.0f, 0.0f
    };
    unsigned int indices[] = {  
        0, 1, 3,  
        0, 2, 3   
    };
    unsigned int VBO1, VAO1, EBO1;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO1);
    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesx), verticesx, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);


    float verticesVertical[] = {
        0.0f,0.8f,0.0f,
        0.0f,-0.8f,0.0f,
    };

    unsigned int vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader2, 1, &vertexShaderVerticalSource, NULL);
    glCompileShader(vertexShader2);
    success;
    infoLog[512];
    glGetShaderiv(vertexShader2, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fragmentShaderVerticalSource, NULL);
    glCompileShader(fragmentShader2);
    glGetShaderiv(fragmentShader2, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    unsigned int shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader2);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);
    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram2, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader2);
    glDeleteShader(fragmentShader2);

    unsigned int VBO2, VAO2, EBO2;
    glGenVertexArrays(1, &VAO2);
    glGenBuffers(1, &VBO2);
    glGenBuffers(1, &EBO2);
    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesVertical), verticesVertical, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    bulidMap();
    pdx = cos(playerRot) * 5;
    pdy = sin(playerRot) * 5;
    while (!glfwWindowShouldClose(window))
    {
        calculateRays();
        float currentFrameTime = static_cast<float>(glfwGetTime());
        deltaTime = currentFrameTime - lastFrameTime;
        lastFrameTime = currentFrameTime;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // --------------------------------------------map---------------------------------------------
        glUseProgram(shaderProgram1);

        glm::mat4 projection = glm::ortho(0.0f, 1024.0f, 0.0f, 512.0f, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram1, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f) , glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram1, "view"), 1, GL_FALSE, glm::value_ptr(view));

        
        glBindVertexArray(VAO1); 

        for (int i = 0; i < obstaclesPositions.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, obstaclesPositions[i]);
            unsigned int modelLoc = glGetUniformLocation(shaderProgram1, "model");
            glUniformMatrix4fv(modelLoc,1, GL_FALSE, glm::value_ptr(model));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            //std::cout << obstaclesPositions[i].x << " " << obstaclesPositions[i].y << std::endl;
        }
        glBindVertexArray(0);

        // ------------------------------player---------------------------------------------

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, playerPos);
        transform = glm::rotate(transform, playerRot, glm::vec3(0, 0, 1.0f));

        //std::cout << playerPos.x << " " << playerPos.y << std::endl;

        glUseProgram(shaderProgram);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, 1024.0f, 0.0f, 512.0f, 0.1f, 100.0f)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));



        glBindVertexArray(VAO); 
        glPointSize(7);
        glDrawArrays(GL_POINTS, 0, 1);
        glDrawArrays(GL_LINES, 0, 4);
        glBindVertexArray(0); 

        // ------------------------------------------------vertical lines-------------------------------------------------------

        

        glUseProgram(shaderProgram2);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "projection"), 1, GL_FALSE, glm::value_ptr(glm::ortho(0.0f, 1024.0f, 0.0f, 512.0f, 0.1f, 100.0f)));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram2, "view"), 1, GL_FALSE, glm::value_ptr(glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

        glBindVertexArray(VAO2);

        for (int i = 0; i < verticalsScale.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(verticalsXPos[i], 256.0f,0.0f));
            model = glm::scale(model, glm::vec3(1.0f, verticalsScale[i], 1.0f));
            unsigned int modelLoc = glGetUniformLocation(shaderProgram2, "model");
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
            glLineWidth(4);
            glDrawArrays(GL_LINES, 0, 2);
        }

        glBindVertexArray(0);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glDeleteVertexArrays(1, &VAO1);
    glDeleteBuffers(1, &VBO1);
    glDeleteBuffers(1, &EBO1);
    glDeleteProgram(shaderProgram1);

    glDeleteVertexArrays(1, &VAO2);
    glDeleteBuffers(1, &VBO2);
    glDeleteBuffers(1, &EBO2);
    glDeleteProgram(shaderProgram2);

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float playerSpeed = static_cast<float>(20 * deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        //std::cout << "A pressed" << std::endl;
        //playerPos = playerPos + glm::vec3(-0.2f,0.0f,0.0f) * playerSpeed;
        
        playerRot -= 0.001;
        if (playerRot < 0) {
            playerRot += 2 * PI;
        }
        
        pdx = cos(playerRot) * 5;
        pdy = sin(playerRot) * 5;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        //std::cout << "D pressed" << std::endl;
        //playerPos = playerPos + glm::vec3(0.2f, 0.0f, 0.0f) * playerSpeed;
        playerRot += 0.001;
        if (playerRot > 2 * PI) {
            playerRot -= 2 * PI;
        }
        pdx = cos(playerRot) * 5;
        pdy = sin(playerRot) * 5;
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        //std::cout << "W pressed" << std::endl;
        //playerPos = playerPos + glm::vec3(0, 4, 0.0f) * playerSpeed;
        playerPos = playerPos + glm::vec3(pdx, pdy, 0.0f) * playerSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        //std::cout << "S pressed" << std::endl;
        //playerPos = playerPos + glm::vec3(0, -4, 0.0f) * playerSpeed;
        playerPos = playerPos + glm::vec3(-pdx, -pdy, 0.0f) * playerSpeed;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}