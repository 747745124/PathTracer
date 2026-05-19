// ======================================================================== //
// Viewer.h - subclass of owl::viewer::OWLViewer that drives our Renderer.
//
// Responsibilities:
//   - open a GLFW window + manage a CUDA/GL-shared framebuffer texture
//   - route camera/resize events to Renderer
//   - call Renderer::render() each frame
// ======================================================================== //

#pragma once

#include "Renderer.h"
#include "owlViewer/OWLViewer.h"

#include <string>

namespace pt {

  class Viewer : public owl::viewer::OWLViewer {
  public:
    Viewer(Renderer &renderer,
           const owl::box3f &sceneBounds,
           const owl::vec2i &initialSize = owl::vec2i(1280, 720),
           bool visible = true,
           bool hasInitialCamera = false,
           const owl::vec3f &initialFrom = owl::vec3f(0.f),
           const owl::vec3f &initialAt = owl::vec3f(0.f),
           const owl::vec3f &initialUp = owl::vec3f(0.f, 1.f, 0.f),
           float initialFovyDegrees = 45.f);

    void render()        override;
    void resize(const owl::vec2i &newSize) override;
    void cameraChanged() override;
    void key(char key, const owl::vec2i &where) override;
    bool saveFrameBuffer(const std::string &path) const;

  private:
    Renderer &renderer_;
    owl::vec2i currentSize_ = owl::vec2i(0, 0);
  };

} // namespace pt
