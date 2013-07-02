/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#if defined (__SNC__)
#pragma diag_push
#pragma diag_suppress=429
#endif

/// \brief
///   The data accessor for a SWIG generated data type.
class SwigTypeDataAccessor : public IVRSDUserDataAccessor
{
public:

  ///
  /// \brief Creates a DataAccessor for the specified SWIG data type.
  ///   
  /// \param [in] szSwigTypeInfo  The name of the class generated via SWIG.
  ///
  SwigTypeDataAccessor(const char * szSwigTypeInfo);

  ///
  /// \brief  Gets the members of a SWIG user data object. 
  ///
  /// \param [in] pUserDataPointer  The userdata object to get the members from. 
  /// \param [in] pEnvironment  The current lua_State.
  /// \param [out] dynArrMembers the member collection to fill. 
  /// \param [out] iMemberCount number of members this object has. 
  ///
  /// \return true if it succeeds, false if it fails. 
  ///
  virtual bool GetUserDataMembers(void* pUserDataPointer, void *pEnvironment, DynArray_cl<VRSDScriptSymbol>& dynArrMembers, unsigned int& iMemberCount);

  ///
  /// \brief  Updates a member value from the given string. (Used by the debugger to update variable values)
  ///
  /// \param [in] pUserDataPointer  The SWIG data object. 
  /// \param [in] pEnvironment  The current lua_State.
  /// \param [in] szMemberName The name of the member to update. 
  /// \param [in] szUpdateString The string containing the new value. 
  ///
  /// \return true if it succeeds, false if it fails (e.g. member doesn't exist..). 
  ///
  virtual bool UpdateMemberFromString(void* pUserDataPointer, void *pEnvironment, const char* szMemberName, const char* szUpdateString);

protected:

  ///
  /// \brief  Adds the dynamic properties of a SWIG type to the user data item. 
  ///
  /// \param [in] pUserDataPointer  The userdata object to get the dynamic properties from. 
  /// \param [in] L  The current lua_State.
  /// \param [out] dynArrMembers the member collection to fill. 
  /// \param [out] iMemberCount number of members this object has. 
  ///
  /// \return The number of added dynamic properties. 
  ///
  int GetMembersFromGlobal(void* pUserDataPointer, lua_State *L, DynArray_cl<VRSDScriptSymbol>& dynArrMembers, unsigned int& iMemberCount);

  ///
  /// \brief  Adds the methods and attributes (members) of the swig data type. 
  ///
  /// \param [in] pSwigLuaClass  The userdata object having the methods and attributes. 
  /// \param [in] L  The current lua_State.
  /// \param [out] dynArrMembers the member collection to fill. 
  /// \param [out] iMemberCount number of members this object has.
  ///
  void AddMembersAndAttributes(void *pSwigLuaClass, lua_State *L, DynArray_cl<VRSDScriptSymbol>& dynArrMembers, unsigned int& iMemberCount);
};

#if defined (__SNC__)
#pragma diag_pop
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
