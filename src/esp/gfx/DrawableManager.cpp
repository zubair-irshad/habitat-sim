// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "DrawableManager.h"

#include "esp/scene/SceneNode.h"

namespace esp {
namespace gfx {

DrawableManager::DrawableManager(std::string defaultDrawableGroupId,
                                 const ShaderConfig& defaultShaderConfig)
    : defaultShaderId_{defaultShaderConfig.id},
      defaultDrawableGroupId_{std::move(defaultDrawableGroupId)} {
  // Order is important! Need to create shader before drawable group
  createShader(defaultShaderConfig);
  createDrawableGroup(defaultDrawableGroupId_, &getShader(defaultShaderId_));
}

bool DrawableManager::createDrawableGroup(std::string id, Shader* shader) {
  DrawableGroup group{id, shader};
  auto inserted = drawableGroups_.emplace(std::move(id), std::move(group));
  if (inserted.second) {
    LOG(INFO) << "Created Drawable Group: " << inserted.first->first;
    return true;
  }
  return false;
}

bool DrawableManager::createShader(const ShaderConfig& cfg) {
  if (shaders_.emplace(cfg.id, ShaderEntry{Shader{cfg}}).second) {
    LOG(INFO) << "Created Shader: " << cfg.id;
    return true;
  }
  return false;
}

}  // namespace gfx
}  // namespace esp
