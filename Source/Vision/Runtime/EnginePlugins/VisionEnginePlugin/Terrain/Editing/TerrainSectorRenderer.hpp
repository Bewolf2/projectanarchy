/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file TerrainSectorRenderer.hpp

#ifndef TERRAINSECTORRENDERER_HPP_INCLUDED
#define TERRAINSECTORRENDERER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/TerrainConfig.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/Terrain.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Geometry/TerrainSector.hpp>

#ifndef _VISION_DOC

/// \brief
///   Unsupported
class VTerrainSectorRenderLoop : public VisionRenderLoop_cl
{
public:
  TERRAIN_IMPEXP VTerrainSectorRenderLoop(int iResX, int iResY);
  TERRAIN_IMPEXP virtual ~VTerrainSectorRenderLoop();

  void SetSector(VTerrainSector* pSector)
  {
    m_pSector = pSector;
    m_AllInst.Clear();
    m_AllInst.AppendEntry(m_pSector);
  }

  TERRAIN_IMPEXP virtual void OnDoRenderLoop(void *pUserData);

  int m_iResX, m_iResY;
  VisStaticGeometryInstanceCollection_cl m_AllInst;
  VTerrainSector *m_pSector;
  VisBitmap_cl *m_pTargetData;

  // DX readback specific:
#if ( defined(_VR_DX9) && defined(WIN32) )
  IDirect3DSurface9 *pRenderTarget;
  IDirect3DTexture9 *pTexture;
  IDirect3DSurface9 *pSurface;
  IDirect3DTexture9 *pLockableTexture;
  IDirect3DSurface9 *pLockableSurface;
#endif
};



/// \brief
///   Unsupported
class VTerrainSectorRenderer : public VRefCounter
{
public:
  TERRAIN_IMPEXP VTerrainSectorRenderer(VTerrain *pOwner, int iRes=512, int iBorderWidth=4);
  TERRAIN_IMPEXP virtual ~VTerrainSectorRenderer();

  TERRAIN_IMPEXP void SetSector(VTerrainSector *pSector);
  TERRAIN_IMPEXP bool SaveToFile(VTerrainSector *pSector=NULL);

private:
  bool SaveRenderTarget(VTerrainSector* pSector, VisRenderableTexture_cl *pTex, const char *szFilePath);

  VTerrainPtr m_spOwner;
  VTerrainSector *m_pSector;
  VisRenderContextPtr m_spContext;
  VTerrainSectorRenderLoop *m_pRenderLoop;
  VisContextCamera_cl *m_pCamera;
  VisRenderableTexturePtr m_spTargetTex;
  VisRenderableTexturePtr m_spDepthStencilTex;
  int m_iBorderWidth;

  VisBitmapPtr m_spTargetData;

};

#endif // _VISION_DOC

#endif

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
