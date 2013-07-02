/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VGUICommandBuffer.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


#define COMMAND_INVALID       0
#define COMMAND_DRAW          1
#define COMMAND_DRAWSHADER    2
#define COMMAND_SCISSOR       3
#define COMMAND_DEPTH         4

VGUICommandBuffer::VGUICommandBuffer() : m_Commands(0,0)
{
  m_iCommandCount = 0;
  m_eCacheState = Cache_Dirty;
  m_fLastDepth = FLT_MAX;
}

void VGUICommandBuffer::Reset()
{
  m_iCommandCount = 0;
  m_fLastDepth = FLT_MAX;
}

void VGUICommandBuffer::Draw2DBuffer(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, const VSimpleRenderState_t &iProperties)
{
  if (m_iLastCommand==COMMAND_DRAW && m_pLastTex==pTexture && m_iLastState==iProperties) // append
  {
    if (m_eCacheState != Cache_DryRun)
    {
      int *pOldVertexCount = ((int *)&m_Commands.GetDataPtr()[m_iLastCommandBlockPos]);
      (*pOldVertexCount) += iVertexCount;
    }
  }
  else
  {
    AppendCommand(COMMAND_DRAW);
    Append(&iVertexCount,sizeof(int));
    Append(&pTexture,sizeof(VTextureObject *));
    Append(&iProperties,sizeof(iProperties));
    m_iLastState = iProperties;
    m_pLastTex = pTexture;
  }

  Append(pVertices,sizeof(Overlay2DVertex_t)*iVertexCount);
}

void VGUICommandBuffer::Draw2DBufferWithShader(int iVertexCount, Overlay2DVertex_t *pVertices, VTextureObject *pTexture, VCompiledShaderPass &shader)
{
  if (m_iLastCommand==COMMAND_DRAWSHADER && m_pLastTex==pTexture && m_pLastShader==&shader) // append
  {
    if (m_eCacheState != Cache_DryRun)
    {
      int *pOldVertexCount = ((int *)&m_Commands.GetDataPtr()[m_iLastCommandBlockPos]);
      (*pOldVertexCount) += iVertexCount;
    }
  }
  else
  {
    m_pLastShader = &shader;
    m_pLastTex = pTexture;
    AppendCommand(COMMAND_DRAWSHADER);
    Append(&iVertexCount,sizeof(int));
    Append(&pTexture,sizeof(VTextureObject *));
    Append(&m_pLastShader,sizeof(VCompiledShaderPass &));
  }
  Append(pVertices,sizeof(Overlay2DVertex_t)*iVertexCount);
}

void VGUICommandBuffer::SetScissorRect(const VRectanglef *pScreenRect)
{
  if (m_iLastCommand==COMMAND_SCISSOR) // take a shortcut: overwrite last command
    m_iCommandCount = m_iLastCommandBlockPos - 1;

  bool bIsNull = pScreenRect==NULL;
  AppendCommand(COMMAND_SCISSOR,&bIsNull,sizeof(bool));
  if (!bIsNull)
    Append(pScreenRect,sizeof(VRectanglef));
}

void VGUICommandBuffer::SetDepth(float fZCoord)
{
  if (m_fLastDepth==fZCoord)
    return;

  m_fLastDepth = fZCoord;

  if (m_iLastCommand==COMMAND_DEPTH) // take a shortcut: overwrite last command
    m_iCommandCount = m_iLastCommandBlockPos - 1;

  AppendCommand(COMMAND_DEPTH,&fZCoord,sizeof(fZCoord));
}




void VGUICommandBuffer::Append(const void *pData, int iByteCount)
{
  if (!iByteCount)
    return;
  if (m_eCacheState != Cache_DryRun)
  {
    m_Commands.EnsureSize(m_iCommandCount + iByteCount);
    memcpy(&m_Commands.GetDataPtr()[m_iCommandCount],pData,iByteCount);
  }
  m_iCommandCount += iByteCount;
}

void VGUICommandBuffer::AppendCommand(char cmd, const void *pData, int iByteCount)
{
  m_iLastCommand = cmd;
  Append(&cmd,1);
  m_iLastCommandBlockPos = m_iCommandCount;
  Append(pData,iByteCount);
}



