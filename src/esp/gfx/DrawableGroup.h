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
 * @brief Class which manages a drawable group, and related settings for that
 * drawable group
 */
class DrawableGroup : public Magnum::SceneGraph::DrawableGroup3D {
 public:
  explicit DrawableGroup(std::string id, Shader* shader = nullptr)
      : Magnum::SceneGraph::DrawableGroup3D{},
        id_{std::move(id)},
        shader_{shader} {}

  const std::string& id() const { return id_; }

  DrawableGroup& add(Drawable& drawable);

  DrawableGroup& remove(Drawable& drawable);

  Shader* shader() { return shader_; }

  DrawableGroup& setShader(Shader* shader) {
    shader_ = shader;
    return *this;
  }

 private:
  const std::string id_;
  Shader* shader_;
};

}  // namespace gfx
}  // namespace esp
