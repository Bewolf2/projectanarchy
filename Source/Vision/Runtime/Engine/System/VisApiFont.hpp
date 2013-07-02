/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiFont.hpp

#ifndef VISAPIFONT_HPP_INCLUDED
#define VISAPIFONT_HPP_INCLUDED


// custom render state flag that uses the '@' character as tabulator, e.g. "Column1@300:Column2@600:Column3"
#define RENDERSTATEFLAG_USETABULATORCHAR    RENDERSTATEFLAG_CUSTOMBIT


// forwards
class VisFontResourceManager_cl;
class VisFont_cl;
typedef VSmartPtr<VisFont_cl> VisFontPtr;

/// \brief
///   Internal class to batch text output entries
struct VBatchedTextEntry_t
{
public:
  // Constructors
  inline VBatchedTextEntry_t(const hkvVec2 &vPos, VColorRef iColor, 
    const VSimpleRenderState_t &iState, float fScaling, 
    const VRectanglef *pClipRect, float fZCoord,
    VCompiledShaderPass *pShader)
    : m_pShader(pShader)
    , m_vPos(vPos)
    , m_iColor(iColor)
    , m_iState(iState)
    , m_fScaling(fScaling)
    , m_fZCoord(fZCoord)
  {
    if (pClipRect)
    {
      VASSERT(pClipRect->IsValid());
      m_eClipRectType = 1;
      m_ClipRect = *pClipRect;
    }
    else 
      m_eClipRectType = 0;
  }
  inline VBatchedTextEntry_t(const hkvVec2 &vPos, const hkvVec2 &vDir, 
    const hkvVec2 &vUp, VColorRef iColor, const VSimpleRenderState_t &iState, 
    float fZCoord, VCompiledShaderPass *pShader)
    : m_pShader(pShader)
    , m_vPos(vPos)
    , m_iColor(iColor)
    , m_iState(iState)
    , m_fZCoord(fZCoord)
    , m_ClipRect(vDir, vUp)
    , m_eClipRectType(2)
  {}

  // Data
  VCompiledShaderPass *m_pShader;
  hkvVec2 m_vPos;
  VColorRef m_iColor;
  VSimpleRenderState_t m_iState;
  float m_fScaling;
  float m_fZCoord;
  VRectanglef m_ClipRect;
  char m_eClipRectType; // 0=NULL, 1=ClipRect, 2=as dir/up
};


/// \brief
///   Derived data object that is sent when the VisFont_cl::OnPrintCallback callback is triggered.
///   It provides references for printing properties that can be changed on the fly.
class VisPrintFontCallbackDataObject_cl : public IVisCallbackDataObject_cl
{
public:
  VisPrintFontCallbackDataObject_cl(
    VCallback *pSender,
    VisFont_cl *pFont,
    hkvVec2 &vCurrentPos,
    VColorRef &iColor,
    float &fYAdvance) 
    : IVisCallbackDataObject_cl(pSender)
    , m_pFont(pFont)
    , m_vCurrentPos(vCurrentPos)
    , m_iColor(iColor)
    , m_fNextYAdvance(fYAdvance)
  {
    m_szRemainingString = NULL;
    m_szRemainingStringW = NULL;
  }

  // Internal functions for template type safety.
  inline void SetStringPtr(const char *szText) {m_szRemainingString=szText;}
  inline void SetStringPtr(const wchar_t *szText) {m_szRemainingStringW=szText;}
  inline const char *GetStringPtr(const char *szDummy) const {return m_szRemainingString;}
  inline const wchar_t *GetStringPtr(const wchar_t *szDummy) const {return m_szRemainingStringW;}

public:
  /// \brief
  ///   Points to the font that is currently used for printing
  VisFont_cl *m_pFont;

  /// \brief
  ///   Points to the remaining string to print. This pointer can be advanced to skip some
  ///   characters. This member is NULL when printing with widechar version
  const char *m_szRemainingString;

  /// \brief
  ///   Wide char version of m_szRemainingString. Either m_szRemainingString or
  ///   m_szRemainingStringW is NULL 
  const wchar_t *m_szRemainingStringW;

  /// \brief
  ///   Current screen position of printed text. Can be modified.
  hkvVec2 &m_vCurrentPos;

  /// \brief
  ///   Current text color. Can be modified.
  VColorRef &m_iColor;

  /// \brief
  ///   When printing multi-line text, this member can be used to set an advance other than the font height. This member is initialized with GetFontHeight(). Scaling is applied afterwards
  float &m_fNextYAdvance;
};




