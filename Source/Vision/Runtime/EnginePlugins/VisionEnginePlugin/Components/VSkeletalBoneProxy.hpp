/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VSkeletalBoneProxy.hpp

#ifndef VSKELETALBONEPROXY_HPP_INCLUDED
#define VSKELETALBONEPROXY_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>


/// \brief
///   An instance of this class represents an attachable bone object. 
///
/// Once it is attached to an entity's bone, other attachable objects such as entities can be attached to this object.
/// Instances of this class can be set up via application code or inside vForge via the "ExposedBones" property of the
/// entity shape. See also sample map "BoneAttachment" in the map.
class VSkeletalBoneProxyObject : public VisObject3D_cl
{
public:
  EFFECTS_IMPEXP VSkeletalBoneProxyObject();
  EFFECTS_IMPEXP virtual ~VSkeletalBoneProxyObject();
  EFFECTS_IMPEXP virtual void DisposeObject();

  /// \brief
  ///   Attaches this object to an entity's bone.
  ///
  /// When this proxy object is properly attached via AttachToEntityBone, other objects (entities, lights, particle effects...) 
  /// can be attached to the proxy to move with it.
  ///
  /// \param pEntity
  ///   The entity to attach to. Can be NULL to de-attach
  ///
  /// \param szBoneName
  ///   Name of the bone in the model's skeleton (case sensitive)
  EFFECTS_IMPEXP void AttachToEntityBone(VisBaseEntity_cl *pEntity, const char *szBoneName);

  /// \brief
  ///   See other overload of AttachToEntityBone. This version takes a 0-based bone index rather than bone name
  EFFECTS_IMPEXP void AttachToEntityBone(VisBaseEntity_cl *pEntity, int iBoneIndex);

  /// \brief
  ///   Internal Function.
  EFFECTS_IMPEXP void UpdateBoneBinding();

  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  V_DECLARE_SERIAL_DLLEXP( VSkeletalBoneProxyObject,  EFFECTS_IMPEXP );

public:
  int m_iBoneIndex; ///< bone index in the parent's entity
};


 
#ifndef _VISION_DOC

/// \brief
///   Internal class to handle the instances of VSkeletalBoneProxyObject
class VSkeletalBoneProxyManager : public IVisCallbackHandler_cl
{
public:
  VSkeletalBoneProxyManager()
  {
    m_bCallbacksRegistered = false;
  }

  virtual ~VSkeletalBoneProxyManager()
  {
  }

  EFFECTS_IMPEXP static VSkeletalBoneProxyManager &GlobalManager();

  /// \brief Initializes the Manager.
  void OneTimeInit();

  /// \brief Deinitializes the Manager.
  void OneTimeDeInit();

  /// \brief Adds another bone proxy object to the manager.
  void AddProxy(VSkeletalBoneProxyObject *pObj);

  /// \brief Removes the given bone proxy object, if it had been added before.
  void RemoveProxy(VSkeletalBoneProxyObject *pObj);

  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///\brief
  /// return the first occurrence of an effect with specified key in this collection
  ///
  ///This is very similar to VisGame_cl::SearchEntity etc.
  ///
  ///\param szKey
  ///  Key to search for
  ///
  ///\param pStoreArray
  ///  Optional array that receives all hits (otherwise just the first hit is returned)
  ///
  ///\return
  ///  First hit with specified key, or NULL if key wasn't found
  EFFECTS_IMPEXP VSkeletalBoneProxyObject* FindByKey(const char *szKey, DynArray_cl<VSkeletalBoneProxyObject *> *pStoreArray=NULL) const;

  static VSkeletalBoneProxyManager g_BoneProxyManager;
  bool m_bCallbacksRegistered;
  VRefCountedCollection<VSkeletalBoneProxyObject> m_AllProxies;
};

#endif

#endif // VSKELETALBONEPROXY_HPP_INCLUDED

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
