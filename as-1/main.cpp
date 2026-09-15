// ================================================================================================
// CPSC 484 - Assignment 1 - OpenGL Mesh Viewer
// ================================================================================================

#include "glad.h"          // OpenGL function loader -- must be included before glfw3.h
#include <GLFW/glfw3.h>    // window/context creation, input, timing
#include <cstdlib>         // std::getenv -- used by isRunningUnderWSL() below
#include <fstream>         // std::ifstream -- used by isRunningUnderWSL() below
#include <iostream>        // std::cerr / std::cout for error and debug messages
#include <string>          // std::string -- used by isRunningUnderWSL() below, and by the titleString you'll add next
#include <memory>          // std::unique_ptr -- used by the Mesh class
#include <vector>          // std::vector -- used by the Mesh class

#include<glm/glm.hpp>		       // Linear algebra library
#include<glm/gtc/matrix_transform.hpp> //
#include<glm/gtc/type_ptr.hpp>	       //

#include "shader.h"	// Shader class
#include "gl_objects.h" // VAO, VBO, EBO

//----------------------------------------------------------------------------------------------
glm::vec3 lightPosInput(GLFWwindow* window) {
    glm::vec3 direction(0.0f);

    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)  {
	direction.z -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)  {
	direction.z += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)  {
	direction.x -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)  {
	direction.x += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS)  {
	direction.y += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)  {
	direction.y -= 1.0f;
    }
    // Prevent diagonal input from being faster 
    if (glm::length(direction) > 1.0f) 
	direction = glm::normalize(direction);
    return direction;
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
glm::vec3 arrowKeyInput(GLFWwindow* window) {
    glm::vec3 direction(0.0f);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)  {
	direction.x -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  {
	direction.x += 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  {
	direction.y -= 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)  {
	direction.y += 1.0f;
    }
    // Prevent diagonal input from being faster 
    if (glm::length(direction) > 1.0f) 
	direction = glm::normalize(direction);
    return direction;
}
//----------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------
glm::vec3 cycleColor(float t) {
    const float pi = 3.14159265f;
    return glm::vec3(
        0.5f + 0.5f * sin(t),
        0.5f + 0.5f * sin(t + 2.0f * pi / 3.0f),
        0.5f + 0.5f * sin(t + 4.0f * pi / 3.0f)
    );
}
//----------------------------------------------------------------------------------------------

//TODO: (2.1) declare your own window-title string here. See Assignment 1
std::string titleString  = "Fall 2026 - Assignment 1 - Riley Berry";

// -----------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// -----------------------------------------------------------------------------

glm::vec3 lightPosInput(GLFWwindow* window); // Handles I/K/J/L/U/O
glm::vec3 arrowKeyInput(GLFWwindow* window); // Handles Up/Down/Left/Right
glm::vec3 cycleColor(float t);               // Calculates color given t

void framebuffer_size_callback(GLFWwindow* window, int width, int height);	    // Called on window resize
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); // Called on key press
unsigned int compileShader(unsigned int type, const char* source);		    // Compiles shader program
unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc);   // Compiles/links shader program


// TODO:: (2.3): declare your vertex shader and fragment shader source here.
//
// NOTE: I abstracted my shader source code into separate files.
//       Please reference /default.vert and /default.frag.

// TODO: (2.2): declare whatever state your mesh needs.
//
// NOTE: I abstraced VAO, VBO, and EBO into their own classes.
//       Please reference /gl_objects.h.
class Mesh {
public:
 
    std::vector<float>        vertices;
    std::vector<unsigned int> indices;

    std::unique_ptr<VAO>      vao;
    std::unique_ptr<VBO>      vbo;
    std::unique_ptr<EBO>      ebo;
};


// TODO: (2.4/2.5/2.6): declare whatever state your input handling needs.
// GLOBALS 
const unsigned int SCR_WIDTH = 800;  // window width in pixels
const unsigned int SCR_HEIGHT = 600; // window height in pixels

auto colorInput       = glm::vec3(1.0f, 1.0f, 1.0f);   // Starting cube color
float t		      = 0.0f;			       // Parameter for cycleColor function
bool smoothColorCycle = false;			       // Toggle smooth color cycle
auto lightPos         = glm::vec3(-8.0f, 15.0f, 8.0f); // Starting light position
auto bgColor	      = glm::vec4(0.1f, 0.1f, 0.2f, 1.0f);// Starting background color

// -----------------------------------------------------------------------------
// PLATFORM DETECTION (Linux/WSL only -- a no-op on Windows/macOS)
// -----------------------------------------------------------------------------
bool isRunningUnderWSL() {
    if (std::getenv("WSL_DISTRO_NAME") != nullptr) return true;
    if (std::getenv("WSL_INTEROP") != nullptr) return true;

    std::ifstream versionFile("/proc/version");
    if (versionFile) {
        std::string contents((std::istreambuf_iterator<char>(versionFile)), std::istreambuf_iterator<char>());
        for (char& c : contents) 
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if (contents.find("microsoft") != std::string::npos) return true;
    }
    return false;
}
//===================================================================================================================