/// \brief
///   Class that represents a font resource and provides functionality to print text.
/// 
/// Valid font files can be generated with the font generation tool of the SDK.
/// 
/// This font class supports unicode fonts. In order to use any non-ASCII characters, pass
/// all text to be printed as UTF-8 encoded strings.
/// 
/// Font resources are managed by an instance of a VisFontResourceManager_cl resource manager,
/// and thus can be loaded via Vision.Fonts.LoadFont("myfont.fnt");
class VisFont_cl : public VManagedResource, public IVisCallbackHandler_cl
{
public:

  /// \brief
  ///   Enumeration for the alignment used in font rendering.
  enum Alignment_e
  {
    ALIGN_UNDEFINED = -1, ///< Alignment is undefined
    ALIGN_LEFT = 0,     ///< Horizontal alignment: left
    ALIGN_TOP = 0,      ///< Vertical aligment: top
    ALIGN_CENTER = 1,   ///< Horizontal/vertical alignment: center
    ALIGN_RIGHT = 2,    ///< Horizontal alignment: right
    ALIGN_BOTTOM = 2    ///< Vertical alignment: bottom
  };

  /// \brief
  ///   Constructor that loads a font resource from .fnt file generated with the font generator
  ///   tool.
  VISION_APIFUNC VisFont_cl(VisFontResourceManager_cl *pManager, const char *szFilename);

  /// \brief
  ///   Destructor
  VISION_APIFUNC VOVERRIDE ~VisFont_cl();

