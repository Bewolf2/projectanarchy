/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiObjectComponent.hpp

#ifndef VISAPIOBJECTCOMPONENT_HPP_INCLUDED
#define VISAPIOBJECTCOMPONENT_HPP_INCLUDED

/// \brief Object Component Flags
enum VObjectComponentFlags_e 
{
  VIS_OBJECTCOMPONENTFLAG_NONE                    = 0,          ///< no flag
  VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE             = V_BIT(0),   ///< don't serialize
  VIS_OBJECTCOMPONENTFLAG_STATICMEMBER            = V_BIT(1),   ///< 
  VIS_OBJECTCOMPONENTFLAG_SERIALIZEWHENRELEVANT   = V_BIT(2),   ///< serialize this component if it is relevant
  VIS_OBJECTCOMPONENTFLAG_EXPOSETOEDITOR          = V_BIT(3),   ///< expose this component to the editor
  VIS_OBJECTCOMPONENTFLAG_INTERNALBUSYSERIALIZING = V_BIT(16)   ///< serialization currently in progress
};

#include <Vision/Runtime/Engine/System/VisApiBase.hpp>

class VTypedObject;
class VisTypedEngineObject_cl;



/// \brief
///   Base class for an object component
/// 
/// Instances of object components are associated with game objects (e.g. entities) to extend their
/// feature set.
/// 
/// With components it is possible to dynamically extend the feature set of game objects.
/// 
/// The alternative would be to have separate base classes for each combination of features which
/// of  course leads to an explosion of the number of subclasses.
/// 
/// Components are typed objects so it is straight forward to determine whether specific game
/// objects have associated components of specific type.
/// 
/// The VisTypedEngineObject_cl class has a component list. See
/// VisTypedEngineObject_cl::Components() function which returns a VObjectComponentCollection
/// instance.
class IVObjectComponent : public VisTypedEngineObject_cl, public VRefCounter, public VUserDataObj
{
public:

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the component class
  /// 
  /// \param iID
  ///   Component ID that might be used to look up the component inside a collection. If needed,
  ///   this ID should be unique along the collection (not globally unique). For instance, it is
  ///   used to serialize components with the VIS_OBJECTCOMPONENTFLAG_STATICMEMBER flag. The
  ///   highest bit of this integer value is reserved for string IDs (see static IsStringID
  ///   function)
  /// 
  /// \param iComponentFlags
  ///   Component bitflags that define the behavior of the component: supported bitflags are:
  ///   \li VIS_OBJECTCOMPONENTFLAG_NONE : No flags,
  /// 
  ///   \li VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE : If specified, the component will not be
  ///     serialized along with the VObjectComponentCollection::SerializeX function. Thus the
  ///     programmer is responsible to re-create this component after loading the owner object from
  ///     binary archive
  /// 
  ///   \li VIS_OBJECTCOMPONENTFLAG_STATICMEMBER :  Specify this flag for static component members
  ///     of objects. Components flagged with this bit will not go through dynamic creation
  ///     (serialization) and also reference counting will be ignored
  ///
  ///   \li VIS_OBJECTCOMPONENTFLAG_EXPOSETOEDITOR : This flag is only relevant when an entity class adds components
  ///     in the constructor or InitFunction. In that case using this flag means that the component is visible in the vForge
  ///     component editor and its properties can be edited there. However, it cannot be removed in vForge, that's why these
  ///     types of components are displayed with gray list icon.
  VISION_APIFUNC IVObjectComponent(int iID=0, int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Alternative constructor of the component class using a component name rather than ID
  /// 
  /// \param szComponentName
  ///   Name of the component, uses RegisterStringID to convert into an integer ID
  /// 
  /// \param iComponentFlags
  ///   See other constructor
  VISION_APIFUNC IVObjectComponent(const char *szComponentName, int iComponentFlags);


  /// \brief
  ///   Destructor
  VISION_APIFUNC ~IVObjectComponent();

  ///
  /// @}
  ///

  ///
  /// @name Owner
  /// @{
  ///

  /// \brief
  ///   Virtual overridable that is called to associate this component with the owner object
  /// 
  /// This function can be overridden and used for cleanup purposes since it is called with NULL at
  /// owner destroy time.
  /// 
  /// This base implementation should always be called by derived classes since it sets the
  /// m_pOwner member.
  /// 
  /// \param pOwner
  ///   The owner that gets this component assigned.
  VISION_APIFUNC virtual void SetOwner(VisTypedEngineObject_cl *pOwner);

  /// \brief
  ///   Returns the current owner of this component
  inline VisTypedEngineObject_cl* GetOwner() const
  {
    return m_pOwner;
  }

  ///
  /// @}
  ///

  ///
  /// @name Component Id
  /// @{
  ///

  /// \brief
  ///   Returns the numerical component ID value. This ID might correspond to a string value (see
  ///   GetIDString)
  inline int GetComponentID() const {return m_iComponentID;}

  /// \brief
  ///   Returns the name of the component as passed in the constructor or set via SetComponentName.
  ///   Returns NULL for pure numerical IDs
  inline const char* GetComponentName() const {return GetIDString(m_iComponentID);}

  /// \brief
  ///   Sets the component ID. Note that for the most significant bit is reserverd for IDs that
  ///   represent a name string 
  inline void SetComponentID(int iValue) {VASSERT(IsValidID(iValue));m_iComponentID=iValue;}

  /// \brief
  ///   Registers a new component name via RegisterStringID and sets the ID. If the passed name is
  ///   NULL, the component ID is set to 0
  inline void SetComponentName(const char* szName) {m_iComponentID = RegisterStringID(szName);}

  ///
  /// @}
  ///

  ///
  /// @name Overridden Functions
  /// @{
  ///

  /// \brief
  ///   Overridden VRefCounter function that only deletes this object if it does not have the
  ///   VIS_OBJECTCOMPONENTFLAG_STATICMEMBER flag
  VISION_APIFUNC VOVERRIDE void DeleteThis();

  /// \brief
  ///   Overridden MessageFunction function that clears the functionality of
  ///   VisTypedEngineObject_cl::MessageFunction.
  ///
  /// This function is not intended to perform any operation, so it overrides the function in the base
  /// class with an empty implementation.
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
  /// \sa VisTypedEngineObject_cl::MessageFunction
  VOVERRIDE void MessageFunction(int iID, INT_PTR iParamA, INT_PTR iParamB) {}

  /// \brief
  ///   Overridable that is called to test whether this instance should be serialized with a
  ///   component collection
  /// 
  /// The default implementation tests the component flags for the
  /// VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE flag.
  VISION_APIFUNC virtual bool IsRelevantForSerialization() const 
  {
    return (m_iComponentFlags&VIS_OBJECTCOMPONENTFLAG_NOSERIALIZE)==0;
  }

  /// \brief
  ///   Overridable that is called to query whether this component may be attached to the passed object
  ///
  /// The result of this function determines whether components can be dragged and dropped onto the object in vForge.
  /// This base implementation checks whether a component of the same type is already attached and in that case addtionally
  /// checks for the VCOMPONENT_ALLOW_MULTIPLE flag on th object's vartable.
  ///
  /// \param pObject
  ///   Object to test.
  ///
  /// \param sErrorMsgOut
  ///   [out] In case of a failure (return value of false) the overrides are supposed to set the reason
  ///   for the failure in this string. vForge shows this error message in case of a failure.
  ///
  /// \returns
  ///   BOOL bCanAttach: TRUE if the component can be attached to the passed object, otherwise FALSE.
  VISION_APIFUNC virtual BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);

