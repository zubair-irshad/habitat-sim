// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "DrawableManager.h"

#include "esp/scene/SceneNode.h"

namespace esp {
namespace gfx {

DrawableManager::DrawableManager(ShaderConfig defaultShaderConfig)
    : defaultShaderId_{defaultShaderConfig.id},
      defaultDrawableGroupId_{std::move(defaultDrawableGroupId)} {
  createDrawableGroup(defaultDrawableGroupId_);
}

bool DrawableManager::createDrawableGroup(std::string id) {
  return drawableGroups_.emplace(std::move(id), gfx::DrawableGroup{id}).second;
}

}  // namespace gfx
}  // namespace esp
