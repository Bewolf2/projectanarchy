/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFollowPathComponent.hpp

#ifndef VENTITYLODCOMPONENT_HPP_INCLUDED
#define VENTITYLODCOMPONENT_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/Visibility/VisApiVisibilityLODHysteresis.hpp>

// Versions
#define ENTITYLODCOMPONENT_VERSION_0          0     // Initial version
#define ENTITYLODCOMPONENT_VERSION_CURRENT    0     // Current version

///
/// \brief
///   Enum that represents commonly used LOD level indices
enum VEntityLODLevel_e
{
  VLOD_HIGH      = 0,     ///< Highest LOD level, mesh defined in the owner entity
  VLOD_MEDIUM    = 1,     ///< Medium LOD level
  VLOD_LOW       = 2,     ///< Low LOD level
  VLOD_ULTRALOW  = 3,     ///< Ultra low LOD level

  VLOD_NONE      = 4,     ///< No LOD, LOD is disabled, use entity mesh (if available)
  VLOD_AUTO      = 5,     ///< Switches LOD by distance
};


///
/// \brief
///   Class that represents information about one LOD level. The VEntityLODComponent has an array of it.
///
class VEntityLODLevelInfo
{
public:
  /// \brief
  ///   Constructor
  VEntityLODLevelInfo()
  {
    m_fMinSwitchDistance = 0.f;
    m_fMaxSwitchDistance = 0.f;
    m_pEntity = NULL;
    m_iTagged = 0;
  }

  ~VEntityLODLevelInfo();

  /// \brief
  ///   Associates a model file with this Level
  bool SetModelFile(const char *szFilename, bool bAppyMotionDelta = false);

  /// \brief
  ///   Sets a distance at which the level switches to next level. So the LOD level will be visible
  ///   in the range [m_fMinSwitchLevel .. m_fMaxSwitchLevel]
  inline void SetSwitchDistance(float fMinDist, float fMaxDist)
  {
    m_fMinSwitchDistance = fMinDist;
    m_fMaxSwitchDistance = fMaxDist;
  }

public:
  // ----- Functions required for LOD fading feature of simulation package (not used in base SDK)

  /// \brief
  ///   Returns the previously set bounding box.
  inline const hkvAlignedBBox &GetBoundingBox() const 
  {
    VASSERT(m_pEntity != NULL)
    return m_pEntity->GetBoundingBox(); 
  }

  /// \brief
  ///    Helper function for visibility loop implementations
  ///
  /// \param vCameraPos
  ///   Camera position to test LOD clipping
  /// \param fLODScaleSqr
  ///   Square of the LOD scaling factor
  ///
  /// \returns
  ///   true if the object is clipped either by near of far clip distance (if specified)
  inline bool IsNearOrFarClipped(const hkvVec3& vCameraPos, float fLODScaleSqr=1.f) const
  {
    VASSERT(m_pEntity != NULL)

    float fDistSqr = m_pEntity->GetPosition().getDistanceToSquared(vCameraPos);
    return ((m_fMinSwitchDistance>0.f) && (fDistSqr<(m_fMinSwitchDistance*m_fMinSwitchDistance))) 
        || ((m_fMaxSwitchDistance>0.f) && (fDistSqr>=(m_fMaxSwitchDistance*m_fMaxSwitchDistance)));
  }

  /// \brief
  ///    Helper function to retrieve current distance to a given plane
  ///
  /// \param vCameraPos
  ///   Camera position
  /// \param fLODScaleSqr
  ///   Square of the LOD scaling factor
  /// \param fPlane
  ///   Reference plane distance
  ///
  /// \returns
  ///   Relative distance from object to given plane
  inline float GetDistanceToPlane(const hkvVec3& vCameraPos, float fLODScaleSqr=1.f, float fPlane=0.f) const
  {
    float fDistSqr = m_pEntity->GetPosition().getDistanceToSquared(vCameraPos);
    return hkvMath::sqrt(fDistSqr) - fPlane;
  }

  /// \brief
  ///   Tags the entity. 
  /// 
  /// Tagging can be used to quickly flag a number of entities for a specific operation. For instance, if you
  /// have multiple potentially overlapping collections of entities, you can use tagging to generate a single
  /// list that contains all entities from the collection, but no duplicates. Tagging (including querying and
  /// resetting tags) is a very fast operation.
  ///
  /// \sa VisEntityCollection_cl::GetTaggedEntries
  inline void Tag() { m_iTagged = s_iTagCtr; }

  /// \brief
  ///   Returns whether an entity is tagged. This is a low-overhead operation.
  inline bool IsTagged() const { return m_iTagged == s_iTagCtr; }