  /// \brief
  ///   Prints a text string.
  /// 
  /// If used inside a renderloop, a valid render interface should be passed
  /// (VisRenderLoopHelper_cl::BeginOverlayRendering())
  /// 
  /// Outside a render loop, the render interface can be NULL so the text entry will be batched and
  /// printed in the next renderloop.
  ///
  /// If the passed szText contains an \c \@ (at) character then this character is treated as tabulator. For example,
  /// you can use \@320 to let the succeeding text start at x position 320.
  /// 
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  /// 
  /// \param vPos
  ///   Text position on screen
  ///
  /// \param szText
  ///   Text to print
  /// 
  /// \param iColor
  ///   text color
  /// 
  /// \param iState
  ///   Text render state (e.g. transparency mode), can be left to VisFont_cl::DEFAULT_STATE
  /// 
  /// \param fScaling
  ///   Text scaling
  /// 
  /// \param pClipRect
  ///   Optional clipping rectangle for software clipping. This function only clips away characters
  ///   that are completely outside the rectangle. Partial clipping has to be done with a scissor
  ///   rectangle by the hardware.
  /// 
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const char *szText, VColorRef iColor, const VSimpleRenderState_t &iState=VisFont_cl::DEFAULT_STATE, float fScaling=1.f, const VRectanglef *pClipRect=NULL, float fZCoord=-1.f);

  /// \brief
  ///   Prints a text string with customizable text direction.
  /// 
  /// This direction allows for rotated and sheared text output.
  /// 
  /// If used inside a renderloop, a valid render interface should be passed
  /// (VisRenderLoopHelper_cl::BeginOverlayRendering())
  /// 
  /// Outside a render loop, the render interface can be NULL so the text entry will be batched and
  /// printed in the next renderloop.
  /// 
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  /// 
  /// \param vPos
  ///   Text position on screen
  /// 
  /// \param vDir
  ///   Text direction. Use hkvVec2(1.f,0.f) for standard direction. The length of this vector
  ///   defines the scaling
  /// 
  /// \param vUp
  ///   The text up-vector. Use hkvVec2(0.f,-1.f) for standard up-direction.
  /// 
  /// \param szText
  ///   Text to print. For correct handling of non-ASCII characters, strings must be UTF-8 encoded.
  /// 
  /// \param iColor
  ///   text color
  /// 
  /// \param iState
  ///   Text render state (e.g. transparency mode), can be left to VisFont_cl::DEFAULT_STATE.
  ///   However, for rotated fonts adding the RENDERSTATEFLAG_FILTERING flag looks better. 
  /// 
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  /// 
  /// \note
  ///   This function does not have a scaling parameter as scaling is implicitly defined by the
  ///   length of vDir and vUp.
  /// 
  /// \note
  ///   The direction defines both the rotation of each character and the character advance.
  /// 
  /// \note
  ///   vDir and vUp can be used to rotate the text. For instance, vDir can be set to
  ///   hkvVec2(hkvMath::cosRad (fAngle),hkvMath::sinRad (fAngle)) and vUp to hkvVec2(vDir.y,-vDir.x) to make it
  ///   orthogonal
  /// 
  /// \note
  ///   To print slanted text, just shear the coordinate system, i.e. use
  ///   hkvVec2(1.f,0.f),hkvVec2(0.25f,-1.f) for vDir and vUp.
  VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const hkvVec2 &vDir, 
    const hkvVec2 &vUp, const char *szText, VColorRef iColor, const VSimpleRenderState_t &iState=VisFont_cl::DEFAULT_STATE, 
    float fZCoord=-1.f);

  /// \brief
  ///   Wide character version of PrintText
  ///
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  /// 
  /// \param vPos
  ///   Text position on screen
  ///
  /// \param szText
  ///   Text to print
  /// 
  /// \param iColor
  ///   text color
  /// 
  /// \param iState
  ///   Text render state (e.g. transparency mode), can be left to VisFont_cl::DEFAULT_STATE
  /// 
  /// \param fScaling
  ///   Text scaling
  /// 
  /// \param pClipRect
  ///   Optional clipping rectangle for software clipping. This function only clips away characters
  ///   that are completely outside the rectangle. Partially clipping has to be done with a scissor
  ///   rectangle by the hardware.
  ///
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  ///
  /// \note
  ///   Wide character (wchar_t) versions are deprecated. Use UTF8 strings for unicode text.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const wchar_t *szText, 
    VColorRef iColor, const VSimpleRenderState_t &iState=VisFont_cl::DEFAULT_STATE, float fScaling=1.f, 
    const VRectanglef *pClipRect=NULL, float fZCoord=-1.f);

  /// \brief
  ///   Wide character version of PrintText with customizable direction
  ///
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  ///
  /// \param vPos
  ///   Text position on screen
  /// 
  /// \param vDir
  ///   Text direction. Use hkvVec2(1.f,0.f) for standard direction. The length of this vector
  ///   defines the scaling
  /// 
  /// \param vUp
  ///   The text up-vector. Use hkvVec2(0.f,-1.f) for standard up-direction.
  /// 
  /// \param szText
  ///   Text to print
  /// 
  /// \param iColor
  ///   text color
  /// 
  /// \param iState
  ///   Text render state (e.g. transparency mode), can be left to VisFont_cl::DEFAULT_STATE
  /// 
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  ///
  /// \note
  ///   Wide character (wchar_t) versions are deprecated. Use UTF8 strings for unicode text.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const hkvVec2 &vDir, 
    const hkvVec2 &vUp, const wchar_t *szText, VColorRef iColor, const VSimpleRenderState_t &iState=VisFont_cl::DEFAULT_STATE, 
    float fZCoord=-1.f);

  /// \brief
  ///   Overloaded version that renders text using a a shader instead of a VSimpleRenderState_t.
  ///
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  /// 
  /// \param vPos
  ///   Text position on screen
  ///
  /// \param szText
  ///   Text to print
  /// 
  /// \param iColor
  ///   text color
  ///
  /// \param shader
  ///   The shader used for rendering
  /// 
  /// \param fScaling
  ///   Text scaling
  /// 
  /// \param pClipRect
  ///   Optional clipping rectangle for software clipping. This function only clips away characters
  ///   that are completely outside the rectangle. Partially clipping has to be done with a scissor
  ///   rectangle by the hardware.
  ///
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const char *szText, VColorRef iColor, 
    VCompiledShaderPass &shader, float fScaling=1.f, const VRectanglef *pClipRect=NULL, float fZCoord=-1.f);

  /// \brief
  ///   Overloaded version that renders with a shader instead of using a VSimpleRenderState_t
  ///
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  /// 
  /// \param vPos
  ///   Text position on screen
  ///
  /// \param szText
  ///   Text to print
  /// 
  /// \param iColor
  ///   text color
  ///
  /// \param shader
  ///   The shader used for rendering
  /// 
  /// \param fScaling
  ///   Text scaling
  /// 
  /// \param pClipRect
  ///   Optional clipping rectangle for software clipping. This function only clips away characters
  ///   that are completely outside the rectangle. Partially clipping has to be done with a scissor
  ///   rectangle by the hardware.
  ///
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const wchar_t *szText, 
    VColorRef iColor, VCompiledShaderPass &shader, float fScaling=1.f, const VRectanglef *pClipRect=NULL, float fZCoord=-1.f);

  /// \brief
  ///   Overloaded version that renders text using a shader instead of a VSimpleRenderState_t.
  ///
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  /// 
  /// \param vPos
  ///   Text position on screen
  /// 
  /// \param vDir
  ///   Text direction. Use hkvVec2(1.f,0.f) for standard direction. The length of this vector
  ///   defines the scaling
  /// 
  /// \param vUp
  ///   The text up-vector. Use hkvVec2(0.f,-1.f) for standard up-direction.
  ///
  /// \param szText
  ///   Text to print
  /// 
  /// \param iColor
  ///   text color
  ///
  /// \param shader
  ///   The shader used for rendering 
  /// 
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const hkvVec2 &vDir, const hkvVec2 &vUp, 
    const char *szText, VColorRef iColor, VCompiledShaderPass &shader, float fZCoord=-1.f);

  /// \brief
  ///   Overloaded version that renders with a shader instead of using a VSimpleRenderState_t
  ///
  /// \param pRI
  ///   target interface. Can be NULL to batch the text and render it later (automatically)
  /// 
  /// \param vPos
  ///   Text position on screen
  /// 
  /// \param vDir
  ///   Text direction. Use hkvVec2(1.f,0.f) for standard direction. The length of this vector
  ///   defines the scaling
  /// 
  /// \param vUp
  ///   The text up-vector. Use hkvVec2(0.f,-1.f) for standard up-direction.
  ///
  /// \param szText
  ///   Text to print
  /// 
  /// \param iColor
  ///   text color
  ///
  /// \param shader
  ///   The shader used for rendering 
  /// 
  /// \param fZCoord
  ///   Optional z-coordinate for sorting into z-buffer. This value only affects the font rendering
  ///   if the RENDERSTATEFLAG_ALWAYSVISIBLE is not specified for the iState member. In
  ///   VisFont_cl::DEFAULT_STATE that flag is set.
  ///
  /// \note
  ///   Wide character (wchar_t) versions are deprecated. Use UTF8 strings for unicode text.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC void PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, const hkvVec2 &vDir, 
    const hkvVec2 &vUp, const wchar_t *szText, VColorRef iColor, VCompiledShaderPass &shader, float fZCoord=-1.f);

  /// \brief
  ///   Creates a mesh buffer from the text, where each character creates two triangles in the mesh
  /// 
  /// This is useful to display 3D text in the scene. To actually display the mesh, set this mesh
  /// on a mesh buffer object (see SimpleFont Example)
  /// 
  /// Scaling can be archived by applying a scaled rotation matrix to the respective mesh buffer
  /// object
  /// 
  /// \param szText
  ///   Text to "print" into the mesh
  /// 
  /// \param iColor
  ///   text color
  /// 
  /// \param iPageFilter
  ///   Page index of the font bitmap. Multiple meshes have to be created if there is more than one
  ///   font bitmap page
  ///
  /// \param pUseMesh
  ///   Optional existing mesh to use. It must have a position, UV0 and color stream, other streams remain untouched. 
  ///   This function re-allocates vertices if the number of vertices (6 per character) is not sufficient.
  ///   The primitive type must be MB_PRIMTYPE_TRILIST.
  /// 
  /// \example
  ///   \code
  ///   new VisMeshBufferObject_cl(pFont->CreateTextMesh("This is 3D text",V_RGBA_RED));
  ///   \endcode
  VISION_APIFUNC VisMeshBuffer_cl *CreateTextMesh(const char *szText, VColorRef iColor, int iPageFilter=0, 
    VisMeshBuffer_cl *pUseMesh=NULL);

  /// \brief
  ///   Wide character version of CreateTextMesh.
  ///
  /// \param szText
  ///   Text to "print" into the mesh
  /// 
  /// \param iColor
  ///   text color
  /// 
  /// \param iPageFilter
  ///   Page index of the font bitmap. Multiple meshes have to be created if there is more than one
  ///   font bitmap page
  ///
  /// \param pUseMesh
  ///   Optional existing mesh to use. It must have a position, UV0 and color stream, other streams remain untouched. 
  ///   This function re-allocates vertices if the number of vertices (6 per character) is not sufficient.
  ///   The primitive type must be MB_PRIMTYPE_TRILIST.
  ///
  /// \note
  ///   Wide character (wchar_t) versions are deprecated. Use UTF8 strings for unicode text.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC VisMeshBuffer_cl *CreateTextMesh(const wchar_t *szText, VColorRef iColor, 
    int iPageFilter=0, VisMeshBuffer_cl *pUseMesh=NULL);

  /// \brief
  ///   Evaluates the dimension of a text string in pixels
  ///
  /// \param szText
  ///   Text to compute the dimensions from.
  ///
  /// \param dest
  ///   Output rectangle.
  ///
  /// \param iCharCount
  ///   The character count of the text. Use -1 if character count is unknown.
  VISION_APIFUNC bool GetTextDimension(const char *szText, VRectanglef &dest, int iCharCount=-1);

  /// \brief
  ///   Wide character version of GetTextDimension (same parameters otherwise)
  ///
  ///
  /// \param szText
  ///   Text to compute the dimensions from.
  ///
  /// \param dest
  ///   Output rectangle.
  ///
  /// \param iCharCount
  ///   The character count of the text. Use -1 if character count is unknown. 
  ///
  /// \note
  ///   Wide character (wchar_t) versions are deprecated. Use UTF8 strings for unicode text.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC bool GetTextDimension(const wchar_t *szText, VRectanglef &dest, int iCharCount=-1);

  /// \brief
  ///   Evaluates the position offset of text using the alignment
  ///
  /// \param szText
  ///   Text to compute the offset from.
  ///
  /// \param vSize
  ///   The size of the rectangle containing the text.
  ///
  /// \param hAlign
  ///   Horizontal aAlignment of the text.
  ///
  /// \param vAlign
  ///   Vertical aAlignment of the text.
  ///
  /// \param fScaling
  ///   Optional scaling parameter.
  ///
  /// \return
  ///   Returns the two-dimensional offset from the upper-left corner of the rectangle containing the text.
  VISION_APIFUNC hkvVec2 GetTextPositionOfs(const char *szText, const hkvVec2 &vSize, Alignment_e hAlign, 
    Alignment_e vAlign, float fScaling=1.f);

  /// \brief
  ///   Wide character version of GetTextPositionOfs.
  ///
  /// \param szText
  ///   Text to compute the offset from.
  ///
  /// \param vSize
  ///   The size of the rectangle containing the text.
  ///
  /// \param hAlign
  ///   Horizontal aAlignment of the text.
  ///
  /// \param vAlign
  ///   Vertical aAlignment of the text.
  ///
  /// \param fScaling
  ///   Optional scaling parameter.
  ///
  /// \return
  ///   Returns the two-dimensional offset from the upper-left corner of the rectangle containing the text.
  ///
  /// \note
  ///   Wide character (wchar_t) versions are deprecated. Use UTF8 strings for unicode text.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC hkvVec2 GetTextPositionOfs(const wchar_t *szText, const hkvVec2 &vSize, 
    Alignment_e hAlign, Alignment_e vAlign, float fScaling=1.f);

  /// \brief
  ///   Returns the character index at horizontal position in pixels. Useful when clicking into a
  ///   text field.
  ///
  /// \param szText
  ///   The text subject to determining the character.
  ///
  /// \param fHorizPos
  ///   The horizontal position of the character.
  ///
  /// \param iCharCount
  ///   The character count of the text. Use -1 if character count is unknown.
  ///
  /// \param bRounding
  ///   If set to true, rounding to the nearest character is enabled.
  ///
  /// \return
  ///   Returns the character index at position fHorizPos.
  VISION_APIFUNC int GetCharacterIndexAtPos(const char *szText, 
    float fHorizPos, int iCharCount=-1, bool bRounding=true);

  /// \brief
  ///   Wide character version of GetCharacterIndexAtPos (same parameters otherwise)
  ///
  /// \param szText
  ///   The text subject to determining the character.
  ///
  /// \param fHorizPos
  ///   The horizontal position of the character.
  ///
  /// \param iCharCount
  ///   The character count of the text. Use -1 if character count is unknown.
  ///
  /// \param bRounding
  ///   If set to true, rounding to the nearest character is enabled.
  ///
  /// \return
  ///   Returns the character index at position fHorizPos.
  ///
  /// \note
  ///   Wide character (wchar_t) versions are deprecated. Use UTF8 strings for unicode text.
  HKV_DEPRECATED_2012_1 VISION_APIFUNC int GetCharacterIndexAtPos(const wchar_t *szText, 
    float fHorizPos, int iCharCount=-1, bool bRounding=true);

  /// \brief
  ///   Returns the height of the font (as set in the tool)
  inline float GetFontHeight() const
  {
    return m_fLineHeight;
  }

  /// \brief
  ///   Returns the relative vertical base line height of the font. This can be useful for 
  ///   positioning rendered text.
  inline float GetBaseLineHeight() const
  {
    return m_fBaseLine;
  }

  /// \brief
  ///   Sets the internal name of the font. Used by VisFontResourceManager_cl::FindFont
  inline void SetFontName(const char *szName)
  {
    m_sFontName = szName;
  }

  /// \brief
  ///   Gets the internal name of the font. See SetFontName.
  inline const char* GetFontName() const
  {
    return m_sFontName;
  }

  /// \brief
  ///   Associates a character (existing or non-existing) with the passed texture
  /// 
  /// Once a custom character has been added, it can be printed by the Print function inside normal
  /// text.
  /// 
  /// If you want to add many custom characters, put them on the same texture and use the texCoord
  /// param.
  /// 
  /// Custom characters survive Unload/Reload of the font resource
  /// 
  /// \param iChar
  ///   character index to associate
  /// 
  /// \param pTexture
  ///   The texture to use
  /// 
  /// \param texCoord
  ///   The texture coordinate range used by the character on the texture 
  /// 
  /// \param screenRect
  ///   The on-screen offset and size of the character at scale 1.0
  /// 
  /// \param fXAdvance
  ///   The number of on-screen units to advance the text position while printing
  /// 
  /// \param fYAdvance
  ///   The number of on-screen units to advance the text vertically for a new line. If <0, the font height is used
  ///
  /// \return
  ///   bool bResult: true if successful. There is a limited number of unique textures, but no
  ///   limit for custom characters. However, this function fails if the font has more than 32768
  ///   characters and thus no space for custom characters.
  VISION_APIFUNC bool AddCustomCharacter(wchar_t iChar, VTextureObject *pTexture, const VRectanglef &texCoord, 
    const VRectanglef &screenRect, float fXAdvance, float fYAdvance=-1.f);

  /// \brief
  ///   Removes all custom characters that have been added via AddCustomCharacter from this font
  ///   instance
  VISION_APIFUNC void RemoveAllCustomCharacters();

  /// \brief
  ///   Clears all text entries that have been collected since the last text rendering. Only
  ///   relevant when batching text with NULL render interface
  VISION_APIFUNC void ClearAllTextEntries();

  /// \brief
  ///   Static helper function to parse the alignment string
  VISION_APIFUNC static Alignment_e GetAlignment(const char *szAlignment);

  /// \brief
  ///   Helper function to batch text for later rendering
  ///
  /// \param pEntry
  ///   The text entry settings.
  ///
  /// \param szText
  ///   The text to batch.
  VISION_APIFUNC void AddTextEntry(VBatchedTextEntry_t *pEntry, const char *szText);

  /// \brief
  ///   Helper function to batch text for later rendering
  ///
  /// \param pEntry
  ///   The text entry settings.
  ///
  /// \param wszText
  ///   The text to batch.
  ///
  /// \note
  ///   Wide character (wchar_t) version is deprecated and slower. Use UTF8 strings for unicode text.
  VISION_APIFUNC void AddTextEntry(VBatchedTextEntry_t *pEntry, const wchar_t *wszText);

  /// \brief
  ///   Public static variable that can be passed to the PrintText function
  VISION_APIDATA static VSimpleRenderState_t DEFAULT_STATE;


  /// \brief
  ///   Global callback that is triggered every time text printing hits a reserved character
  ///   defined by SetPrintCallbackCharacter. See VisPrintFontCallbackDataObject_cl
  /// 
  /// The data object can be casted to VisPrintFontCallbackDataObject_cl and provides useful
  /// members to modify the current print progress on-the-fly.
  /// 
  /// For instance, a custom character can be introduced to change the color of printed text on a
  /// per-character basis.
  /// 
  /// The callback and the global character both are defined globally and not font specific.
  VISION_APIDATA static VisCallback_cl OnPrintCallback;

  /// \brief
  ///   This function sets the character that triggers the global VisFont_cl::OnPrintCallback
  ///   function. By default this is set to -1, i.e. never hit.
  inline static void SetPrintCallbackCharacter(int iChar) {g_iPrintCallbackCharacter=iChar;}

  /// \brief
  ///   Returns the global character that has been set via SetPrintCallbackCharacter. The default
  ///   is -1
  inline static int GetPrintCallbackCharacter() {return g_iPrintCallbackCharacter;}

  /// \brief
  ///   Sets an offset that is added to every x-advance value of characters while printing text
  ///
  /// The default value is 0.0f. By setting different value the character spacing can be modified. Positive and negative values are allowed.
  /// The value will be applied when the text is printed. It can be changed at any time but note that deferred text printing (passing NULL
  /// as IVRender2DInterface* in the print routines) will use the value at the time the actual printing happens inside the render loop.
  ///
  /// \param fPixels
  ///   The additional border in pixels
  inline void SetAdditionalCharacterSpacing(float fPixels)
  {
    m_fAdditionalSpacing = fPixels;
  }

  /// \brief
  ///   Returns the additional spacing value that has been set via SetAdditionalCharacterSpacing. The default is 0.0f
  inline float GetAdditionalCharacterSpacing() const
  {
    return m_fAdditionalSpacing;
  }

  /// \brief
  ///   Set the filter bitmask for rendering fonts only in some render contexts
  /// 
  /// Fonts will only be rendered in render contexts, if the result of the logical AND
  /// operation of this bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) 
  /// is not zero.
  ///
  /// @param iMask
  ///   the visibility bitmask to set
  inline void SetVisibleBitmask(unsigned int iMask)
  {
    m_iVisibleBitmask = iMask;
  }

  /// \brief
  ///   Get the filter bitmask that has been set via SetVisibleBitmask
  inline unsigned int GetVisibleBitmask() const
  {
    return m_iVisibleBitmask;
  }

