// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "Drawable.h"

#include "esp/gfx/DrawableGroup.h"
#include "esp/scene/SceneNode.h"

namespace esp {
namespace gfx {

Drawable::Drawable(scene::SceneNode& node,
                   Magnum::GL::Mesh& mesh,
                   DrawableGroup* group /* = nullptr */)
    : Magnum::SceneGraph::Drawable3D{node}, node_(node), mesh_(mesh) {
  if (group) {
    group->add(*this);
    LOG(INFO) << "New drawable created in group: " << group->id();
  }
}

void Drawable::draw(const Magnum::Matrix4& transformationMatrix,
                    Magnum::SceneGraph::Camera3D& camera) {
  draw(transformationMatrix, camera, group_->shader());
}

}  // namespace gfx
}  // namespace esp
