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



///////////////////////////////////////////////////////////////////////////////////////////////////////
// VImageState(s) class
///////////////////////////////////////////////////////////////////////////////////////////////////////

#define CHECK_TYPESTR(t) {if (!_stricmp(szModeStr,#t)) return t;}
VImageState::ImageStretchMode_e VImageState::GetStretchMode(const char *szModeStr)
{
  if (!szModeStr || !szModeStr[0]) return STRETCHED; // default
  CHECK_TYPESTR(TEXTURE_SIZE);
  CHECK_TYPESTR(STRETCHED);
  CHECK_TYPESTR(BORDER);
  VASSERT(!"Unknown ImageStretchMode_e mode");
  return STRETCHED;
}


static inline void _ApplyTransformation(const hkvMat3 &matrix, float &x, float &y)
{
  float oldX = x;
  float oldY = y;
  x=oldX*matrix.m_Column[0][0] + oldY*matrix.m_Column[1][0] + matrix.m_Column[2][0];
  y=oldX*matrix.m_Column[0][1] + oldY*matrix.m_Column[1][1] + matrix.m_Column[2][1];
}


void VImageState::ApplyTransformation(int iVertCount, Overlay2DVertex_t *v, const hkvMat3 *pUVTransform, const hkvMat3 *pPosTransform)
{
  if (!pUVTransform && !pPosTransform)
    return;
  for (int i=0;i<iVertCount;i++,v++)
  {
    // transform every UV with a 3x3 matrix (rotation+translation for 2D)
    if (pUVTransform)
      _ApplyTransformation(*pUVTransform, v->texCoord.x, v->texCoord.y);

    // transform every position with a 3x3 matrix (rotation+translation for 2D)
    if (pPosTransform)
      _ApplyTransformation(*pPosTransform, v->screenPos.x, v->screenPos.y);
  }
}


void VImageState::CreateTransformation(hkvMat3 &dest, const hkvVec2 &vOfs, float fRotation, float fScaling)
{
  dest.setIdentity();

  float sw = fScaling * hkvMath::sinDeg (fRotation);
  float cw = fScaling * hkvMath::cosDeg (fRotation);
  // upper 2x2 rotation part
  dest.m_Column[0][0] = cw; dest.m_Column[1][0] = -sw;
  dest.m_Column[0][1] = sw; dest.m_Column[1][1] = cw;
  
  // translation column
  dest.m_Column[2][0] = vOfs.x;
  dest.m_Column[2][1] = vOfs.y;
}


#define RENDER_VERTICES(_count) \
    ApplyTransformation(_count,v,m_pUVTransform,m_pPosTransform);\
    if (m_spTechnique!=NULL)\
    {\
      for (int i=0;i<m_spTechnique->GetShaderCount();i++)\
        Graphics.Renderer.Draw2DBufferWithShader(_count, v, pTex, *m_spTechnique->GetShader(i));\
    }\
    else\
      Graphics.Renderer.Draw2DBuffer(_count, v, pTex, state);

void VImageState::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VTextureObject *pTex = GetCurrentTexture();
  if (!pTex)
    return;

  VASSERT(parentState.m_pWindow);
  VRectanglef rect = parentState.m_pWindow->GetBoundingBox();
  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState(m_eTranspType);
  VColorRef iColor = parentState.iFadeColor*m_iColor;
  state.SetFlag(m_iAdditionalStateFlags); // apply filtering for instance

  if (m_eStretchMode==BORDER)
  {
    hkvVec2 v1 = rect.m_vMin;
    hkvVec2 v2 = rect.m_vMax;
    float bx = (float)pTex->GetTextureWidth()*0.5f;
    float by = (float)pTex->GetTextureHeight()*0.5f;
    float hx = 1.f / bx;
    float hy = 1.f / by;
    bx -= 1.f;
    by -= 1.f;
    Overlay2DVertex_t v[(4+4+1)*6]; // 9 quads
    int iVertexCount = 0;

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

    RENDER_VERTICES(iVertexCount);
  } 
  else if (m_eStretchMode==STRETCHED)
  {
    Overlay2DVertex_t v[6];    
    IVRender2DInterface::CreateQuadVertices(rect.m_vMin.x,rect.m_vMin.y,rect.m_vMax.x,rect.m_vMax.y,texCoord.m_vMin.x,texCoord.m_vMin.y,texCoord.m_vMax.x,texCoord.m_vMax.y,iColor,v);
    RENDER_VERTICES(6);
  }
  else // actual texture size
  {
    Overlay2DVertex_t v[6];
    rect.m_vMax.x = rect.m_vMin.x + (float)pTex->GetTextureWidth();
    rect.m_vMax.y = rect.m_vMin.y + (float)pTex->GetTextureHeight();
    IVRender2DInterface::CreateQuadVertices(rect.m_vMin.x,rect.m_vMin.y,rect.m_vMax.x,rect.m_vMax.y,texCoord.m_vMin.x,texCoord.m_vMin.y,texCoord.m_vMax.x,texCoord.m_vMax.y,iColor,v);
    RENDER_VERTICES(6);
  }
}