protected:
  VISION_APIFUNC virtual BOOL Reload() HKV_OVERRIDE;
  VISION_APIFUNC virtual BOOL Unload() HKV_OVERRIDE;
  VISION_APIFUNC virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, 
    size_t &iDependentSys, size_t &iDependentGPU) HKV_OVERRIDE;
#ifdef SUPPORTS_SNAPSHOT_CREATION
  VISION_APIFUNC virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif
  VISION_APIFUNC virtual int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars) HKV_OVERRIDE;
  VISION_APIFUNC virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

private:
  inline int GetCharRef(const unsigned int c) const
  {
    // try to fall back to question mark if character is not available
    // (only if missing character is not white-space)
    return (static_cast<int>(c) < m_iCharRefTableSize) ? m_pCharRefTable[c] : 
      ((c >= 32 && c != static_cast<unsigned int>('?')) ? 
        GetCharRef(static_cast<unsigned int>('?')) : 0xffff);
  }

public:
  static const int MAX_PAGE_COUNT = 32;
  static const int STATIC_CHARACTER_COUNT = 256; ///< number of characters without re-allocation (static member)
  VISION_APIDATA static unsigned int g_iPrintCallbackCharacter;

  /// \brief
  ///   Structure that holds information about a single character on the texture
  struct CharacterInfo_t
  {
    VRectanglef m_TexCoord;     ///< source texture coordinates on the page
    VRectanglef m_ScreenRect;   ///< on-screen coordinates (support for offset)
    float m_fXAdvance;          ///< x-progress of this character
    float m_fYAdvance;          ///< y-progress of this character (height by default)
    BYTE m_iPage;               ///< texture page index
  };

  /// \brief
  ///   Structure that holds information about a kerning distance towards another character. 
  ///   A character may reference a list of these structures
  struct CharKerningInfo_t
  {
    unsigned short iOtherChar;
    short iAmount;
  };

  /// \brief
  ///   Structure that defines entry points for each character in the kerning list of the font
  struct CharKerningRef_t
  {
    inline CharKerningRef_t() {iFirstEntry=0;iListCount=0;}
    int iFirstEntry;
    unsigned short iListCount;
  };

  VString m_sFontName;          ///< optional name of the font
  short m_iPageCount, m_iOverallPageCount; ///< number of character (texture) pages
  VTextureObjectPtr m_spPageTexture[MAX_PAGE_COUNT]; ///< page textures
  unsigned int m_iPageUsesLuminanceBitMask;

  int m_iCharCount;   ///< overall number of characters
  VMemoryTempBuffer<STATIC_CHARACTER_COUNT*sizeof(CharacterInfo_t)> m_CharInfo; ///< character information buffer
  unsigned short m_iCharRefTable[0x100]; ///< table for ASCII range, m_pCharRefTable allocates more if necessary
  unsigned short *m_pCharRefTable;  ///< unicode->character info mapping table for fast access
  int m_iCharRefTableSize;          ///< either 256 or 65536

  float m_fLineHeight; ///< height of a line (in pixels)
  float m_fBaseLine; ///< baseline height
  float m_fAdditionalSpacing; ///< optional spacing (default=0)

  /// \brief
  ///   Returns a list of kerning references for a specific character
  inline CharKerningInfo_t* GetCharacterKerningInfo(int iCharacter, int &iListCount) const
  {
    if (m_pCharKerningRef==NULL)
      return NULL;
    const CharKerningRef_t& ref(m_pCharKerningRef[iCharacter]);
    iListCount = ref.iListCount;
    return &m_pKerningBlock[ref.iFirstEntry];
  }

  /// \brief
  ///   Returns the additional kerning distance between two characters. This function can be called for all character pairs
  inline float GetKerningDistance(int iFirstChar, int iSecondChar)
  {
    if ( m_pCharKerningRef==NULL || iFirstChar<0 || iSecondChar<0 || iFirstChar>=m_iCharCount || iSecondChar>=m_iCharCount)
      return 0.f;
    const CharKerningRef_t& ref(m_pCharKerningRef[iFirstChar]);
    const CharKerningInfo_t *pInfo = &m_pKerningBlock[ref.iFirstEntry];
    for (int i=0;i<ref.iListCount;i++,pInfo++)
      if (pInfo->iOtherChar==iSecondChar)
        return (float)pInfo->iAmount;
    return 0.f;
  }

  // delayed printing:
  bool m_bUsesRenderHook;
  int m_iDelayedPrintBufferLen;
  DynArray_cl<char> m_DelayedPrintBuffer;

  // custom characters inserted via AddCustomCharacter:
  int m_iCustomCharCount;
  DynArray_cl<CharacterInfo_t> m_CustomCharacterInfo;
  
  // kerning
  CharKerningInfo_t *m_pKerningBlock;
  CharKerningRef_t *m_pCharKerningRef;


  
