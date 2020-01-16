// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "esp/core/esp.h"
#include "esp/gfx/DrawableGroup.h"
#include "magnum.h"

namespace esp {
namespace scene {
class SceneNode;
}
namespace gfx {

class Drawable : public Magnum::SceneGraph::Drawable3D {
 public:
  Drawable(scene::SceneNode& node,
           Magnum::GL::Mesh& mesh,
           DrawableGroup* group = nullptr);
  virtual ~Drawable() {}

  virtual scene::SceneNode& getSceneNode() { return node_; }

 protected:
  virtual void draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera) override;

  // Each derived drawable class needs to implement this draw() function. It's
  // nothing more than setting up shader parameters and drawing the mesh.
  virtual void draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera,
                    Shader& shader) = 0;

  scene::SceneNode& node_;
  Magnum::GL::AbstractShaderProgram& shader_;
  Magnum::GL::Mesh& mesh_;
};

}  // namespace gfx
}  // namespace esp
