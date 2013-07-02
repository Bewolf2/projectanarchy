/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once


/// \brief
///   Dummy class (for putting dummies in a scene, etc.)
class VGEOM2_IMPEXP_CLASS VGDummy : public VGNode, public VGInstantiable
{
public:
  VGDummy() : VGNode("")                                                          { }
  VGDummy(const VGDummy& other)                                                   { operator=(other); }
  virtual ~VGDummy()                                                              { }



  /// \brief
  ///   Copies essential data to passed-in instance, e.g. initial name and transformation, etc..
  ///
  /// \param inst
  ///   Instance to copy data to
  virtual void          CopyEssentialsToInstance(VGInstance& inst) const          { inst.SetName(GetName()); inst.SetTransformation(GetTransformation()); }

  /// \brief
  ///   Copies essential data from passed-in instance, e.g. initial name and transformation, etc.
  ///
  /// \param inst
  ///   Instance to copy data from
  virtual void          CopyEssentialsFromInstance(const VGInstance& inst)        { SetName(inst.GetName()); SetTransformation(inst.GetTransformation()); }



  /// \brief
  ///   Gets some brief information about the type of this instantiable
  ///
  /// \return
  ///   String with name of instantiable type
  virtual const char*   GetTypeString() const throw()                             { return "Dummy"; }



  /// \brief
  ///   Assignment operator
  ///
  /// \param rhs
  ///   Dummy to assign
  ///
  /// \return
  ///   Reference to this dummy
  inline VGDummy&       operator=(const VGDummy& rhs)                             { VGNode::operator=(rhs); VGInstantiable::operator=(rhs); return *this; }
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