  /// \brief
  ///   Resets all entity tags. This is a low-overhead operation.
  static inline void ResetTags() { s_iTagCtr++; }

  /// \brief
  ///   Returns near clip distance.
  inline float GetNearClipDistance() const
  {
    return m_fMinSwitchDistance;
  }

  /// \brief
  ///   Returns far clip distance.
  inline float GetFarClipDistance() const
  {
    return m_fMaxSwitchDistance;
  }

public:
  VDynamicMeshPtr m_spMesh;                                   ///< Mesh of this LOD level
  VisAnimConfigPtr m_spAnimConfig;                            ///< Animation Config of this LOD level  
  VisAnimFinalSkeletalResultPtr m_spFinalSkeletalResult;      ///< Final Animation Result of this LOD level
  float m_fMinSwitchDistance;                                 ///< Min switching distance of this LOD level
  float m_fMaxSwitchDistance;                                 ///< Max switching distance of this LOD level
  VisBaseEntity_cl* m_pEntity;                                ///< Entity instance that is owner of the VEntityLODComponent
  unsigned int m_iTagged;                                     ///< Tagging value
  EFFECTS_IMPEXP static unsigned int s_iTagCtr;               ///< Global consecutive tagging number
  EFFECTS_IMPEXP static VisCallback_cl OnEntityLODDestroyed;  ///< Callback that gets triggered when an instance is destroyed
};


/// \brief
///   Data object that is sent by the static VEntityLODLevelInfo::OnEntityLODDestroyed callback
class VEntityLODDataObject : public IVisCallbackDataObject_cl
{
public:
  inline VEntityLODDataObject(VCallback* pSender, VEntityLODLevelInfo* pObj) : IVisCallbackDataObject_cl(pSender), m_pEntityLODObject(pObj) {}
  VEntityLODLevelInfo* m_pEntityLODObject;
};


/// 
/// \brief
///   Entity LOD component that can be added to entities so they can have multiple LOD levels each represented by a different model.
/// 
#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
  class VEntityLODComponent : public IVLODHysteresisComponent
#else
  class VEntityLODComponent : public IVObjectComponent
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
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
  EFFECTS_IMPEXP VEntityLODComponent(int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// 
  /// \brief
  ///   Component Destructor
  /// 
  EFFECTS_IMPEXP virtual ~VEntityLODComponent();

  ///
  /// @}
  ///


  ///
  /// @name Component methods
  /// @{
  ///

  ///
  /// \brief
  ///   Initializes the entity LOD component by setting the initial values
  /// 
  EFFECTS_IMPEXP void CommonInit(bool bFirstInit);

  ///
  /// \brief
  ///   Updates the LOD level of the owner object
  /// 
  /// The PerFrameUpdate() function takes care of evaluating the current distance of the object 
  /// to the camera and updates the LOD level by assigning the respective model.
  ///
  /// \note
  ///   You do not have to call this function manually, since the VEntityLODComponentManager
  ///   class will take care of this.
  ///
  void PerFrameUpdate();
  
  ///
  /// \brief
  ///   Updates the LOD level depending on the camera distance
  ///
  void UpdateLOD(bool bFirstTime);

  ///
  /// \brief
  ///   Updates the passed LOD level
  ///
  void UpdateLODLevel(int newLevel);

  ///
  /// \brief
  ///   Accesses the LOD level info of specified index, where the index must be in the allocated range.
  ///
  inline VEntityLODLevelInfo &GetLevelInfo(int iIndex)
  {
    VASSERT(iIndex > -1 && iIndex < LOD_LevelCount + 2);
    return m_pLevels[iIndex];
  }

  ///
  /// \brief
  ///   Updates the skeletal animation root node on all animation configs
  ///
  void SetSkeletalAnimRootNode(IVisAnimResultGenerator_cl *pRoot)
  {
    for (int i = 0; i < LOD_LevelCount + 2; i++)
      if (m_pLevels[i].m_spFinalSkeletalResult)
        m_pLevels[i].m_spFinalSkeletalResult->SetSkeletalAnimInput(pRoot);
  }

  ///
  /// \brief
  ///   Forwards the new frozen state to all animation configs involved
  ///
  void SetFrozen(bool bNewState)
  {
    for (int i=0; i < LOD_LevelCount; i++)
      m_pLevels[i].m_spAnimConfig->SetFrozen(bNewState);
  }

  ///
  /// \brief
  ///   Sets a LOD index. Can be LOD_AUTO to detect according to distance in the Update() function
  ///
  void SetLODLevel(VEntityLODLevel_e newLevel);

  ///
  /// \brief
  ///   Returns the current LOD level, which is always >= 0
  ///
  inline int GetCurrentLODLevel() const { return m_iCurrentLevel; }

  ///
  /// \brief
  ///   Evaluates the current distance of the owner's entity to the camera
  ///
  inline float GetDistanceToCamera() const { return (((VisBaseEntity_cl*)GetOwner())->GetPosition() - Vision::Camera.GetCurrentCameraPosition()).getLength(); }

  ///
  /// \brief
  ///   Try connect to a transition state machine component
  ///
  bool ConnectStateMachine();

  ///
  /// \brief
  ///   Try to find an anim config that might have been set from outside (animation component, transition state machine)
  ///
  bool FindAnimConfig();

  ///
  /// @}
  ///

#ifdef SUPPORTS_LOD_HYSTERESIS_THRESHOLDING
  ///
  /// @name IVLODHysteresisLevelProvider Virtual Overrides
  /// @{
  ///
  EFFECTS_IMPEXP virtual int GetLODLevel() const HKV_OVERRIDE;

  ///
  /// @}
  ///
#endif //SUPPORTS_LOD_HYSTERESIS_THRESHOLDING

  ///
  /// @name VTypedObject Virtual Overrides
  /// @{
  ///

#if defined(WIN32) || defined(_VISION_DOC)

  ///
  /// \brief
  ///   Overridable that is called by the editor to retrieve per-instance variable display hints. 
  ///
  /// \param pVariable
  ///   Variable to retrieve dynamic display hints for.
  ///
  /// \param destInfo
  ///   Structure that can be modified to affect the editor's displaying 
  ///   of the respective variable (read-only, hidden).
  ///
  EFFECTS_IMPEXP virtual void GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo) HKV_OVERRIDE;

