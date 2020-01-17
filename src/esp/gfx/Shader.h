// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#pragma once

#include <Magnum/GL/AbstractShaderProgram.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/Shaders.h>

#include <memory>

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
  ShaderType type = ShaderType::TEXTURED_SHADER_PHONG;
  int numLights = 1;
};

/**
 * @brief Shader class
 */
class Shader {
 public:
  explicit Shader(const ShaderConfig& config = {});

  const ShaderConfig& getConfig() const { return config_; }
  Magnum::GL::AbstractShaderProgram* getShaderProgram() {
    return shaderProgram_.get();
  }

  void setConfig(const ShaderConfig& config);

 private:
  ShaderConfig config_;

  std::unique_ptr<Magnum::GL::AbstractShaderProgram> shaderProgram_;
};

}  // namespace gfx
}  // namespace esp
