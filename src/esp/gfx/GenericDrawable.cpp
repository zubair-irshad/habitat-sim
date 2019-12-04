// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "GenericDrawable.h"

#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Phong.h>

#include "esp/scene/SceneNode.h"

namespace esp {
namespace gfx {

GenericFlatDrawable::GenericFlatDrawable(
    scene::SceneNode& node,
    Magnum::Shaders::Flat3D& shader,
    Magnum::GL::Mesh& mesh,
    Magnum::SceneGraph::DrawableGroup3D* group /* = nullptr */,
    Magnum::GL::Texture2D* texture /* = nullptr */,
    int objectId /* = ID_UNDEFINED */,
    const Magnum::Color4& color /* = Magnum::Color4{1} */)
    : Drawable{node, shader, mesh, group},
      texture_(texture),
      objectId_(objectId),
      color_{color} {}

void GenericFlatDrawable::draw(const Magnum::Matrix4& transformationMatrix,
                               Magnum::SceneGraph::Camera3D& camera) {
  Magnum::Shaders::Flat3D& shader =
      static_cast<Magnum::Shaders::Flat3D&>(shader_);

  shader.setTransformationProjectionMatrix(camera.projectionMatrix() *
                                           transformationMatrix);

  if ((shader.flags() & Magnum::Shaders::Flat3D::Flag::Textured) && texture_) {
    shader.bindTexture(*texture_);
  }

  if (!(shader.flags() & Magnum::Shaders::Flat3D::Flag::VertexColor)) {
    shader.setColor(color_);
  }

  shader.setObjectId(node_.getId());
  mesh_.draw(shader_);
}

void GenericDrawable::draw(const Magnum::Matrix4& transformationMatrix,
                           Magnum::SceneGraph::Camera3D& camera) {
  Magnum::Shaders::Phong& shader =
      static_cast<Magnum::Shaders::Phong&>(shader_);

  std::vector<Magnum::Vector3> lightsTransformed;
  for (auto& p : lightPositions_) {
    if (lightPoisitonMode_ == GLOBAL) {
      lightsTransformed.push_back(camera.cameraMatrix().transformPoint(p));
    } else if (lightPoisitonMode_ == OBJECT) {
      lightsTransformed.push_back(transformationMatrix.transformPoint(p));
    } else if (lightPoisitonMode_ == CAMERA) {
      lightsTransformed.push_back(p);
    }
  }

  std::vector<Magnum::Color4> finalLightColors_;
  for (size_t i = 0; i < lightColors_.size(); i++) {
    finalLightColors_.push_back(lightColors_[i] * lightIntensities_[i]);
  }

  shader.setTransformationMatrix(transformationMatrix)
      .setProjectionMatrix(camera.projectionMatrix())
      .setNormalMatrix(transformationMatrix.rotationScaling())
      .setObjectId(node_.getId())
      .setLightPositions(lightsTransformed)
      .setLightColors(finalLightColors_);

  if ((shader.flags() & Magnum::Shaders::Phong::Flag::DiffuseTexture) &&
      texture_) {
    shader.bindDiffuseTexture(*texture_);
  }

  if (!(shader.flags() & Magnum::Shaders::Phong::Flag::VertexColor)) {
    shader.setDiffuseColor(color_);
  }

  mesh_.draw(shader_);
}

GenericDrawable::GenericDrawable(
    scene::SceneNode& node,
    Magnum::Shaders::Phong& shader,
    Magnum::GL::Mesh& mesh,
    Magnum::SceneGraph::DrawableGroup3D* group /* = nullptr */,
    Magnum::GL::Texture2D* texture /* = nullptr */,
    int objectId /* = ID_UNDEFINED */,
    const Magnum::Color4& color /* = Magnum::Color4{1} */)
    : Drawable{node, shader, mesh, group},
      texture_(texture),
      objectId_(objectId),
      color_{color} {}

}  // namespace gfx
}  // namespace esp
