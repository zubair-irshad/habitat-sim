// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/Shaders.h>

#include "esp/gfx/Shader.h"

#include "esp/gfx/PrimitiveIDShader.h"
#ifdef ESP_BUILD_PTEX_SUPPORT
#include "esp/gfx/PTexMeshShader.h"
#endif

namespace esp {
namespace gfx {

// TODO(MM): remove all this and change into a string based factory method with
// json configuration

/**
 * @brief Enumeration of supported shader program options.
 */
enum ShaderType {
  /**
   * Shader program for instance mesh data. See @ref gfx::PrimitiveIDShader,
   * @ref GenericInstanceMeshData, @ref Mp3dInstanceMeshData, @ref
   * AssetType::INSTANCE_MESH, @ref loadInstanceMeshData.
   */
  INSTANCE_MESH_SHADER = 0,

  /**
   * Shader program for PTex mesh data. See @ref gfx::PTexMeshShader, @ref
   * gfx::PTexMeshDrawable, @ref loadPTexMeshData, @ref PTexMeshData.
   */
  PTEX_MESH_SHADER = 1,

  /**
   * Shader program for flat shading with uniform color. Used to render object
   * identifier or semantic types (e.g. chair, table, etc...). Also the
   * default shader for assets with unidentified rendering parameters. See
   * @ref Magnum::Shaders::Flat3D.
   */
  COLORED_SHADER = 2,

  /**
   * Shader program for vertex color shading. Used to render meshes with
   * per-vertex colors defined.
   */
  VERTEX_COLORED_SHADER = 3,

  /**
   * Shader program for meshes with textured defined.
   */
  TEXTURED_SHADER = 4,

  COLORED_SHADER_PHONG = 5,
  VERTEX_COLORED_SHADER_PHONG = 6,
  TEXTURED_SHADER_PHONG = 7
};

struct ShaderConfig {
  std::string id = "";
  ShaderType type = ShaderType::COLORED_SHADER;
  int numLights = 1;
};

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
          Magnum::Shaders::Flat3D::Flag::ObjectId, 3 /*lights*/);
    } break;

    case VERTEX_COLORED_SHADER: {
      return std::make_unique<Magnum::Shaders::Flat3D>(
          Magnum::Shaders::Flat3D::Flag::ObjectId |
              Magnum::Shaders::Flat3D::Flag::VertexColor,
          3 /*lights*/);
    } break;

    case TEXTURED_SHADER: {
      return std::make_unique<Magnum::Shaders::Flat3D>(
          Magnum::Shaders::Flat3D::Flag::ObjectId |
              Magnum::Shaders::Flat3D::Flag::Textured,
          3 /*lights*/);
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
  if (cfg.type == COLORED_SHADER || cfg.type == VERTEX_COLORED_SHADER ||
      cfg.type == TEXTURED_SHADER) {
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

/**
 * @brief Shader class
 */
class Shader {
 public:
  explicit Shader(
      ShaderConfig config = {} /* shaderProgramFactory = DEFAULT_FACTORY */)
      : config_{config}, shaderProgram_{shaderProgramFactory(config)} {}

  const ShaderConfig& getConfig() { return config_; }

  void setConfig(const ShaderConfig& config) {
    if (config.id != config_.id) {
      // TODO: I don't like this, have better logic here...
      throw std::exception();
    }
    config_ = config;
    // MAKE SURE WE UPDATE PROGRAM WITH NEW CONFIG
    shaderProgram_ = shaderProgramFactory(config);
  }

 private:
  ShaderConfig config_;

  std::unique_ptr<Magnum::GL::AbstractShaderProgram> shaderProgram_;
};

}  // namespace gfx
}  // namespace esp
