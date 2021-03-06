#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION

#include <stb_image.h>
#include <array>
#include "Shader.hpp"

// Stored globally so it can be modified in framebufferSizeCallback() and used in main()
glm::mat4 projection;

// The field of view
constexpr float FOV = 50.0f;

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  projection = glm::perspective(glm::radians(FOV), (float) width / (float) height, 0.1f, 100.0f);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }
}

uint32_t loadTexture(char const *path) {
  uint32_t textureID;
  glGenTextures(1, &textureID);

  int32_t width, height, nrComponents;
  unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
  if (data) {
    GLenum format;

    switch (nrComponents) {
      case 1:
        format = GL_RED;
        break;
      case 3:
        format = GL_RGB;
        break;
      case 4:
        format = GL_RGBA;
        break;
      default:
        format = GL_RGB;
    }

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
  } else {
    std::cout << "ERROR: Texture failed to load at path: " << path << std::endl;
    stbi_image_free(data);
  }

  return textureID;
}

int32_t main() {
  // The initial window size
  constexpr int32_t SCREEN_WIDTH = 1280;
  constexpr int32_t SCREEN_HEIGHT = 720;

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow *window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Multiple Lights",
                                        nullptr,
                                        nullptr);
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

  if (window == nullptr) {
    std::cout << "Failed to create GLFW window." << std::endl;
    glfwTerminate();
    return -1;
  }

  // GLFW doesn't mark the context as current automatically
  // <https://stackoverflow.com/questions/48650497/glad-failing-to-initialize>
  glfwMakeContextCurrent(window);

  if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
    std::cout << "ERROR: Failed to initialize GLAD." << std::endl;
    return -1;
  }

  glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

  Shader containerShader = Shader(
      "../resources/shaders/multiple_lights.vertex.glsl",
      "../resources/shaders/multiple_lights.fragment.glsl"
  );

  // The cube's vertice and normal coordinates
  float vertices[] = {
      // Positions          // Normals           // Texture coordinates
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
      0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
      -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
      0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
      -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
      -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
  };

  // Cube positions
  glm::vec3 cubePositions[] = {
      glm::vec3(-2.0f, 0.0f, -3.0f),
      glm::vec3(2.0f, 0.0f, -3.0f),
      glm::vec3(2.0f, -2.0f, 3.0f),
      glm::vec3(2.0f, -2.5f, -3.0f),
      glm::vec3(-2.0f, 3.0f, -4.0f),
      glm::vec3(-1.0f, 1.0f, -5.0f),
      glm::vec3(1.0f, -1.0f, -6.0f),
      glm::vec3(-2.0f, 3.5f, -7.0f),
      glm::vec3(-2.0f, -1.0f, -10.0f),
      glm::vec3(0.0f, 0.0f, -1.0f),
  };

  // Point light definitions
  std::array<std::array<glm::vec3, 2>, 4> pointLights = {{
      {
          glm::vec3(0.7f, 0.2f, 2.0f),   // Position
          glm::vec3(1.0f, 0.25f, 0.25f), // Color
      },
      {
          glm::vec3(2.3f, -3.3f, -4.0f),
          glm::vec3(0.25f, 1.0f, 0.25f),
      },
      {
          glm::vec3(-4.0f, 2.0f, -12.0f),
          glm::vec3(0.25f, 0.5f, 1.0f),
      },
      {
          glm::vec3(0.0f, 0.0f, -3.0f),
          glm::vec3(1.0f, 1.0f, 1.0f),
      },
  }};

  // Load textures
  unsigned int textureDiffuse = loadTexture("../resources/textures/container2_diffuse.png");
  unsigned int textureSpecular = loadTexture("../resources/textures/container2_specular.png");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textureDiffuse);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textureSpecular);

  glEnable(GL_DEPTH_TEST);

  // Initialize buffers (vertex array, vertex buffer, element buffer)
  uint32_t vao, vbo;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  // Bind buffers
  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  // Copy vertex data into the VBO
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // Set vertex attribute parameters
  // Position attribute
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) nullptr);
  glEnableVertexAttribArray(0);
  // Normal attribute
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
  glEnableVertexAttribArray(1);
  // Texture coordinates attribute
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
  glEnableVertexAttribArray(2);

  // Set the projection matrix here so it's defined on application start too
  projection = glm::perspective(glm::radians(FOV), (float) SCREEN_WIDTH / (float) SCREEN_HEIGHT,
                                0.1f, 100.0f);

  while (!glfwWindowShouldClose(window)) {
    processInput(window);

    // Clear the viewport with a constant color
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Make the camera rotate in a circle around the center point
    float radius = 3.0f;
    double cameraX = sin(glfwGetTime() * 0.75f) * radius;
    double cameraZ = cos(glfwGetTime() * 0.75f) * radius;
    glm::vec3 cameraPosition = glm::vec3(cameraX, 0.0f, cameraZ);
    glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
    // Use an "up" vector to determine the camera's right axis using a cross product
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::mat4 view;
    view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

    containerShader.use();
    containerShader.setMat4("view", view);
    containerShader.setMat4("projection", projection);
    containerShader.setVec3("viewPos", cameraPosition);

    // Set material properties
    // Set diffuse and specular to the appropriate texture ID
    containerShader.setInt("material.diffuse", 0);
    containerShader.setInt("material.specular", 1);
    containerShader.setFloat("material.glossiness", 24.0f);

    // Set directional light properties
    containerShader.setVec3("directionalLight.direction", -0.2f, -1.0f, -0.3f);
    containerShader.setVec3("directionalLight.ambient", 0.08f, 0.08f, 0.08f);
    containerShader.setVec3("directionalLight.diffuse", 0.5f, 0.5f, 0.5f);
    containerShader.setVec3("directionalLight.specular", 0.5f, 0.5f, 0.5f);

    // Set point light properties
    for (int32_t pointLight = 0; pointLight < pointLights.size(); pointLight++) {
      std::string positionUniform = "pointLights[" + std::to_string(pointLight) + "].position";
      std::string constantUniform = "pointLights[" + std::to_string(pointLight) + "].constant";
      std::string linearUniform = "pointLights[" + std::to_string(pointLight) + "].linear";
      std::string quadraticUniform = "pointLights[" + std::to_string(pointLight) + "].quadratic";
      std::string ambientUniform = "pointLights[" + std::to_string(pointLight) + "].ambient";
      std::string diffuseUniform = "pointLights[" + std::to_string(pointLight) + "].diffuse";
      std::string specularUniform = "pointLights[" + std::to_string(pointLight) + "].specular";

      containerShader.setVec3(positionUniform, pointLights[pointLight][0]);
      containerShader.setFloat(constantUniform, 1.0f);
      containerShader.setFloat(linearUniform, 0.14f);
      containerShader.setFloat(quadraticUniform, 0.07f);
      containerShader.setVec3(ambientUniform, 0.0f, 0.0f, 0.0f);
      containerShader.setVec3(diffuseUniform, pointLights[pointLight][1]);
      containerShader.setVec3(specularUniform, pointLights[pointLight][1]);
    }

    // Set spot light properties
    containerShader.setVec3("spotLight.position", cameraPosition);
    glm::vec3 spotLightDirection = -cameraPosition;
    containerShader.setVec3("spotLight.direction", spotLightDirection);
    containerShader.setFloat("spotLight.innerCutoff", glm::cos(glm::radians(5.0f)));
    containerShader.setFloat("spotLight.outerCutoff", glm::cos(glm::radians(10.0f)));
    containerShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
    containerShader.setVec3("spotLight.diffuse", 0.7f, 0.7f, 0.7f);
    containerShader.setVec3("spotLight.specular", 0.7f, 0.7f, 0.7f);

    // Draw container cubes
    glBindVertexArray(vao);

    for (uint32_t i = 0; i < 10; i++) {
      glm::mat4 model;
      model = glm::translate(model, cubePositions[i]);
      float angle = 20.0f * i;
      model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
      containerShader.setMat4("model", model);
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clean up
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glfwTerminate();

  return 0;
}