bool VImageState::Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!pNode)
    return false;

  // button texture name
  const char *szTexture = XMLHelper::Exchange_String(pNode,"texture",NULL,bWrite);
  if (szTexture) // only assign when defined to preserve normal state
    SetTexture(VGUIManager::LoadTexture(szTexture,szPath));

  // texture coordinates and stretch mode
  float *texCoordData = (float*)(&texCoord.m_vMin);
  XMLHelper::Exchange_Floats(pNode,"texcoords",texCoordData,4,bWrite);
  const char *szStretchMode = XMLHelper::Exchange_String(pNode,"stretchmode",NULL,bWrite);
  if (szStretchMode)
    m_eStretchMode = GetStretchMode(szStretchMode);

  // button color
  XMLHelper::Exchange_Color(pNode,"color",m_iColor,bWrite);
  XMLHelper::Exchange_Transparency(pNode,"transparency",m_eTranspType,bWrite);

  // other states
  bool bFiltering = false;
  XMLHelper::Exchange_Bool(pNode,"filtering",bFiltering,bWrite);
  if (bFiltering)
    m_iAdditionalStateFlags |= RENDERSTATEFLAG_FILTERING;

  // mouse over cursor
  const char *szCursor = XMLHelper::Exchange_String(pNode,"cursor",NULL,bWrite);
  if (szCursor) // only assign when defined to preserve normal state
    m_spCursor = pOwner->GetMenuManager()->LoadCursorResource(szCursor,szPath);

  return true;
}

  
hkvVec2 VImageState::GetSize() const
{
  if (!m_spTexture)
    return hkvVec2(0,0);
  return hkvVec2(
    (float)m_spTexture->GetTextureWidth() * texCoord.GetSizeX(),
    (float)m_spTexture->GetTextureHeight() * texCoord.GetSizeY()
    );
}

hkvVec2 VImageState::GetTextureSize() const
{
  if (!m_spTexture)
    return hkvVec2(0,0);
  return hkvVec2((float)m_spTexture->GetTextureWidth(),(float)m_spTexture->GetTextureHeight());
}


void VImageStates::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  // render with button config dependent on owner state
  VWindowBase::ControlState_e eState = parentState.m_pWindow->GetCurrentState(parentState.bForceDisabled);

  VImageState *pState = &m_States[eState];    
  if (!pState->IsDefined())
    pState = &m_States[VWindowBase::NORMAL];
  pState->OnPaint(Graphics,parentState);
}



// this is the exact same code as in VTextStates::Build
bool VImageStates::Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!pNode)
    return false;

  // first, get the normal state from this node
  m_States[VWindowBase::NORMAL].Build(pOwner,pNode,szPath,bWrite);

  // copy over to the other states...
  m_States[VWindowBase::MOUSEOVER] = m_States[VWindowBase::NORMAL];
  m_States[VWindowBase::SELECTED] = m_States[VWindowBase::NORMAL];
  m_States[VWindowBase::DISABLED] = m_States[VWindowBase::NORMAL];

  // ...and assign the differences in the "statemodifier" subnodes
  for (TiXmlElement *pStateNode=pNode->FirstChildElement("statemodifier"); pStateNode; pStateNode=pStateNode->NextSiblingElement("statemodifier") )
  {
    const char *szState = XMLHelper::Exchange_String(pStateNode,"state",NULL,bWrite);
    VWindowBase::ControlState_e eState = VWindowBase::GetState(szState);
    m_States[eState].Build(pOwner,pStateNode,szPath,bWrite);
  }

  return m_States[VWindowBase::NORMAL].IsDefined();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// VTextState class
