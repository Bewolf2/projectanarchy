/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSliderControl.hpp

#ifndef VSLIDERCONTROL_HPP_INCLUDED
#define VSLIDERCONTROL_HPP_INCLUDED

class VSliderControl;
class VSlider;
typedef VSmartPtr<VSlider> VSliderPtr;
typedef VSmartPtr<VSliderControl> VSliderControlPtr;


/// \brief
///   A slider control (vertical or horizontal)
class VSliderControl : public VDlgControlBase
{
public:
  GUI_IMPEXP VSliderControl();
  GUI_IMPEXP ~VSliderControl();

  // VWindowBase overrides
  GUI_IMPEXP virtual VWindowBase* TestMouseOver(VGUIUserInfo_t &user, const hkvVec2 &vAbsMouse) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnKeyPressed(int iKey, int iKeyModifier) HKV_OVERRIDE;

  // slider specific
  GUI_IMPEXP void SetSliderPos(const hkvVec2 &vRelMousePos, bool bChanging);
  GUI_IMPEXP void SetValue(float fValue, bool bChanging=false);
  GUI_IMPEXP float GetValue() const {return m_fCurrentValue;}
  GUI_IMPEXP void SetSliderRange(float fMin, float fMax, int iTicks);
  GUI_IMPEXP void SetSliderRelSize(float fVal);
  GUI_IMPEXP void SetVertical(bool bStatus) {m_bVertical=bStatus;}
  
  inline float GetSliderRangeMin() const {return m_fRangeMin;}
  inline float GetSliderRangeMax() const {return m_fRangeMax;}
  inline int GetSliderTickCount() const {return m_iTickCount;}
  inline float GetSingleSliderStep() const {if (m_iTickCount<2) return 0.f; return (m_fRangeMax-m_fRangeMin)/(float)(m_iTickCount-1);}

  GUI_IMPEXP VSlider* GetSlider();
  inline VImageStates &Frame() {return m_Frame;}
  GUI_IMPEXP virtual void OnClick(VMenuEventDataObject *pEvent);

protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VSliderControl, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;
// renderable
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnTick(float dtime) HKV_OVERRIDE {}

protected:
  // data members:
  float m_fRangeMin, m_fRangeMax; ///< range
  int m_iTickCount;
  bool m_bVertical;
  float m_fSliderRelWidth;
  VRectanglef m_BorderSize; ///< display border in all directions

  VImageStates m_Frame;
  VSliderPtr m_spSlider;

  // temp:
  float m_fCurrentValue;
};



/// \brief
///   Represents the slider of a slider control
class VSlider : public VWindowBase
{
public:
  GUI_IMPEXP VSlider(VSliderControl *pOwner=NULL) 
  {
    m_pSliderCtrl = pOwner;
    SetParent(pOwner);
  }

// renderable
  GUI_IMPEXP virtual void OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState) HKV_OVERRIDE;
  GUI_IMPEXP virtual bool Build(TiXmlElement *pNode, const char *szPath, bool bWrite) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnBuildFinished() HKV_OVERRIDE;

  // VWindowBase
  GUI_IMPEXP virtual void OnDragBegin(const hkvVec2 &vMousePos, int iButtonMask) HKV_OVERRIDE {m_vDragStart=m_vDragPos=vMousePos; m_vDragPos+=GetPosition();}
  GUI_IMPEXP virtual void OnDragging(const hkvVec2 &vMouseDelta) HKV_OVERRIDE;
  GUI_IMPEXP virtual void OnDragEnd(VWindowBase *pOver) HKV_OVERRIDE;
  GUI_IMPEXP virtual VCursor *GetMouseOverCursor(VGUIUserInfo_t &user) HKV_OVERRIDE {return m_Image.m_States[GetCurrentState()].GetCursor();}

  inline void SetBorderMode(bool bStatus) {m_Image.SetStretchMode(bStatus ? VImageState::BORDER : VImageState::STRETCHED);}
  inline const hkvVec2 &GetUnscaledSize() const {return m_vUnscaledSize;}

  inline VImageStates &Images() {return m_Image;}
protected:
// serialization
  V_DECLARE_SERIAL_DLLEXP( VSlider, GUI_IMPEXP_DATA );
  GUI_IMPEXP virtual void Serialize( VArchive &ar );

  friend class VSliderControl;
  VSliderControl *m_pSliderCtrl; ///< owner
  VImageStates m_Image;
  hkvVec2 m_vUnscaledSize;
  hkvVec2 m_vDragStart,m_vDragPos; ///< temp values
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