int main() {

    // ---- Step 0: steer GLFW away from WSLg's buggy Wayland backend -----
#if defined(GLFW_VERSION_MAJOR) && (GLFW_VERSION_MAJOR > 3 || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4))
    if (isRunningUnderWSL() && glfwPlatformSupported(GLFW_PLATFORM_X11)) {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    }
#endif

    // ---- Step 1: create a window + OpenGL context via GLFW -------------
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Ask for an OpenGL 3.3 Core Profile context, same as Assignment 0.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // required for macOS

    // TODO: (2.1): pass your titleString.c_str() as the window title below.
    // Open GLFW window
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, titleString.c_str(), nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Register our callbacks: GLFW calls these automatically
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // Step 2: Load OpenGL functions with GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // TODO: (2.3): compile + link your shaders here.
    Shader shaderProgram("default.vert", "default.frag");

    glEnable(GL_DEPTH_TEST); // Enable depth testing

    // TODO: (2.2): build your cube's vertex/incex data and upload it to the GPU.
    GLfloat vertices[] = {

    // POSITION            / COLOR             / TEXCOORD  / NORMALS
    // Front Face (Z = 1.0f)
    -1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  0.0f,  1.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f,  0.0f,  1.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  0.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f,  0.0f,  1.0f,

    // Back Face (Z = -1.0f)
    -1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f,  0.0f, -1.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  0.0f, -1.0f,

    // Left Face (X = -1.0f)
    -1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
    -1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,

    // Right Face (X = 1.0f)
     1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   1.0f,  0.0f,  0.0f,

    // Top Face (Y = 1.0f)
    -1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
     1.0f,  1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
     1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
    -1.0f,  1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f,  1.0f,  0.0f,

    // Bottom Face (Y = -1.0f)
    -1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
     1.0f, -1.0f, -1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
     1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
    -1.0f, -1.0f,  1.0f,   1.0f, 1.0f, 1.0f,   0.0f, 1.0f,   0.0f, -1.0f,  0.0f
};

// Indices for vertices order (6 indices per face * 6 faces = 36 total)
GLuint indices[] = {
    0, 1, 2,     0, 2, 3,    // Front
    4, 5, 6,     4, 6, 7,    // Back
    8, 9, 10,    8, 10, 11,  // Left
    12, 13, 14,  12, 14, 15, // Right
    16, 17, 18,  16, 18, 19, // Top
    20, 21, 22,  20, 22, 23  // Bottom
};

    VAO VAO1;    // Generate VAO
    VAO1.Bind(); // Bind VAO1
    VBO VBO1(vertices, sizeof(vertices)); // Generates VBO and links it to vertices
    EBO EBO1(indices, sizeof(indices));   // Generates EBO and links it to indices
    GLsizei stride = 11 * sizeof(float);  // Set stride: number of elements per vertex
    
    // Links VBO attributes to VAO1
    VAO1.LinkAttrib(VBO1, 0, 3, GL_FLOAT, stride, (void*)0);		       // 1. Position
    VAO1.LinkAttrib(VBO1, 1, 3, GL_FLOAT, stride, (void*)(3 * sizeof(float))); // 2. Color    ~unused for now
    VAO1.LinkAttrib(VBO1, 2, 2, GL_FLOAT, stride, (void*)(6 * sizeof(float))); // 3. TexCoord ~unused for now
    VAO1.LinkAttrib(VBO1, 3, 3, GL_FLOAT, stride, (void*)(8 * sizeof(float))); // 4. Normals
    VAO1.Unbind(); // Unbinds the VAO
    VBO1.Unbind(); // Unbinds the VBO
    EBO1.Unbind(); // Unbinds the EBO

    //-------------------------------------------------------------------------------------------------
    // Frame state variables
    float currentTime	= glfwGetTime();    // Time of current frame
    float prevTime	= currentTime;	    // Time of previous frame
    float deltaTime	= 0.0f;		    // Time since last frame
    float rotationSpeed = 65.0f;	    // Degrees per second
    float movementSpeed = 10.0f;		    // Model units per second
    auto  rotationAxis  = glm::vec3(0.0f, 0.0f, 0.0f); // 3D axis of rotation (normalized)
    auto  lightMovement = glm::vec3(0.0f, 0.0f, 0.0f); // 3D vector of movement (normalized)

    // Initializes matrices
    auto model	    = glm::mat4(1.0f); // Model matrix: cube
    auto view	    = glm::mat4(1.0f); // View matrix : camera
    auto proj	    = glm::mat4(1.0f); // Projection matrix: perspective
    auto lightModel = glm::mat4(1.0f); // Model matrix: light

    // Transform matrices
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
    model = glm::translate(model, glm::vec3(0.0f, 0.75f, 0.0f));
    view  = glm::translate(view, glm::vec3(0.0f, -0.5f, -2.0f));
    proj  = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / SCR_HEIGHT, 0.1f, 100.0f);
    lightModel = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, -1.0f, 0.0f));

    //===================================================================================================
    //
    // 6. Render Loop
    // --------------------------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window)) {
    // TODO:: poll any continuously-held keys here.  

	// Tick the clock
	currentTime = glfwGetTime();
	deltaTime   = currentTime - prevTime;
	prevTime    = currentTime;
	
	// Process 
	rotationAxis = arrowKeyInput(window);
	float angle = rotationSpeed * glm::length(rotationAxis) * deltaTime;

	lightMovement = lightPosInput(window);
	lightPos = lightPos + (lightMovement * movementSpeed * deltaTime);

	if (smoothColorCycle) {
	    t += 0.01;
	    colorInput = cycleColor(t);
	}
	
        glClearColor(bgColor.r, bgColor.g, bgColor.b, bgColor.a); // Set the clear color
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	  // Clear the screen
	//
        // TODO: (2.3/2.5/2.6/2.7): use your shader program, compute and
        // upload this frame's transform/color/light uniforms, bind your
        // cube's VAO, and issue the draw call. This is the part of the demo's
        // render loop that was specific to drawing letters -- yours will be
        // specific to drawing (and rotating, and recoloring, and relighting)
        // your cube instead.
		
       shaderProgram.Activate();

	// Assigns different transformations to each matrix
	if (glm::length(rotationAxis) > 0.0f)
	    model = glm::rotate(model, glm::radians(angle), glm::normalize(rotationAxis));

	// Outputs the matrices into the Vertex Shader
	int modelLoc = glGetUniformLocation(shaderProgram.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
	int viewLoc = glGetUniformLocation(shaderProgram.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projLoc = glGetUniformLocation(shaderProgram.ID, "proj");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	// Set Uniform: colorInput
	int colorInputLoc = glGetUniformLocation(shaderProgram.ID, "colorInput");
	glUniform3f(colorInputLoc, colorInput.r, colorInput.g, colorInput.b);
	
	// Set Uniform: lightPos
	int lightPosLoc = glGetUniformLocation(shaderProgram.ID, "lightPos");
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);

	// Set Uniform: isEmissive
	int isEmissiveLoc = glGetUniformLocation(shaderProgram.ID, "isEmissive");
	glUniform1i(isEmissiveLoc, false);

	VAO1.Bind(); // Bind the VAO
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);

	// Draw call for light	
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::translate(lightModel, lightPos)));
	glUniform3f(colorInputLoc, 1.0f, 1.0f, 1.0f);
	glUniform1i(isEmissiveLoc, true);
	glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(int), GL_UNSIGNED_INT, 0);
    
        glfwSwapBuffers(window); // Swap front and back buffers
        glfwPollEvents();        // Poll for and process events
    }

    //-----------------------------------------------------------------------------------------
    // CLEANUP --  TODO:: delete whatever VAOs/VBOs/EBOs and shader program you created.
    VAO1.Delete();
    VBO1.Delete();
    EBO1.Delete();
    shaderProgram.Delete();
    glfwTerminate();

    //-----------------------------------------------------------------------------------------
    return 0;
}