///////////////////////////////////////////////////////////////////////////////////////////////////////


void VTextState::Paint(VGraphicsInfo *pGraphics, VWindowBase *pParentWnd, VColorRef iColor)
{
  m_iNumTextLines = 0;
  const char *szText = GetText();
  if (!m_spFont || !szText || !szText[0])
    return;

  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState();
  if (m_fFontScaling!=1.0f)
    state.SetFlag(RENDERSTATEFLAG_FILTERING);

  VRectanglef v = pParentWnd->GetBoundingBox(); // clipping box of parent control

  if (m_bTextWrap)
  {
    float fMaxWidth = v.GetSizeX()/m_fFontScaling;
    float fLineHeight = m_spFont->GetFontHeight() * m_fRelativeFontHeight * m_fFontScaling;
    hkvVec2 vPos = v.m_vMin;

    const char *szStart = szText;
    VMemoryTempBuffer<512> tmpBuffer;

    while (szStart[0])
    {
      // byte offsets
      int iOffset;
      int iNextOffset;

      // search for next newline
      const char *pCR = strchr(szStart, '\n');

      // compute automatic wrap character index
      int iCharCount = m_spFont->GetCharacterIndexAtPos(szStart, fMaxWidth, -1, false);
      int iWrapOffset = VString::GetUTF8CharacterOffset(szStart, iCharCount);

      if (pCR != NULL && (pCR - szStart) < iCharCount)
      {
        // newline occurs before automatic text wrap
        iOffset = static_cast<int>(pCR - szStart);
        iNextOffset = iOffset + 1;
      }
      else
      {
        // automatic text wrap
        iOffset = iWrapOffset;
        // try to find white space
        if (iOffset > 0)
        {
          while (iOffset > 0 && szStart[iOffset] != 0 && szStart[iOffset] != ' ')
            iOffset--;
          if (iOffset == 0) // no whitespace found? then wrap inside word
            iOffset = iWrapOffset;
        }
        else 
          iOffset = 1;
        iNextOffset = iOffset;
      }

      // put together line
      const int iLineBufferSize = (iOffset+1) * sizeof(char);
      tmpBuffer.EnsureCapacity(iLineBufferSize);
      char *szLineBuffer = static_cast<char*>(tmpBuffer.GetBuffer());

      memcpy(szLineBuffer, szStart, iLineBufferSize);
      szLineBuffer[iOffset] = 0;
      m_iNumTextLines++;

      // draw line
      if (pGraphics)
      {
        hkvVec2 vAlignedPos = m_spFont->GetTextPositionOfs(szLineBuffer, v.GetSize(), 
          m_hAlign, m_vAlign) * m_fFontScaling;
        vAlignedPos.set(vAlignedPos.x + vPos.x, vPos.y); // only consider the x-ofs
        m_spFont->PrintText(&pGraphics->Renderer, vAlignedPos, szLineBuffer, iColor, state, 
          m_fFontScaling, m_pCustomBBox ? m_pCustomBBox : &v);
      }
      vPos.y += fLineHeight;

      szStart = &szStart[iNextOffset];
      while (szStart[0] == ' ')
        szStart++;
    }
    return;
  }

  // recompute the alignment offset
  if (!m_bAlignmentValid)
  {
    m_vCurrentOfs = m_vOfs + m_spFont->GetTextPositionOfs(szText, v.GetSize(),
      m_hAlign,m_vAlign) * m_fFontScaling;
    m_bAlignmentValid = true;
  }

  m_iNumTextLines = 1;
  if (pGraphics)
  {
    m_spFont->PrintText(&pGraphics->Renderer, v.m_vMin+m_vCurrentOfs, szText, 
      iColor, state, m_fFontScaling, m_pCustomBBox ? m_pCustomBBox : &v);
  }
}



