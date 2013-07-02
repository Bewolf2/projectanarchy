/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VControlHelperObjects.hpp

#ifndef VCONTROLHELPEROBJECTS_HPP_INCLUDED
#define VCONTROLHELPEROBJECTS_HPP_INCLUDED



/// \brief
///   Helper class that can render a single image button state. Each instance represents one state in VImageStates
class VImageState
{
public:

  /// \brief
  ///   Enum used for the SetStretchMode function
  enum ImageStretchMode_e
  {
    TEXTURE_SIZE = 0,  ///< Use actual pixel size of the texture
    STRETCHED = 1,     ///< Stretch the image to fit into the screen rectangle
    BORDER = 2         ///< Render texture in border mode
  };

  /// \brief
  ///   Constructor
  GUI_IMPEXP VImageState()
  {
    m_eStretchMode = STRETCHED;
    m_bDefined = false;
    m_iColor = V_RGBA_WHITE;
    texCoord.Set(0,0,1,1);
    m_pUVTransform = m_pPosTransform = NULL;
    m_eTranspType = VIS_TRANSP_ALPHA;
    m_iAdditionalStateFlags = 0;
  }

  /// \brief
  ///   Internal paint function to render this image
  GUI_IMPEXP void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);
  /// \brief
  ///   Internal function to parse information from XML
  GUI_IMPEXP bool Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite);

  /// \brief
  ///   Valid state of this image (i.e. has a valid texture)
  inline bool IsDefined() const {return m_bDefined;}

  /// \brief
  ///   Return the size in pixels
  GUI_IMPEXP hkvVec2 GetSize() const;

  /// \brief
  ///   Return the native size of the texture
  GUI_IMPEXP hkvVec2 GetTextureSize() const;

  /// \brief
  ///   Associate a custom cursor with this image, when the mouse drags over it
  inline void SetCursor(VCursor* pCursor) {m_spCursor=pCursor;}

  /// \brief
  ///   Return the current cursor. Can be NULL for default curor
  inline VCursor* GetCursor() const {return m_spCursor;}

  /// \brief
  ///   Set the texture coordinates to use (0..1 range)
  inline void SetTextureRange(const VRectanglef &coords) {texCoord = coords;}

  /// \brief
  ///   Get the texture coordinates to use (0..1 range)
  inline const VRectanglef& GetTextureRange() const {return texCoord;}

  /// \brief
  ///   Defines how to fit the texture into the image rectangle
  inline void SetStretchMode(ImageStretchMode_e eMode) {m_eStretchMode = eMode;}

  /// \brief
  ///   Set the image texture 
  inline void SetTexture(VTextureObject *pNewTex)
  {
    m_spTexture = pNewTex;
    m_spTexAnim = Vision::TextureManager.GetAnimationInstance(pNewTex);
    m_bDefined = (pNewTex!=NULL);
  }

  /// \brief
  ///   Return the current texture. For animated textures, it returns the actual frame
  inline VTextureObject* GetCurrentTexture() const
  {
    if (m_spTexAnim)
      return m_spTexAnim->GetCurrentFrame();
    return m_spTexture;
  }

  /// \brief
  ///   Return the texture animation instance (or NULL)
  inline VisTextureAnimInstance_cl *GetTextureAnimInstance() const {return m_spTexAnim;}

  /// \brief
  ///   Return the current tint color
  inline VColorRef GetColor() const {return m_iColor;}

  /// \brief
  ///   Set the current tint color
  inline void SetColor(VColorRef iColor) {m_iColor=iColor;}

  /// \brief
  ///   Returns the current blend type
  inline VIS_TransparencyType GetTransparency() const {return m_eTranspType;}

  /// \brief
  ///   Set a blend type for this image
  inline void SetTransparency(VIS_TransparencyType eTransp) {m_eTranspType=eTransp;}

  /// \brief
  ///   Set a custom shader rendering technique for this image. NULL can be passed for default
  inline void SetTechnique(VCompiledTechnique *pTechnique) {m_spTechnique = pTechnique;}


  /// \brief
  ///   Sets matrix pointers (matrices are not copied, so the pointers have to survive)
  ///
  /// A 3x3 matrix transforms a 2D position (either screen position or UV) by any linear operation such as
  /// rotation and/or translation
  ///
  /// \param pUVTransform
  ///   2D transformation of the texture coordinates
  ///
  /// \param pPosTransform
  ///   2D transformation of the vertex positions
  inline void SetTransformation(const hkvMat3 *pUVTransform, const hkvMat3 *pPosTransform)
  {
    m_pUVTransform = pUVTransform;
    m_pPosTransform = pPosTransform;
  }

  /// \brief
  ///   Assignment operator
  GUI_IMPEXP VImageState & operator = (const VImageState &other)
  {
    m_bDefined = other.m_bDefined;
    m_iColor = other.m_iColor;
    m_spTexture = other.m_spTexture;
    m_spTexAnim = other.m_spTexAnim; ///< create a new instance?
    texCoord = other.texCoord;
    m_spCursor = other.m_spCursor;
    m_eStretchMode = other.m_eStretchMode;
    m_eTranspType = other.m_eTranspType;
    m_iAdditionalStateFlags = other.m_iAdditionalStateFlags;
    return *this;
  }

  /// \brief
  ///   Static parsing helper
  static GUI_IMPEXP ImageStretchMode_e GetStretchMode(const char *szModeStr);

  /// \brief
  ///   Static transformation helper
  static GUI_IMPEXP void ApplyTransformation(int iVertCount, Overlay2DVertex_t *v, const hkvMat3 *pUVTransform, const hkvMat3 *pPosTransform);

  /// \brief
  ///   Static transformation helper
  static GUI_IMPEXP void CreateTransformation(hkvMat3 &dest, const hkvVec2 &vOfs, float fRotation=0.f, float fScaling=1.0f);

  V_DECLARE_SERIALX( VImageState, GUI_IMPEXP );
  GUI_IMPEXP void SerializeX( VArchive &ar );

