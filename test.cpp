
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <sstream>

#include "stb_image.h"

#include <time.h>
#include <random>
#include <thread>
#include <chrono>

#include <windows.h>
#include <mmsystem.h>
#include <mciapi.h>

#pragma comment(lib, "Winmm.lib")

#include <cmath>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
using namespace std;

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 getViewMatrix() {
    return ViewMatrix;
}
glm::mat4 getProjectionMatrix() {
    return ProjectionMatrix;
}

//////////////////////////////////////////////
/// Add camera function here

void camera_function(GLFWwindow* window) {
    // Define static camera position and target
    static glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 20.0f); // Camera position
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.25f);           // Target position at the maze center
    glm::vec3 upVector = glm::vec3(0.0f, 1.0f, 0.0f);               // Up vector for orientation

    // Define rotation angles
    static float angleX = 0.0f;
    static float angleY = 0.0f;
    static float zoom = 20.0f; // Start with some initial zoom level

    // Define panning values
    static glm::vec3 panOffset = glm::vec3(0.0f, 0.0f, 0.0f);

    // Check key presses for rotation and zoom
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        angleX += 0.1f; // Rotate around the X-axis (positive)
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        angleX -= 0.1f; // Rotate around the X-axis (negative)
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        angleY += 0.1f; // Rotate around the Y-axis (positive)
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        angleY -= 0.1f; // Rotate around the Y-axis (negative)
    }
    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        zoom -= 0.1f; // Zoom in
    }
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        zoom += 0.1f; // Zoom out
    }

    // Limit the zoom level
    zoom = glm::clamp(zoom, 5.0f, 50.0f); // Restrict zoom level to avoid clipping or too far away

    // Check key presses for panning
    float panSpeed = 0.1f; // Adjust speed of panning
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        panOffset.x -= panSpeed; // Move left along the x-axis
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        panOffset.x += panSpeed; // Move right along the x-axis
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        panOffset.y += panSpeed; // Move up along the y-axis
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        panOffset.y -= panSpeed; // Move down along the y-axis
    }

    // Calculate rotation matrices around X and Y
    glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), angleX, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), angleY, glm::vec3(0.0f, 1.0f, 0.0f));

    // Apply the rotations and set the camera position
    glm::vec3 rotatedPosition = glm::vec3(rotationY * rotationX * glm::vec4(0.0f, 0.0f, zoom, 1.0f));
    cameraPosition = rotatedPosition + panOffset;

    // Update the view matrix
    ViewMatrix = glm::lookAt(
        cameraPosition,                 // Camera position
        cameraTarget + panOffset,       // Target position (adjusted for panning)
        upVector                        // Up vector
    );
}

/////////////////////////////////////////////////

GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    printf("Compiling shader : %s\n", vertex_file_path);
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    printf("Compiling shader : %s\n", fragment_file_path);
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}
///////////////////////////////////////////////////

