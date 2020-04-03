// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "Drawable.h"
#include "RenderCamera.h"

#include <Magnum/EigenIntegration/Integration.h>
#include <Magnum/Math/Frustum.h>
#include <Magnum/Math/Intersection.h>
#include <Magnum/Math/Range.h>
#include <Magnum/SceneGraph/Drawable.h>

namespace Mn = Magnum;
namespace Cr = Corrade;

namespace esp {
namespace gfx {

/**
 * @brief do frustum culling with temporal coherence
 * @param range, the axis-aligned bounding box
 * @param frustum, the frustum
 * @param frustumPlaneIndex, the frustum plane in last frame that culled the
 * aabb (default: 0)
 * @return NullOpt if aabb intersects the frustum, otherwise the fustum plane
 * that culls the aabb
 */
Cr::Containers::Optional<int> rangeFrustum(const Mn::Range3D& range,
                                           const Mn::Frustum& frustum,
                                           int frustumPlaneIndex = 0) {
  const Mn::Vector3 center = range.min() + range.max();
  const Mn::Vector3 extent = range.max() - range.min();

  for (int iPlane = 0; iPlane < 6; ++iPlane) {
    int index = (iPlane + frustumPlaneIndex) % 6;
    const Mn::Vector4& plane = frustum[index];

    const Mn::Vector3 absPlaneNormal = Mn::Math::abs(plane.xyz());

    const float d = Mn::Math::dot(center, plane.xyz());
    const float r = Mn::Math::dot(extent, absPlaneNormal);
    if (d + r < -2.0 * plane.w())
      return Cr::Containers::Optional<int>{index};
  }

  return Cr::Containers::NullOpt;
}

RenderCamera::RenderCamera(scene::SceneNode& node) : MagnumCamera{node} {
  node.setType(scene::SceneNodeType::CAMERA);
  setAspectRatioPolicy(Mn::SceneGraph::AspectRatioPolicy::NotPreserved);
}

RenderCamera::RenderCamera(scene::SceneNode& node,
                           const vec3f& eye,
                           const vec3f& target,
                           const vec3f& up)
    : RenderCamera(node) {
  // once it is attached, set the transformation
  node.setTransformation(Mn::Matrix4::lookAt(
      Mn::Vector3{eye}, Mn::Vector3{target}, Mn::Vector3{up}));
}

RenderCamera& RenderCamera::setProjectionMatrix(int width,
                                                int height,
                                                float znear,
                                                float zfar,
                                                float hfov) {
  const float aspectRatio = static_cast<float>(width) / height;
  MagnumCamera::setProjectionMatrix(
      Mn::Matrix4::perspectiveProjection(Mn::Deg{hfov}, aspectRatio, znear,
                                         zfar))
      .setViewport(Magnum::Vector2i(width, height));
  return *this;
}

size_t RenderCamera::frustumCull(
    std::vector<std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>>& drawableTransforms) {
  // camera frustum relative to world origin
  const Mn::Frustum frustum =
      Mn::Frustum::fromMatrix(projectionMatrix() * cameraMatrix());

  auto newEndIter = std::remove_if(
      drawableTransforms.begin(), drawableTransforms.end(),
      [&](const std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>& a) {
        // obtain the absolute aabb
        auto& node = static_cast<scene::SceneNode&>(a.first.get().object());
        Corrade::Containers::Optional<Mn::Range3D> aabb =
            node.getAbsoluteAABB();
        if (aabb) {
          // if it has an absolute aabb, it is a static mesh
          Cr::Containers::Optional<int> culledPlane =
              rangeFrustum(*aabb, frustum, node.getFrustumPlaneIndex());
          if (culledPlane) {
            node.setFrustumPlaneIndex(*culledPlane);
          }
          // if it has value, it means the aabb is culled
          return (culledPlane != Cr::Containers::NullOpt);
        } else {
          // keep the drawable if its node does not have an absolute AABB
          return false;
        }
      });

  return (drawableTransforms.end() - newEndIter);
}

size_t RenderCamera::occlusionCull(
    std::vector<std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>>& drawableTransforms) {
    // partition the drawables into "OC-invisibile" and "OC-visible" based on
    // the *last* frame;
    // put OC-visibles in the 2nd part
    std::vector<std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>>::iterator firstVisibleIter =
        std::partition(
            drawableTransforms.begin(), drawableTransforms.end(),
            [&](const std::pair<
                std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                Mn::Matrix4>& a) {
              Drawable& d = static_cast<Drawable&>(a.first.get());
              return (ocVisible_.find(d.getDrawableID()) == ocVisible_.end());
            });

    // render FC-visibile and OC-visible (last frame) drawables
    for (auto iter = firstVisibleIter; iter != drawableTransforms.end();
         ++iter) {
      (*iter).first.get().draw((*iter).second, *this);
    }
    // after rendering, remove them
    drawableTransforms.erase(firstVisibleIter, drawableTransforms.end());

    // for any FC-visible, but not OC-visible (last frame) drawables, do OC using bbox
    // against the current z-buffer
    for (auto iter = drawableTransforms.begin(); iter != drawableTransforms.end(); ++iter) {

    }
    
    // render the drawables that are OC-visible in
    // current frame (passed the previous OC test)

  // This is wrong!! XXXX MUST change !!! ===
  return 0;
}

uint32_t RenderCamera::draw(MagnumDrawableGroup& drawables,
                            bool frustumCulling,
                            bool occlusionCulling) {
  if (!frustumCulling && !occlusionCulling) {
    MagnumCamera::draw(drawables);
    return drawables.size();
  }

  // compute the model matices
  std::vector<std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                        Mn::Matrix4>>
      drawableTransforms = drawableTransformations(drawables);

  // do frustum culling
  size_t numTotalCulled = 0;
  if (frustumCulling) {
    size_t numCulled = frustumCull(drawableTransforms);
    // erase all items, which are outside of the frustum
    drawableTransforms.erase(drawableTransforms.end() - numCulled,
                             drawableTransforms.end());
    numTotalCulled += numCulled;
  }

  // do occlusion culling
  if (occlusionCulling) {
    size_t numCulled = occlusionCull(drawableTransforms);
    // erase all items, which are occluded
    drawableTransforms.erase(drawableTransforms.end() - numCulled,
                             drawableTransforms.end());
    numTotalCulled += numCulled;
  }

  MagnumCamera::draw(drawableTransforms);

  return drawables.size() - numTotalCulled;
}

}  // namespace gfx
}  // namespace esp
