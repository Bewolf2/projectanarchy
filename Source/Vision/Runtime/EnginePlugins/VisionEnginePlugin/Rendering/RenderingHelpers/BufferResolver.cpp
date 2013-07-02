/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/BufferResolver.hpp>

VBufferResolver::VBufferResolver(IVRendererNode* pRenderNode, VisRenderContext_cl* pRenderContextToResolve, unsigned int uiRenderHook) :
  m_spRenderContextToResolve(pRenderContextToResolve), 
  m_uiRenderHook(uiRenderHook), 
  m_pRenderNode(pRenderNode),
  m_eInitMode(VIM_Invalid), 
  m_iBufferWidth(0),
  m_iBufferHeight(0),
  m_eBufferFormat(VTextureLoader::UNKNOWN)
{
  if(uiRenderHook != 0)
    Vision::Callbacks.OnRenderHook += this;
}

bool VBufferResolver::Initialize(VInitMode_e eInitMode, VTextureObject* pResolvedBuffer /*= NULL */, VisRenderableTextureConfig_t* pOverrideTextureConfig /*= NULL*/)
{
  m_eInitMode = eInitMode;    
  switch(eInitMode)
  {
  case VIM_CreateNewResolveBuffer:
    {
      CreateResolveBuffer(pOverrideTextureConfig);
    }
    break;
  case VIM_UseGivenResolveBuffer:
    {
      m_spResolvedBuffer = pResolvedBuffer;
    }
  default:
    VASSERT_MSG(0, "Invalid init mode");
  }
  
  return m_spResolvedBuffer != NULL;
}

VBufferResolver::~VBufferResolver()
{
  m_eInitMode = VIM_Invalid;
  if(m_uiRenderHook != 0)
    Vision::Callbacks.OnRenderHook -= this;
}

void VBufferResolver::SetRenderHook(unsigned int uiRenderHook)
{
  if(m_uiRenderHook != uiRenderHook)
  {
    if(m_uiRenderHook != 0 && uiRenderHook == 0)
      Vision::Callbacks.OnRenderHook -= this;
    if(m_uiRenderHook == 0 && uiRenderHook != 0)
      Vision::Callbacks.OnRenderHook += this;
    m_uiRenderHook = uiRenderHook;
  }
}

void VBufferResolver::ResolveBuffer()
{
  INSERT_PERF_MARKER_SCOPE("VBufferResolver::ResolveBuffer");

  switch(m_eInitMode)
  {
    case VIM_CreateNewResolveBuffer:
      {
        VisRenderableTextureConfig_t pConfig = *m_spRenderContextToResolve->GetTargetConfig();

        if(pConfig.m_iWidth != m_iBufferWidth || pConfig.m_iHeight != m_iBufferHeight || pConfig.m_eFormat != m_eBufferFormat)
        {
          CreateResolveBuffer(NULL); // No override config needed, will use rendercontext config anyway
        }

        m_spRenderContextToResolve->ResolveToTexture(m_spResolvedBuffer, false);
      }
      break;

    case VIM_UseGivenResolveBuffer:
      {
        m_spRenderContextToResolve->ResolveToTexture(m_spResolvedBuffer, false);
      }
      break;

    default:
      VASSERT_MSG(0, "unhandled enum value");
  }
}

void VBufferResolver::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnRenderHook && 
    ((VisRenderHookDataObject_cl *)pData)->m_iEntryConst == m_uiRenderHook)
  {
    if(Vision::Renderer.GetCurrentRendererNode() == m_pRenderNode)
      ResolveBuffer();
  }
}

void VBufferResolver::CreateResolveBuffer(VisRenderableTextureConfig_t* pOverrideTextureConfig)
{
  VisRenderableTextureConfig_t pConfig = pOverrideTextureConfig != NULL ? *pOverrideTextureConfig : *m_spRenderContextToResolve->GetTargetConfig();
  pConfig.m_iMultiSampling = VVIDEO_MULTISAMPLE_OFF;
  pConfig.m_bRenderTargetOnly = false;
  pConfig.m_bResolve = false;
  m_spResolvedBuffer = Vision::TextureManager.CreateRenderableTexture("<ResolvedColorBuffer>", pConfig);
  m_iBufferWidth = pConfig.m_iWidth;
  m_iBufferHeight = pConfig.m_iHeight;
  m_eBufferFormat = pConfig.m_eFormat;
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
