// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "esp/core/esp.h"
#include "magnum.h"

#include <Magnum/GL/Renderer.h>
#include <Magnum/Shaders/MeshVisualizer.h>

namespace esp {
namespace scene {
class SceneNode;
}
namespace gfx {

class Drawable : public Magnum::SceneGraph::Drawable3D {
 public:
  Drawable(scene::SceneNode& node,
           Magnum::GL::AbstractShaderProgram& shader,
           Magnum::GL::Mesh& mesh,
           Magnum::SceneGraph::DrawableGroup3D* group = nullptr);
  virtual ~Drawable() {}

  virtual scene::SceneNode& getSceneNode() { return node_; }

 protected:
  // Each derived drawable class needs to implement this draw() function. It's
  // nothing more than setting up shader parameters and drawing the mesh.
  virtual void draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera) = 0;

  scene::SceneNode& node_;
  Magnum::GL::AbstractShaderProgram& shader_;
  Magnum::GL::Mesh& mesh_;
};

class WireframeDrawable : public Drawable {
 public:
  WireframeDrawable(scene::SceneNode& node,
                    Magnum::GL::AbstractShaderProgram& shader,
                    Magnum::GL::Mesh& mesh,
                    Magnum::SceneGraph::DrawableGroup3D* group = nullptr)
      : Drawable{node, shader, mesh, group} {};

 protected:
  void draw(const Magnum::Matrix4& transformationMatrix,
            Magnum::SceneGraph::Camera3D& camera) override {
    Magnum::Shaders::MeshVisualizer& shader =
        static_cast<Magnum::Shaders::MeshVisualizer&>(shader_);
    shader.setTransformationProjectionMatrix(camera.projectionMatrix() *
                                             transformationMatrix);

    shader.setViewportSize(Magnum::Vector2{camera.viewport()});

    Magnum::GL::Renderer::enable(Magnum::GL::Renderer::Feature::Blending);
    mesh_.draw(shader_);
    Magnum::GL::Renderer::disable(Magnum::GL::Renderer::Feature::Blending);
  }
};

}  // namespace gfx
}  // namespace esp
