// =============================================================================
// CPSC 484 - Assignment 1 - OpenGL Mesh Viewer (STARTER)
// =============================================================================
// This file is NOT a fill-in-the-blanks starter. It gives you the parts that
// are the same in literally every OpenGL program -- opening a window and
// getting a context (Step 1), loading OpenGL's function pointers (Step 2),
// and the compile/link utility functions for turning GLSL text into a usable
// shader program -- because none of that is specific to THIS assignment, and
// you already built it once, understanding every line, in the Assignment 0
// demo (cpsc484_a1_demo_letterviewer.cpp / its line-by-line commented
// twin). Re-typing it here wouldn't teach you anything new.
//
// Everything below a "// TODO (2.x)" comment is yours to write from scratch.
// There are no predefined variables named cubeColor or lightPos waiting for
// you to fill in a value -- you decide what state your program needs and how
// to store it, the same way you'll have to on every assignment after this
// one. The section numbers in the TODOs match the Assignment 1 Instructions
// document; that's where the actual requirements and grading weights live.
// This file only tells you WHERE things go, not WHAT to write.
//
// Before you write a single line here, make sure you can explain -- to
// yourself, out loud -- every piece of the Assignment 0 demo: why GLFW hints
// have to be set before glfwCreateWindow(), what a VAO records versus what a
// VBO holds, why glVertexAttribPointer's stride/offset have to match your
// vertex layout exactly, and why compileShader() checks GL_COMPILE_STATUS.
// If any of those feel shaky, that demo (not this file) is where to go back
// to. This assignment assumes you already own that material.
// =============================================================================

#include "glad.h"          // OpenGL function loader -- must be included before glfw3.h
#include <GLFW/glfw3.h>    // window/context creation, input, timing
#include <cstdlib>         // std::getenv -- used by isRunningUnderWSL() below
#include <fstream>         // std::ifstream -- used by isRunningUnderWSL() below
#include <iostream>        // std::cerr / std::cout for error and debug messages
#include <string>          // std::string -- used by isRunningUnderWSL() below, and by the titleString you'll add next

// TODO (2.1): declare your own window-title string here. See Assignment 1
// Instructions, Section 2.1. Something like:
//     std::string titleString = "Fall 2026 - Assignment 1 - <Your Full Name>";

// -----------------------------------------------------------------------------
// FUNCTION PROTOTYPES
// C++ requires a function to be declared before it's used. main() (further
// down) calls these, so they're declared here and defined later in the file.
// -----------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height); // called by GLFW whenever the window is resized
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods); // called by GLFW on every key press/release/repeat
unsigned int compileShader(unsigned int type, const char* source);         // compiles one GLSL shader, returns its OpenGL ID
unsigned int createShaderProgram(const char* vertexSrc, const char* fragmentSrc); // compiles + links both shaders into one usable program

// Screen dimensions -- passed to glfwCreateWindow() below. Feel free to
// change these, but keep them as named constants rather than magic numbers
// scattered through the file.
const unsigned int SCR_WIDTH = 800;  // window width in pixels
const unsigned int SCR_HEIGHT = 600; // window height in pixels

// TODO (2.3): declare your vertex shader and fragment shader source here, as
// C++ raw string literals (see the Assignment 0 demo for the R"GLSL(...)GLSL"
// pattern and why it's safer than a plain R"(...)"). At minimum your vertex
// shader needs:
//   - a position input attribute and a normal input attribute
//   - a "transform" uniform (mat4) to place/rotate the mesh
// and your fragment shader needs:
//   - a "color" uniform (vec3) for the current cube color
//   - some simple ambient + diffuse shading using the light's position/
//     direction, so the cube's faces are visibly shaded differently rather
//     than being flat silhouettes (see the demo's fragment shader for one
//     way to do this -- yours doesn't have to match it exactly).
//
// const char* vertexShaderSource = R"GLSL(
// ...
// )GLSL";
//
// const char* fragmentShaderSource = R"GLSL(
// ...
// )GLSL";

// TODO (2.2): declare whatever state your mesh needs. At minimum you'll want
// somewhere to put your vertex data (positions + normals) and your index
// data once you've decided on a layout -- see Section 2.2 for the required
// float vertices[] / unsigned int indices[] shape. You'll also need VAO/VBO/
// EBO ids once you get to uploading that data to the GPU.

