/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCursor.hpp

#ifndef VCURSOR_HPP_INCLUDED
#define VCURSOR_HPP_INCLUDED


#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/IVMenuSystemResource.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/IVRenderableItem.hpp>

/// \brief
///   Visual properties of a cursor. The VCursor class has one of these instances per user
struct VCursorProperties_t
{
  VCursorProperties_t()
  {
    m_eTransp = VIS_TRANSP_ALPHA;
    iColor = V_RGBA_WHITE;
    texCoord.Set(0,0,1,1);
    m_vSize.set(-1,-1); // undefined
    m_bVisible = true;
  }

  /// \brief Copy constructor.
  VCursorProperties_t(VCursorProperties_t& rhs)
  {
    *this = rhs;
  }

  /// \brief Returns the rectangle around the cursor.
  inline VRectanglef GetCursorRect() const 
  {
    return VRectanglef(m_vHotSpot, m_vSize+m_vHotSpot);
  }

  /// \brief Changes the cursor's texture.
  inline void SetTexture(VTextureObject *pNewTex)
  {
    VASSERT(pNewTex);
    m_spTexture = pNewTex;
    m_spTexAnim = Vision::TextureManager.GetAnimationInstance(pNewTex);
  }

  /// \brief Returns the current texture used for rendering the cursor.
  inline VTextureObject* GetCurrentTexture() const
  {
    if (m_spTexAnim)
      return m_spTexAnim->GetCurrentFrame();
    return m_spTexture;
  }

  /// \brief Changes the cursor size by using the dimensions of the texture for its new size.
  inline void SetSizeFromTexture()
  {
    if (m_spTexture)
      m_vSize.set((float)m_spTexture->GetTextureWidth(), (float)m_spTexture->GetTextureHeight());
  }

  /// \brief Assignment operator.
  inline VCursorProperties_t &operator = (const VCursorProperties_t &other)
  {
    m_eTransp = other.m_eTransp;
    iColor = other.iColor;
    m_vSize = other.m_vSize;
    m_vHotSpot = other.m_vHotSpot;
    texCoord = other.texCoord;
    m_spTexture = NULL;
    m_spTexAnim = NULL;
    if (other.m_spTexture!=NULL)
      SetTexture(other.m_spTexture);
    return *this;
  }

  /// \brief Reads the cursor properties from the XML file.
  GUI_IMPEXP bool Parse(VGUIManager *pManager, TiXmlElement *pNode, const char *szPath);

  VIS_TransparencyType m_eTransp;
  VColorRef iColor;
  VTextureObjectPtr m_spTexture;
  VisTextureAnimInstancePtr m_spTexAnim;
  hkvVec2 m_vSize, m_vHotSpot;
  VRectanglef texCoord;
  bool m_bVisible;
};


/// \brief
///   Cursor resource (can also be rendered directly)
class VCursor : public IVMenuSystemResource, IVRenderableItem
{
public:

  /// \brief
  ///   Constructor that loads a cursor file (either xml or texture file)
  GUI_IMPEXP VCursor(class VGUIManager *pManager, const char *szFilename);

  /// \brief
  ///   Returns a relative rectangle around the cursor center
  inline VRectanglef GetCursorRect(VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0) const 
  {
    return m_UserProperties[eUser].GetCursorRect();
  }

  /// \brief
  ///   Returns the size of the cursor
  inline const hkvVec2& GetSize(VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0) const 
  {
    return m_UserProperties[eUser].m_vSize;
  }

public:
  GUI_IMPEXP virtual BOOL Reload() HKV_OVERRIDE;
  GUI_IMPEXP virtual BOOL Unload() HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnTick(float dtime) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Parse(TiXmlElement *pNode, const char *szPath) HKV_OVERRIDE;
  GUI_IMPEXP virtual IVSerializationProxy *CreateProxy() HKV_OVERRIDE;

  /// \brief Returns the cursor texture for the given user.
  inline VTextureObject* GetCurrentTexture(VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0) const
  {
    return m_UserProperties[eUser].GetCurrentTexture();
  }

  /// \brief Changes the hotspot of the cursor for the given user.
  inline void SetHotspot(const hkvVec2& vPnt,VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0)
  {
    m_UserProperties[eUser].m_vHotSpot = vPnt;
  }

  /// \brief Changes the size of the cursor for the given user.
  inline void SetSize(const hkvVec2& vSize,VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0)
  {
    m_UserProperties[eUser].m_vSize = vSize;
  }

  /// \brief Changes which texture coordinates to use for the cursor for the given user.
  inline void SetTextureCoords(const VRectanglef& vTexCoordRect,VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0)
  {
    m_UserProperties[eUser].texCoord = vTexCoordRect;
  }

  /// \brief Changes the color of the cursor for the given user.
  inline void SetColor(const VColorRef& vColor,VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0)
  {
    m_UserProperties[eUser].iColor = vColor;
  }

  /// \brief Changes the transparency mode of the cursor for the given user.
  inline void SetTransparency(const VIS_TransparencyType& vTransType,VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0)
  {
    m_UserProperties[eUser].m_eTransp = vTransType;
  }
  
  /// \brief Changes whether the cursor is visible for the given user.
  inline void SetVisible(bool bVisible = true, VGUIUserInfo_t::VGUIUserID_e eUser = VGUIUserInfo_t::GUIUser0)
  {
    m_UserProperties[eUser].m_bVisible = bVisible;
  }

private:

  VCursorProperties_t m_UserProperties[VGUIUserInfo_t::GUIMaxUser];
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
