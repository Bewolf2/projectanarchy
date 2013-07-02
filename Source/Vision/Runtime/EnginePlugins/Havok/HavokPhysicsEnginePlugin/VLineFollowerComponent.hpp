/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VLINEFOLLOWERCOMPONENT_HPP_INCLUDED
#define VLINEFOLLOWERCOMPONENT_HPP_INCLUDED

#define RESET_ACTION    "resetpath"
#define SET_ACTION      "setpath"

#define ENTITY_PROPERTY "entity"
#define TO_PROPERTY     "to"
#define ACTION_PROPERTY "action"

//#include <Effects/EffectsModule.hpp>


#define SHARED_IMPEXP VHAVOK_IMPEXP
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
class vHavokCharacterController;


///
/// \brief
///   Component which follows a predefined path, using a character controller for physics.
/// 
/// Component that can be attached to an entity that will play an animation and follow 
/// a path using physics. Make sure the entity has an animation and that the animation 
/// has  motion delta
///
class VLineFollowerComponent : public IVObjectComponent, public IVisCallbackHandler_cl
{
public:


  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  /// \brief
  ///   Default constructor. Sets up the input map.
  ///
  /// The actual initialisation of the component happens in the ::SetOwner function
  ///
  SHARED_IMPEXP VLineFollowerComponent();

  /// \brief
  ///   Destructor
  SHARED_IMPEXP ~VLineFollowerComponent();

  /// \brief
  ///   Init Function
  SHARED_IMPEXP void CommonInit();

  /// \brief
  ///   DeInit Function
  SHARED_IMPEXP void CommonDeInit();

  ///
  /// @}
  ///


  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes
  SHARED_IMPEXP VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden function. Blob shadows can be attached to VisObject3D_cl instances
  SHARED_IMPEXP VOVERRIDE BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridden function to respond to variable changes
  SHARED_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

  /// \brief
  ///   Overridden function to process incoming messages, such as collision events and property changes
  SHARED_IMPEXP VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

  ///
  /// @}
  ///


  ///
  /// @name VTypedObject Overrides
  /// @{
  ///

  /// \brief
  ///   Virtual overridable that indicates whether OnDeserializationCallback 
  ///   should be called for this object
  VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) {return context.m_eType!=VSerializationContext::VSERIALIZATION_EDITOR;}

  /// \brief
  ///   Virtual overridable that gets called when a loading archive closes
  /// 
  VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);

  ///
  /// @}
  ///


  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VLineFollowerComponent, SHARED_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VLineFollowerComponent, SHARED_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  SHARED_IMPEXP VOVERRIDE void Serialize( VArchive &ar );

  ///
  /// @}
  ///


  ///
  /// @name IVisCallbackHandler_cl Overrides
  /// @{
  ///

  /// 
  /// \brief
  ///   Overridden function that gets called by all callbacks that this handler is registered to
  /// 
  /// \param pData
  ///   See IVisCallbackHandler_cl
  ///
  /// \see
  ///   IVisCallbackHandler_cl
  /// 
  VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  ///
  /// @}
  ///


  ///
  /// @name Update Method
  /// @{
  ///

  ///
  /// \brief
  ///   Updates the the state machine for the current frame.
  /// 
  /// The PerFrameUpdate() function takes care of processing the input map (keyboard events)
  /// and updating the gravity gun component functionality.
  ///
  /// \note
  ///   You do not have to call this function manually, since the VGravityGunComponentManager
  ///   class will take care of this
  ///
  void PerFrameUpdate();

  ///
  /// @}
  ///


  ///
  /// @name Helper
  /// @{
  ///

  SHARED_IMPEXP void InitPhysics(float fPathPos);
  SHARED_IMPEXP BOOL StartAnimation(const char *szAnimName);

  ///
  /// @}
  ///

  vHavokCharacterController *m_pPhys;

  // entity parameter
  char Model_AnimationName[128];
  char Path_Key[128];
  float Path_NumberSteps;
  float Path_TriggerDistance;
  VisPath_cl *m_pFollowPath;
  float Path_InitialOffset;
  float m_fCurrentPathPos;
  BOOL Debug_DisplayBoxes;
  bool m_bPlayingAnim;
  
  float Model_DeltaRotation;
  hkvMat3 m_mDeltaRotation;
  float Model_GroundOffset;
  float Model_CapsuleHeight;
  float Model_CapsuleRadius;

  BOOL Debug_RenderMesh;
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
