/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

VDialogFrame::VDialogFrame()
{
  m_pOwner = NULL;
  m_fBorderTop = m_fBorderBottom = m_fBorderLeft = m_fBorderRight = 0.f;
  m_iColor = V_RGBA_WHITE;
}


VRectanglef VDialogFrame::GetClientRect() const
{
  VASSERT(m_pOwner);
  VRectanglef rect = m_pOwner->GetBoundingBox();
  rect.m_vMin += hkvVec2(m_fBorderLeft,m_fBorderTop);
  rect.m_vMax -= hkvVec2(m_fBorderRight,m_fBorderBottom);
  return rect;
}


void VDialogFrame::SetTexture(VTextureObject *pTex)
{
  m_spFrameTex=pTex;
  m_spTexAnim = Vision::TextureManager.GetAnimationInstance(pTex);
  if (m_spFrameTex)
  {
    m_fBorderLeft = m_fBorderRight = (float)m_spFrameTex->GetTextureWidth() * 0.5f;
    m_fBorderTop = m_fBorderBottom = (float)m_spFrameTex->GetTextureHeight() * 0.5f;
  }
  else
  {
    m_fBorderLeft = m_fBorderRight = m_fBorderTop = m_fBorderBottom = 0.f;
  }
}


bool VDialogFrame::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!pNode) 
    return false;

  // frame texture name
  SetTexture(VGUIManager::LoadTexture(XMLHelper::Exchange_String(pNode,"texture",NULL,bWrite),szPath));
  XMLHelper::Exchange_Color(pNode, "color", m_iColor, bWrite);

  return true;
}


V_IMPLEMENT_SERIALX( VDialogFrame);
void VDialogFrame::SerializeX( VArchive &ar )
{
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    ar >> m_pOwner;
    VTextureObject *pTex = (VTextureObject *)ar.ReadProxyObject();
    SetTexture(pTex);
    ar >> m_iColor;
  } else
  {
    ar << iLocalVersion;
    ar << m_pOwner;
    ar.WriteProxyObject(m_spFrameTex);
    ar << m_iColor;
  }
}



void VDialogFrame::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VASSERT(m_pOwner);
  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState();
  hkvVec2 v1 = m_pOwner->GetAbsPosition();
  hkvVec2 v2 = v1 + m_pOwner->GetSize();
  VTextureObject *pTex = GetTexture();
  if (pTex)
  {
    float bx = m_fBorderLeft;
    float by = m_fBorderTop;
    float hx = 1.f / bx;
    float hy = 1.f / by;
    bx -= 1.f;
    by -= 1.f;

    Overlay2DVertex_t v[(4+4+1)*6]; // 9 quads
    int iVertexCount = 0;
    VColorRef iColor = m_iColor*parentState.iFadeColor;

    // corners
    IVRender2DInterface::CreateQuadVertices(v1.x,v1.y, v1.x+bx,v1.y+by, 0,0,0.5f-hx,0.5f-hy, iColor, &v[iVertexCount]); iVertexCount+=6;
    IVRender2DInterface::CreateQuadVertices(v2.x-bx,v1.y, v2.x,v1.y+by, 0.5f+hx,0,1,0.5f-hy, iColor, &v[iVertexCount]); iVertexCount+=6;
    IVRender2DInterface::CreateQuadVertices(v2.x-bx,v2.y-by, v2.x,v2.y, 0.5f+hx,0.5f+hy,1,1, iColor, &v[iVertexCount]); iVertexCount+=6;
    IVRender2DInterface::CreateQuadVertices(v1.x,v2.y-by, v1.x+bx,v2.y, 0,0.5f+hy,0.5f-hx,1, iColor, &v[iVertexCount]); iVertexCount+=6;
  
    // edges
    IVRender2DInterface::CreateQuadVertices(v1.x+bx,v1.y, v2.x-bx,v1.y+by, 0.5f-hx,0,0.5f+hx,0.5f-hy, iColor, &v[iVertexCount]); iVertexCount+=6;
    IVRender2DInterface::CreateQuadVertices(v1.x+bx,v2.y-by, v2.x-bx,v2.y, 0.5f-hx,0.5f+hy,0.5f+hx,1, iColor, &v[iVertexCount]); iVertexCount+=6;
    IVRender2DInterface::CreateQuadVertices(v1.x,v1.y+by, v1.x+bx,v2.y-by, 0,0.5f-hy,0.5f-hx,0.5f+hy, iColor, &v[iVertexCount]); iVertexCount+=6;
    IVRender2DInterface::CreateQuadVertices(v2.x-bx,v1.y+by, v2.x,v2.y-by, 0.5f+hx,0.5f-hy,1,0.5f+hy, iColor, &v[iVertexCount]); iVertexCount+=6;
    
    // inner rect
    IVRender2DInterface::CreateQuadVertices(v1.x+bx,v1.y+by, v2.x-bx,v2.y-by, 0.5f-hx,0.5f-hy,0.5f+hx,0.5f+hy, iColor, &v[iVertexCount]); iVertexCount+=6;

    Graphics.Renderer.Draw2DBuffer(iVertexCount, v, pTex, state);
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
