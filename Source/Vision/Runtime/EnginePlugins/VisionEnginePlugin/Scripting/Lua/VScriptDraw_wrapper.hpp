/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptInput_wrapper.hpp

#ifndef __VSCRIPTDRAW_WRAPPER_HPP
#define __VSCRIPTDRAW_WRAPPER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/VWallmarkManager.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleGroupBase.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptConstants.hpp>

/// 
/// \brief
///   Small wrapper to the draw interface inside the scripting binding
/// 
class VScriptDraw_wrapper
{
private:
  bool m_bEnabled;
public:

  VScriptDraw_wrapper() : m_bEnabled(true) {}
  ~VScriptDraw_wrapper() {}

  inline void Enable(bool bEnable) { m_bEnabled = bEnable; }

  inline void Line(const hkvVec3* pPoint1, const hkvVec3* pPoint2, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pPoint1!=NULL && pPoint2!=NULL)
      Vision::Game.DrawSingleLine(*pPoint1, *pPoint2, color);
  }

  inline void Line2D(float x1, float y1, float x2, float y2, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled)
      Vision::Game.DrawSingleLine2D(x1, y1, x2, y2, color);
  }

  inline void Box(const hkvVec3* pCenter, float fSize, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pCenter!=NULL)
      Vision::Game.DrawCube( *pCenter, fSize, color);
  }

  inline void Box(const hkvVec3* pMin, const hkvVec3* pMax, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pMin!=NULL && pMax!=NULL)
      Vision::Game.DrawSimpleBox( pMin->x, pMin->y, pMin->z, pMax->x, pMax->y, pMax->z, color);
  }

  inline void BoundingBox(const hkvAlignedBBox *pBox, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pBox!=NULL)
    {
      Vision::Game.DrawBoundingBox(*pBox, color);
    }
  }

  inline void BoundingBox(VisBaseEntity_cl *pEntity, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pEntity!=NULL)
    {
      pEntity->DrawBoundingBox(false, color);
    }
  }

  inline void OrientedBoundingBox(const hkvAlignedBBox *pBox, const hkvMat3 *pRotMat, const hkvVec3* pPos, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pBox!=NULL && pRotMat!=NULL && pPos!=NULL)
    {
      Vision::Game.DrawOrientedBoundingBox(*pBox, *pRotMat, *pPos, color);
    }
  }

  inline void OrientedBoundingBox(VisBaseEntity_cl *pEntity, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pEntity!=NULL)
    {
      pEntity->DrawBoundingBox(true, color);
    }
  }

  inline void BoneBoundingBox(VisBaseEntity_cl *pEntity, const char * szSpecificBone = NULL, VColorRef color = V_RGBA_WHITE)
  {
    if(m_bEnabled && pEntity!=NULL && pEntity->GetMesh()!=NULL)
    {
      VisSkeleton_cl *pSkeleton = pEntity->GetMesh()->GetSkeleton();
      if(pSkeleton && pSkeleton->GetBoneIndexByName(szSpecificBone)>0)
      {
        if(szSpecificBone==NULL)  pEntity->DrawBoneBoundingBoxes(color);
        else                      pEntity->DrawBoneBoundingBox(szSpecificBone, color);
      }
    }
  }

  inline void ParticleBoundingBox(VisParticleEffect_cl *pParticleEffect)
  {
    if(m_bEnabled && pParticleEffect!=NULL)
      pParticleEffect->RenderParticleBoundingBoxes();
  }

  inline void Path(VisPath_cl *pPath, VColorRef color=V_RGBA_GREEN)
  {
    if(m_bEnabled && pPath!=NULL)
    {
      pPath->DrawPath(color, 1.0f);
    }
  }

  inline void Wallmark(const hkvVec3* pCurrentPos,
                           const hkvVec3* pDir,
                           const char * szTexture,
                           int iBlendMode = BLEND_ALPHA,
                           float fSize = 50.f,
                           float fRotation = 0.f,
                           float fLifeTime = 20.f                 )
  {
    if(m_bEnabled && pCurrentPos!=NULL && pDir!=NULL && szTexture!=NULL)
    {
      hkvVec3 vTraceDir = *pDir;
      vTraceDir.setLength(10000);
      hkvVec3 vTraceEnd = (hkvVec3) *pCurrentPos + vTraceDir;
      VisTraceLineInfo_t traceInfo;
      if (!Vision::CollisionToolkit.TraceLine(*pCurrentPos, vTraceEnd, (ULONG)VIS_TRACE_ALL, (ULONG)VIS_TRACE_ALL, NULL, NULL, &traceInfo))
      {
        VTextureObject *pTexture = Vision::TextureManager.Load2DTexture(szTexture);

        VWallmarkManager::GlobalManager().CreateProjectedWallmark(
          traceInfo.touchPoint, *pCurrentPos,         // hit point and origin
          fSize, 2.f*fSize,                           // radius and depth
          pTexture, (VIS_TransparencyType)iBlendMode, // texture and blend mode
          V_RGBA_WHITE,                   // modulation color
          fRotation,                      // rotation angle (degree)
          fLifeTime*0.4f,fLifeTime*0.6f); // lifetime and additional fade-out time
      }
    }
  }

};

#endif // __VSCRIPTDRAW_WRAPPER_HPP

/*
 * Havok SDK - Base file, BUILD(#20130624)
 * 
 * Confidential Information of Havok.  (C) Copyright 1999-2013
 * Telekinesys Research Limited t/a Havok. All Rights Reserved. The Havok
 * Logo, and the Havok buzzsaw logo are trademarks of Havok.  Title, ownership
 * rights, and intellectual property rights in the Havok software remain in
 * Havok and/or its suppliers.
 * 
 * Use of this software for evaluation purposes is subject to and indicates
 * acceptance of the End User licence Agreement for this product. A copy of
 * the license is included with this software and is also available from salesteam@havok.com.
 * 
 */