//-----------------------------------------------------------------------------------------
// WINDOW RESIZE CALLBACK
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------------------
// KEY CALLBACK
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    // TODO:  (2.6): handle I/K/J/L/U/O to move the light
    // TODO:  (2.7): handle Up/Down/Left/Right to rotate the cube
    
     if (action == GLFW_PRESS || action == GLFW_REPEAT) {
	
	// TODO:  (2.4): handle ESC to close the window
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

	// TODO:  (2.5): handle C to cycle color
        if (key == GLFW_KEY_C) {
            // Cycle color
            colorInput = cycleColor(t++);
        }

        // TODO: (Extra Credit): handle V to toggle smooth color cycle
        if (key == GLFW_KEY_V) {
            // Cycle color
            smoothColorCycle = !smoothColorCycle;
        }
    }
}
//-----------------------------------------------------------------------------------------

// -----------------------------------------------------------------------------
// SHADER COMPILE HELPERS
// -----------------------------------------------------------------------------
// Carried over from the Assignment 0 demo, unchanged -- this is reusable
// boilerplate, not something specific to this assignment's mesh. You WILL
// need to call createShaderProgram() with your own shader source strings
// (Section 2.3) -- that's the assignment-specific part.
//
// ALWAYS check compile/link status like this. When you get a blank screen
// later, this is almost always where the answer is -- read the console
// output before you touch anything else.

unsigned int compileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shader;
}

unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc) {
    unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return program;
}
//==========================================================================================
