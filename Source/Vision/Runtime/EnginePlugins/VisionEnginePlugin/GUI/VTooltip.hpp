/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VTooltip.hpp

#ifndef VTOOLTIP_HPP_INCLUDED
#define VTOOLTIP_HPP_INCLUDED

//#include <GUI/VControlHelperObjects.hpp>

class VTextState;
class VTooltip;
typedef VSmartPtr<VTooltip> VTooltipPtr;


/// \brief
///   Class that represents a tooltip. Overrides relevant VWindowBase functions
class VTooltip : public VWindowBase
{
public:
  GUI_IMPEXP VTooltip(IVGUIContext *pContext=NULL);
  GUI_IMPEXP virtual ~VTooltip();

  /// \brief Changes the tooltip text.
  GUI_IMPEXP virtual void SetText(const char* szString);

  /// \brief Returns the current tooltip text.
  GUI_IMPEXP virtual const char* GetText() const;


  /// \brief Sets the delay in seconds, before the tooltip is shown.
  inline void SetDelay(float fDelay) {m_fDelay = fDelay;}

  /// \brief Returns the delay in seconds, before the tooltip is shown.
  inline float GetDelay() const {return m_fDelay;}

  /// \brief Returns the VTextState object for this tooltip.
  inline VTextState& GetTextState() const {return* m_pText;}

  /// \brief Sets the border colors, sizes, etc. for the tooltip box.
  inline void SetBorderProperties(VColorRef backgroundColor=V_RGBA_YELLOW, VColorRef borderColor=V_RGBA_BLACK, float fBorderSize=1.0f, float fTextBorder=2.0f)
  {
    m_iBackgroundColor = backgroundColor;
    m_iBorderColor = borderColor;
    m_fBorderSize = fBorderSize;
    m_fTextBorder = fTextBorder;
  }

  /// \brief
  ///   Creates a cloned tooltip. This base implementation creates actual (derived) class instance and copies the base properties
  GUI_IMPEXP virtual VTooltip* CloneTooltip();

  //overridden UI functions
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnTick(float dtime) HKV_OVERRIDE;
  V_DECLARE_SERIAL_DLLEXP( VTooltip, GUI_IMPEXP_DATA );

protected:
  float m_fDelay;
  VTextState *m_pText; ///< have to use pointer here because of c++ header file order
  VColorRef m_iBackgroundColor, m_iBorderColor;
  float m_fBorderSize, m_fTextBorder;
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
