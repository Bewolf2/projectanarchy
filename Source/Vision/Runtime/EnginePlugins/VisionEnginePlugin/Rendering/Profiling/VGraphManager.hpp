/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VGraphManager.hpp

#ifndef VGRAPH_MANAGER_HPP
#define VGRAPH_MANAGER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

//forward declaration
class VGraphObject;

/// \brief Manager for VGraphObject 
class VGraphManager : public IVisCallbackHandler_cl
{
public:

  /// \brief
  ///   One-time initialization function
  EFFECTS_IMPEXP void OneTimeInit();

  /// \brief
  ///   One-time de-initialization function
  EFFECTS_IMPEXP void OneTimeDeInit();

  /// \brief
  ///   Accesses the global instance.
  EFFECTS_IMPEXP static VGraphManager& GlobalManager();

  /// \brief
  ///  add a instance to the manager
  ///
  /// \param pInstance
  ///   the instance to add
  ///
  /// \return the index of the instance
  EFFECTS_IMPEXP int AddInstance(VGraphObject* pInstance);

  /// \brief
  ///  Removes a instance from the manager
  ///
  /// \param iIndex
  ///   the index of the instance to remove
  EFFECTS_IMPEXP void RemoveInstance(int iIndex);

  /// \brief
  /// Removes all graph objects
  EFFECTS_IMPEXP void ReleaseAll();

private:

  VGraphManager();

  ~VGraphManager();

  //non-copyable
  VGraphManager(const VGraphManager& rhs);
  void operator= (const VGraphManager& rhs);

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  VArray<VGraphObject*> m_Instances;
  static VGraphManager g_GlobalManager;

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
