#ifndef _RTGS_SHADER_H
#define _RTGS_SHADER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <variant>
#include <vector>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace rtgs {

class Shader {
 private:
  const std::string vertexShaderFilepath;
  std::string vertexShaderSource;
  const std::string fragmentShaderFilepath;
  std::string fragmentShaderSource;
  GLuint vertexShader;
  GLuint fragmentShader;
  GLuint program;

  // convert file content to string
  static std::string fileToString(const std::string& filepath);

  void compileShader();
  void linkShader();

 public:
  Shader();

  // load vertex shader and fragment shader from given filepath
  Shader(const std::string& vertexShaderFilepath,
         const std::string& fragmentShaderFilepath);

  // destroy shader object
  void destroy();

  // activate shader on the currect context
  void activate() const;
  // deactivate shader on the currect context
  void deactivate() const;

  // set value on shader's uniform variable
  void setUniform(const std::string& uniformName,
                  const std::variant<bool, GLint, GLuint, GLfloat, glm::vec2,
                                     glm::vec3, glm::mat4>& value) const;

  // set texture on shader's sampler2D uniform variable
  void setUniformTexture(const std::string& uniformName, GLuint texture,
                         GLuint textureUnitNumber) const;

  // set uniform buffer object on shader
  void setUBO(const std::string& blockName, GLuint bindingNumber) const;
};

}  // namespace rtgs

#endif