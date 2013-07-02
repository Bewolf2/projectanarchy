/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
#ifndef VCUSTOM_VOLUME_MANAGER_HPP
#define VCUSTOM_VOLUME_MANAGER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

//forward declaration
class VCustomVolumeObject;

/// \brief Manager for VCustomVolumeObject 
class VCustomVolumeManager : public IVisCallbackHandler_cl
{
public:

  /// \brief
  ///   One-time initialization function
  EFFECTS_IMPEXP void OneTimeInit();

  /// \brief
  ///   One-time de-initialization function
  EFFECTS_IMPEXP void OneTimeDeInit();

  /// \brief
  ///   Accesses the global scratch texture pool instance.
  EFFECTS_IMPEXP static VCustomVolumeManager& GlobalManager();

  /// \brief
  ///  add a instance to the manager
  ///
  /// \param pInstance
  ///   the instance to add
  ///
  /// \return the index of the instance
  EFFECTS_IMPEXP int AddInstance(VCustomVolumeObject* pInstance);

  /// \brief
  ///  Removes a engine instance from the manager
  ///
  /// \param index
  ///   the index of the instance to remove
  EFFECTS_IMPEXP void RemoveInstance(int index);

  /// \brief
  ///   Returns the first occurrence (or NULL) of an instance with specified object key. The object key can be set in vForge.
  EFFECTS_IMPEXP VCustomVolumeObject* SearchInstance(const char* szObjectKey) const;

  /// \brief
  /// Removes all custom volume objects
  EFFECTS_IMPEXP void ReleaseAll();

protected:

  VCustomVolumeManager();
  virtual ~VCustomVolumeManager();

  //non-copyable
  VCustomVolumeManager(const VCustomVolumeManager& rh);
  void operator = (const VCustomVolumeManager& rh);

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

  VArray<VCustomVolumeObject*> m_instances;
  bool m_bAllowDeletion;

  static VCustomVolumeManager g_GlobalManager;

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
