// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "Shader.h"

namespace esp {
namespace gfx {

namespace {
// TODO: don't hardcode this
std::unique_ptr<Magnum::GL::AbstractShaderProgram> shaderProgramFactory(
    const ShaderConfig& cfg) {
  std::unique_ptr<Magnum::GL::AbstractShaderProgram> shaderProgram;
  switch (cfg.type) {
    case INSTANCE_MESH_SHADER: {
      return std::make_unique<gfx::PrimitiveIDShader>();
    } break;
#ifdef ESP_BUILD_PTEX_SUPPORT
    case PTEX_MESH_SHADER: {
      return std::make_unique<gfx::PTexMeshShader>();
    } break;
#endif

    case COLORED_SHADER: {
      return std::make_unique<Magnum::Shaders::Flat3D>(
          Magnum::Shaders::Flat3D::Flag::ObjectId);
    } break;

    case VERTEX_COLORED_SHADER: {
      return std::make_unique<Magnum::Shaders::Flat3D>(
          Magnum::Shaders::Flat3D::Flag::ObjectId |
          Magnum::Shaders::Flat3D::Flag::VertexColor);
    } break;

    case TEXTURED_SHADER: {
      return std::make_unique<Magnum::Shaders::Flat3D>(
          Magnum::Shaders::Flat3D::Flag::ObjectId |
          Magnum::Shaders::Flat3D::Flag::Textured);
    } break;

    case COLORED_SHADER_PHONG: {
      shaderProgram = std::make_unique<Magnum::Shaders::Phong>(
          Magnum::Shaders::Phong::Flag::ObjectId, 3 /*lights*/);
    } break;

    case VERTEX_COLORED_SHADER_PHONG: {
      shaderProgram = std::make_unique<Magnum::Shaders::Phong>(
          Magnum::Shaders::Phong::Flag::ObjectId |
              Magnum::Shaders::Phong::Flag::VertexColor,
          3 /*lights*/);
    } break;

    case TEXTURED_SHADER_PHONG: {
      shaderProgram = std::make_unique<Magnum::Shaders::Phong>(
          Magnum::Shaders::Phong::Flag::ObjectId |
              Magnum::Shaders::Phong::Flag::DiffuseTexture,
          3 /*lights*/);
    } break;

    default:
      return nullptr;
      break;
  }

  /* Default setup for Phong, shared by all models */
  if (cfg.type == COLORED_SHADER_PHONG ||
      cfg.type == VERTEX_COLORED_SHADER_PHONG ||
      cfg.type == TEXTURED_SHADER_PHONG) {
    using namespace Magnum::Math::Literals;

    static_cast<Magnum::Shaders::Phong&>(*shaderProgram)
        .setLightPositions({Magnum::Vector3{10.0f, 10.0f, 10.0f} * 100.0f,
                            Magnum::Vector3{-5.0f, -5.0f, 10.0f} * 100.0f,
                            Magnum::Vector3{0.0f, 10.0f, -10.0f} * 100.0f})
        .setLightColors(
            {0xffffff_rgbf * 0.8f, 0xffcccc_rgbf * 0.8f, 0xccccff_rgbf * 0.8f})
        .setSpecularColor(0x11111100_rgbaf)
        .setShininess(80.0f);
  }
  return shaderProgram;
}
}  // namespace

Shader::Shader(const ShaderConfig& config)
    : config_{config}, shaderProgram_{shaderProgramFactory(config)} {}

void Shader::setConfig(const ShaderConfig& config) {
  config_ = config;
  // update program now that we have a new config
  // TODO: error check if config is invalid
  shaderProgram_ = shaderProgramFactory(config);
}

}  // namespace gfx
}  // namespace esp
