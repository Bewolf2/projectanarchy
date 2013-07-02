/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiTypedEngineObject.hpp

#ifndef FR_DEFINE_VISAPITYPEDENGINEOBJECT
#define FR_DEFINE_VISAPITYPEDENGINEOBJECT

class IVObjectComponent;
#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponentCollection.hpp>
#include <Vision/Runtime/Engine/System/PluginSystem/VisShareEntityPlugin.hpp>

class VisZoneResource_cl;
class VisVariable_cl;
class IVNetworkSynchronizationGroup;
//class IVNetworkObjectReplicator;
struct VNetworkSynchronizationGroupInstanceInfo_t;
struct VNetworkViewContext;
struct VisZoneRepositionInfo_t;

/// \brief
///   Common base class for engine objects. 
/// 
/// Provides support for serialization via the VTypedObject  class (which is part of vBase).
/// 
/// This class adds a component list to its base class VTypedObject.
/// 
/// Furthermore, objects of this type can be attached to zones. See SetParentZone/GetParentZone.
class VisTypedEngineObject_cl : public VTypedObject
{
protected:
  VISION_APIFUNC VisTypedEngineObject_cl();
  
public:
  V_DECLARE_SERIAL_DLLEXP( VisTypedEngineObject_cl, VISION_APIDATA );
  V_DECLARE_VARTABLE(VisTypedEngineObject_cl,VISION_APIDATA);

  /// \brief
  ///   Destructor, cleans up the components
  VISION_APIFUNC VOVERRIDE ~VisTypedEngineObject_cl();
  
  ///
  /// @name VTypedObject Overrides
  /// @{
  ///

  /// \brief
  ///   Override this method to provide post-create intialization of this object
  /// 
  /// This is especially useful for objects that contain any parameters based on VTypedObject's
  /// parameter system, since these parameters are not yet initialized at construction time.
  /// 
  /// Please note that the engine objects use the VTypedObject RTTI system for serialization
  /// purposes only. The Initialize function will thus only be called when deserializing objects
  /// and not when dynamically creating them.
  VISION_APIFUNC virtual void Initialize() HKV_OVERRIDE;
  
  /// \brief
  ///   Overridden function that removes this object from its parent zone. It does not delete this
  ///   object, this must be done in derived implementations
  VISION_APIFUNC virtual void DisposeObject() HKV_OVERRIDE;


  /// \brief
  ///   Overridable function that verifies that the object is valid. 
  /// 
  /// The default implementation verifies that the object pointer is still valid. You can override
  /// this function to perform additional sanity checks on the object.
  VISION_APIFUNC virtual void AssertValid() HKV_OVERRIDE;

#ifdef SUPPORTS_SNAPSHOT_CREATION
  /// \brief
  ///   Overridden function that gets the resources used for this instance. This implementation
  ///   calls GetDependencies on all serializable components
  VISION_APIFUNC virtual void GetDependencies(VResourceSnapshot &snapshot) HKV_OVERRIDE;
#endif

#ifdef WIN32

  /// \brief
  ///   Internal function used by vForge
  VISION_APIFUNC virtual void SetMarkInsideSerializationSession(bool bStatus) HKV_OVERRIDE;

#endif


  ///
  /// @}
  ///

  ///
  /// @name Message Function
  /// @{
  ///

