// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <Magnum/Shaders/Shaders.h>

#include "esp/gfx/DrawableGroup.h"
#include "esp/gfx/Shader.h"

namespace esp {
namespace gfx {

/**
 * @brief Class which manages shaders, TODO(MM): scene graph and drawable groups
 */
class DrawableManager {
  using ShaderType = Magnum::GL::AbstractShaderProgram;
  using DrawableGroups = std::map<std::string, DrawableGroup>;

 public:
  explicit DrawableManager(std::string defaultDrawableGroupId = {},
                           ShaderConfig defaultShaderConfig = {});

  // Shader management
  std::shared_ptr<ShaderType> getShaderForDrawableGroup(
      const std::string& drawableGroupId);
  std::shared_ptr<ShaderType> getShaderByID(const std::string& shaderId);

  // DrawableGroup management
  DrawableGroups& getDrawables() { return drawableGroups_; }
  const DrawableGroups& getDrawables() const { return drawableGroups_; }

  bool createDrawableGroup(std::string id);

  gfx::DrawableGroup& getDrawableGroup() {
    return getDrawableGroup(defaultDrawableGroupId_);
  }

  gfx::DrawableGroup& getDrawableGroup(const std::string& id) {
    return drawableGroups_.at(id);
  }

  const gfx::DrawableGroup& getDrawableGroup(const std::string& id) const {
    return drawableGroups_.at(id);
  }

 private:
  // TODO: implement deletions using some type of ref counting
  enum class ShaderDeletionPolicy { NoDeletion, DeleteIfUnused };

  struct ShaderEntry {
    Shader shader;
    size_t refCount;
    ShaderDeletionPolicy deletionPolicy;
  };

  std::string defaultShaderId_;
  std::map<std::string, ShaderEntry> shaderIdToShader_;
  // std::map<std::string, std::string> drawableGroupIdToShaderId_;
  const std::string defaultDrawableGroupId_;
  // map of (drawable group ID -> drawable group) for this scene graph
  DrawableGroups drawableGroups_;
};

}  // namespace gfx
}  // namespace esp
