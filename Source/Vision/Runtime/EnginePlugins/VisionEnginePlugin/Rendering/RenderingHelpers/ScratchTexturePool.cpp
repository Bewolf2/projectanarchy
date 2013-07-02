/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/ScratchTexturePool.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

ScratchTexturePool_cl ScratchTexturePool_cl::g_GlobalManager;


ScratchTexturePool_cl::ScratchTexturePool_cl()
{
  m_bCallbackRegistered = false;
  m_iNumScratchTextures = 0;
  memset(m_iScratchIndex, 0, MAX_NUM_SCRATCH_TEXTURES*sizeof(int));  
}

ScratchTexturePool_cl::~ScratchTexturePool_cl()
{
  VASSERT(m_iNumScratchTextures == 0);
}

void ScratchTexturePool_cl::OneTimeInit()
{
  if (!m_bCallbackRegistered)
  {
    Vision::Callbacks.OnVideoDeinitializing += this;
    Vision::Callbacks.OnReferenceContextChanged += this;
  }
  m_bCallbackRegistered = true;
}

void ScratchTexturePool_cl::OneTimeDeInit()
{
  if (m_bCallbackRegistered)
  {
    Vision::Callbacks.OnReferenceContextChanged -= this;
    Vision::Callbacks.OnVideoDeinitializing -= this;
  }
  m_bCallbackRegistered = false;
}

ScratchTexturePool_cl& ScratchTexturePool_cl::GlobalManager()
{
  return g_GlobalManager;
}

void ScratchTexturePool_cl::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender == &Vision::Callbacks.OnVideoDeinitializing)
  {
    ReleaseAllScratchTextures();
  }
  else if(pData->m_pSender == &Vision::Callbacks.OnReferenceContextChanged)
  {
    PurgeUnusedTextures();
  }
}

void ScratchTexturePool_cl::ReleaseAllScratchTextures()
{
  for (int i=0; i<m_iNumScratchTextures; i++)
    m_spScratchTextures[i] = NULL;
  m_iNumScratchTextures = 0;
}

void ScratchTexturePool_cl::PurgeUnusedTextures()
{
  int i,j = 0;
  for (i=0; i<m_iNumScratchTextures; i++)
  {
    if (m_spScratchTextures[i] != NULL)
    {
      VASSERT(m_spScratchTextures[i]->GetRefCount() >= 2);
      if (m_spScratchTextures[i]->GetRefCount() > 2)
      {
        m_iScratchIndex[j] = m_iScratchIndex[i];
        m_spScratchTextures[j++] = m_spScratchTextures[i];
      }
      else
      {
        m_spScratchTextures[i] = NULL;
      }
    }
  }
  for (i=j; i<m_iNumScratchTextures; i++)
  {
    m_iScratchIndex[i] = 0;
    m_spScratchTextures[i] = NULL;
  }
  m_iNumScratchTextures = j;
}

VisRenderableTexture_cl *ScratchTexturePool_cl::GetScratchTexture(int iWidth, int iHeight, 
  VTextureLoader::VTextureFormat_e eFormat, int iIndex, int iEdramOffset, bool bPreferSwizzled, 
  int iMSAASamples, int iHiZOffset, bool bUseTiledMemoryOnPS3, bool bUseExpandFormatOnXbox360)
{
  PurgeUnusedTextures();

  if (iMSAASamples == 1)
    iMSAASamples = 0;

  for (int i=0; i<m_iNumScratchTextures; i++)
  {
    VASSERT(m_spScratchTextures[i]!=NULL);
    const VisRenderableTextureConfig_t *pConfig = m_spScratchTextures[i]->GetConfig();
    if (pConfig->m_iHeight == iHeight && pConfig->m_iWidth == iWidth && pConfig->m_eFormat == eFormat && m_iScratchIndex[i] == iIndex && (pConfig->m_iMultiSampling == iMSAASamples)
      
      #if defined (_VISION_XENON)
        && pConfig->m_iEdramOffset == iEdramOffset
        && pConfig->m_bExpand == bUseExpandFormatOnXbox360
      #endif
      
      
      #ifdef _VISION_PS3
        && pConfig->m_bSwizzled == bPreferSwizzled
        && (pConfig->m_bUseTiledMemory == bUseTiledMemoryOnPS3)
      #endif
      )
    {
      return m_spScratchTextures[i];
    } 
  }

  int iNewIndex = m_iNumScratchTextures++;
  VASSERT(iNewIndex < MAX_NUM_SCRATCH_TEXTURES);
  m_iScratchIndex[iNewIndex] = iIndex;
  VisRenderableTextureConfig_t config;
  config.m_iWidth = iWidth;
  config.m_iHeight = iHeight;
  config.m_eFormat = eFormat;
  config.m_bIsDepthStencilTarget = VTextureObject::IsDepthStencilFormat(eFormat);
  config.m_iMultiSampling = iMSAASamples;

  #ifdef _VISION_PS3
    config.m_bResolve = false;
    config.m_bSwizzled = bPreferSwizzled;
    config.m_bUseTiledMemory = bUseTiledMemoryOnPS3;
    config.m_iHiZOffset = iHiZOffset;
    if ((bPreferSwizzled) || (!bUseTiledMemoryOnPS3))
    {
      config.m_bUseTiledMemory = false;
      config.m_bUseCompression = false;
      config.m_iHiZOffset = 0;
    }
  #endif
  
  #if defined (_VISION_XENON)
    config.m_bResolve = false;
    config.m_iEdramOffset = iEdramOffset;
    config.m_iHiZOffset = iHiZOffset;
    config.m_bExpand = bUseExpandFormatOnXbox360;
  #endif

  char szScratchTexname[1024];
  sprintf(szScratchTexname, "<ScratchTexture%i>", iNewIndex);

  m_spScratchTextures[iNewIndex] = Vision::TextureManager.CreateRenderableTexture(szScratchTexname, config);
  m_spScratchTextures[iNewIndex]->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  return m_spScratchTextures[iNewIndex];
}

VisRenderableTexture_cl *ScratchTexturePool_cl::GetScratchTexture(VisRenderableTextureConfig_t &config, int iIndex)
{
  PurgeUnusedTextures();

  for (int i=0; i<m_iNumScratchTextures; i++)
  {
    VASSERT(m_spScratchTextures[i]!=NULL);
    if ((*m_spScratchTextures[i]->GetConfig() == config) && (m_iScratchIndex[i] == iIndex))
      return m_spScratchTextures[i];
  }

  int iNewIndex = m_iNumScratchTextures++;
  VASSERT(iNewIndex < MAX_NUM_SCRATCH_TEXTURES);
  m_iScratchIndex[iNewIndex] = iIndex;

  char szScratchTexname[1024];
  sprintf(szScratchTexname, "<ScratchTexture%i>", iNewIndex);

  m_spScratchTextures[iNewIndex] = Vision::TextureManager.CreateRenderableTexture(szScratchTexname, config);
  m_spScratchTextures[iNewIndex]->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);

  return m_spScratchTextures[iNewIndex];
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
