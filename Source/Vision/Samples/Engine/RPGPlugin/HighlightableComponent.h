/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_HIGHLIGHTABLE_COMPONENT_H__
#define RPG_PLUGIN_HIGHLIGHTABLE_COMPONENT_H__

#include <Vision/Samples/Engine/RPGPlugin/BaseComponent.h>

class RPG_HighlightableComponent : public RPG_BaseComponent
{
public:
  RPG_HighlightableComponent();

  void SetColor(VColorRef const& color);
  VColorRef const& GetColor() const;
    
  void Flash(VColorRef const& color, float const duration = 1.0f);
  void Update(float time);

protected:
  // IVObjectComponent
  BOOL CanAttachToObject(VisTypedEngineObject_cl *object, VString& errorOut) HKV_OVERRIDE;

  void SetOwner(VisTypedEngineObject_cl *owner) HKV_OVERRIDE;

private:
  VColorRef m_color;          ///< Highlight color
  VColorRef m_flashColor;     ///< Separate flash color
  float m_flashStartTime;     ///< Time where flash fadeout effect started
  float m_flashDuration;      ///< Duration of flash fadeout effect for this instance
  bool m_flashing;            ///< Flashing state

  V_DECLARE_SERIAL_DLLEXP(RPG_HighlightableComponent, RPG_PLUGIN_IMPEXP);
  V_DECLARE_VARTABLE(RPG_HighlightableComponent, RPG_PLUGIN_IMPEXP);
};

typedef VSmartPtr<RPG_HighlightableComponent> RPG_HighlightableComponentPtr;
typedef VRefCountedCollection<RPG_HighlightableComponent> RPG_HighlightableComponentCollection;

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
