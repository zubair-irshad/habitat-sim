// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "GenericDrawable.h"

#include <Magnum/Shaders/Phong.h>

#include "esp/scene/SceneNode.h"

namespace esp {
namespace gfx {

GenericDrawable::GenericDrawable(
    scene::SceneNode& node,
    Magnum::GL::Mesh& mesh,
    DrawableGroup* group /* = nullptr */,
    Magnum::GL::Texture2D* texture /* = nullptr */,
    int objectId /* = ID_UNDEFINED */,
    const Magnum::Color4& color /* = Magnum::Color4{1} */)
    : Drawable{node, mesh, group},
      texture_(texture),
      objectId_(objectId),
      color_{color} {}

void GenericDrawable::draw(const Magnum::Matrix4& transformationMatrix,
                           Magnum::SceneGraph::Camera3D& camera,
                           Shader* shader) {
  ShaderType type = shader.getConfig().type;
  Magnum::GL::AbstractShaderProgram* shaderProgram = shader->getShaderProgram();

  // TODO: use polymorphism to do double dispatch here
  if (type == COLORED_SHADER_PHONG || type == VERTEX_COLORED_SHADER_PHONG ||
      type == TEXTURED_SHADER_PHONG) {
    Magnum::Shaders::Phong& shader =
        static_cast<Magnum::Shaders::Phong&>(*shaderProgram);
    shader.setTransformationMatrix(transformationMatrix)
        .setProjectionMatrix(camera.projectionMatrix())
        .setNormalMatrix(transformationMatrix.rotationScaling())
        .setObjectId(node_.getId());

    if ((shader.flags() & Magnum::Shaders::Phong::Flag::DiffuseTexture) &&
        texture_) {
      shader.bindDiffuseTexture(*texture_);
    }

    if (!(shader.flags() & Magnum::Shaders::Phong::Flag::VertexColor)) {
      shader.setDiffuseColor(color_);
    }

  } else {
    Magnum::Shaders::Flat3D& shader =
        static_cast<Magnum::Shaders::Flat3D&>(*shaderProgram);
    shader.setTransformationProjectionMatrix(camera.projectionMatrix() *
                                             transformationMatrix);

    if ((shader.flags() & Magnum::Shaders::Flat3D::Flag::Textured) &&
        texture_) {
      shader.bindTexture(*texture_);
    }
    if (!(shader.flags() & Magnum::Shaders::Flat3D::Flag::VertexColor)) {
      shader.setColor(color_);
    }

    shader.setObjectId(node_.getId());
  }
  mesh_.draw(shaderProgram);
}

}  // namespace gfx
}  // namespace esp
