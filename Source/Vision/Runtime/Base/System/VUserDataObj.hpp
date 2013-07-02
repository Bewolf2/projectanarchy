/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VUserDataObj.hpp

#ifndef VUSERDATAOBJ_HPP_INCLUDED
#define VUSERDATAOBJ_HPP_INCLUDED


/// \brief
///   Simple inlined class that provides GetUserData and SetUserData function for attaching a user
///   data pointer.
class VUserDataObj
{
public:
  /// \brief
  ///   Constructor; initializes the user data pointer to \c NULL.
  inline VUserDataObj() {m_pUserData=NULL;}

  /// \brief
  ///   Destructor.
  /// \note
  ///   The destructor does \b not free the user data object!
  inline virtual ~VUserDataObj(){;}

  /// \brief
  ///   Returns the custom user data pointer
  /// 
  ///  The custom user data pointer can be used by the game programmer to attach data to an engine
  /// object. The engine initialises this pointer with NULL in the constructor of the object, it
  /// does neither use the pointer during the game nor free it when the object gets deleted.
  /// 
  /// \return
  ///   void *pUserData : pointer to the custom user defined data
  inline void *GetUserData() const {return m_pUserData;}

  /// \brief
  ///   Sets the custom user data pointer
  /// 
  ///  The custom user data pointer can be used by the game programmer to attach data to an engine
  /// object. The constructor initializes this pointer with NULL. The user data is neither used nor
  /// free'd by the engine.
  /// 
  /// User data pointers will also not be serialized. Instead, the pointer is initialized with NULL
  /// for any deserialized object, so the user is responsible for reassigning this pointer
  /// 
  /// \param pData
  ///   custom user defined data pointer
  inline void SetUserData(void *pData) {m_pUserData=pData;}

protected:
  void *m_pUserData;
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