protected:
  bool m_bDefined;
  ImageStretchMode_e m_eStretchMode;
  VColorRef m_iColor;
  VTextureObjectPtr m_spTexture; ///< display texture
  VisTextureAnimInstancePtr m_spTexAnim; ///< display texture animation instance

  VRectanglef texCoord;
  VCursorPtr m_spCursor;  ///< mouse over cursor
  const hkvMat3 *m_pUVTransform;
  const hkvMat3 *m_pPosTransform;
  VIS_TransparencyType m_eTranspType;
  int m_iAdditionalStateFlags;
  VCompiledTechniquePtr m_spTechnique;
};


/// \brief
///   Helper class to render an image button with up to 4 states represented by ControlState_e
class VImageStates
{
public:

  /// \brief
  ///   Constructor
  GUI_IMPEXP VImageStates() {}


  /// \brief
  ///   Internal Paint function
  GUI_IMPEXP void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief
  ///   Internal function to parse XML
  GUI_IMPEXP bool Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite);

  /// \brief
  ///   Calls VImageState::SetStretchMode on each of the 4 states
  inline void SetStretchMode(VImageState::ImageStretchMode_e eMode) 
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      m_States[i].SetStretchMode(eMode);
  }

  /// \brief
  ///   Calls VImageState::SetTexture on each of the 4 states
  inline void SetTexture(VTextureObject *pNewTex)
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      m_States[i].SetTexture(pNewTex);
  }

  /// \brief
  ///   Calls VImageState::SetTextureRange on each of the 4 states
  inline void SetTextureRange(const VRectanglef &coords) 
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      m_States[i].SetTextureRange(coords);
  }

  /// \brief
  ///   Calls VImageState::SetTransformation on each of the 4 states
  inline void SetTransformation(const hkvMat3 *pUVTransform, const hkvMat3 *pPosTransform)
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      m_States[i].SetTransformation(pUVTransform, pPosTransform);
  }

  /// \brief
  ///   Calls VImageState::SetTransparency on each of the 4 states
  inline void SetTransparency(VIS_TransparencyType eTransp)
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      m_States[i].SetTransparency(eTransp);
  }

  /// \brief
  ///   Calls VImageState::SetTechnique on each of the 4 states
  inline void SetTechnique(VCompiledTechnique *pTechnique)
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      m_States[i].SetTechnique(pTechnique);
  }

  VImageState m_States[VWindowBase::STATE_COUNT]; ///< for direct ControlState_e enum lookup

  V_DECLARE_SERIALX( VImageStates, GUI_IMPEXP );
  GUI_IMPEXP void SerializeX( VArchive &ar );
};



/// \brief
///   Helper class that represents text output of a single state. Each instance represents one state in VTextStates
class VTextState
{
public:

  /// \brief
  ///   Constructor
  GUI_IMPEXP VTextState() 
  {
    m_iColor = V_RGBA_WHITE;
    m_hAlign = m_vAlign = VisFont_cl::ALIGN_LEFT;
    m_bAlignmentValid = false;
    m_bTextWrap = false;
    m_pCustomBBox = NULL;
    m_pStringPtr = &m_sText;
    m_iNumTextLines = 0;
    m_fRelativeFontHeight = 1.f;
    m_fFontScaling = 1.f;
  }


  /// \brief
  ///   Internal function
  void Paint(VGraphicsInfo *pGraphics, VWindowBase *pParentWnd, VColorRef iColor);

