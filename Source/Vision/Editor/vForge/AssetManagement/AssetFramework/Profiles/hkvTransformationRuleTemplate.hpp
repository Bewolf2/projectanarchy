/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTransformationRuleTemplate.hpp

#ifndef HKVTRANSFORMATIONRULETEMPLATE_HPP_INCLUDED
#define HKVTRANSFORMATIONRULETEMPLATE_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRule.hpp>
#include <Common/Base/Container/StringMap/hkStorageStringMap.h>

class hkvProfileManager;
class hkvTransformRuleTemplate;




class hkvTransformRuleTemplate : public hkvIProperties
{
  friend class hkvProfileManager;

public:
  class ProfileTemplate : public hkvIProperties
  {
    friend class hkvTransformRuleTemplate;
  public:
    ASSETFRAMEWORK_IMPEXP ProfileTemplate (const char* profileName, hkvTargetPlatform platform);
    ASSETFRAMEWORK_IMPEXP ~ProfileTemplate();
    ASSETFRAMEWORK_IMPEXP ProfileTemplate* clone() const;

    ASSETFRAMEWORK_IMPEXP hkvTransformationRule* getTransformRule(const char* assetType) const;

    ASSETFRAMEWORK_IMPEXP VOVERRIDE const char* getTypeName() const;
    ASSETFRAMEWORK_IMPEXP VOVERRIDE void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
    ASSETFRAMEWORK_IMPEXP VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

  private:
    ASSETFRAMEWORK_IMPEXP ProfileTemplate(const ProfileTemplate& profileTemplate);
    ASSETFRAMEWORK_IMPEXP void operator=(const ProfileTemplate& rhs);

  private:
    hkStringPtr m_profileName;
    hkvTargetPlatform m_platform;
    mutable hkStorageStringMap<hkvTransformationRule*> m_assetRules;
  };

public:
  ASSETFRAMEWORK_IMPEXP hkvTransformRuleTemplate(const char* name);
  ASSETFRAMEWORK_IMPEXP ~hkvTransformRuleTemplate();
  ASSETFRAMEWORK_IMPEXP hkvTransformRuleTemplate* clone(const char* cloneName) const;

  ASSETFRAMEWORK_IMPEXP const char* getName() const;
  ASSETFRAMEWORK_IMPEXP ProfileTemplate* getTransformRuleProfileTemplate(const char* profileName) const;
  ASSETFRAMEWORK_IMPEXP hkvTransformationRule* getTransformRule(const char* profileName, const char* assetType) const;

  ASSETFRAMEWORK_IMPEXP VOVERRIDE const char* getTypeName() const;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void getProperties(hkvPropertyList& properties, hkvProperty::Purpose purpose) const HKV_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP VOVERRIDE void setProperty(const hkvProperty& prop, const hkArray<hkStringPtr>& path, hkUint32 stackIndex, hkvProperty::Purpose purpose) HKV_OVERRIDE;

private:
  ASSETFRAMEWORK_IMPEXP hkvTransformRuleTemplate(const hkvTransformRuleTemplate& ruleTemplate);
  ASSETFRAMEWORK_IMPEXP void operator=(const hkvTransformRuleTemplate& rhs);

private:
  hkStringPtr m_name;
  mutable hkStorageStringMap<ProfileTemplate*> m_profileTemplates;
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
