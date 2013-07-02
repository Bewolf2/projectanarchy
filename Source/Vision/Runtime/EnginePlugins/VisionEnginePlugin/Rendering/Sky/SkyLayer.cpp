/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <string.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/Sky.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Sky/SkyLayer.hpp>

extern VProgressStatus g_DiscardProgress;
//#define LOADINGPROGRESS   g_DiscardProgress
#define LOADINGPROGRESS   Vision::GetApplication()->GetLoadingProgress()


VSkyLayer::VSkyLayer() 
  : m_vColor(1.f,1.f,1.f,1.f), m_vTextureTransform(1.f,1.f,0.f,0.f),
  m_pOwnerSky(NULL)
{
  SetMapping(VIS_SKYMAPPING_SPHERICAL);
  SetHeightScale(2.5f);
  SetHeightPos(0.f);
}


VSkyLayer::~VSkyLayer()
{
}


VSkyLayer &VSkyLayer::operator = (const VSkyLayer &other)
{
  m_vColor = other.m_vColor;
  m_vMappingParams = other.m_vMappingParams;
  m_vTextureTransform = other.m_vTextureTransform;
  m_vScrollSpeed = other.m_vScrollSpeed;
  m_spTexture = other.m_spTexture;
  for (int i=0;i<6;i++)
    m_spCubemapFace[i] = other.m_spCubemapFace[i];

  SetMapping(other.m_eMapping);

  return *this;
}



// helper:
static VTextureObject *LoadSkyTexture(const char *szFilename)
{
  if (!szFilename || !szFilename[0])
    return NULL;//texmanager.GetPlainBlackTexture();
  return Vision::TextureManager.Load2DTexture(szFilename,VTM_DEFAULT_FLAGS|VTM_FLAG_NO_MIPMAPS|VTM_FLAG_ASSUME_SRGB);
}

void VSkyLayer::SetMapping(VIS_SKY_MappingType mapping)
{
  m_eMapping = mapping;
  m_vMappingParams.x = (float)mapping; ///< used in vertex shaders

  if(m_pOwnerSky)
  {
    m_pOwnerSky->OnLayerChanged(this);
  }
}

void VSkyLayer::SetTextureFile(const char *texFile)
{
  m_spTexture = LoadSkyTexture(texFile);
}

void VSkyLayer::SetTexture(VTextureObject *pTex)
{
  m_spTexture = pTex;
}


void VSkyLayer::SetCubemapTextures(VTextureObject *pRight, VTextureObject *pLeft, VTextureObject *pFront, VTextureObject *pBack, VTextureObject *pTop, VTextureObject *pBottom)
{
  m_spCubemapFace[0] = pLeft;
  m_spCubemapFace[1] = pRight;
  m_spCubemapFace[2] = pBack;
  m_spCubemapFace[3] = pFront;
  m_spCubemapFace[4] = pTop;
  m_spCubemapFace[5] = pBottom;
}

void VSkyLayer::SetCubemapTextureFiles(const char *rightTex, const char *leftTex, const char *frontTex, const char *backTex, const char *topTex, const char *bottomTex)
{
  m_spCubemapFace[0] = LoadSkyTexture(leftTex);
  m_spCubemapFace[1] = LoadSkyTexture(rightTex);
  m_spCubemapFace[2] = LoadSkyTexture(backTex);
  m_spCubemapFace[3] = LoadSkyTexture(frontTex);
  m_spCubemapFace[4] = LoadSkyTexture(topTex);
  m_spCubemapFace[5] = LoadSkyTexture(bottomTex);
}

#define SAFE_WRITE_TEXTURE(pTex)  {ar.WriteStringBinary(pTex ? pTex->GetFilename() : NULL);}
#define SAFE_LOAD_TEXTURE(pTex,bAssign)   \
  {\
    const char *szFilename = buffer.ReadStringBinary(ar);\
    if (bAssign) \
    {\
      pTex=LoadSkyTexture(szFilename);\
      fProgress += fProgressStep;\
      LOADINGPROGRESS.SetProgress(fProgress);\
    }\
  }

V_IMPLEMENT_SERIALX( VSkyLayer);
void VSkyLayer::SerializeX( VArchive &ar )
{
  char iVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iVersion; VVERIFY(iVersion==0 && "Invalid sky serialization version");

    // main properties
    char mapping;
    ar >> mapping;
    ar >> m_vMappingParams;
    ar >> m_vColor;
    ar >> m_vTextureTransform;
    ar >> m_vScrollSpeed;
    SetMapping((VIS_SKY_MappingType)mapping);

    // textures
    float fProgressStep = mapping==VIS_SKYMAPPING_CUBEMAP ? 100.f/6.f : 100.f;
    float fProgress = 0.f;
    VMemoryTempBuffer<FS_MAX_PATH> buffer;
    SAFE_LOAD_TEXTURE(m_spTexture, mapping==VIS_SKYMAPPING_SPHERICAL || mapping==VIS_SKYMAPPING_CYLINDRICAL);
    for (int i=0;i<6;i++)
      SAFE_LOAD_TEXTURE(m_spCubemapFace[i],mapping==VIS_SKYMAPPING_CUBEMAP);

  } else
  {
    ar << iVersion; // layer version

    // main properties
    ar << (char)m_eMapping;
    ar << m_vMappingParams;
    ar << m_vColor;
    ar << m_vTextureTransform;
    ar << m_vScrollSpeed;

    // textures
    SAFE_WRITE_TEXTURE(m_spTexture);
    for (int i=0;i<6;i++)
      SAFE_WRITE_TEXTURE(m_spCubemapFace[i]);

  }
}

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
