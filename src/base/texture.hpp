#pragma once
#include "../utils/matrix.hpp"
#include "../utils/pattern.hpp"
#include "../utils/utility.hpp"
#include <string>

enum class LERP_MODE { NEAREST, BILINEAR, CORNER };

class Texture2D {
public:
  Texture2D() = default;
  ~Texture2D() = default;
  virtual gl::vec3 getTexelColor(float u, float v,
                                 LERP_MODE mode = LERP_MODE::BILINEAR) = 0;
};

class ConstantTexture : public Texture2D {
public:
  ConstantTexture() = default;
  ConstantTexture(gl::vec3 color) : _color(color){};
  ~ConstantTexture() = default;
  gl::vec3 getTexelColor(float u, float v,
                         LERP_MODE mode = LERP_MODE::BILINEAR) override {
    return this->_color;
  }

private:
  gl::vec3 _color;
};

class ImageTexture : public Texture2D {
public:
  ImageTexture() = default;
  // by default rgb
  ImageTexture(const std::string &filename, bool flip_y = false);
  ~ImageTexture() = default;
  gl::vec3 getTexelColor(float u, float v,
                         LERP_MODE mode = LERP_MODE::BILINEAR) override;

private:
  std::string _path;
  std::vector<std::vector<gl::vec3>> texels;
  uint _width;
  uint _height;
};

class CheckerTexture : public Texture2D {
public:
  CheckerTexture(float scale = 10.f);
  CheckerTexture(gl::vec3 color1, gl::vec3 color2, float scale = 10.f);
  ~CheckerTexture() = default;
  gl::vec3 getTexelColor(float u, float v,
                         LERP_MODE mode = LERP_MODE::BILINEAR) override;

private:
  gl::vec3 _color1;
  gl::vec3 _color2;
  float _scale;
};

class NoiseTexture : public Texture2D {
public:
  NoiseTexture() = default;
  NoiseTexture(float scale, int fractal = 1)
      : _scale(scale), _fractal(fractal){};
  ~NoiseTexture() = default;
  gl::vec3 getTexelColor(float u, float v,
                         LERP_MODE mode = LERP_MODE::BILINEAR) override;

private:
  float _scale;
  int _fractal;
};