protected:
  bool LoadFromDEFFile(const char *szFileNoExt); ///< old .def definition file loading
  bool LoadFromFNTFile(const char *szFileName); ///< new .fnt definition file loading
  inline void AllocateCharacters(int iCount);
  inline CharacterInfo_t* GetCharInfo() const 
  {
    return (CharacterInfo_t *)m_CharInfo.GetBuffer();
  }
  void AddCharacterInfo(struct charInfo *pInfo, int iIndex, float fYAdvance);
  int AddCustomPageTexture(VTextureObject *pTex);
  void EnsureCharRefTableSize(int iIndex);
  
  // template versions for char/wchar_t
  template <class CHARTYPE, bool bNeedConversionToUnsignedChar> void _PrintText(IVRender2DInterface *pRI, const hkvVec2 &vPos, 
    const CHARTYPE *szText, VColorRef iColor, const VSimpleRenderState_t &iState, float fScaling, 
    const VRectanglef *pClipRect, float fZCoord, VCompiledShaderPass *pShader=NULL);
  template <class CHARTYPE> void _PrintTextDir(IVRender2DInterface *pRI, const hkvVec2 &vPos, 
    const hkvVec2 &vDir, const hkvVec2 &vUp, const CHARTYPE *szText, VColorRef iColor, 
    const VSimpleRenderState_t &iState, float fZCoord, VCompiledShaderPass *pShader=NULL);
  template <class CHARTYPE> bool _GetTextDimension(const CHARTYPE *szText, VRectanglef &dest, 
    int iCharCount=-1);
  template <class CHARTYPE> VisMeshBuffer_cl *_CreateTextMesh(const CHARTYPE *szText, VColorRef iColor, 
    int iPageFilter=0, VisMeshBuffer_cl *pUseMesh=NULL);
  template <class CHARTYPE> hkvVec2 _GetTextPositionOfs(const CHARTYPE *szText, const hkvVec2 &vSize, 
    Alignment_e hAlign, Alignment_e vAlign, float fScaling=1.f);

  template <class CHARTYPE> int _GetCharacterIndexAtPos(const CHARTYPE *szText, float fHorizPos, 
    int iCharCount=-1, bool bRounding=true);

  unsigned int m_iVisibleBitmask;
};



