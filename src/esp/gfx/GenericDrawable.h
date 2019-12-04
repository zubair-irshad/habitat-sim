// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <Magnum/Shaders/Shaders.h>

#include "Drawable.h"

namespace esp {
namespace gfx {

class GenericDrawable : public Drawable {
 public:
  //! Create a GenericDrawable for the given object using shader and mesh.
  //! Adds drawable to given group and uses provided texture, objectId, and
  //! color for textured, object id buffer and color shader output respectively
  explicit GenericDrawable(scene::SceneNode& node,
                           Magnum::Shaders::Phong& shader,
                           Magnum::GL::Mesh& mesh,
                           Magnum::SceneGraph::DrawableGroup3D* group = nullptr,
                           Magnum::GL::Texture2D* texture = nullptr,
                           int objectId = ID_UNDEFINED,
                           const Magnum::Color4& color = Magnum::Color4{1});

  //! Types of light position setups
  enum LightPositionMode {
    GLOBAL = 0,
    OBJECT = 1,
    CAMERA = 2,
  };

  /**
   * @brief Set the stored light positions for this drawable.
   */
  void setStoredLightPositions(
      const std::vector<Magnum::Vector3>& lightPositions) {
    lightPositions_ = lightPositions;
  };

  /**
   * @brief Set the light position mode for the drawable (determines how this
   * drawble consumes light positions)
   */
  void setLightPoisitionMode(const LightPositionMode lightPositionMode) {
    lightPoisitonMode_ = lightPositionMode;
  };

 protected:
  virtual void draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera) override;

  Magnum::GL::Texture2D* texture_;
  int objectId_;
  Magnum::Color4 color_;

  LightPositionMode lightPoisitonMode_ = GLOBAL;

  std::vector<Magnum::Vector3> lightPositions_;
};

class GenericFlatDrawable : public Drawable {
 public:
  //! Create a GenericDrawable for the given object using shader and mesh.
  //! Adds drawable to given group and uses provided texture, objectId, and
  //! color for textured, object id buffer and color shader output respectively
  explicit GenericFlatDrawable(
      scene::SceneNode& node,
      Magnum::Shaders::Flat3D& shader,
      Magnum::GL::Mesh& mesh,
      Magnum::SceneGraph::DrawableGroup3D* group = nullptr,
      Magnum::GL::Texture2D* texture = nullptr,
      int objectId = ID_UNDEFINED,
      const Magnum::Color4& color = Magnum::Color4{1});

 protected:
  virtual void draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera) override;

  Magnum::GL::Texture2D* texture_;
  int objectId_;
  Magnum::Color4 color_;
};

}  // namespace gfx
}  // namespace esp
