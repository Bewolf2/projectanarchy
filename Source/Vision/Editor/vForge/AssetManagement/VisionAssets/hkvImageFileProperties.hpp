/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvImageFileProperties.hpp

#ifndef HKV_IMAGE_INFO_HPP_INCLUDED
#define HKV_IMAGE_INFO_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBase.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvIProperties.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvImageFile.hpp>

class hkStreamReader;

class hkvImageFileProperties : public hkvIProperties
{
public:
  hkvImageFileProperties();
  hkvImageFileProperties(const hkvImageFileProperties& rhs);
  hkvImageFileProperties& operator=(const hkvImageFileProperties& rhs);

public:
  // IProperties implementation
  VOVERRIDE const char* getTypeName() const HKV_OVERRIDE;
  VOVERRIDE void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;


public:
  void clear();
  hkResult examineFile(const char* fileName);
  hkResult examineStream(hkStreamReader& stream, hkvImageFileFormat format);

  hkvImageFileFormat getImageFileFormat() const;
  hkUint32 getWidth() const;
  hkUint32 getHeight() const;
  hkBool hasAlpha() const;

private:
  hkResult examineBmp(hkStreamReader& reader);
  hkResult examineDds(hkStreamReader& reader);
  hkResult examineJpg(hkStreamReader& reader);
  hkResult examinePng(hkStreamReader& reader);
  hkResult examineTga(hkStreamReader& reader);


private:
  hkvEnumInstance m_imageFormatInstance;
  hkUint32 m_width;
  hkUint32 m_height;
  hkBool m_hasAlpha;
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