/// \brief
///   Resource manager class that manages all loaded font resources
class VisFontResourceManager_cl : public VisResourceManager_cl
{
public:
  VISION_APIFUNC VisFontResourceManager_cl();


  /// \brief
  ///   Returns a reference to the font that is used for debug rendering. This font is always
  ///   available
  VISION_APIFUNC VisFont_cl &DebugFont();

  /// \brief
  ///   Returns a reference to the font that is used for the console. This font is always available
  VISION_APIFUNC VisFont_cl &ConsoleFont();


  /// \brief
  ///   Loads a font resource or returns existing instance if it is already loaded
  VISION_APIFUNC VisFont_cl *LoadFont(const char *szFilename);

  /// \brief
  ///   Finds a font by name or by filename
  VISION_APIFUNC VisFont_cl *FindFont(const char *szFontname, const char *szPath=NULL);

  /// \brief
  ///   Overridden resource manager function
  VISION_APIFUNC VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo)
  {
    return LoadFont(szFilename);
  }

  /// \brief
  ///   Enables or disables loading of kerning information for subsequent calls of LoadFont.
  ///
  /// Kerning allows for using individual character spacing based on specific combination of characters. For instance,
  /// the character spacing between combination 'Te' might differ from 'Th'. Without kerning, every character has its own
  /// fixed width to advance the font position.
  /// Fonts that have been loaded with kerning information automatically output text with kerning. Kerning information
  /// is provided through the font resource. Font files are aways saved with kerning information, but the info is only loaded
  /// depending on this flag. 
  /// The default state for SetSupportKerning is FALSE. To enable it, call Vision::Fonts.SetSupportKerning(TRUE) rather at the beginning
  /// of the application. Note that using kerning information might have some minor impact on font rendering performance.
  ///
  /// \param bStatus
  ///   New state
  inline void SetSupportKerning(BOOL bStatus)
  {
    m_bSupportKerning = bStatus==TRUE;
  }


  /// \brief
  ///   Returns the global state previously set with SetSupportKerning
  inline BOOL GetSupportKerning() const
  {
    return m_bSupportKerning;
  }

// internal:
  VISION_APIFUNC void InitOneTime();
  VISION_APIFUNC void DeInitOneTime();
  VisFontPtr m_spDebugFont;
  VisFontPtr m_spConsoleFont;
  bool m_bSupportKerning;


};


#endif // VISAPIFONT_HPP_INCLUDED

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
