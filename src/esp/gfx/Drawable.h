// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "esp/core/esp.h"
#include "esp/gfx/DrawableGroup.h"
#include "esp/gfx/Shader.h"
#include "magnum.h"

namespace esp {
namespace scene {
class SceneNode;
}
namespace gfx {

class DrawableGroupClient;

/**
 * @brief Drawable for use with @ref DrawableGroup.
 *
 * Drawable will retrieve its shader program from its group, and draw
 * itself with the program.
 */
class Drawable : public Magnum::SceneGraph::Drawable3D {
  friend DrawableGroupClient;

 public:
  /**
   * @brief Constructor
   *
   * @param node Node which is now drawable.
   * @param mesh Mesh which will be drawn when this drawable is rendered.
   * @param group Drawable group this drawable will be added to.
   */
  Drawable(scene::SceneNode& node,
           Magnum::GL::Mesh& mesh,
           DrawableGroup* group = nullptr);
  virtual ~Drawable() {}

  virtual scene::SceneNode& getSceneNode() { return node_; }

 protected:
  /**
   * @brief Draw the object using given camera and group's shader
   */
  virtual void draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera) override;

  /**
   * @brief Draw the object using given camera and shader
   *
   * @param transformationMatrix  Transformation relative to camera.
   * @param camera                Camera to draw from.
   * @param shader                Shader to draw with.
   *
   * Each derived drawable class needs to implement this draw() function. It's
   * nothing more than setting up shader parameters and drawing the mesh.
   * TODO: Remove this and do shader setup inside ShaderProgram subclasses.
   */
  virtual void draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera,
                    Shader* shader) = 0;

  scene::SceneNode& node_;
  DrawableGroup* group_;
  Magnum::GL::Mesh& mesh_;
};

/**
 * @brief Expose group membership to DrawableGroups (Attorney-Client pattern)
 */
class DrawableGroupClient {
 private:
  static DrawableGroup* getGroup(Drawable& d) { return d.group_; }
  static void setGroup(Drawable& d, DrawableGroup* g) { d.group_ = g; }
  friend class DrawableGroup;
};

}  // namespace gfx
}  // namespace esp