  /// \brief
  ///   Overridable that is called by vForge to retrieve a nice name for displaying the component in the list. This default implementation does not modify the passed reference so the nice name from the variable table macro is used.
  VISION_APIFUNC virtual void GetCustomDisplayName(VString &sDestName);

  ///
  /// @}
  ///

  ///
  /// @name Static Id Helpers
  /// @{
  ///

  /// \brief
  ///   Globally registers a new component name and returns the unique name ID (highest bit is
  ///   set).
  /// 
  /// If the same name (case insensitive) is already there, it returns that ID.
  /// Valid name IDs have the highest bit set (see IsStringID).
  /// This function returns 0 for a NULL string or empty string.
  ///
  /// \param szName
  ///   Name of the new component.
  ///
  /// \returns
  ///   int iID: Unique name ID.
  VISION_APIFUNC static int RegisterStringID(const char *szName);

  /// \brief
  ///   Finds existing ID without registering the name. Returns 0 if not found
  ///
  /// \param szName
  ///   The string (component name) to look up.
  ///
  /// \returns
  ///   int iID: The ID, or 0 if not found.
  VISION_APIFUNC static int LookupStringID(const char *szName);

  /// \brief
  ///   Performs a sanity check on the passed ID, i.e. tests for valid range for string IDs
  ///
  /// \param iID
  ///   The ID to validate.
  ///
  /// \returns
  ///   BOOL bIsValid: TRUE if ID is in valid range, otherwise FALSE.
  VISION_APIFUNC static BOOL IsValidID(int iID);

  /// \brief
  ///   Indicates whether the passed ID represents a string ID. Basically just tests for the
  ///   highest bit.
  ///
  /// \param iID
  ///   The ID to check.
  ///
  /// \returns
  ///   BOOL bIsString: TRUE if ID represents a valid string ID, otherwise FALSE.
  VISION_APIFUNC static BOOL IsStringID(int iID);

  /// \brief
  ///   Returns the string that corresponds to the passed string ID. If the passed ID is not a
  ///   string ID (IsStringID returns FALSE), this function returns NULL
  ///
  /// \param iID
  ///   The ID to look up.
  ///
  /// \returns
  ///   const char* szString: The corresponding string.
  ///   
  VISION_APIFUNC static const char* GetIDString(int iID);

  /// \brief
  ///   Helper function to serialize an ID to or from archive. Handles string vs. user ID
  ///   gracefully.
  ///
  /// \param ar
  ///   The archive to serialize to/from.
  ///
  /// \param iID
  ///   The ID to serialize.
  VISION_APIFUNC static int SerializeComponentID(VArchive &ar, int iID);

#ifndef _VISION_DOC

  //serialization
  V_DECLARE_SERIAL_DLLEXP( IVObjectComponent,  VISION_APIDATA )
  VISION_APIFUNC VOVERRIDE void Serialize( VArchive &ar );
  V_DECLARE_VARTABLE(IVObjectComponent, VISION_APIFUNC)

  VisTypedEngineObject_cl *m_pOwner;
  int m_iComponentFlags, m_iComponentID;

  static VStrList g_IDList;

#endif

  ///
  /// @}
  ///
};


#endif  // VISAPIOBJECTCOMPONENT_HPP_INCLUDED

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