bool VGUICommandBuffer::UpdateCache(IVRenderableItem *pItem, VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  if (m_eCacheState == Cache_Valid)
  {
    ProcessCache(Graphics,parentState);
    return true;
  }

  if (m_eCacheState == Cache_Dirty)
  {
    m_iLastCommand = COMMAND_INVALID;
    m_pLastTex = NULL;
    m_iLastState = 0;
    m_pLastShader = NULL;
    m_iLastCommandBlockPos = 0;

    m_eCacheState = Cache_DryRun;
    VGraphicsInfo newGfx(*this,Graphics.GUIContext); // note this calls scissor already so reset afterwards
    newGfx.ClippingStack = Graphics.ClippingStack;

    Reset();
    pItem->OnPaint(newGfx,parentState);

    m_eCacheState = Cache_FinalRun;
    Reset();
    pItem->OnPaint(newGfx,parentState);

    m_eCacheState = Cache_Valid;
    ProcessCache(Graphics,parentState);
    return true;
  }

  return false;
}

#define TEST_CACHE_SPEED
#undef TEST_CACHE_SPEED

void VGUICommandBuffer::ProcessCache(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  int iPos = 0;
  const char *pBuffer = m_Commands.GetDataPtr();
  IVRender2DInterface &renderer(Graphics.Renderer);
  int iCommands = 0;

  while (iPos<m_iCommandCount)
  {
    iCommands++;
    char cmd = pBuffer[iPos++];
    switch (cmd)
    {
    case COMMAND_DRAW: {
        int iVertexCount = *((int *)&pBuffer[iPos]);iPos+=sizeof(int);
        VTextureObject *pTexture = *((VTextureObject **)&pBuffer[iPos]);iPos+=sizeof(VTextureObject *);
        VSimpleRenderState_t *pProperties = ((VSimpleRenderState_t *)&pBuffer[iPos]);iPos+=sizeof(VSimpleRenderState_t);
        Overlay2DVertex_t *pVertex = (Overlay2DVertex_t *)&pBuffer[iPos];iPos+=sizeof(Overlay2DVertex_t)*iVertexCount;
      #ifndef TEST_CACHE_SPEED
        renderer.Draw2DBuffer(iVertexCount,pVertex,pTexture,*pProperties);
      #endif
      }break;
    case COMMAND_DRAWSHADER: {
        int iVertexCount = *((int *)&pBuffer[iPos]);iPos+=sizeof(int);
        VTextureObject *pTexture = *((VTextureObject **)&pBuffer[iPos]);iPos+=sizeof(VTextureObject *);
        VCompiledShaderPass *pShader = *((VCompiledShaderPass **)&pBuffer[iPos]);iPos+=sizeof(VCompiledShaderPass &);
        Overlay2DVertex_t *pVertex = (Overlay2DVertex_t *)&pBuffer[iPos];iPos+=sizeof(Overlay2DVertex_t)*iVertexCount;
      #ifndef TEST_CACHE_SPEED
        renderer.Draw2DBufferWithShader(iVertexCount,pVertex,pTexture,*pShader);
      #endif
      }break;
    case COMMAND_SCISSOR: {
        bool bIsNull = *((bool *)&pBuffer[iPos]); iPos+=sizeof(bool);
        VRectanglef *pRect = NULL;
        if (!bIsNull)
          pRect = (VRectanglef *)&pBuffer[iPos];iPos+=sizeof(VRectanglef);
        #ifndef TEST_CACHE_SPEED
          renderer.SetScissorRect(pRect);
        #endif
      }break;
    case COMMAND_DEPTH: {
        float fDepth = *((float *)&pBuffer[iPos]);iPos+=sizeof(float);
        #ifndef TEST_CACHE_SPEED
          renderer.SetDepth(fDepth);
        #endif
      }break;
    default:
      VASSERT_MSG(FALSE,"Invalid command buffer");
      return;
    }
  }

  VASSERT_MSG(iPos==m_iCommandCount,"Invalid command buffer length");
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
