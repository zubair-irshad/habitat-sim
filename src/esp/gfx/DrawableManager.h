// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include "esp/gfx/DrawableGroup.h"
#include "esp/gfx/Shader.h"

namespace esp {
namespace gfx {

/**
 * @brief Class which manages shaders, TODO(MM): scene graph and drawable groups
 */
class DrawableManager {
  using DrawableGroups = std::map<std::string, DrawableGroup>;

 public:
  // TODO: what if defaultDrawableGroup doesn't use default shader?
  explicit DrawableManager(std::string defaultDrawableGroupId = {},
                           const ShaderConfig& defaultShaderConfig = {});

  // Shader management

  Shader& getShader() { return getShader(defaultShaderId_); };
  const Shader& getShader() const { return getShader(defaultShaderId_); };
  Shader& getShader(const std::string& shaderId) {
    return shaders_.at(shaderId).shader;
  };
  const Shader& getShader(const std::string& shaderId) const {
    return shaders_.at(shaderId).shader;
  };

  bool createShader(const ShaderConfig& cfg);

  // DrawableGroup management
  DrawableGroups& getDrawables() { return drawableGroups_; }
  const DrawableGroups& getDrawables() const { return drawableGroups_; }

  gfx::DrawableGroup& getDrawableGroup() {
    return getDrawableGroup(defaultDrawableGroupId_);
  }
  const gfx::DrawableGroup& getDrawableGroup() const {
    return getDrawableGroup(defaultDrawableGroupId_);
  }
  gfx::DrawableGroup& getDrawableGroup(const std::string& id) {
    return drawableGroups_.at(id);
  }
  const gfx::DrawableGroup& getDrawableGroup(const std::string& id) const {
    return drawableGroups_.at(id);
  }

  bool createDrawableGroup(std::string id, Shader* shader = nullptr);

 private:
  // TODO: implement deletions using some type of ref counting
  enum class ShaderDeletionPolicy { NoDeletion, DeleteIfUnused };

  struct ShaderEntry {
    Shader shader;
    size_t refCount = 0;
    ShaderDeletionPolicy deletionPolicy = ShaderDeletionPolicy::NoDeletion;
  };

  std::string defaultShaderId_;
  std::map<std::string, ShaderEntry> shaders_;
  const std::string defaultDrawableGroupId_;
  // map of (drawable group ID -> drawable group) for this scene graph
  DrawableGroups drawableGroups_;
};

}  // namespace gfx
}  // namespace esp