  /// \brief
  ///   Internal Paint function
  GUI_IMPEXP void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief
  ///   Internal function to parse XML
  GUI_IMPEXP bool Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite);

  /// \brief
  ///   Return the current cursor (or NULL for default)
  inline VCursor* GetCursor() const {return m_spCursor;}

  /// \brief
  ///   Indicates whether this state has a unique string rather than sharing string of the VTextStates object
  inline bool UsesCustomText() const {return m_pStringPtr == &m_sText;}

  /// \brief
  ///   Set a (unique) string on this state
  inline void SetText(const char *szString, bool bForceCustom) {if (bForceCustom || UsesCustomText()) {m_sText=szString;m_pStringPtr=&m_sText;}UpdateAlignment();}

  /// \brief
  ///   Set a string pointer. The string itself is not copied
  inline void SetTextPtr(VString *pText) {m_pStringPtr=pText;UpdateAlignment();}

  /// \brief
  ///   Return a non-NULL text string
  inline const char *GetText() const {return m_pStringPtr->GetSafeStr();}

  /// \brief
  ///   Return the text height in pixels (font height * number of lines)
  inline float GetHeight() const 
  {
    float fHeight = m_spFont->GetFontHeight();
    if (m_bTextWrap && m_iNumTextLines>0) fHeight *= (float)m_iNumTextLines;
    return fHeight;
  }

  /// \brief
  ///   Return the font used for printing
  inline VisFont_cl *GetFont() const {return  m_spFont;}

  /// \brief
  ///   Set a font
  inline void SetFont(VisFont_cl *pFont) {m_spFont=pFont;}

  /// \brief
  ///   Return the tint color
  inline VColorRef GetColor() const {return m_iColor;}

  /// \brief
  ///   Set the tint color
  inline void SetColor(VColorRef iColor) {m_iColor=iColor;}

  /// \brief
  ///   Return the current 2D offset for printing
  inline hkvVec2 GetTextOfs() const {return m_vOfs;}

  /// \brief
  ///   Set a new 2D offset for printing
  inline void SetTextOfs(const hkvVec2 &vOfs) {m_vOfs=vOfs;m_bAlignmentValid = false;}

  /// \brief
  ///   Set text to line wrap mode (i.e. if it does not fit into the size)
  inline void SetTextWrap(bool bWrap) {m_bTextWrap=bWrap;}

  /// \brief
  ///   Return the current line wrap mode
  inline bool GetTextWrap() const {return m_bTextWrap;}

  /// \brief
  ///   Sets the horizontal alignment of the text
  inline void SetHorizontalAlignment(VisFont_cl::Alignment_e align) { m_hAlign = align; UpdateAlignment(); }

  /// \brief
  ///   Gets the horizontal alignment currently set on this text state
  inline VisFont_cl::Alignment_e GetHorizontalAlignment() const { return m_hAlign; }

  /// \brief
  ///   Sets the vertical alignment of the text
  inline void SetVerticalAlignment(VisFont_cl::Alignment_e align) { m_vAlign = align; UpdateAlignment(); }

  /// \brief
  ///   Gets the vertical alignment currently set on this text state
  inline VisFont_cl::Alignment_e GetVerticalAlignment() const { return m_vAlign; }

  /// \brief
  ///   Set a custom box to clip the text. The box is not copied
  inline void SetCustomClipRectPtr(const VRectanglef *pCustomBBox) {m_pCustomBBox=pCustomBBox;UpdateAlignment();}

  /// \brief
  ///   Invalidate the alignment of text
  inline void UpdateAlignment() {m_bAlignmentValid = false;}

  /// \brief
  ///   Return the number of printed text lines in the last OnPaint call
  inline int GetNumberOfPrintedLines() const {return m_iNumTextLines;}

  /// \brief
  ///   Return the size of this text box. A rect can be passed to receive the full text box
  GUI_IMPEXP hkvVec2 GetSize(VRectanglef *pRect=NULL) const;

  /// \brief
  ///   Assignment operator
  GUI_IMPEXP VTextState & operator = (const VTextState &other)
  {
    m_iColor = other.m_iColor;
    if (!m_pStringPtr || m_pStringPtr->IsEmpty())
      m_pStringPtr = other.m_pStringPtr; ///< just copy the references rather than copy the string
    m_spCursor = other.m_spCursor;
    m_spFont = other.m_spFont;
    m_hAlign = other.m_hAlign;
    m_vAlign = other.m_vAlign;
    m_vOfs = other.m_vOfs;
    m_bTextWrap = other.m_bTextWrap;
    m_fFontScaling = other.m_fFontScaling;
    m_bAlignmentValid = false;
    m_fRelativeFontHeight = other.m_fRelativeFontHeight;
    return *this;
  }

  V_DECLARE_SERIALX( VTextState, GUI_IMPEXP );
  GUI_IMPEXP void SerializeX( VArchive &ar );

