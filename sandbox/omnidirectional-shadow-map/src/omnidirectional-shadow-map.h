#pragma once
#include <filesystem>
#include <string>

#include "glad/glad.h"
#include "glm/glm.hpp"
//
#include "scene.hpp"
#include "shader.hpp"

using namespace ogls;

class OmnidirectionalShadowMap
{
 public:
  int width;
  int height;
  glm::vec3 lightPosition;
  float zNear;
  float zFar;

  GLuint FBO;
  GLuint cubemap;

  Shader vertex_shader;
  Shader geometry_shader;
  Shader fragment_shader;
  Pipeline pipeline;

  OmnidirectionalShadowMap(int width, int height)
      : width(width), height(height), zNear(0.1f), zFar(10000.0f)
  {
    // setup shader
    vertex_shader = Shader::createVertexShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
        "shaders/shadow-map.vert");
    geometry_shader = Shader::createGeometryShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
        "shaders/shadow-map.geom");
    fragment_shader = Shader::createFragmentShader(
        std::filesystem::path(CMAKE_CURRENT_SOURCE_DIR) /
        "shaders/shadow-map.frag");
    pipeline.attachVertexShader(vertex_shader);
    pipeline.attachGeometryShader(geometry_shader);
    pipeline.attachFragmentShader(fragment_shader);

    // setup shadow map FBO
    glGenFramebuffers(1, &FBO);

    // setup cubemap
    glGenTextures(1, &cubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (unsigned int i = 0; i < 6; ++i) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                   width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // attach shadow map texture to FBO
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, cubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }

  void destroy()
  {
    glDeleteTextures(1, &cubemap);
    glDeleteFramebuffers(1, &FBO);
  }

  void setResolution(int width, int height)
  {
    this->width = width;
    this->height = height;

    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
    for (unsigned int i = 0; i < 6; ++i) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                   width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
  }

  void setLightPosition(const glm::vec3& lightPosition)
  {
    this->lightPosition = lightPosition;
  }

  void draw(const Scene& scene) const
  {
    // render to shadow map
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_FRONT);  // prevent peter panning

    // set uniforms
    const glm::mat4 projection = glm::perspective(
        glm::radians(90.0f), static_cast<float>(width) / height, zNear, zFar);
    geometry_shader.setUniform(
        "lightSpaceMatrix[0]",
        projection * glm::lookAt(lightPosition,
                                 lightPosition + glm::vec3(1, 0, 0),
                                 glm::vec3(0, -1, 0)));
    geometry_shader.setUniform(
        "lightSpaceMatrix[1]",
        projection * glm::lookAt(lightPosition,
                                 lightPosition + glm::vec3(-1, 0, 0),
                                 glm::vec3(0, -1, 0)));
    geometry_shader.setUniform(
        "lightSpaceMatrix[2]",
        projection * glm::lookAt(lightPosition,
                                 lightPosition + glm::vec3(0, 1, 0),
                                 glm::vec3(0, 0, 1)));
    geometry_shader.setUniform(
        "lightSpaceMatrix[3]",
        projection * glm::lookAt(lightPosition,
                                 lightPosition + glm::vec3(0, -1, 0),
                                 glm::vec3(0, 0, -1)));
    geometry_shader.setUniform(
        "lightSpaceMatrix[4]",
        projection * glm::lookAt(lightPosition,
                                 lightPosition + glm::vec3(0, 0, 1),
                                 glm::vec3(0, -1, 0)));
    geometry_shader.setUniform(
        "lightSpaceMatrix[5]",
        projection * glm::lookAt(lightPosition,
                                 lightPosition + glm::vec3(0, 0, -1),
                                 glm::vec3(0, -1, 0)));

    fragment_shader.setUniform("lightPosition", lightPosition);
    fragment_shader.setUniform("zFar", zFar);

    // render
    scene.draw(pipeline, fragment_shader);

    glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }
};