  /// \brief
  ///   Virtual function that gets called whenever a message is sent to this object
  /// 
  /// The virtual MessageFunction gets called whenever a message is sent to this object. You can
  /// override the virtual message function in order to process your own messages or listen to
  /// important events such as collisions.
  /// 
  /// Each message has a message ID defined as a constant. The message IDs between VIS_MSG_USER and
  /// VIS_MSG_LASTUSER are available for user defined game code messages.
  /// 
  /// Physics modules have a reserved range of VIS_MSG_PHYSICS to VIS_MSG_LASTPHYSICS.
  /// 
  /// vForge also uses the message system; it reserves the range VIS_MSG_EDITOR to
  /// VIS_MSG_LASTEDITOR.
  /// 
  /// All message IDs larger than VIS_MSG_INTERNAL are reserved for internal messages.
  /// 
  /// The event with the ID constant VIS_MSG_EVENT is sent by the animation system when an
  /// animation event is triggered. See also VisAnimControl_cl::AddEventListener
  /// 
  /// \param iID
  ///   The ID constant of the message.
  /// 
  /// \param iParamA
  ///   message data value
  /// 
  /// \param iParamB
  ///   message data value
  /// 
  /// \note
  ///   It is important to call the base implementation of this function when overriding it,
  ///   otherwise the internal engine messages won't get processed anymore.
  /// 
  /// \sa VisTypedEngineObject_cl::SendMsg
  /// 
  /// \example
  ///   \code
  ///   void MyClass::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
  ///   {
  ///     if ( iID == VIS_MSG_EVENT )
  ///     {
  ///       VisAnimControl_cl *pSender = (VisAnimControl_cl *)iParamB;
  ///       handle event message
  ///     }
  ///     else if ( (iID >= VIS_MSG_USER) && (iID <= VIS_MSG_LASTUSER) )
  ///     {
  ///       handle user messages
  ///     }
  ///     else
  ///       VisBaseEntity_cl::MessageFunction( iID, iParamA, iParamB );
  ///   }
  ///   \endcode
  VISION_APIFUNC virtual void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB );

  /// \brief
  ///   Serializes the object. Currently empty.
  /// 
  /// For more information about serialization, refer to the Vision API Documentation.
  /// 
  /// \param ar
  ///   The archive object to read from or write to.
  VISION_APIFUNC virtual void Serialize(VArchive &ar) HKV_OVERRIDE;

  /// \brief
  ///   Finalize the object after the serialization is complete. This Method calls SetOwner on all components.
  VISION_APIFUNC virtual void OnSerialized(VArchive &ar) HKV_OVERRIDE;

  /// \brief
  ///   Send a message to another VisTypedEngineObject_cl object; e.g., an entity.
  /// 
  /// This is a convienience function that forwards to Vision::Game.SendMsg (VisGame_cl::SendMsg).
  /// 
  /// \param pDest
  ///   destination object the message will be sent to
  /// 
  /// \param iID
  ///   message ID. Must be a valid, non-reserved ID (see VisTypedEngineObject_cl::MessageFunction)
  /// 
  /// \param iParamA
  ///   message data parameter 1
  /// 
  /// \param iParamB
  ///   message data parameter 2
  /// 
  /// \sa VisTypedEngineObject_cl::MessageFunction
  /// \sa VisGame_cl::SendMsg
  /// 
  /// \example
  ///   \code
  ///   const int GAME_MSG_TRIGGER = VIS_MSG_USER + 1;
  ///   SendMsg( pDestEntity, GAME_MSG_TRIGGER, data[1], data[2] );
  ///   \endcode
  VISION_APIFUNC void SendMsg(VisTypedEngineObject_cl *pDest, int iID, INT_PTR iParamA = 0, INT_PTR iParamB = 0);

  ///
  /// @}
  ///

  ///
  /// @name Scripting
  /// @{
  ///


  /// \brief
  ///   Triggers a script callback function with optional parameters and return value
  /// 
  /// \param pszEvent
  ///   Name of the event
  /// 
  /// \param pszArgFormat
  ///   Format of the arguments passed to this function. A script function can also return an
  ///   arbitrary number of parameters which have to be passed as references. See Example. Every
  ///   character in this string represents the format of one parameter. That is:
  ///   \li 'd' : double,
  ///   \li 'f' : float,
  ///   \li 'i' : integer (32bit),
  ///   \li 's' : string (char*),
  ///   \li 'm' : bitmask (32bit int used as a bitmask),
  ///   \li 'v' : vector3 (hkvVec3),
  ///   \li 'c' : color (VColorRef),
  ///   \li '#' : bounding box (hkvAlignedBBox),
  ///   \li 'o' : typed object (derived from VisTypedEngineObject).
  ///   \li More types might be supported for specific implementations. The '>' character separates
  ///     the input parameter list from the return values. Thus, a function that takes two floats
  ///     as input and returns an integer should pass "ff>i" and pass the parameters accordingly.
  ///     Furthermore, for member functions the 'this' pointer has to be passed always as the first
  ///     parameter. This is done by passing '*' as the first character in the szArgFormat string.
  ///     This advises the function to put a proxy object on the stack that stands for the owner
  ///     object. See second example.
  /// 
  /// \example
  ///   \code
  ///   pObj->TriggerScriptEvent("GetItemValue","s>f","SwordItem",&fSwordValue);
  ///   pObj->TriggerScriptEvent("OnThink","*"); // call the OnThink member function (no additional parameters)
  ///   \endcode
  VISION_APIFUNC virtual void TriggerScriptEvent(const char *pszEvent, const char *pszArgFormat, ...);
  
  /// \brief
  ///   Triggers a script callback function passing no arguments and returning no value.
  /// 
  /// \param pszEvent
  ///   Name of the event
  VISION_APIFUNC virtual void TriggerScriptEvent(const char *pszEvent)
  {
    TriggerScriptEvent(pszEvent,"*");
  }
  
  
  ///
  /// @}
  ///

  ///
  /// @name Components
  /// @{
  ///
  /// \brief
  ///   Overridable that is called by vForge to test whether the passed component can be added to this object
  /// 
  /// This default implementation simply calls pComponent->CanAttachToObject(this); thus forwarding the request to 
  /// the counterpart on the component's side
  ///
  /// \param pComponent
  ///     Component to test
  ///
  /// \param [out] sErrorMsgOut
  ///   In case of a failure (return value of false), the function is supposed to set the reason
  ///   for the failure in this string. vForge shows this error message in case of a failure.
  ///
  /// \returns
  ///   TRUE if the component can be added to the passed object, otherwise FALSE.
  ///
  VISION_APIFUNC virtual BOOL CanAddComponent(IVObjectComponent *pComponent, VString &sErrorMsgOut);


  /// \brief
  ///   Adds a single component to the collection of components (See Components())
  /// 
  /// This function is virtual and can be overwritten to add some initialization code to the game
  /// object (or specific component).
  /// 
  /// This base implementation should be called to add the component to the collection member.
  /// 
  /// In addition, this function calls the virtual SetOwner(this) function on the passed component.
  /// 
  /// \param pComponent
  ///   The component to add
  /// 
  /// \return
  ///   \c TRUE if successful
  VISION_APIFUNC virtual BOOL AddComponent(IVObjectComponent *pComponent);

  /// \brief
  ///   Removes a single component from the component list of this object
  /// 
  /// This function is virtual and can be overwritten to add some cleanup code to the game object
  /// (or specific component).
  /// 
  /// This base implementation should be called to remove the component from the collection member.
  /// 
  /// In addition, this function calls the virtual SetOwner(NULL) function on the passed component.
  /// 
  /// \param pComponent
  ///   The component to remove
  /// 
  /// \return
  ///   \c TRUE if successful
  VISION_APIFUNC virtual BOOL RemoveComponent(IVObjectComponent *pComponent);

  /// \brief
  ///   Non-virtual function to remove all components. Calls RemoveComponent on each entry.
  VISION_APIFUNC void RemoveAllComponents();

  /// \brief
  ///   Accesses the collection of components. It is const since Add/Remove operations should not
  ///   be called on it. Instead rather go through AddComponent/RemoveComponent
  inline const VObjectComponentCollection& Components() const
  {
    return m_Components;
  }


  ///
  /// @}
  ///


  ///
  /// @name Unique Id
  /// @{
  ///

  /// \brief
  ///   Returns the 64bit unique ID value of this object.
  /// 
  /// The default value of it is 0 (invalid ID), valid ID's have to be set manually via SetUniqueID
  /// at runtime.
  /// 
  /// vForge sets the shape ID here. Since the ID gets serialized, unique IDs are available for
  /// exported shapes.
  inline __int64 GetUniqueID() const
  {
    return m_iUniqueID64;
  }

  /// \brief
  ///   Sets the 64bit unique ID value of this object. 
  ///
  /// See GetUniqueID. This function adds and removes it to the global ID map if enabled 
  /// through SetUseUniqueIDMap().
  ///
  /// \param iNewVal64
  ///   the new unique ID to set
  VISION_APIFUNC void SetUniqueID(__int64 iNewVal64);
  
  /// \brief
  ///   This function enables or disables a global map that identifies all objects in the scene by their unique ID
  ///
  /// Rather than the VisElementManager_cl::FindByUniqueID function, which is class specific and which uses a naive 
  /// search, this function holds all object instances that are derived from VisTypedEngineObject_cl - also shape 
  /// instances of classes from external plugins. Furthermore this function uses optimized search through a hash map.
  ///
  /// By default this feature is disabled, so calling the static GetObjectByUniqueID function will always return 
  /// \c NULL. However, if the application heavily relies on fast lookups of shapes by their unique IDs, then this 
  /// feature should be enabled. This function should be called before the first scene is loaded.
  ///
  /// The unique ID of objects is automatically assigned in vForge and exported to vscene/vzone file.
  ///
  /// \param bStatus
  ///   If true, all object references are stored in a global ID map so they can be queried via GetObjectByUniqueID. 
  ///   Passing false will destroy this table if it was allocated.
  VISION_APIFUNC static void SetUseUniqueIDMap(bool bStatus);

  /// \brief
  ///   Finds an object in the scene by its unique ID
  ///
  /// The ID map must be globally enabled through the static VisTypedEngineObject_cl::SetUseUniqueIDMap(true) function, 
  /// otherwise this function always returns NULL.
  ///
  /// The unique ID of objects is automatically assigned in vForge and exported to vscene/vzone file. This function 
  /// thus only finds objects in loaded scenes or zones, because the object instance does not exist otherwise. Object 
  /// instances that are created through code during runtime do have a unique ID of 0 by default.
  ///
  /// These objects are not stored in the map, because 0 is not unique.
  ///
  /// \param iID
  ///   The unique ID to find (should be !=0)
  ///
  /// \return
  ///   The object instance with the specified unique ID, or NULL if not found
  VISION_APIFUNC static VisTypedEngineObject_cl* GetObjectByUniqueID(__int64 iID);

  /// \brief
  ///   Useful static helper function to write the passed instance to archive
  ///
  /// This function handles three cases:
  ///   \li it writes a NULL tag if NULL is passed as pObject
  ///   \li if the passed object has the VObjectFlag_InsideSerializationSession flag it is assumed to be part 
  ///       of the current serialization session so it is written directly (using ar.WriteObject)
  ///   \li if the VObjectFlag_InsideSerializationSession flag is not set, the unique ID of the object is 
  ///       written so it can be looked up in the existing scene at loading time
  ///
  /// \param ar
  ///   The archive to write to
  ///
  /// \param pObject
  ///   The object reference to write to the archive. Can be NULL
  ///
  /// \param pForceClass
  ///   See VArchive::WriteObject
  ///
  /// \note
  ///   References saved this way are not compatible with pointers written directly through VArchive::WriteObject
  ///
  VISION_APIFUNC static void SerializeObjectReference(VArchive &ar, VisTypedEngineObject_cl *pObject, const VType *pForceClass=NULL);

  /// \brief
  ///   Static helper function that reads an object from archive that has been written with SerializeObjectReference
  ///
  /// This function reads an object reference from the archive and resolves the pointer. If the object was not part of the scene at saving time
  /// (VObjectFlag_InsideSerializationSession flag was not set) then this function reads the unique ID from file and looks up the object in the
  /// scene by its unique ID. This function requires that the application enables VisTypedEngineObject_cl::SetUseUniqueIDMap globally at
  /// startup time, otherwise this function asserts. If the unique ID cannot be found this function returns NULL without assertion.
  ///
  /// \param ar
  ///   The archive to read from
  ///
  /// \param pClassRefRequested
  ///   Archive::ReadObject
  ///
  /// \returns
  ///   The resolved object pointer
  VISION_APIFUNC static VisTypedEngineObject_cl *DeSerializeObjectReference(VArchive &ar, const VType* pClassRefRequested=NULL);

  ///
  /// @}
  ///

  ///
  /// @name Zone
  /// @{
  ///

  /// \brief
  ///   Returns the current zone that this instance is attached to. Can be NULL.
  inline VisZoneResource_cl *GetParentZone() const {return m_pParentZone;}

  /// \brief
  ///   Overridable that is called to attach this instance to a new zone
  /// 
  /// This default implementation calls OnRemoveEngineObject on the old zone and OnAddEngineObject
  /// on the new zone.
  ///
  /// \param pNewZone
  ///   The new parent zone for this instance.
  VISION_APIFUNC virtual void SetParentZone(VisZoneResource_cl *pNewZone);

  /// \brief
  ///   Internal function to just set the pointer to new zone
  VISION_APIFUNC void SetParentZoneInternal(VisZoneResource_cl *pNewZone);

  /// \brief
  ///   Internal function to get the index of the object inside the parent zone.
  inline int GetParentZoneIndex() const 
  {
    return m_iParentZoneIndex;
  }

  /// \brief
  ///   Internal function to set the index of the object inside the parent zone.
  inline void SetParentZoneIndex(int iParentZoneIndex) 
  { 
    m_iParentZoneIndex = iParentZoneIndex;
  }

  /// \brief
  ///   Returns the lightgrid instance that is relevant for lighting this object.
  /// 
  /// This might be the zone's own lightgrid (if it has one) or the global lightgrid.
  VISION_APIFUNC VLightGrid_cl *GetRelevantLightGrid() const;

  /// \brief
  ///   This overridable is called by the scene manager in case of re-positioning for every object that is attached to a zone.
  ///
  /// Repositioning occurs when the camera leaves a certain radius around the origin. The re-positioning mechanism is optional and disabled
  /// by deafult. See IVisSceneManager_cl::SetRepositionInterval.
  ///
  /// \param info
  ///   Information structure that holds information about the new global position pivot. See VisZoneRepositionInfo_t
  ///
  /// \param vLocalPos
  ///   The original local position of this object, relative to the parent zone's pivot
  VISION_APIFUNC virtual void OnReposition(const VisZoneRepositionInfo_t &info, const hkvVec3 &vLocalPos)
  {
  }

  /// \brief
  ///   Returns this object's offset within its parent zone.
  VISION_APIFUNC virtual bool GetZoneLocalSpacePosition(hkvVec3& vDest);

  ///
  /// @}
  ///

  ///
  /// @name Network Related
  /// @{
  ///

  /// \brief
  ///   This function is called by a network view component (IVNetworkViewComponent) to retrieve all synchronization groups of an object.
  ///
  /// Custom classes (e.g. entity classes) can override this function and return specific synchronization groups to synchronize game play states
  /// or other object specific properties.
  ///
  /// \param context
  ///   Structure that provides hints about the cause for serialization. See VNetworkViewContext::VNetworkViewType_e for instance
  ///
  /// \param pDestList
  ///   The list that can be filled by this function. tHe respective instance pointer in the pair should be set to \c this
  ///
  /// \return
  ///   The number of elements written to the list
  ///
  /// \example
  ///   \code
  ///   // sample implementation
  ///   int iCount = VisObject3D_cl::GetSynchronizationGroupList(context, pDestList); // call base impl.
  ///   pDestList[iCount++].Set(this, g_MySyncGroup);
  ///
  ///   return iCount;
  ///   \endcode
  VISION_APIFUNC virtual int GetSynchronizationGroupList(const VNetworkViewContext &context, VNetworkSynchronizationGroupInstanceInfo_t *pDestList);

  /// \brief
  ///   Overridable that is called by the network manager to determine whether this object wants network synchronization in this simulation tick
  VISION_APIFUNC virtual BOOL WantsSynchronization(VNetworkViewContext &context)
  {
    return TRUE;
  }

  ///
  /// @}
  ///

  ///
  /// @name Editor-only functions
  /// @{
  ///

