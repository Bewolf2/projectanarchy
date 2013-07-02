/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file _AnimEntity.hpp
/// Generic entity class with support for playing an animation.
///


#ifndef VISION__ANIMENTITY
#define VISION__ANIMENTITY

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>

// class AnimEntity_cl is deprecated - should not be used anymore
#ifndef _VISION_DOC

class AnimEntity_cl : public VisBaseEntity_cl, public IVisCallbackHandler_cl
{
public:

  enum  AnimEntitySkinningMode
  {
    SKINNINGMODE_DEFAULT,
    SKINNINGMODE_SOFTWARE,
    SKINNINGMODE_HARDWARE
  };

  HKV_DEPRECATED EFFECTS_IMPEXP AnimEntity_cl();
  EFFECTS_IMPEXP ~AnimEntity_cl();
  HKV_DEPRECATED EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE;
  HKV_DEPRECATED EFFECTS_IMPEXP virtual void ThinkFunction() HKV_OVERRIDE;
  HKV_DEPRECATED EFFECTS_IMPEXP virtual void MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB ) HKV_OVERRIDE;

  HKV_DEPRECATED EFFECTS_IMPEXP bool StartAnimation(const char *szAnimName);

  HKV_DEPRECATED EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

#ifdef WIN32

  HKV_DEPRECATED EFFECTS_IMPEXP virtual void GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo) HKV_OVERRIDE;

#endif

  // entity parameter
  char AnimationName[128];
  char PathKey[128];
  float PathTime;
  VisPath_cl *m_pFollowPath; // no smart pointer here as de-referencing it would delete the path
  float m_fPathPos;
  int SkinningMode;

  //serialization
  V_DECLARE_SERIAL_DLLEXP( AnimEntity_cl,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  IMPLEMENT_OBJ_CLASS(AnimEntity_cl)
};

#endif // _VISION_DOC

// class StaticCollisionEntity_cl is deprecated - should not be used anymore
#ifndef _VISION_DOC

class StaticCollisionEntity_cl : public VisBaseEntity_cl
{
public:
  EFFECTS_IMPEXP void CommonInit();

  EFFECTS_IMPEXP virtual void InitFunction() HKV_OVERRIDE {CommonInit();}
  //serialization
  V_DECLARE_SERIAL_DLLEXP( StaticCollisionEntity_cl,  EFFECTS_IMPEXP );
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  IMPLEMENT_OBJ_CLASS(StaticCollisionEntity_cl)
};

#endif // _VISION_DOC


/// \brief
///   Component class that adds simple animation playback capabilities to entities.
///
/// The animation with specified name is started as soon as the component is attached to its owner.
class VSimpleAnimationComponent : public IVObjectComponent
{
public:
  /// \brief Constructor takes VObjectComponentFlags_e flags.
  EFFECTS_IMPEXP VSimpleAnimationComponent(int iComponentFlags = VIS_OBJECTCOMPONENTFLAG_NONE);
  EFFECTS_IMPEXP virtual ~VSimpleAnimationComponent();

  // IVObjectComponent overrides
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual void OnVariableValueChanged(VisVariable_cl *pVar, const char * value) HKV_OVERRIDE;
  EFFECTS_IMPEXP virtual bool IsRelevantForSerialization() const HKV_OVERRIDE;

  //serialization
  V_DECLARE_SERIAL_DLLEXP( VSimpleAnimationComponent, EFFECTS_IMPEXP)
  V_DECLARE_VARTABLE(VSimpleAnimationComponent, EFFECTS_IMPEXP)
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar ) HKV_OVERRIDE;

  /// \brief
  ///   Updates the animation of the owner object.
  /// 
  /// \note
  ///   You do not have to call this function manually, since the VSimpleAnimationComponentManager
  ///   class will take care of this.
  void PerFrameUpdate();

  VString AnimationName; ///< Name of the animation that should be started
  BOOL UseMotionDelta;   ///< Whether motion delta should be used to move the entity around by the animation

private:
  friend class AnimEntity_cl;

  enum
  {
    VERSION0 = 0, ///< initial version
    VERSION1 = 1, ///< added UseAnimationDelta
    VERSION_CURRENT = VERSION1 ///< current version
  };

  /// \brief
  ///   Static helper function to start the animation of an entity.
  static EFFECTS_IMPEXP bool StartAnimation(VisBaseEntity_cl* pEntity, const char* szAnimName);

};


/// \brief
///   Collection for handling entity LOD components
class VSimpleAnimationComponentCollection : public VRefCountedCollection<VSimpleAnimationComponent>
{
};


/// \brief
///   Manager for all VPlayableCharacterComponent instance
///
/// This manager class has a list of all available VSimpleAnimationComponen instances
/// and takes care of calling their VSimpleAnimationComponen::PerFrameUpdate function
/// each frame.
class VSimpleAnimationComponentManager : public IVisCallbackHandler_cl
{
public:

  /// \brief
  ///   Gets the singleton of the manager
  static VSimpleAnimationComponentManager& GlobalManager()
  {
    return g_GlobalManager;
  }

  /// \brief
  ///   Should be called at plugin initialization time
  void OneTimeInit()
  {
    // Register
    Vision::Callbacks.OnUpdateSceneBegin += this; 
  }

  /// \brief
  ///   Should be called at plugin de-initialization time
  void OneTimeDeInit()
  {
    // De-register
    Vision::Callbacks.OnUpdateSceneBegin -= this;
  }

  /// \brief
  ///   Gets all VSimpleAnimationComponen instances this manager holds
  inline VSimpleAnimationComponentCollection& Instances() { return m_Components; }

  /// \brief
  ///   Callback method that takes care of updating the managed instances each frame
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

protected:

  /// Holds the collection of all instances of VSimpleAnimationComponen
  VSimpleAnimationComponentCollection m_Components;

  /// One global instance of our manager
  static VSimpleAnimationComponentManager g_GlobalManager;

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
