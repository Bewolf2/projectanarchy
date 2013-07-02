/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once



/// \brief
///   Container for object/entity instances
///
/// This class stores a pointer to an instanceable object, effectively representing
/// an instance of the latter. Instances have their own name and transformation.
class VGEOM2_IMPEXP_CLASS VGInstance : public VGNode
{
public:
  VGInstance(const VGInstantiable& inst) : m_inst(inst)           { m_inst.CopyEssentialsToInstance(*this); }
  virtual ~VGInstance()                                           { }



  /// \brief
  ///   Gets the instanced object
  ///
  /// \return
  ///   Instanced object
  inline const VGInstantiable& GetInstantiable() const throw()    { return m_inst; }



protected:
  // Pointer to instanceable object.
  const VGInstantiable& m_inst;
};

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