#ifdef WIN32

  /// \brief
  ///   Overridable to provide an instance with instance specific variables. Only called by vForge to retrieve additional component properties.
  VISION_APIFUNC virtual int GetCustomVariableInfo(DynObjArray_cl<VisVariable_cl> &customVarList);

#endif // WIN32

  ///
  /// @}
  ///

protected:
  __int64 m_iUniqueID64;                      ///< unique ID, usually exported from scene file
  VObjectComponentCollection m_Components;    ///< list of object components
  VisZoneResource_cl *m_pParentZone;          ///< parent zone where the object belongs to
  int m_iParentZoneIndex;                     ///< object index inside the parent zone object list

  friend class VisGame_cl;
  static VMapUniqueIDToPtr *g_pUniqueIDMap;
};



#include <Vision/Runtime/Engine/SceneElements/VisApiObjectComponent.hpp>

inline IVObjectComponent* VObjectComponentCollection::GetComponentOfType(VType *pType) const
{
  // try cached entry:
  if (pType==m_pLastAccessedType)
    return m_pLastAccessedComp;
    
  const int iCount = Count();
  IVObjectComponent **ppComp = ((VObjectComponentCollection *)this)->GetPtrs();
  for (int i=0;i<iCount;i++)
    if (ppComp[i]->GetTypeId()==pType)
    {
      m_pLastAccessedType = pType;
      m_pLastAccessedComp = ppComp[i]; 
      return m_pLastAccessedComp;
    }
  return NULL;
}

