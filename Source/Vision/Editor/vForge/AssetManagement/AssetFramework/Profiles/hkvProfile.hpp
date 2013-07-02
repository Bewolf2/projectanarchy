/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvProfile.hpp

#ifndef HKV_PROFILE_HPP_INCLUDED
#define HKV_PROFILE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>

class hkvProfileManager;


/// \brief
///   This class defines a profile. Profiles are defined on an SDK level and are visible
///   in each project that resides within the same SDK.
///   A profile defines a platform, a name and default settings for the asset management.
///   Each profile is uniquely identified via a GUID within one SDK.
class hkvProfile : public hkReferencedObject, public hkvIProperties
{
  friend class hkvProfileManager;

public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  typedef hkRefNew<hkvProfile> RefNew;
  typedef hkRefPtr<hkvProfile> RefPtr;
  typedef hkRefPtr<const hkvProfile> RefCPtr;

public:
  ASSETFRAMEWORK_IMPEXP hkvProfile();
  ASSETFRAMEWORK_IMPEXP hkvProfile(const hkvProfile& profile);
  ASSETFRAMEWORK_IMPEXP hkvProfile(hkvTargetPlatform platform, const char* name);
  ASSETFRAMEWORK_IMPEXP hkvProfile(const char* name);

  ASSETFRAMEWORK_IMPEXP void operator=(const hkvProfile& rhs);

  ASSETFRAMEWORK_IMPEXP const char* getName () const;
  ASSETFRAMEWORK_IMPEXP hkvTargetPlatform getPlatform () const;

  ASSETFRAMEWORK_IMPEXP VOVERRIDE const char* getTypeName() const;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

private:
  hkvEnumInstance m_platformInstance;
  hkStringPtr m_name;
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
