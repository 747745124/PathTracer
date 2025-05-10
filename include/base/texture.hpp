#pragma once
#include "utils/matrix.hpp"
#include "utils/pattern.hpp"
#include "utils/utility.hpp"
#include <string>
#include <variant>
#include <memory>

class Texture2D;
class ConstantTexture;

enum class LERP_MODE
{
  NEAREST,
  BILINEAR,
  CORNER
};

class Texture2D
{
public:
  Texture2D() = default;
  ~Texture2D() = default;
  virtual gl::vec3 getTexelColor(float u, float v,
                                 LERP_MODE mode = LERP_MODE::BILINEAR) = 0;
  virtual gl::vec3 getTexelColor(gl::vec2 uv,
                                 LERP_MODE mode = LERP_MODE::BILINEAR)
  {
    return this->getTexelColor(uv.x(), uv.y(), mode);
  }
};

class ConstantTexture : public Texture2D
{
public:
  ConstantTexture() = default;
  ConstantTexture(const gl::vec3 color) : _color(color) {};
  ~ConstantTexture() = default;
  gl::vec3 getTexelColor(float u, float v,
                         LERP_MODE mode = LERP_MODE::BILINEAR) override
  {
    return this->_color;
  }

private:
  gl::vec3 _color;
};

class ImageTexture : public Texture2D
{
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

class CheckerTexture : public Texture2D
{
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

class NoiseTexture : public Texture2D
{
public:
  NoiseTexture() = default;
  NoiseTexture(float scale, int fractal = 1)
      : _scale(scale), _fractal(fractal) {};
  ~NoiseTexture() = default;
  gl::vec3 getTexelColor(float u, float v,
                         LERP_MODE mode = LERP_MODE::BILINEAR) override;

private:
  float _scale;
  int _fractal;
};

using ColorVariant = std::variant<gl::vec3, std::shared_ptr<Texture2D>>;
namespace gl::texture
{
  inline std::shared_ptr<Texture2D>
  to_texture2d(const ColorVariant &color_like)
  {
    return std::visit(
        [](const auto &color_arg) -> std::shared_ptr<Texture2D>
        {
          if constexpr (std::is_same_v<std::decay_t<decltype(color_arg)>, gl::vec3>)
          {
            return std::make_shared<ConstantTexture>(color_arg);
          }
          else
          { // This branch is taken if color_arg is const std::shared_ptr<Texture2D>&
            if (!color_arg)
            {
              // Handle the case where a nullptr std::shared_ptr<Texture2D> was passed in the variant
              // Option: return a default texture, throw, or allow nullptr to propagate.
              // Propagating nullptr is often acceptable, caller must check.
              // For example, returning a default black texture:
              // return std::make_shared<ConstantTexture>(gl::vec3{0.0f, 0.0f, 0.0f});
              // Or simply return the null pointer:
              return nullptr;
            }
            return color_arg; // Returns the std::shared_ptr<Texture2D>
          }
        },
        color_like);
  };
};