protected:
  friend class VTextStates;
  VCursorPtr m_spCursor;  ///< mouse over cursor
  VColorRef m_iColor;
  VString m_sText;
  VisFontPtr m_spFont;
  VisFont_cl::Alignment_e m_hAlign, m_vAlign; ///< horizontal and vertical alignment
  hkvVec2 m_vOfs, m_vCurrentOfs;
  float m_fFontScaling;

  // temp vars:
  VString *m_pStringPtr; ///< points to the VString that has the actual text
  bool m_bAlignmentValid, m_bTextWrap;
  float m_fRelativeFontHeight; ///< when textwrap is enabled, this specifies the relative font height
  const VRectanglef *m_pCustomBBox;
  int m_iNumTextLines; ///< resulting number of printed lines (output only)
};


/// \brief
///   Helper class to render text with up to 4 states represented by ControlState_e
/// 
/// The text string between the states is shared, but can also be distinct (e.g. different mouse
/// over text).
class VTextStates
{
public:
  /// \brief
  ///   Constructor
  GUI_IMPEXP VTextStates() {}

  /// \brief
  ///   Internal paint function
  GUI_IMPEXP void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief
  ///   Internal function to parse XML
  GUI_IMPEXP bool Build(VWindowBase *pOwner, TiXmlElement *pNode, const char *szPath, bool bWrite, const VTextStates *pDefaults=NULL);

  /// \brief
  ///   Set (and copy) a new string
  GUI_IMPEXP void SetText(const char *szString);

  /// \brief
  ///   Set a string pointer (string is not copied)
  GUI_IMPEXP void SetTextPtr(VString *pText);

  /// \brief
  ///   Return the current text (from normal state)
  inline const char *GetText() const {return m_States[VWindowBase::NORMAL].GetText();}

  /// \brief
  ///   Calls VTextState::SetFont on each of the 4 states
  inline void SetFont(VisFont_cl *pFont) {for (int i=0;i<VWindowBase::STATE_COUNT;i++) m_States[i].SetFont(pFont);}

  /// \brief
  ///   Return the current font (from normal state)
  inline VisFont_cl *GetFont() const {return  m_States[VWindowBase::NORMAL].GetFont();}

  /// \brief
  ///   Calls VTextState::SetFont on each of the 4 states
  inline const hkvVec2& GetTextOfs() const {return  m_States[VWindowBase::NORMAL].m_vOfs;}

  /// \brief
  ///   Calls VTextState::SetTextOfs on each of the 4 states
  inline void SetTextOfs(const hkvVec2 &vOfs) {for (int i=0;i<VWindowBase::STATE_COUNT;i++) m_States[i].SetTextOfs(vOfs);}

  /// \brief
  ///   Calls VTextState::SetColor on each of the 4 states
  inline void SetColor(VColorRef iColor) {for (int i=0;i<VWindowBase::STATE_COUNT;i++) m_States[i].SetColor(iColor);}

  /// \brief
  ///   Calls VTextState::SetTextWrap on each of the 4 states
  inline void SetTextWrap(bool bWrap) {for (int i=0;i<VWindowBase::STATE_COUNT;i++) m_States[i].SetTextWrap(bWrap);}

  /// \brief
  ///   Calls VTextState::SetScaling on each of the 4 states
  inline void SetScaling(float fScale) {for (int i=0;i<VWindowBase::STATE_COUNT;i++) m_States[i].m_fFontScaling=fScale;}

  /// \brief
  ///   Calls VTextState::SetHorizontalAlignment on each of the 4 states
  inline void SetHorizontalAlignment(VisFont_cl::Alignment_e align) {for (int i=0;i<VWindowBase::STATE_COUNT;i++) m_States[i].SetHorizontalAlignment(align); }

  /// \brief
  ///   Calls VTextState::SetVerticalAlignment on each of the 4 states
  inline void SetVerticalAlignment(VisFont_cl::Alignment_e align) {for (int i=0;i<VWindowBase::STATE_COUNT;i++) m_States[i].SetVerticalAlignment(align); }

  /// \brief
  ///   Calls VTextState::SetCustomClipRectPtr on each of the 4 states
  GUI_IMPEXP void SetCustomClipRectPtr(const VRectanglef *pCustomBBox);

  /// \brief
  ///   Assignment operator
  GUI_IMPEXP VTextStates & operator = (const VTextStates &other)
  {
    for (int i=0;i<VWindowBase::STATE_COUNT;i++)
      m_States[i] = other.m_States[i];
    return *this;
  }

  /// \brief
  ///   Internal function
  GUI_IMPEXP void FinishSetup();

  V_DECLARE_SERIALX( VTextStates, GUI_IMPEXP );
  GUI_IMPEXP void SerializeX( VArchive &ar );

public:
  VTextState m_States[VWindowBase::STATE_COUNT]; ///< for direct ControlState_e enum lookup
};


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
