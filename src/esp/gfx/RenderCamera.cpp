// Copyright (c) Facebook, Inc. and its affiliates.
// This source code is licensed under the MIT license found in the
// LICENSE file in the root directory of this source tree.

#include "RenderCamera.h"
#include "Drawable.h"

#include <Magnum/EigenIntegration/Integration.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/GL/SampleQuery.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/Frustum.h>
#include <Magnum/Math/Intersection.h>
#include <Magnum/Math/Range.h>
#include <Magnum/Math/Vector.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/Trade/MeshData.h>

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

  Mn::Trade::MeshData cube = Mn::Primitives::cubeSolidStrip();
  bbox_ = Mn::MeshTools::compile(cube);

  // XXX
  Mn::Trade::MeshData cubeWire = Mn::Primitives::cubeWireframe();
  bboxWire_ = Mn::MeshTools::compile(cubeWire);
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
        Cr::Containers::Optional<Mn::Range3D> aabb = node.getAbsoluteAABB();
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

  int numCulled = drawableTransforms.end() - newEndIter;
  // erase all items, which are outside of the frustum
  drawableTransforms.erase(newEndIter, drawableTransforms.end());
  return numCulled;
}

size_t RenderCamera::occlusionCull(
    std::vector<std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>>& drawableTransforms) {
  // if a node does not have absolute aabb, skip the OC test
  // put them in the 1st half
  auto ocBegin = std::partition(
      drawableTransforms.begin(), drawableTransforms.end(),
      [&](const std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>& a) {
        auto& node = static_cast<scene::SceneNode&>(a.first.get().object());
        Cr::Containers::Optional<Mn::Range3D> aabb = node.getAbsoluteAABB();

        return (!aabb);
      });

  // from now, we only focus on range between [ocBegin,
  // drawableTransforms.end()]
  // sort based on the distance to the eye in front-to-back order
  // distance is computed using center of the absAABB to the eye (the z value in
  // camera frame)

  std::sort(
      ocBegin, drawableTransforms.end(),
      [&](const std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>& a,
          const std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>& b) {
        auto& nA = static_cast<scene::SceneNode&>(a.first.get().object());
        auto& nB = static_cast<scene::SceneNode&>(b.first.get().object());
        Cr::Containers::Optional<Mn::Range3D> boxA = nA.getAbsoluteAABB();
        Cr::Containers::Optional<Mn::Range3D> boxB = nB.getAbsoluteAABB();
        Mn::Vector3 cA = (*boxA).max() + (*boxA).min();
        Mn::Vector3 cB = (*boxB).max() + (*boxB).min();
        const auto& cam = cameraMatrix();
        Mn::Vector3 cz{cam[2][0], cam[2][1], cam[2][2]};
        return (Mn::Math::dot(cz, cA - cB) < 0.0);
      });

  // we divide the drawables into two groups, occluders and occludees
  float ratio = 0.4;
  int numOccluders = (drawableTransforms.end() - ocBegin) * ratio;
  if (numOccluders <= 0) {
    numOccluders = 1;
  }

  // for the occluders, draw directly
  int counter = 0;
  auto iter = ocBegin;
  for (; counter < numOccluders && iter != drawableTransforms.end();
       ++iter, ++counter) {
    Drawable& d = static_cast<Drawable&>(iter->first.get());
    d.draw(iter->second, *this);
  }

  // for the occludees, do OC culling
  auto testDraw =
      [&](const std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>& a) {
        auto& node = static_cast<scene::SceneNode&>(a.first.get().object());
        Cr::Containers::Optional<Mn::Range3D> aabb = node.getAbsoluteAABB();

        Mn::Vector3 t = (*aabb).center();
        Mn::Vector3 scale = (*aabb).size() / 2.0;
        Mn::Matrix4 cmvp = cameraMatrix() * Mn::Matrix4::translation(t) *
                           Mn::Matrix4::scaling(scale);

        using namespace Mn::Math::Literals;
        // render the bounding box
        shader_.setColor(0x2f83cc_rgbf)
            .setTransformationProjectionMatrix(projectionMatrix() * cmvp)
            .draw(bbox_);
      };

  // turn off the rasterizer
  glDepthMask(GL_FALSE);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDisable(GL_CULL_FACE);

  std::vector<Mn::GL::SampleQuery> boxQuery;
  for (auto ocIter = iter; ocIter != drawableTransforms.end(); ++ocIter) {
    boxQuery.emplace_back(Mn::GL::SampleQuery::Target::AnySamplesPassed);
    int idx = boxQuery.size() - 1;
    boxQuery[idx].begin();
    testDraw(*ocIter);
    boxQuery[idx].end();
  }
  // now, turn on the rasterizer again
  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_CULL_FACE);

  // render the drawables that passed the test
  int idx = 0;
  for (auto ocIter = iter; ocIter != drawableTransforms.end(); ++ocIter) {
    boxQuery[idx].beginConditionalRender(
        Mn::GL::SampleQuery::ConditionalRenderMode::Wait);
    Drawable& d = static_cast<Drawable&>(ocIter->first.get());
    d.draw(ocIter->second, *this);
    boxQuery[idx].endConditionalRender();
  }

  return 0;
}

