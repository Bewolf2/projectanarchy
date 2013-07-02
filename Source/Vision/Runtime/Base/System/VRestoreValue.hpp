/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_RESTORE_VALUE_HPP
#define V_RESTORE_VALUE_HPP

/// \brief
///   Scope helper to restore a value at the end of a scope.
/// \tparam T
///   the type of the value to restore. Must be assignable.
template <typename T>
class VRestoreValue
{
public:
  /// \brief
  ///   Constructs the object and saves the current value
  /// \param value
  ///   the current value. A reference to and a copy of this value are
  ///   saved.
  VRestoreValue(T& value) :
    m_value(value), m_savedValue(value)
  {
  }
  
  /// \brief
  ///   Destructs this object, and assigns the saved copy to the reference
  ///   of the value.
  ~VRestoreValue()
  {
    m_value = m_savedValue;
  }

private:
  VRestoreValue(const VRestoreValue&);
  VRestoreValue& operator=(const VRestoreValue&);

private:
  T& m_value;
  T m_savedValue;
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