#endif

  ///
  /// \brief
  ///   Virtual function that gets called whenever a message is sent to this object
  ///
  /// \param iID
  ///   The ID constant of the message.
  /// 
  /// \param iParamA
  ///   Message data value
  /// 
  /// \param iParamB
  ///   Message data value
  ///
  /// \see
  ///   VisTypedEngineObject_cl::MessageFunction
  ///
  EFFECTS_IMPEXP virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB);

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
  V_DECLARE_SERIAL_DLLEXP(VEntityLODComponent, EFFECTS_IMPEXP)

  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE(VEntityLODComponent, EFFECTS_IMPEXP)

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
  int LOD_LevelMode;
  int LOD_LevelCount;
  VString Level_Medium_Mesh;    
  VString Level_Low_Mesh;        
  VString Level_UltraLow_Mesh;
  float Level_Medium_Distance;  
  float Level_Low_Distance;    
  float Level_UltraLow_Distance;

  // Not exposed:
  VString Level_High_Mesh;              ///< High level mesh defined by owner entity mesh
  float Level_High_Distance;            ///< High level distance is always 0.0f 
  int m_iCurrentLevel;                  ///< Current LOD level
  VEntityLODLevelInfo *m_pLevels;       ///< Information about LOD levels
};


///
/// \brief
///   Collection for handling entity LOD components
///
class VEntityLODComponentCollection : public VRefCountedCollection<VEntityLODComponent>
{
};


///
/// \brief
///   Manager for all VPlayableCharacterComponent instance
///
/// This manager class has a list of all available VEntityLODComponent instances
/// and takes care of calling their VEntityLODComponent::PerFrameUpdate function
/// each frame.
///
class VEntityLODComponentManager : public IVisCallbackHandler_cl
{
public:

  ///
  /// \brief
  ///   Gets the singleton of the manager
  ///
  static VEntityLODComponentManager &GlobalManager()
  {
    return g_GlobalManager;
  }

  ///
  /// \brief
  ///   Should be called at plugin initialization time
  ///
  void OneTimeInit()
  {
    // Register
    Vision::Callbacks.OnUpdateSceneFinished += this; 
    Vision::Callbacks.OnAfterSceneLoaded += this;
  }

  ///
  /// \brief
  ///   Should be called at plugin de-initialization time
  ///
  void OneTimeDeInit()
  {
    // De-register
    Vision::Callbacks.OnUpdateSceneFinished -= this;
    Vision::Callbacks.OnAfterSceneLoaded -= this;
  }

  ///
  /// \brief
  ///   Gets all VEntityLODComponent instances this manager holds
  /// 
  inline VEntityLODComponentCollection &Instances() { return m_Components; }

  /// 
  /// \brief
  ///   Callback method that takes care of updating the managed instances each frame
  ///  
  virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData);

protected:

  /// Holds the collection of all instances of VEntityLODComponent
  VEntityLODComponentCollection m_Components;

  /// One global instance of our manager
  static VEntityLODComponentManager g_GlobalManager;

};

#endif // VENTITYLODCOMPONENT_HPP_INCLUDED

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