size_t RenderCamera::occlusionCull_slow(
    std::vector<std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>>& drawableTransforms) {
  // if a node does not have absolute aabb, skip the OC test
  // put them in the 1st half
  auto ocBegin = std::partition(
      drawableTransforms.begin(), drawableTransforms.end(),
      [&](const std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>& a) {
        auto& node = static_cast<scene::SceneNode&>(a.first.get().object());
        Cr::Containers::Optional<Mn::Range3D> aabb = node.getAbsoluteAABB();

        return (!aabb);
      });

  // from now, we only focus on range between [ocBegin,
  // drawableTransforms.end()]

  // further partition the drawables into "OC-invisibile" and "OC-visible" based
  // on the *last* frame; put the OC-visibles in the 2nd part
  std::vector<std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                        Mn::Matrix4>>::iterator firstVisibleIter =
      std::partition(
          ocBegin, drawableTransforms.end(),
          [&](const std::pair<
              std::reference_wrapper<Mn::SceneGraph::Drawable3D>, Mn::Matrix4>&
                  a) {
            Drawable& d = static_cast<Drawable&>(a.first.get());
            return (ocVisibles_.find(d.getDrawableID()) == ocVisibles_.end());
          });

  // render OC-visibles in the last frame
  for (auto iter = firstVisibleIter; iter != drawableTransforms.end(); ++iter) {
    Drawable& d = static_cast<Drawable&>(iter->first.get());
    d.draw(iter->second, *this);
  }
  // LOG(INFO) << "step 1: Last total: " << drawableTransforms.end() -
  // drawableTransforms.begin()
  //  << "#Last oc-visibles: " << drawableTransforms.end() - firstVisibleIter;

  auto testDraw =
      [&](const std::pair<std::reference_wrapper<Mn::SceneGraph::Drawable3D>,
                          Mn::Matrix4>& a) {
        auto& node = static_cast<scene::SceneNode&>(a.first.get().object());
        Cr::Containers::Optional<Mn::Range3D> aabb = node.getAbsoluteAABB();

        Mn::Vector3 t = (*aabb).center();
        Mn::Vector3 scale = (*aabb).size() / 2.0;
        /*
        Mn::Matrix4 mvp = a.second * Mn::Matrix4::translation(t) *
                          Mn::Matrix4::scaling(scale);
        */
        Mn::Matrix4 cmvp = cameraMatrix() * Mn::Matrix4::translation(t) *
                           Mn::Matrix4::scaling(scale);

        using namespace Mn::Math::Literals;
        // render the bounding box
        shader_
            .setColor(0x2f83cc_rgbf)
            // .setTransformationProjectionMatrix(projectionMatrix() * mvp)
            .setTransformationProjectionMatrix(projectionMatrix() * cmvp)
            .draw(bbox_);

        // XXX
        /*
        glDepthMask(GL_TRUE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glEnable(GL_CULL_FACE);

        shader_
            .setColor(0xff3333_rgbf)
            // .setTransformationProjectionMatrix(projectionMatrix() * mvp)
            .setTransformationProjectionMatrix(projectionMatrix() * cmvp)
            .draw(bboxWire_);

        // turn off the rasterizer
        //
        Mn::GL::Renderer::enable(Mn::GL::Renderer::Feature::RasterizerDiscard);

        glDepthMask(GL_FALSE);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        // start occlusion queries and render for the current slice
        glDisable(GL_CULL_FACE);
        */
      };

  // turn off the rasterizer
  // Mn::GL::Renderer::enable(Mn::GL::Renderer::Feature::RasterizerDiscard);
  glDepthMask(GL_FALSE);
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  // start occlusion queries and render for the current slice
  glDisable(GL_CULL_FACE);

  // clear the list, making it ready for the current frame
  ocVisibles_.clear();

  // to update the OC visibility list, run the OC tests again even we already
  // rendered them
  {
    Mn::GL::SampleQuery q{Mn::GL::SampleQuery::Target::AnySamplesPassed};
    for (auto iter = firstVisibleIter; iter != drawableTransforms.end();
         ++iter) {
      q.begin();
      testDraw(*iter);
      q.end();
      if (q.result<bool>()) {
        Drawable& d = static_cast<Drawable&>(iter->first.get());
        ocVisibles_.insert(d.getDrawableID());
      }
    }
  }
  // LOG(INFO) << "step 2: last ocVisibles, also ocVisibles this frame: "<<
  // ocVisibles_.size();

  // remove these OC-visibles that are already rendered and oc-tested
  drawableTransforms.erase(firstVisibleIter, drawableTransforms.end());

  // for any FC-visible, but not OC-visible (last frame) drawables, do OC using
  // bbox against the current z-buffer
  std::vector<Mn::GL::SampleQuery> boxQuery;

  for (auto iter = ocBegin; iter != drawableTransforms.end(); ++iter) {
    // TODO: add a corrade assertion, it must have an absAABB
    boxQuery.emplace_back(Mn::GL::SampleQuery::Target::AnySamplesPassed);
    int idx = boxQuery.size() - 1;
    boxQuery[idx].begin();
    testDraw(*iter);
    boxQuery[idx].end();
  }
  // now, turn on the rasterizer again
  // Mn::GL::Renderer::disable(Mn::GL::Renderer::Feature::RasterizerDiscard);
  glDepthMask(GL_TRUE);
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  glEnable(GL_CULL_FACE);

  // render the drawables that are OC-visible in
  // current frame (passed the previous OC test)
  int idx = 0;
  for (auto iter = ocBegin; iter != drawableTransforms.end(); ++iter, ++idx) {
    boxQuery[idx].beginConditionalRender(
        Mn::GL::SampleQuery::ConditionalRenderMode::Wait);
    // if (boxQuery[idx].result<bool>())
    {
      Drawable& d = static_cast<Drawable&>(iter->first.get());
      d.draw(iter->second, *this);
      // LOG(INFO) << "I draw something! ";
    }
    // else {
    // LOG(INFO) << "NO, I did not draw anything! ";
    // }
    boxQuery[idx].endConditionalRender();
  }

  // number of objects that were culled in current frame
  int numCulled = 0;
  // update the visibility list
  idx = 0;
  for (auto iter = ocBegin; iter != drawableTransforms.end(); ++iter, ++idx) {
    if (boxQuery[idx].result<bool>()) {
      Drawable& d = static_cast<Drawable&>(iter->first.get());
      ocVisibles_.insert(d.getDrawableID());
    } else {
      numCulled++;  // it has been culled
    }
  }
  // LOG(INFO) << "step 3: # Total: " << drawableTransforms.end() - ocBegin << "
  // OC-culled: " << numCulled;

  // last, remove all the processed drawables
  drawableTransforms.erase(ocBegin, drawableTransforms.end());

  return numCulled;
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
    numTotalCulled += numCulled;
  }

  // do occlusion culling
  if (occlusionCulling) {
    size_t numCulled = occlusionCull(drawableTransforms);
    numTotalCulled += numCulled;
  }

  // draw anything that remains in the queue
  MagnumCamera::draw(drawableTransforms);

  return drawables.size() - numTotalCulled;
}

}  // namespace gfx
}  // namespace esp
