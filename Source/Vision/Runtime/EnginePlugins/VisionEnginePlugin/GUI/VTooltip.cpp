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

V_IMPLEMENT_SERIAL( VTooltip, VWindowBase, 0, &g_VisionEngineModule );

VTooltip::VTooltip(IVGUIContext *pContext)
{
  m_pContext = pContext;
  m_fDelay = 1.2f; // time until tooltip appears
  m_pText = new VTextState();
  m_iBackgroundColor = V_RGBA_YELLOW;
  m_iBorderColor = V_RGBA_BLACK;
  m_pText->SetColor(V_RGBA_BLACK);
  m_fBorderSize = 1.f; // black border thickness
  m_fTextBorder = 2.f; // border around text
}


VTooltip::~VTooltip()
{
  delete m_pText;
}

void VTooltip::SetText(const char *szString) 
{
  if (!m_pText->GetFont())
    m_pText->SetFont(GetContext()->GetManager()->GetDefaultFont());

  m_pText->SetText(szString,true);
}

const char *VTooltip::GetText() const 
{
  return m_pText->GetText();
}


void VTooltip::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  if (m_fDelay>0.f)
    return;

  hkvVec2 vMousePos = GetContext()->GetCurrentMousePos();
  VCursor *pCursor = GetContext()->GetCurrentCursor();
  if (!pCursor)
    return;

  // offset tooltip position by cursor size
  VRectanglef rect;
  rect = pCursor->GetCursorRect();
  vMousePos += rect.m_vMax;

  const float fFullBorder = m_fBorderSize+m_fTextBorder;
  
  VRectanglef textRect;
  hkvVec2 v = m_pText->GetSize(&textRect);
  SetSize(v.x,v.y);
  m_pText->SetTextOfs(-textRect.m_vMin);

  float xpos= vMousePos.x-fFullBorder;
  float ypos= vMousePos.y-fFullBorder;

  int iSizeX, iSizeY;
  Vision::Contexts.GetCurrentContext()->GetSize(iSizeX, iSizeY);

  if((xpos + v.x) > iSizeX)
    xpos = iSizeX - v.x - fFullBorder*2;

  if((ypos + v.y) > iSizeY)
    ypos = iSizeY - v.y - fFullBorder*2;

  SetPosition(xpos,ypos);
  

  // fade in the tooltip
  float fFadeValue = hkvMath::Abs (m_fDelay)*4.f;
  if (fFadeValue>1.f) fFadeValue = 1.f;

  // render text on background
  VItemRenderInfo thisState(parentState,this,fFadeValue);
  if (m_iBackgroundColor.a>0)
  {
    VRectanglef clientRect = GetBoundingBox();
    VSimpleRenderState_t iState = VGUIManager::DefaultGUIRenderState();
    if (m_fBorderSize>0.f)
    {
      VColorRef iColor = m_iBorderColor;
      iColor.a = (int)((float)iColor.a*fFadeValue);
      hkvVec2 vb(fFullBorder,fFullBorder);
      Graphics.Renderer.DrawSolidQuad(clientRect.m_vMin-vb,clientRect.m_vMax+vb,iColor,iState);
    }
    
    VColorRef iColor = m_iBackgroundColor;
    iColor.a = (int)((float)iColor.a*fFadeValue);
    hkvVec2 vb(m_fTextBorder,m_fTextBorder);
    Graphics.Renderer.DrawSolidQuad(clientRect.m_vMin-vb,clientRect.m_vMax+vb,iColor,iState);
  }
  m_pText->OnPaint(Graphics,thisState);
}


void VTooltip::OnTick(float dtime)
{
  m_fDelay -= dtime;
}


#define COPY_MEMBER(m) {pClone->m = m;}
VTooltip* VTooltip::CloneTooltip()
{
  VTooltip *pClone = (VTooltip *)GetTypeId()->CreateInstance();
  COPY_MEMBER(m_pContext);
  COPY_MEMBER(m_fDelay);
  *pClone->m_pText = *m_pText; // assignment operator
  COPY_MEMBER(m_iBackgroundColor);
  COPY_MEMBER(m_iBorderColor);
  COPY_MEMBER(m_fBorderSize);
  COPY_MEMBER(m_fTextBorder);

  return pClone;
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
