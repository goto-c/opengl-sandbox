#include "ogls/texture.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ogls {

Texture::Texture() {
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_LINEAR);
  glBindTexture(GL_TEXTURE_2D, 0);
}

Texture::Texture(const std::string& filepath, const TextureType& textureType)
    : Texture() {
  this->filepath = filepath;
  this->textureType = textureType;
  loadImage(filepath);
}

void Texture::destroy() { glDeleteTextures(1, &id); }

void Texture::loadImage(const std::string& filepath) const {
  // load image with stb_image
  int width, height, channels;
  unsigned char* image =
      stbi_load(filepath.c_str(), &width, &height, &channels, 3);

  if (!image) {
    std::cerr << "failed to open " << filepath << std::endl;
    return;
  }

  // send image to texture
  glBindTexture(GL_TEXTURE_2D, id);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
               GL_UNSIGNED_BYTE, image);
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);

  // free memory
  stbi_image_free(image);
}

}  // namespace ogls