inline IVObjectComponent* VObjectComponentCollection::GetComponentOfTypeAndName(VType *pType, const char *szComponentName) const
{
  const int iCount = Count();
  IVObjectComponent **ppComp = ((VObjectComponentCollection *)this)->GetPtrs();
  for (int i=0;i<iCount;i++)
  {
    if (ppComp[i]->GetTypeId()==pType && VStringHelper::SafeCompare(ppComp[i]->GetComponentName(), szComponentName, false)==0)
      return ppComp[i];
  }
  
  return NULL;
}

// inlined implementation for faster access
inline IVObjectComponent* VObjectComponentCollection::GetComponentOfBaseType(VType *pBaseType) const
{
  // try cached entry (also with exact type match since this is very likely)
  if (pBaseType==m_pLastAccessedType)
    return m_pLastAccessedComp;

  const int iCount = Count();
  IVObjectComponent **ppComp = ((VObjectComponentCollection *)this)->GetPtrs();
  for (int i=0;i<iCount;i++)
    if (ppComp[i]->IsOfType(pBaseType))
    {
      m_pLastAccessedComp = ppComp[i]; 
      m_pLastAccessedType = m_pLastAccessedComp->GetTypeId();
      return m_pLastAccessedComp;
    }
  return NULL;
}

inline IVObjectComponent* VObjectComponentCollection::GetComponentOfBaseTypeAndName(VType *pBaseType, const char *szComponentName) const
{
  const int iCount = Count();
  IVObjectComponent **ppComp = ((VObjectComponentCollection *)this)->GetPtrs();
  for (int i=0;i<iCount;i++)
  {
    if (ppComp[i]->IsOfType(pBaseType) && VStringHelper::SafeCompare(ppComp[i]->GetComponentName(), szComponentName, false)==0)
      return ppComp[i]; 
  }
  return NULL;
}


#endif  // FR_DEFINE_VISAPITYPEDENGINEOBJECT

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
