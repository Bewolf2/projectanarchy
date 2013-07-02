/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file AttachedEntity.hpp

//***********************************************************************
// AttachedEntity_cl Class
//***********************************************************************

// You must first include Vision.hpp to use this header
#ifndef _ATTACHEDENTITY_CL_HEADER_
#define _ATTACHEDENTITY_CL_HEADER_

#include <Vision/Runtime/Engine/System/Vision.hpp>
#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Engine/System/ModuleSystem/VisApiModuleSystemChild.hpp>

/// \brief
///   This class has not yet been tested.
/// 
/// Entity class which supports attaching to other entities and/or bones.
/// 
/// In contrast to the AttachedWeapon_cl, this class automatically updates itself if the parent
/// entity moves.
class AttachedEntity_cl : public VisBaseEntity_cl
{
public:

  /// \brief
  ///   Initialises the class members
  virtual void InitFunction();

  
  ///
  /// @name Entity Attaching
  /// @{
  ///
  
  /// \brief
  ///   Attach the current entity to another entity with a given anchor
  /// 
  /// You can specify a position and rotation relative to the target entity.
  /// 
  /// \param anchorEntity
  ///   the entity to which this entity will be attached
  /// 
  /// \param pos
  ///   position of the entity relative to the target entity
  /// 
  /// \param rot
  ///   rotation of the entity relative to the target entity
  /// 
  /// \return
  ///   bool result: if TRUE then the entity has been attached successfully
  /// 
  /// \note
  ///   For attached entities the CollisionFunction of the anchorEntity will be called whenever a
  ///   collision between the attached entity and another entity or the world happens.
  /// 
  /// \note
  ///   Collisions between the attached entity and the anchor entity are automatically disabled as
  ///   long as the Unbind function is not called.
  bool AttachToEntity(VisBaseEntity_cl *anchorEntity, const hkvVec3 &pos, const hkvQuat &rot);


  /// \brief
  ///   Attach the current entity at the current position to another entity
  /// 
  /// The transformation will automatically be calculated based on the current worldspace
  /// transformation of the attached entity.
  /// 
  /// \param anchorEntity
  ///   the entity to which this entity will be attached
  /// 
  /// \return
  ///   bool result: if TRUE then the entity has been attached successfully
  /// 
  /// \note
  ///   For attached entities the CollisionFunction of the anchorEntity will be called whenever a
  ///   collision between the attached entity and another entity or the world happens.
  /// 
  /// \note
  ///   Collisions between the attached entity and the anchor entity are automatically disabled as
  ///   long as the Unbind function is not called.
  /// 
  /// \note
  ///   Use this function if want an entity to be attached to another entity in the way to are
  ///   currently placed and rotated.
  bool AttachToEntity(VisBaseEntity_cl *anchorEntity);


  /// \brief
  ///   Attach the current entity to a bone of an other entity with a given anchor
  /// 
  /// You can specify a position and rotation relative to the target entity.
  /// 
  /// \param anchorEntity
  ///   the entity to which this entity will be attached
  /// 
  /// \param boneName
  ///   name of the bone of the anchorEntity to which this entity will be attached
  /// 
  /// \param pos
  ///   position of the entity relative to the anchor entity
  /// 
  /// \param rot
  ///   rotation of the entity relative to the anchor entity
  /// 
  /// \return
  ///   bool result: if TRUE then the entity has been attached successfully
  /// 
  /// \note
  ///   For attached entities the CollisionFunction of the anchorEntity will be called whenever a
  ///   collision between the attached entity and another entity or the world happens.
  /// 
  /// \note
  ///   Collisions between the attached entity and the anchor entity are automatically disabled as
  ///   long as the Unbind function is not called.
  bool AttachToBone(VisBaseEntity_cl *anchorEntity, const char *boneName, const hkvVec3 &pos, const hkvQuat &rot);

  
  /// \brief
  ///   Attach the current entity at the current position to a bone of an other entity with a given
  ///   anchor
  /// 
  /// The anchor matrix will automatically be calculated based 
  /// on the current position of the attached entity to the
  /// anchor bone.
  /// 
  /// \param anchorEntity
  ///   the entity to which this entity will be attached
  /// 
  /// \param boneName
  ///   name of the bone of the anchorEntity to which this entity will be attached
  /// 
  /// \return
  ///   bool result: if TRUE then the entity has been attached successfully
  /// 
  /// \note
  ///   For attached entities the CollisionFunction of the anchorEntity will be called whenever a
  ///   collision between the attached entity and another entity or the world happens.
  /// 
  /// \note
  ///   Collisions between the attached entity and the anchor entity are automatically disabled as
  ///   long as the Unbind function is not called.
  bool AttachToBone(VisBaseEntity_cl *anchorEntity, const char *boneName);
  

  /// \brief
  ///   Get the current anchor rotation and translation as well as the anchor flags
  /// 
  /// The anchor defines the rotation and translation of this entity relative to
  /// the anchor entity or bone and has been set before with one of the 
  /// BindTo functions mentioned above. The current anchor flags can
  /// also be requested with this function.
  /// 
  /// \param pos
  ///   vector where the current translation will be stored
  /// 
  /// \param rot
  ///   quaternion where the current rotation will be stored
  /// 
  /// \return
  ///   bool anchorDefined: if FALSE then the entity is not attached to an other entity or bone,
  ///   which means that no anchor rotation and translations are defined
  bool GetCurrentAnchor(hkvVec3 &pos, hkvQuat &rot);
  
  
  /// \brief
  ///   Set the current anchor rotation, translation and/or flags
  /// 
  /// The anchor defines the rotation and translation of this entity relative to the anchor
  /// entity or bone. All the parameters of this function are pointers, NULL parameters will cause
  /// this function not to update the specific anchor value.
  /// 
  /// \param pos
  ///   The new anchor translation vector
  /// 
  /// \param rot
  ///   The new anchor rotation quaternion
  /// 
  /// \return
  ///   bool anchorDefined: if FALSE then the entity is not yet attached
  bool SetCurrentAnchor(const hkvVec3 &pos, const hkvQuat &rot);


  /// \brief
  ///   Dettach the entity from the entity or bone
  /// 
  /// \return
  ///   bool status: if TRUE then the entity has been detached sucessfully
  bool Dettach();

  bool IsAttached() { return m_anchorMode != NOBIND; }


  void ModSysNotifyFunctionParentAltered( int iFlags );
  void ModSysNotifyFunctionParentDestroyed();
  void ModSysNotifyFunctionCommand(int command);
  
  void UpdateAttachment();

  IMPLEMENT_OBJ_CLASS(AttachedEntity_cl);

  ///
  /// @}
  ///

private:

  // available anchor modes for attachment
  enum AnchorMode_e
  {
    NOBIND = 0,
    BINDTOENTITY = 1,
    BINDTOBONE = 2
  };

  // entity anchoring / attaching
  hkvQuat m_AnchorRotation;                          ///< anchor rotation quaternion
  hkvVec3 m_AnchorTranslation;                           ///< anchor translation vector
  VisBaseEntity_cl *m_pAnchorEntity;                          ///< anchor entity
  int m_iAnchorBoneIndex;
  AnchorMode_e m_anchorMode;                                  ///< anchor mode, bone or entity or disabled

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
