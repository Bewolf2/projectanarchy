/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPlayerCamera.hpp

#ifndef VPLAYERCAMERA_HPP_INCLUDED
#define VPLAYERCAMERA_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

// Versions
#define PLAYERCAMERACOMPONENT_VERSION_0          0     // Initial version
#define PLAYERCAMERACOMPONENT_VERSION_2          2     // Updated version
#define PLAYERCAMERACOMPONENT_VERSION_3          3     // Added support for fixed camera orientation (not linked to parent object)
#define PLAYERCAMERACOMPONENT_VERSION_CURRENT    3     // Current version

/// 
/// \brief
///   linked to the parent entity.
/// 
/// This component can be attached to an entity to observe its movement. 
/// Basically this camera has two different modes - free orbit and follower. In the
/// free orbit mode you can move the camera freely around the parent entity by moving
/// the mouse. In this mode you can also enable zooming which can be controlled with 
/// the mouse wheel. The follower mode aligns the camera orientation to the parent
/// entity and follows its rotation. In this mode you can control the smoothness of
/// the camera movement, meaning the ease in/out of the camera alignment.
/// When setting up a player camera component via code, the relevant public member variables
/// (InitialYaw,...) have to be set before attaching the component to an entity.
class VPlayerCamera : public IVObjectComponent, public IVisCallbackHandler_cl
{
public: 

  ///
  /// @name Constructor
  /// @{
  ///

  ///
  /// \brief
  ///   Component Constructor
  /// 
  /// \param iComponentFlags
  ///   See IVObjectComponent.
  /// 
  /// \see
  ///   IVObjectComponent
  /// 
  EFFECTS_IMPEXP VPlayerCamera(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// 
  /// \brief
  ///   Component Destructor
  /// 
  EFFECTS_IMPEXP virtual ~VPlayerCamera();

  ///
  /// @}
  ///

  ///
  /// @name Player Camera Properties
  /// @{
  ///

  ///
  /// \brief
  ///   Enables or disables the player camera.
  /// 
	/// \param bStatus
	///   State value specifying the enabled state of the player camera.
	/// 
  /// \see
  ///   IsEnabled
	/// 
  EFFECTS_IMPEXP void SetEnabled(BOOL bStatus=true);

  ///
  /// \brief
  ///   Indicates whether the player camera is currently enabled.
  /// 
	/// \returns
	///   TRUE if player camera is enabled, FALSE otherwise.
	/// 
	/// \see
  ///   SetEnabled
	/// 
	inline BOOL IsEnabled() const {return Enabled;}

  ///
  /// Resets the old position that is used for blending in follow mode
  /// 
  EFFECTS_IMPEXP inline void ResetOldPosition() { m_vOldPos = ((VisBaseEntity_cl *)GetOwner())->GetPosition(); }

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes.
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden function. Blob shadows can be attached to VisObject3D_cl instances.
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridden function to respond to variable changes.
  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);

  ///
  /// @}
  ///

  ///
  /// @name Serialization
  /// @{
  ///

  /// \brief
  ///   RTTI macro
  V_DECLARE_SERIAL_DLLEXP(VPlayerCamera, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VPlayerCamera, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   binary archive
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  /// \brief
  ///   Virtual overridable that gets called when a loading archive closes
  /// 
  EFFECTS_IMPEXP virtual void OnDeserializationCallback(const VSerializationContext &context);

  /// \brief
  ///   Virtual overridable that gets called to determine if the object needs the OnDeserializationCallback
  /// 
  EFFECTS_IMPEXP virtual VBool WantsDeserializationCallback(const VSerializationContext &context)
  {
    return TRUE;
  }

  ///
  /// @}
  ///

  ///
  /// @name IVisCallbackHandler_cl Overrides
  /// @{
  ///

  /// 
  /// \brief
  ///   Overridden function that gets called by all callbacks that this handler is registered to.
  /// 
  /// \param pData
  ///   See IVisCallbackHandler_cl.
  ///
  /// \see
  ///   IVisCallbackHandler_cl
  /// 
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

  /// \brief
  ///   This function supplies the sorting key for the calling order. Handlers with small key value
  ///   are called first.
  /// 
  /// \param pCallback
  ///   The current callback instance that performs sorting.
  /// 
  /// \return
  ///   int iSortinKey: Arbitrary sorting key value. The value can be positive or negative.
  ///   We want the camera to update after other callbacks as they can change the owner entity's position.
  virtual int GetCallbackSortingKey(VCallback *pCallback)
  {
	  return 42000;
  }

  ///
  /// @}
  ///

private:  

  // Initial Camera Setup (called automatically in SetOwner)
  void SetCamera(float fYaw, float fPitch, float fDistance, float fHeight);

  // Camera Update Step (called automatically each frame in OnHandleCallback) 
  void SetCameraStep(float fYawStep, float fPitchStep, float fDistanceStep, float fHeightStep);

  // Attaches/Detaches the camera
  void UpdateAttachment();

public:

  // Exposed to vForge:
  BOOL Enabled;                       ///< State of the camera
  BOOL Collides;                      ///< Whether camera collides with world geometry
  BOOL Follow;                        ///< Whether camera follows character
  BOOL Zoom;                          ///< Whether camara zoom is enabled
  BOOL DepthOfField;                  ///< Whether depth of field focus is adjusted automatically
  BOOL FollowFixed;                   ///< Whether the camera rotates with the parent object

  float InitialYaw;                   ///< Yaw angle - Rotate around the character
  float InitialPitch;                 ///< Pitch angle - Rotate above the character
  float CameraDistance;               ///< Distance of the camera from the character
  float MinimalDistance;              ///< Minimal camera distance
  float MaximalDistance;              ///< Maximal camera distance
  float RelativeLookAtHeight;         ///< The relative lookat height of the character the camera looks at
  
  float CameraSensitivity;            ///< Camera movement sensitivity
  float MoveSmoothness;               ///< Smoothness of the camera movement (ease in/out only for follow camera)
  
private:

  float m_fRelativeLookAtHeight;      ///< Relative lookat height in absolute coords. 

  // Not exposed to vForge:
  VSmartPtr<VisBaseEntity_cl> m_spCameraProxy;        ///< Camera entity placeholder
  hkvVec3 m_vOldPos;                  ///< Position of camera in last frame (needed for ease in/out)

public:
  VInputMap *m_pInputMap;             ///< Input map for the camera control;
};

#endif // VPLAYERCAMERA_HPP_INCLUDED

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
