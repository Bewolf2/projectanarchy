/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTextureAsset.hpp

#ifndef VTEXTUREASSETTYPE_HPP_INCLUDED
#define VTEXTUREASSETTYPE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetDecl.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvImageFileProperties.hpp>

class hkvAssetTypeManager;

class hkvTextureAsset : public hkvAsset
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public: // static functions
  static void StaticInit();
  static void StaticDeInit();
  static hkvAsset* CreateAsset();

public: // public functions
  hkvTextureAsset();
  ~hkvTextureAsset();

public: // overrides
  VOVERRIDE unsigned int getTypeIndex() const HKV_OVERRIDE;
  VOVERRIDE const char* getTypeName() const HKV_OVERRIDE;

  VOVERRIDE void getSpecificProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  VOVERRIDE void setSpecificProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, unsigned int stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

protected:
  virtual void clearAssetSpecificData() const HKV_OVERRIDE;
  virtual hkvAssetOperationResult updateAssetSpecificData(hkStreamReader& fileData) const HKV_OVERRIDE;

private:
  static hkUint32 s_iAssetTypeIndex;
  static hkvEnumDefinition s_imageFormatsDefinition;
  static hkvEnumDefinition s_usageDefinition;

  mutable hkvImageFileProperties m_imageProperties;
  hkvEnumInstance m_usageInstance;
  hkBool m_sRgb;
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