void VTextState::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  VColorRef iColor = parentState.iFadeColor*m_iColor;
  Paint(&Graphics,parentState.m_pWindow,iColor);
}



bool VTextState::Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!pNode)
    return false;

  VGUIManager *pManager = pOwner->GetMenuManager();

  // mouse over cursor
  const char *szCursor = XMLHelper::Exchange_String(pNode,"cursor",NULL,bWrite);
  if (szCursor) // only assign when defined to preserve normal state
    m_spCursor = pManager->LoadCursorResource(szCursor,szPath);

  // custom text value
  const char *szUTF8 = XMLHelper::Exchange_String(pNode,"text",NULL,bWrite);
  const char *szText =  pManager->TranslateString(pOwner, szUTF8);  if (szText)
    SetText(szText, true); // sets custom text

  // text color
  XMLHelper::Exchange_Color(pNode,"color",m_iColor,bWrite);

  // font
  const char *szFont = XMLHelper::Exchange_String(pNode,"font",NULL,bWrite);
  if (szFont || !m_spFont) // preserve normal state
    m_spFont = pOwner->GetMenuManager()->LoadFont(szFont,szPath);

  // alignment
  const char *szAlign;
  szAlign = XMLHelper::Exchange_String(pNode,"halign",NULL,bWrite);
  if (szAlign) 
    m_hAlign = VisFont_cl::GetAlignment(szAlign);

  szAlign = XMLHelper::Exchange_String(pNode,"valign",NULL,bWrite);
  if (szAlign)
    m_vAlign = VisFont_cl::GetAlignment(szAlign);

  // relative offset
  XMLHelper::Exchange_Floats(pNode,"ofs",m_vOfs.data,2,bWrite);
  XMLHelper::Exchange_Bool(pNode,"textwrap",m_bTextWrap,bWrite);
  XMLHelper::Exchange_Float(pNode,"lineheight",m_fRelativeFontHeight,bWrite);

  // scaling
  XMLHelper::Exchange_Float(pNode,"scaling",m_fFontScaling,bWrite);
  

  return true;
}




hkvVec2 VTextState::GetSize(VRectanglef *pRect) const
{
  VRectanglef rc(false);
  if (!pRect)
    pRect = &rc;
  if (!m_spFont->GetTextDimension(m_pStringPtr->GetSafeStr(), *pRect))
    return hkvVec2();
  return pRect->m_vMax * m_fFontScaling; // otherwise we have a wrong offset
//  return pRect->GetSize();
}

void VTextStates::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  // render with button config dependent on owner state
  VWindowBase::ControlState_e eState = parentState.m_pWindow->GetCurrentState(parentState.bForceDisabled);
  VTextState *pState = &m_States[eState];    
  pState->OnPaint(Graphics,parentState);
}


void VTextStates::FinishSetup()
{
  VString *pDefault = &m_States[VWindowBase::NORMAL].m_sText;

  // fixup empty strings and fonts
  for (int i=0;i<VWindowBase::STATE_COUNT;i++)
  {
    if (!m_States[i].m_spFont)
      m_States[i].m_spFont = VGUIManager::GlobalManager().GetDefaultFont();
    if (m_States[i].m_pStringPtr->IsEmpty())
        m_States[i].m_pStringPtr = pDefault;
  }

}



// this is the exact same code as in VImageStates::Build
bool VTextStates::Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, 
                        bool bWrite, const VTextStates *pDefaults)
{
  if (!pNode)
    return false;

  // if defaults are defined, init the states with it
  if (pDefaults)
    (*this) = (*pDefaults);

  // first, get the normal state from this node
  m_States[VWindowBase::NORMAL].Build(pOwner,pNode,szPath,bWrite);

  // assign defaults from normal state
  if (!pDefaults)
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      if (i!=VWindowBase::NORMAL)
        m_States[i] = m_States[VWindowBase::NORMAL];
  }

  // then assign the values in the "statemodifier" subnodes
  for (TiXmlElement *pStateNode=pNode->FirstChildElement("statemodifier"); pStateNode; 
    pStateNode=pStateNode->NextSiblingElement("statemodifier") )
  {
    const char *szState = XMLHelper::Exchange_String(pStateNode,"state",NULL,bWrite);
    VWindowBase::ControlState_e eState = VWindowBase::GetState(szState);
    m_States[eState].Build(pOwner,pStateNode,szPath,bWrite);
  }

  // any empty strings left? then use the normal state
  FinishSetup();
  return true;
}



