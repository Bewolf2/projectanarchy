/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFollowPathComponent.hpp

#ifndef VFOLLOWPATHCOMPONENT_HPP_INCLUDED
#define VFOLLOWPATHCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>

// Versions
#define FOLLOWPATHCOMPONENT_VERSION_0          0     // Initial version
#define FOLLOWPATHCOMPONENT_VERSION_1          1     // Path object serialization
#define FOLLOWPATHCOMPONENT_VERSION_2          2     // constant speed flag
#define FOLLOWPATHCOMPONENT_VERSION_CURRENT    2     // Current version

/// 
/// \brief
///   Path component that can be added 3D objects so they will be able to follow path objects.
/// 
class VFollowPathComponent : public IVObjectComponent
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
  ///   See IVObjectComponent
  /// 
  /// \see
  ///   IVObjectComponent
  /// 
  EFFECTS_IMPEXP VFollowPathComponent(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// 
  /// \brief
  ///   Component Destructor
  /// 
  EFFECTS_IMPEXP virtual ~VFollowPathComponent();

  ///
  /// @}
  ///

  ///
  /// @name Component methods
  /// @{
  ///

  ///
  /// \brief
  ///   Initializes the follow path component by setting the initial values
  /// 
  EFFECTS_IMPEXP void Init();

  ///
  /// \brief
  ///   Updates the the position of the owner object on the path
  /// 
  /// The PerFrameUpdate() function takes care of evaluating the current position of
  /// the owner object on the path depending on the passed time.
  ///
  /// \note
  ///   You do not have to call this function manually, since the VFollowPathComponentManager
  ///   class will take care of this.
  ///
  void PerFrameUpdate();

  ///
  /// @}
  ///

  ///
  /// @name IVObjectComponent Overrides
  /// @{
  ///

  /// \brief
  ///   Overridden function to respond to owner changes
  EFFECTS_IMPEXP virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Overridden function. Blob shadows can be attached to VisObject3D_cl instances
  EFFECTS_IMPEXP virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridden function to respond to variable changes
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
  V_DECLARE_SERIAL_DLLEXP(VFollowPathComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VFollowPathComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   Serialization function
  ///
  /// \param ar
  ///   Binary archive
  EFFECTS_IMPEXP virtual void Serialize( VArchive &ar );

  ///
  /// @}
  ///

private:  

  // Exposed to vForge:

  VString PathKey;                          ///< The object key of the path
  VObjectReference<VisPath_cl> PathObject;  ///< Path object reference
  float Time;                               ///< Time that owner object will take to move along the path
  float InitialOffset;                      ///< Initial offset on the path [0..1]
  hkvVec3 PositionOffset;                   ///< Position offset relative to the evaluated path point position
  hkvVec3 OrientationOffset;                ///< Orientation offset relative to the evaluated path point direction
  BOOL Looped;                              ///< If true, the owner object will move along the path over and over
  BOOL Direction;                           ///< If true, the owner will rotate according to the path (first derivative)
  BOOL ConstantSpeed;                       ///< If true, the speed along the path is constant regardless of the path node distribution

  // Not exposed:
  VisPath_cl *m_pPath;   ///< Path object with the specified key if found
  float m_fCurrentTime;  ///< Current time need for evaluating current position on path
  bool m_bFirstFrame;    ///< If true, the PerFrameUpdate function is called for the first time

};


///
/// \brief
///   Collection for handling follow path components
///
class VFollowPathComponentCollection : public VRefCountedCollection<VFollowPathComponent>
{
};


///
/// \brief
///   Manager for all VPlayableCharacterComponent instance
///
/// This manager class has a list of all available VFollowPathComponent instances
/// and takes care of calling their VFollowPathComponent::PerFrameUpdate function
/// on each frame.
///
class VFollowPathComponentManager : public IVisCallbackHandler_cl
{
public:

  ///
  /// \brief
  ///   Gets the singleton of the manager
  ///
  EFFECTS_IMPEXP static VFollowPathComponentManager &GlobalManager()
  {
    return g_GlobalManager;
  }

  ///
  /// \brief
  ///   Should be called at plugin initialization time
  ///
  inline void OneTimeInit()
  {    
    m_bHandleOnUpdateSceneBegin = false;

    // register
    Vision::Callbacks.OnUpdateSceneFinished += this;
    Vision::Callbacks.OnAfterSceneLoaded += this;
  }

  ///
  /// \brief
  ///   Should be called at plugin de-initialization time
  ///
  inline void OneTimeDeInit()
  {
    // de-register
    if(m_bHandleOnUpdateSceneBegin)
    {
      Vision::Callbacks.OnUpdateSceneBegin -= this;    
    }
    else
    {
      Vision::Callbacks.OnUpdateSceneFinished -= this;    
    }
    
    Vision::Callbacks.OnAfterSceneLoaded -= this;
  }

  ///
  /// \brief
  ///   This allows to resolve possible update ordering issues e.g. in conjunction with the PathCamera. 
  ///
  /// \param bOnUpdateSceneBegin
  ///   If set to true, the components of type VFollowPathComponent will be updated in OnUpdateSceneBegin
  ///   instead of OnUpdateSceneFinished.
  ///
  /// Only use this if you are experiencing these problems,
  /// as using this flag possibly introduces update ordering issues
  /// in other places (e.g. when the path is parented to another
  /// moving object).
  EFFECTS_IMPEXP void SetHandleOnUpdateSceneBegin(bool bOnUpdateSceneBegin);

  ///
  /// \brief
  ///   Gets all VPlayableCharacterComponent instances this manager holds
  /// 
  inline VFollowPathComponentCollection &Instances() { return m_Components; }

  /// 
  /// \brief
  ///   Callback method that takes care of updating the managed instances each frame
  ///  
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

protected:

  /// Holds the collection of all instances of VFollowPathComponent
  VFollowPathComponentCollection m_Components;

  /// One global instance of our manager
  static VFollowPathComponentManager g_GlobalManager;

private:

  /// By default the Follow Path Components are updated in OnUpdateSceneFinished.
  /// If this flag is enabled, they will be updated during OnUpdateSceneBegin.
  /// This can prevent update ordering issues e.g. in conjunction with the PathCamera shape.
  bool m_bHandleOnUpdateSceneBegin;

};

#endif // VFOLLOWPATHCOMPONENT_HPP_INCLUDED

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
