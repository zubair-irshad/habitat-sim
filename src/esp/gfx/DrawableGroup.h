// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <Magnum/SceneGraph/SceneGraph.h>
#include "esp/gfx/Shader.h"

namespace esp {
namespace gfx {

class Drawable;

/**
 * @brief Group of drawables, and shared group parameters.
 */
class DrawableGroup : public Magnum::SceneGraph::DrawableGroup3D {
 public:
  /**
   * @brief Constructor
   *
   * @param shader Shader all Drawables in this group will use.
   */
  explicit DrawableGroup(Shader::ptr shader = nullptr)
      : Magnum::SceneGraph::DrawableGroup3D{}, shader_{std::move(shader)} {}

  /**
   * @brief Add a drawable to this group.
   *
   * @param drawable Drawable to add
   * @return Reference to self (for method chaining)
   *
   * If the drawable is part of another group, it is removed from it.
   */
  DrawableGroup& add(Drawable& drawable);

  /**
   * @brief Removes the drawable from this group.
   *
   * @param drawable Drawable to remove
   * @return Reference to self (for method chaining)
   *
   * The drawable must be part of this group.
   */
  DrawableGroup& remove(Drawable& drawable);

  /**
   * @brief Get the shader this group is using.
   */
  Shader::ptr getShader() { return shader_; }

  /**
   * @brief Set the shader this group uses.
   * @return Reference to self (for method chaining)
   */
  DrawableGroup& setShader(Shader::ptr shader) {
    shader_ = std::move(shader);
    return *this;
  }

 private:
  Shader::ptr shader_ = nullptr;

  ESP_SMART_POINTERS(DrawableGroup);
};

}  // namespace gfx
}  // namespace esp
