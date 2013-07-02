/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VDialogFrame.hpp

#ifndef VDIALOGFRAME_HPP_INCLUDED
#define VDIALOGFRAME_HPP_INCLUDED

class VDialog;
class VWindowBase;


/// \brief
///   Class that can render a dialog frame border. Can be a static member of a control.
class VDialogFrame : public IVRenderableItem
{
public:
  virtual ~VDialogFrame(){}

  /// \brief
  ///   Empty constructor
  GUI_IMPEXP VDialogFrame();

  /// \brief
  ///   Sets the owner window of the frame (needed for rendering)
  inline void SetOwner(VWindowBase *pOwner) {m_pOwner = pOwner;}

  /// \brief
  ///   Overridden OnPaint function
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState);

  /// \brief
  ///   Overridden Build function
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite);

  /// \brief
  ///   Get the width of the border (left+right)
  inline float GetSizeX() const {return m_fBorderRight+m_fBorderLeft;}

  /// \brief
  ///   Get the height of the border (top+bottom)
  inline float GetSizeY() const {return m_fBorderTop+m_fBorderBottom;}

  /// \brief
  ///   Set the texture that is used to render this frame
  GUI_IMPEXP void SetTexture(VTextureObject *pTex);

  /// \brief
  ///   Get the texture that is used to render this frame
  inline VTextureObject *GetTexture() const
  {
    if (m_spTexAnim)
      return m_spTexAnim->GetCurrentFrame();
    return m_spFrameTex;
  }

  /// \brief
  ///   Set the modulation color
  inline void SetColor(VColorRef iColor) {m_iColor=iColor;}

  /// \brief
  ///   Get the modulation color
  inline VColorRef GetColor() const {return m_iColor;}

  /// \brief
  ///   Get the client rectangle (parent's rectangle minus border)
  GUI_IMPEXP VRectanglef GetClientRect() const;

  float m_fBorderTop, m_fBorderBottom, m_fBorderLeft, m_fBorderRight;

  V_DECLARE_SERIALX( VDialogFrame, GUI_IMPEXP );
  GUI_IMPEXP void SerializeX( VArchive &ar );

protected:
  VWindowBase *m_pOwner;
  VTextureObjectPtr m_spFrameTex;
  VisTextureAnimInstancePtr m_spTexAnim; ///< display texture animation instance
  VColorRef m_iColor;
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