// TODO (2.4/2.5/2.6): declare whatever state your input handling needs to
// read and modify -- e.g. the cube's current color, a list of colors to
// cycle through, the light's position, and the mesh's current rotation
// angles. Nothing here is pre-named for you; pick names that make sense to
// you, since you're the one who has to keep using them.

// -----------------------------------------------------------------------------
// PLATFORM DETECTION (Linux/WSL only -- a no-op on Windows/macOS)
// -----------------------------------------------------------------------------
// Carried over from the Assignment 0 demo unchanged: under WSLg, GLFW's
// default Wayland backend has a known window-resize bug, so on WSL
// specifically we ask GLFW to use X11 instead. See the demo's own comments
// (right above its isRunningUnderWSL()) for the full explanation -- there's
// nothing assignment-specific to change here.
bool isRunningUnderWSL() {
    if (std::getenv("WSL_DISTRO_NAME") != nullptr) return true;
    if (std::getenv("WSL_INTEROP") != nullptr) return true;

    std::ifstream versionFile("/proc/version");
    if (versionFile) {
        std::string contents((std::istreambuf_iterator<char>(versionFile)),
                              std::istreambuf_iterator<char>());
        for (char& c : contents) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        if (contents.find("microsoft") != std::string::npos) return true;
    }
    return false;
}

/* *************************************************** */

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
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE); // required on macOS to get a core-profile context at all; harmless no-op on Windows/Linux

    // TODO (2.1): pass your titleString.c_str() as the window title below
    // instead of the placeholder "Assignment 1" literal.
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Assignment 1", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    // Register our callbacks. GLFW calls these automatically -- we never
    // call them ourselves.
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);

    // ---- Step 2: load OpenGL function pointers via GLAD ----------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // TODO (2.3): compile + link your shaders here, once vertexShaderSource
    // and fragmentShaderSource exist above.
    // unsigned int shaderProgram = createShaderProgram(vertexShaderSource, fragmentShaderSource);

    glEnable(GL_DEPTH_TEST); // near surfaces should hide far ones -- you'll want this once you have a 3D cube

    // TODO (2.2): build your cube's vertex/index data and upload it to the
    // GPU here (glGenVertexArrays / glGenBuffers / glBindBuffer /
    // glBufferData / glVertexAttribPointer / glEnableVertexAttribArray),
    // once you've declared the arrays and layout above. This happens once,
    // before the render loop -- not every frame.

    // ---- Step 6 (numbering matches the Assignment 0 demo): render loop --
    while (!glfwWindowShouldClose(window)) {
        // TODO: poll any continuously-held keys here, if you're using that
        // input style for anything (see the demo's processInput() for the
        // pattern, and its INPUT HANDLING comment block for when polling is
        // the right tool vs. when the key_callback below is).

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO (2.3/2.5/2.6/2.7): use your shader program, compute and
        // upload this frame's transform/color/light uniforms, bind your
        // cube's VAO, and issue the draw call. This is the part of the demo's
        // render loop that was specific to drawing letters -- yours will be
        // specific to drawing (and rotating, and recoloring, and relighting)
        // your cube instead.

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // ---- Cleanup ---------------------------------------------------------
    // TODO: delete whatever VAOs/VBOs/EBOs and shader program you created
    // above, the same way the Assignment 0 demo cleans up its letter
    // buffers and shader program before glfwTerminate().
    glfwTerminate();
    return 0;
}

// Called every time the window is resized.
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    (void)window;
    glViewport(0, 0, width, height);
}

// Called by GLFW whenever a key is pressed, released, or repeated.
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    (void)scancode;
    (void)mods;

    // TODO (2.4): handle ESC to close the window (see the Assignment 1
    // Instructions example), and add whatever other keys Sections 2.5
    // (color) and 2.6 (light position) and 2.7 (rotation) need. Remember:
    // action == GLFW_PRESS means "just went down this frame" -- check that
    // (or don't, depending on whether you want one-shot or repeat-while-
    // held behavior) the same way the Assignment 0 demo's key_callback does.
}

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