const float maze[10][10] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 1, 1, 1, 1, 0, 1, 0, 1},
    {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
    {1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
    {1, 0, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 1, 1, 0, 1, 1, 1, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 1, 0, 0},
    {1, 0, 1, 0, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

const int xCoord[] = { 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 5, 5, 5, 6, 6, 6, 6, 6, 6, 6, 7, 7, 7, 8, 8, 8, 8, 8, 8, 8, 8 };
const int yCoord[] = { 1, 2, 3, 4, 5, 6, 7, 8, 1, 5, 7, 1, 3, 4, 5, 7, 8, 1, 3, 5, 6, 7, 1, 3, 7, 1, 2, 3, 4, 5, 7, 8, 1, 5, 8, 1, 2, 3, 4, 5, 6, 7, 8 };

static  GLfloat cubeColorData[] = {
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
};

void ChangeCubeColorToRed() {
    for (int i = 0; i < sizeof(cubeColorData) / sizeof(cubeColorData[0]); i += 3) {
        cubeColorData[i] = 1.0f;
        cubeColorData[i + 1] = 0.0f;
        cubeColorData[i + 2] = 0.0f;
    }
}

void ChangeCubeColorToYellow() {
    for (int i = 0; i < sizeof(cubeColorData) / sizeof(cubeColorData[0]); i += 3) {
        cubeColorData[i] = 1.0f;
        cubeColorData[i + 1] = 1.0f;
        cubeColorData[i + 2] = 0.0f;
    }
}

void FlashRedToYellow() {
    ChangeCubeColorToRed();

    // Delay
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    ChangeCubeColorToYellow();
}

int getRandomNumber() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(0, 42);

    return distr(gen);
}

int getRandomNum3() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> distr(0, 2);

    return distr(gen);
}

// Start coordinates
float characterX = -4.50f;
float characterY = 3.5f;
float characterZ = 0.5f;

int start = getRandomNumber();

float charX = xCoord[start] - 5 + 0.5f;
float charY = 5 - yCoord[start] + 0.5f;
float charZ = 0.5f;


GLuint treasureVertexBuffer;
GLfloat trsize = 0.4f; // Define trsize as a GLfloat to represent the size of the treasure

void playMP3() {
    static bool hasPlayed = false;

    if (!hasPlayed) {
        PlaySound(TEXT("sus.wav"), NULL, SND_SYNC | SND_FILENAME);
        hasPlayed = true;
    }
}

void minimizeTreasure() {
    for (int i = 0; i < 4; i++) {
        if (trsize > 0.2) {
            trsize = trsize * 0.99f;
        }
        else if (trsize <= 0.2) {
            charX = 0 - 5 + 0.5;
            charY = 5 - 0 + 0.5;
            trsize = 0.0f;

            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

std::atomic<bool> isRunning(false);
void minimizeTreasureThread() {
    while (isRunning) {
        minimizeTreasure();
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Delay
    }
}

void playMP3Thread() {
    while (isRunning) {
        playMP3();
        std::this_thread::sleep_for(std::chrono::milliseconds(1)); // Delay
    }
}

void FlashThread() {
    while (isRunning) {
        FlashRedToYellow();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Delay
    }
}

int main(void)
{
    auto start = std::chrono::steady_clock::now(); // Initial starting time 
    if (!glfwInit())
    {
        fprintf(stderr, "Failed to initialize GLFW\n");
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(950, 950, u8"Άσκηση 1Γ 2024 - Κυνήγι θυσαυρού", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // Background color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glewInit();

    glEnable(GL_DEPTH_TEST);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders("P1BVertexShader.vertexshader", "P1BFragmentShader.fragmentshader");
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");

    GLuint programID1 = LoadShaders("P1CVertexShader.vertexshader", "P1CFragmentShader.fragmentshader");
    GLuint MatrixID1 = glGetUniformLocation(programID1, "MVP");

    // Vertices and colors
    std::vector<GLfloat> allWallVertices;
    std::vector<GLfloat> allWallColors;

    for (int i = 0; i < 10; ++i) {
        for (int j = 0; j < 10; ++j) {
            if (maze[i][j] == 1) {
                float x = j - 5.0f;
                float y = 5.0f - i;
                float z = 0.0f;

                GLfloat wall_vertices[] = {
                    // Down side
                    x, y, z, x + 1.0f, y, z, x, y, z + 1.0f,
                    x + 1.0f, y, z, x + 1.0f, y, z + 1.0f, x, y, z + 1.0f,
                    // Up side
                    x, y + 1.0f, z, x + 1.0f, y + 1.0f, z, x, y + 1.0f, z + 1.0f,
                    x + 1.0f, y + 1.0f, z, x + 1.0f, y + 1.0f, z + 1.0f, x, y + 1.0f, z + 1.0f,
                    // Frond side
                    x, y, z + 1.0f, x + 1.0f, y, z + 1.0f, x, y + 1.0f, z + 1.0f,
                    x + 1.0f, y, z + 1.0f, x + 1.0f, y + 1.0f, z + 1.0f, x, y + 1.0f, z + 1.0f,
                    // Back side
                    x, y, z, x + 1.0f, y, z, x, y + 1.0f, z,
                    x + 1.0f, y, z, x + 1.0f, y + 1.0f, z, x, y + 1.0f, z,
                    // Left side
                    x, y, z, x, y + 1.0f, z, x, y, z + 1.0f,
                    x, y + 1.0f, z, x, y + 1.0f, z + 1.0f, x, y, z + 1.0f,
                    // Right side
                    x + 1.0f, y, z, x + 1.0f, y, z + 1.0f, x + 1.0f, y + 1.0f, z,
                    x + 1.0f, y, z + 1.0f, x + 1.0f, y + 1.0f, z + 1.0f, x + 1.0f, y + 1.0f, z
                };

                // Add vertices
                allWallVertices.insert(allWallVertices.end(), std::begin(wall_vertices), std::end(wall_vertices));

                for (int v = 0; v < 36; ++v) {
                    allWallColors.push_back(0.0f);
                    allWallColors.push_back(0.0f);
                    allWallColors.push_back(1.0f);
                }
            }
        }
    }

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, allWallVertices.size() * sizeof(GLfloat), allWallVertices.data(), GL_STATIC_DRAW);

    GLuint colorbuffer;
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, allWallColors.size() * sizeof(GLfloat), allWallColors.data(), GL_STATIC_DRAW);

    bool key_J_pressed = false;
    bool key_L_pressed = false;
    bool key_I_pressed = false;
    bool key_K_pressed = false;

    int width, height, nrChannels;
    unsigned char* data0 = stbi_load("coins.jpg", &width, &height, &nrChannels, 0);
    unsigned char* data1 = stbi_load("tree.jpg", &width, &height, &nrChannels, 0);
    unsigned char* data2 = stbi_load("tom.jpg", &width, &height, &nrChannels, 0);

    unsigned char* pics[] = { data0, data1, data2 };
    unsigned char* data = pics[getRandomNum3()];
    boolean touched = false;
    do {

        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<int> elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(now - start);

        if (elapsed_seconds.count() >= 5 && touched == false) {

            data = pics[getRandomNum3()];

            int rand = getRandomNumber();
            int k1 = characterX - 0.5;
            int w1 = characterY - 0.5;
            int tx = xCoord[rand] - 5;
            int ty = 5 - yCoord[rand];
            if ((tx == k1) && (ty == w1)) {
                if (rand == 42) {
                    charX = xCoord[rand - 10] - 5 + 0.5f;
                    charY = 5 - yCoord[rand - 10] + 0.5f;
                    charZ = 0.5;
                    start = now;
                }
                if (rand != 42) {
                    charX = xCoord[rand + 1] - 5 + 0.5f;
                    charY = 5 - yCoord[rand + 1] + 0.5f;
                    charZ = 0.5;
                    start = now;
                }
            }
            else {
                charX = xCoord[rand] - 5 + 0.5f;
                charY = 5 - yCoord[rand] + 0.5f;
                charZ = 0.5;
                start = now;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Use our shader
        glUseProgram(programID1);
        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 4.0f / 4.0f, 0.1f, 100.0f);
        camera_function(window); // Initialize camera
        glm::mat4 View = getViewMatrix(); // Use the camera's view matrix

        glm::mat4 Model = glm::mat4(1.0f);
        glm::mat4 MVP = Projection * View * Model;
        // Update MVP matrix if camera or model changes
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        //MOVE LEFT
        if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
            if (!key_J_pressed) {
                int k = characterX - 0.5;
                int w = characterY - 0.5;
                int tx = charX - 0.5;
                int ty = charY - 0.5;
                if (w == ty && k == tx + 1) {
                    touched = true;
                    if (!isRunning) {
                        isRunning = true;
                        std::thread m(playMP3Thread);
                        m.detach(); // Independent thread

                        std::thread t(minimizeTreasureThread);
                        t.detach(); // Independent thread

                        std::thread z(FlashThread);
                        z.detach(); // Independent thread
                    }
                }
                if (characterX == -4.50f && characterY == 3.50f && characterZ == 0.5f) {  // Special teleport condition
                    characterX += 9.0f;
                    characterY -= 5.0f;
                }
                else if (maze[int(5 - w)][int((k - 1) + 5)] == 0 && (w != ty or k != tx + 1)) {  // Check if space to the left is not a wall
                    characterX -= 1.0f;

                }

                key_J_pressed = true;
            }
        }
        else {
            key_J_pressed = false;
        }

        // MOVE RIGHT
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
            if (!key_L_pressed) {
                int k = characterX - 0.5;
                int w = characterY - 0.5;
                int tx = charX - 0.5;
                int ty = charY - 0.5;
                if (w == ty && k == tx - 1) {
                    touched = true;
                    if (!isRunning) {
                        isRunning = true;
                        std::thread m(playMP3Thread);
                        m.detach(); // Independent thread

                        std::thread t(minimizeTreasureThread);
                        t.detach(); // Independent thread

                        std::thread z(FlashThread);
                        z.detach(); // Independent thread
                    }
                }
                if (characterX == 4.50f && characterY == -1.50f && characterZ == 0.5f) {  // Special teleport condition
                    characterX -= 9.0f;
                    characterY += 5.0f;
                }
                else if (maze[int(5 - w)][int((k + 1) + 5)] == 0 && (w != ty or k != tx - 1)) {  // Check if space to the right is not a wall
                    characterX += 1.0f;
                }
                key_L_pressed = true;
            }
        }
        else {
            key_L_pressed = false;
        }

        // MOVE UP
        if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
            if (!key_I_pressed) {
                int k = characterX - 0.5;
                int w = characterY - 0.5;
                int tx = charX - 0.5;
                int ty = charY - 0.5;
                if (w + 1 == ty && k == tx) {
                    touched = true;
                    if (!isRunning) {
                        isRunning = true;
                        std::thread m(playMP3Thread);
                        m.detach(); // Independent thread

                        std::thread t(minimizeTreasureThread);
                        t.detach(); // Independent thread

                        std::thread z(FlashThread);
                        z.detach(); // Independent thread
                    }
                }
                if (maze[int(5 - (w + 1))][int(k + 5)] == 0 && (w + 1 != ty or k != tx)) {  // Check if space above is not a wall
                    characterY += 1.0f;
                }
                key_I_pressed = true;
            }
        }
        else {
            key_I_pressed = false;
        }

        // MOVE DOWN
        if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
            int k = characterX - 0.5;
            int w = characterY - 0.5;
            int tx = charX - 0.5;
            int ty = charY - 0.5;
            if (w - 1 == ty && k == tx) {
                touched = true;
                if (!isRunning) {
                    isRunning = true;
                    std::thread m(playMP3Thread);
                    m.detach(); // Independent thread

                    std::thread t(minimizeTreasureThread);
                    t.detach(); // Independent thread

                    std::thread z(FlashThread);
                    z.detach(); // Independent thread
                }
            }
            if (!key_K_pressed) {
                if (maze[int(5 - (w - 1))][int(k + 5)] == 0 && (w - 1 != ty or k != tx)) {  // Check if space below is not a wall
                    characterY -= 1.0f;
                }
                key_K_pressed = true;
            }
        }
        else {
            key_K_pressed = false;
        }

        GLfloat cubeVertices[] = {
            // Back (Z = +0.25)
            characterX - 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ + 0.25f,

            characterX + 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ + 0.25f,

            // Front (Z = -0.25)
            characterX - 0.25f, characterY - 0.25f, characterZ - 0.25f,
            characterX + 0.25f, characterY - 0.25f, characterZ - 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ - 0.25f,

            characterX + 0.25f, characterY - 0.25f, characterZ - 0.25f,
            characterX + 0.25f, characterY + 0.25f, characterZ - 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ - 0.25f,

            // Left (X = -0.25)
            characterX - 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX - 0.25f, characterY - 0.25f, characterZ - 0.25f,

            characterX - 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ - 0.25f,
            characterX - 0.25f, characterY - 0.25f, characterZ - 0.25f,

            // Right (X = +0.25)
            characterX + 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY - 0.25f, characterZ - 0.25f,

            characterX + 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY + 0.25f, characterZ - 0.25f,
            characterX + 0.25f, characterY - 0.25f, characterZ - 0.25f,

            // Up (Y = +0.25)
            characterX - 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ - 0.25f,

            characterX + 0.25f, characterY + 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY + 0.25f, characterZ - 0.25f,
            characterX - 0.25f, characterY + 0.25f, characterZ - 0.25f,

            // Down (Y = -0.25)
            characterX - 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX - 0.25f, characterY - 0.25f, characterZ - 0.25f,

            characterX + 0.25f, characterY - 0.25f, characterZ + 0.25f,
            characterX + 0.25f, characterY - 0.25f, characterZ - 0.25f,
            characterX - 0.25f, characterY - 0.25f, characterZ - 0.25f
        };

        GLuint cubeVertexBuffer;
        glGenBuffers(1, &cubeVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        GLuint cubeColorBuffer;
        glGenBuffers(1, &cubeColorBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeColorData), cubeColorData, GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        //maze
    // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // 2nd attribute buffer : colors
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, allWallVertices.size() / 3);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        //character
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, cubeColorBuffer);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        glDrawArrays(GL_TRIANGLES, 0, 36);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        glUseProgram(programID);

        GLfloat treasureVertices[] = {
            // Back (Z = +trsize)
            charX - trsize, charY - trsize, charZ + trsize,
            charX + trsize, charY - trsize, charZ + trsize,
            charX - trsize, charY + trsize, charZ + trsize,

            charX + trsize, charY - trsize, charZ + trsize,
            charX + trsize, charY + trsize, charZ + trsize,
            charX - trsize, charY + trsize, charZ + trsize,

            // Front (Z = -trsize)
            charX - trsize, charY - trsize, charZ - trsize,
            charX + trsize, charY - trsize, charZ - trsize,
            charX - trsize, charY + trsize, charZ - trsize,

            charX + trsize, charY - trsize, charZ - trsize,
            charX + trsize, charY + trsize, charZ - trsize,
            charX - trsize, charY + trsize, charZ - trsize,

            // Left (X = -trsize)
            charX - trsize, charY - trsize, charZ + trsize,
            charX - trsize, charY + trsize, charZ + trsize,
            charX - trsize, charY - trsize, charZ - trsize,

            charX - trsize, charY + trsize, charZ + trsize,
            charX - trsize, charY + trsize, charZ - trsize,
            charX - trsize, charY - trsize, charZ - trsize,

            // Right (X = +trsize)
            charX + trsize, charY - trsize, charZ + trsize,
            charX + trsize, charY + trsize, charZ + trsize,
            charX + trsize, charY - trsize, charZ - trsize,

            charX + trsize, charY + trsize, charZ + trsize,
            charX + trsize, charY + trsize, charZ - trsize,
            charX + trsize, charY - trsize, charZ - trsize,

            // Up (Y = +trsize)
            charX - trsize, charY + trsize, charZ + trsize,
            charX + trsize, charY + trsize, charZ + trsize,
            charX - trsize, charY + trsize, charZ - trsize,

            charX + trsize, charY + trsize, charZ + trsize,
            charX + trsize, charY + trsize, charZ - trsize,
            charX - trsize, charY + trsize, charZ - trsize,

            // Down (Y = -trsize)
            charX - trsize, charY - trsize, charZ + trsize,
            charX + trsize, charY - trsize, charZ + trsize,
            charX - trsize, charY - trsize, charZ - trsize,

            charX + trsize, charY - trsize, charZ + trsize,
            charX + trsize, charY - trsize, charZ - trsize,
            charX - trsize, charY - trsize, charZ - trsize
        };

        float UVData[] = {
            // Back (Z = +0.4)
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            // Front (Z = -0.4)
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            // Left (X = -0.4)
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            // Right (X = +0.4)
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            // Up (Y = +0.4)
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f,

            // Down (Y = -0.4)
            0.0f, 0.0f,
            1.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 0.0f,
            1.0f, 1.0f,
            0.0f, 1.0f
        };

        //treasure
        GLuint treasureVertexBuffer;
        glGenBuffers(1, &treasureVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, treasureVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(treasureVertices), treasureVertices, GL_STATIC_DRAW);

        GLuint treasureUVBuffer;
        glGenBuffers(1, &treasureUVBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, treasureUVBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(UVData), UVData, GL_STATIC_DRAW);

        // Bind Vertex Buffer
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, treasureVertexBuffer);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

        // Bind UV Buffer
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, treasureUVBuffer);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

        GLuint textureID;
        glGenTextures(1, &textureID);

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Bind Texture
        glActiveTexture(GL_TEXTURE0);

        glUniform1i(glGetUniformLocation(programID, "myTextureSampler"), 0);

        // Send MVP matrix
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);

        // Draw Call
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Disable attributes after drawing
        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(16));

    } while (glfwGetKey(window, GLFW_KEY_SPACE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0);

    // Cleanup VBO
    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &colorbuffer);
    glDeleteVertexArrays(1, &VertexArrayID);

    glfwTerminate();
    return 0;
}