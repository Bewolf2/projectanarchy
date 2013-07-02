/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once



/// \brief
///   Base class for instanceable objects/entities
class VGEOM2_IMPEXP_CLASS VGInstantiable
{
public:
  VGInstantiable()                { }
  virtual ~VGInstantiable() = 0   { }



  /// \brief
  ///   Copies essential data to passed-in instance, e.g. initial name and transformation, etc.
  ///
  /// \param inst
  ///   Instance to copy data to
  virtual void            CopyEssentialsToInstance(class VGInstance& inst) const = 0;

  /// \brief
  ///   Copies essential data from passed-in instance, e.g. initial name and transformation, etc.
  ///
  /// \param inst
  ///   Instance to copy data from
  virtual void            CopyEssentialsFromInstance(const class VGInstance& inst) = 0;



  /// \brief
  ///   Gets some brief information about the type of this instantiable
  ///
  /// \return
  ///   String with name of instantiable type (see derived classes for details)
  virtual const char*     GetTypeString() const throw() = 0;



  /// \brief
  ///   Gets the object's (optional) path name
  ///
  /// See VGInstantiable::SetPathName function for explanation.
  ///
  /// \return
  ///   Currently set path name
  inline const VString&   GetPathName() const throw()       { return m_pathName; }

  /// \brief
  ///   Sets the object's (optional) path name
  ///
  /// Instantiable objects (like meshes) might be written out to a file on their own. In order to
  /// setup scenes in a way so that instances pointing to instantiables can be used or written to
  /// a file (e.g. prefabs) in a flexible way, it is useful to have a way to specify what exactly
  /// the instance is referring to in a project or scene on disk.
  /// Consider this being nothing else than some optional user-data string, however, some export
  /// processors might use this data by convention (as is the case with e.g. Vision prefab writing
  /// using VGProcessor_VisionExporter).
  ///
  /// \param s
  ///   Path name to set
  inline void             SetPathName(const VString& s)     { m_pathName = s; }



protected:
  VString m_pathName;
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
