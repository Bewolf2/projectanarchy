/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#ifndef RPG_PLUGIN_HIGHLIGHTABLE_COMPONENT_MANAGER_H__
#define RPG_PLUGIN_HIGHLIGHTABLE_COMPONENT_MANAGER_H__

#include <Vision/Samples/Engine/RPGPlugin/HighlightableComponent.h>

class RPG_HighlightableComponentManager : public IVisCallbackHandler_cl
{
public:
  void OneTimeInit();
  void OneTimeDeInit();

  RPG_HighlightableComponentCollection& GetHighlightables();
  RPG_HighlightableComponentCollection const& GetHighlightables() const;

  void SetHighlighted(RPG_HighlightableComponent *highlighted);

public:
  static RPG_HighlightableComponentManager s_instance;

  int GetTraceBitmask() const;

private:
  void InitShaders();

  static void InitShader(VCompiledTechnique *shader, DynArray_cl<int>& regsOut);

  static void UpdateHighlightableEntities(RPG_HighlightableComponentCollection& highlightables);

  static void RenderHighlightableEntities(VCompiledTechnique *shader, DynArray_cl<int> const& regs, RPG_HighlightableComponentCollection const& highlightables);

  // IVisCallbackHandler_cl
  void OnHandleCallback(IVisCallbackDataObject_cl *data) HKV_OVERRIDE;

private:
  VCompiledTechniquePtr m_xrayTechnique;
  DynArray_cl<int> m_xrayPassColorRegisters;

  VCompiledTechniquePtr m_highlightTechnique;
  DynArray_cl<int> m_highlightPassColorRegisters;

  RPG_HighlightableComponentCollection m_highlightableComponents;
  RPG_HighlightableComponentPtr m_highlightedComponent;
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
