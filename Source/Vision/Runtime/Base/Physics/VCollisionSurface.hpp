/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VCollisionSurface.hpp

#ifndef VBASE_VCOLLISIONSURFACE_HPP
#define VBASE_VCOLLISIONSURFACE_HPP


class VCollisionData;

// version number for the collision surface chunk in the collision data file
#define VCOLLISIONSURFACE_CHUNK_VERSION 1

/// \brief
///   A class describing a collision surface. It really only wraps the material name for the
///   surface. See VSimpleCollisionMesh32.
class VCollisionSurface
{
public:

  /// \brief
  ///   Default constructor
  VBASE_IMPEXP VCollisionSurface();
  
  /// \brief
  ///   Constructor taking the material name as input
  VBASE_IMPEXP VCollisionSurface(const char* pszMaterialName);
  
  /// \brief
  ///   Destructor
  VBASE_IMPEXP ~VCollisionSurface();

  /// \brief
  ///   Returns the material name for this surface.
  inline const char* GetMaterialName() const {return m_materialName;}
  
  /// \brief
  ///   Setsthe material name for this surface.
  inline void SetMaterialName(const char* materialName) {m_materialName = materialName;}


  // loading/saving callbacks ... these should not be called directly
  VBASE_IMPEXP void OnStartSaving(VCollisionData* pCollData);
  VBASE_IMPEXP void OnLoading(VCollisionData* pCollData);

private:

  VString m_materialName;
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