void VTextStates::SetText(const char *szString) 
{
  for (int i=0;i<VWindowBase::STATE_COUNT;i++)
    m_States[i].SetText(szString, false);
}

void VTextStates::SetTextPtr(VString *pText)
{
  for (int i=0;i<VWindowBase::STATE_COUNT;i++)
    m_States[i].SetTextPtr(pText);
}

  
void VTextStates::SetCustomClipRectPtr(const VRectanglef *pCustomBBox)
{
  for (int i=0;i<VWindowBase::STATE_COUNT;i++)
    m_States[i].SetCustomClipRectPtr(pCustomBBox);
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Serialization
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


V_IMPLEMENT_SERIALX( VImageState);
void VImageState::SerializeX( VArchive &ar )
{
  char iLocalVersion = 1;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=1,"Invalid version");

    ar >> (int &)m_eStretchMode >> m_iColor;
    SetTexture((VTextureObject *)ar.ReadProxyObject());
    ar >> texCoord;
    m_spCursor = (VCursor *)ar.ReadProxyObject();
    ar >> (int &)m_eTranspType >> m_iAdditionalStateFlags;
    if (iLocalVersion>=1)
    {
      VisEffectConfig_cl fxConfig;
      fxConfig.SerializeX(ar);
      VCompiledEffect *pFX = fxConfig.GetEffect();
      if (pFX)
        m_spTechnique = pFX->GetDefaultTechnique();
    }

  } 
  else
  {
    ar << iLocalVersion;

    ar << (int)m_eStretchMode << m_iColor;
    ar.WriteProxyObject(m_spTexture);
    ar << texCoord;
    ar.WriteProxyObject(m_spCursor);
    ar << (int)m_eTranspType << m_iAdditionalStateFlags;

    VisEffectConfig_cl fxConfig;
    if (m_spTechnique!=NULL)
      fxConfig.SetEffect(m_spTechnique->m_pOwnerFX); // currently there is no built-in function to (de-)serialize a technique.
    fxConfig.SerializeX(ar); // vers.1
  }
}


V_IMPLEMENT_SERIALX( VImageStates);
void VImageStates::SerializeX( VArchive &ar )
{
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      ar >> m_States[i];
  } 
  else
  {
    ar << iLocalVersion;
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      ar << m_States[i];
  }
}


V_IMPLEMENT_SERIALX( VTextState);
void VTextState::SerializeX( VArchive &ar )
{
  char iLocalVersion = 1;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion<=1,"Invalid version");
    ar >> m_sText;
    ar >> m_iColor;
    VMemoryTempBuffer<FS_MAX_PATH> buffer;
    const char *szFontName = buffer.ReadStringBinary(ar);
    if (szFontName != NULL && szFontName[0])
      m_spFont = VGUIManager::GlobalManager().LoadFont(szFontName);

    ar >> (int &)m_hAlign >> (int &)m_vAlign;
    ar >> m_vOfs >> m_bTextWrap >> m_fRelativeFontHeight;
    if (iLocalVersion>=1)
      ar >> m_fFontScaling; // vers.1
  } 
  else
  {
    ar << iLocalVersion;

    ar << m_sText;
    ar << m_iColor;
    const char *szFontName = 
      (m_spFont!=NULL && m_spFont!=VGUIManager::GlobalManager().GetDefaultFont()) ? 
      m_spFont->GetFilename() : NULL;
    ar << szFontName;
    ar << (int)m_hAlign << (int)m_vAlign;
    ar << m_vOfs << m_bTextWrap << m_fRelativeFontHeight; 
    ar << m_fFontScaling; // vers.1
  }
}

V_IMPLEMENT_SERIALX( VTextStates);
void VTextStates::SerializeX( VArchive &ar )
{
  char iLocalVersion = 0;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      ar >> m_States[i];
    FinishSetup();
  } 
  else
  {
    ar << iLocalVersion;
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      ar << m_States[i];
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
