/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvProfileManager.hpp

#ifndef VPROFILEMANAGER_HPP_INCLUDED
#define VPROFILEMANAGER_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCallback.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Profiles/hkvProfile.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Profiles/hkvTransformationRuleTemplate.hpp>
#include <vector>
#include <Vision/Runtime/Base/System/IO/System/FileLock.hpp>

class hkvProfileManager;


/// \brief
///   Data object for the OnProfileAdded callback in the hkvProfileManager.
class hkvProfileAddedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvProfileAddedData(const hkvCallback* sender) : hkvCallbackData(sender)
  {
    m_profileIndex = HKV_INVALID_INDEX;
  }

  hkUint32 m_profileIndex;
  hkvTargetPlatform m_platform;
  hkStringPtr m_profileName;
  hkStringPtr m_copySettingsFromProfileName;
};


/// \brief
///   Data object for the OnProfileDeleted callback in the hkvProfileManager.
class hkvProfileDeletedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvProfileDeletedData(const hkvCallback* sender) : hkvCallbackData(sender)
  {
    m_profileIndex = HKV_INVALID_INDEX;
  }

  hkUint32 m_profileIndex;
  hkvTargetPlatform m_platform;
  hkStringPtr m_profileName;
};


/// \brief
///   Data object for the OnActiveProfileChanged callback in the hkvProfileManager.
class hkvActiveProfileChangedData : public hkvCallbackData
{
public:
  ASSETFRAMEWORK_IMPEXP hkvActiveProfileChangedData(const hkvCallback* sender) : hkvCallbackData(sender)
  {
    m_profileIndex = HKV_INVALID_INDEX;
  }

  hkUint32 m_profileIndex;
  hkStringPtr m_profileName;
  hkBool m_assetPreviewMode;

  hkBool m_profileChanged;
  hkBool m_assetPreviewModeChanged;
};


/// \brief
///   This manager holds all profiles and templates loaded from the profile manifest file.
class hkvProfileManager : public hkvPropertyReader
{
public: // static functions
  ASSETFRAMEWORK_IMPEXP static void initProfileManager();
  ASSETFRAMEWORK_IMPEXP static void deInitProfileManager();
  ASSETFRAMEWORK_IMPEXP static hkvProfileManager* getGlobalInstance();

  ASSETFRAMEWORK_IMPEXP static bool isTargetPlatformSupported(VTargetDevice_e platform);

public: // public functions
  ASSETFRAMEWORK_IMPEXP hkvProfileManager();
  ASSETFRAMEWORK_IMPEXP ~hkvProfileManager();
  ASSETFRAMEWORK_IMPEXP void clearProfiles(bool emitCallbacks = true);

  ASSETFRAMEWORK_IMPEXP bool loadProfileManifest(const char* projectDir);
  ASSETFRAMEWORK_IMPEXP bool saveProfileManifest(const char* fileName = NULL);
  ASSETFRAMEWORK_IMPEXP bool isProfileManifestLoaded() const;
  ASSETFRAMEWORK_IMPEXP const char* getProfileManifestFile() const {return m_manifestFile.cString();};
  ASSETFRAMEWORK_IMPEXP inline bool isDirty() const {return m_dirty;}
  ASSETFRAMEWORK_IMPEXP void setDirty();
  
  // profile functions
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumberOfProfiles() const;
  ASSETFRAMEWORK_IMPEXP hkvProfile::RefCPtr getProfileByIndex(hkUint32 index) const;
  ASSETFRAMEWORK_IMPEXP hkvProfile::RefCPtr getProfileByName(const char* name) const;
  ASSETFRAMEWORK_IMPEXP const std::vector<hkvProfile::RefPtr>& getProfiles() const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getProfileIndexByName(const char* name) const;

  ASSETFRAMEWORK_IMPEXP hkUint32 addProfile(const char* name, hkvTargetPlatform platform, hkUint32 copySettingsFromProfileIndex = HKV_INVALID_INDEX);
  ASSETFRAMEWORK_IMPEXP hkBool deleteProfile(hkUint32 index);

  ASSETFRAMEWORK_IMPEXP hkUint32 getActiveProfileIndex() const;
  ASSETFRAMEWORK_IMPEXP hkvProfile::RefCPtr getActiveProfile() const;
  ASSETFRAMEWORK_IMPEXP hkBool setActiveProfile(hkUint32 index);
  ASSETFRAMEWORK_IMPEXP hkBool isAssetPreviewModeEnabled() const;
  ASSETFRAMEWORK_IMPEXP void setAssetPreviewMode(bool enableAssetPreviewMode);

  // RCS
  ASSETFRAMEWORK_IMPEXP bool getLockStat(VRCSFileStatus& out_status) const;
  ASSETFRAMEWORK_IMPEXP bool isUpToDate() const;
  ASSETFRAMEWORK_IMPEXP bool getLock();
  ASSETFRAMEWORK_IMPEXP bool releaseLock();
  ASSETFRAMEWORK_IMPEXP bool ownsLock() const;

  ASSETFRAMEWORK_IMPEXP const char* getEditorProfileName() const;

  // transform rule template functions
  ASSETFRAMEWORK_IMPEXP hkUint32 getNumberOfTransformRuleTemplates() const;
  ASSETFRAMEWORK_IMPEXP const hkvTransformRuleTemplate* getTransformRuleTemplateByIndex(hkUint32 index) const;
  ASSETFRAMEWORK_IMPEXP hkUint32 getTransformRuleTemplateIndexByName(const char* name) const;
  ASSETFRAMEWORK_IMPEXP hkUint32 addTransformRuleTemplate(const char* name);
  ASSETFRAMEWORK_IMPEXP hkUint32 duplicateTransformRuleTemplate (hkUint32 index, const char* name);
  ASSETFRAMEWORK_IMPEXP hkBool deleteTransformRuleTemplate (hkUint32 index);
  ASSETFRAMEWORK_IMPEXP const char* getTransformRuleTemplateMapping() const;

  virtual hkvJsonStreamReadHandler::ParsingResult ParseMapKey(const unsigned char* value, size_t len);

private:
  ASSETFRAMEWORK_IMPEXP void callOnActiveProfileChanged(bool profileChanged, bool assetPreviewModeChanged) const;
  ASSETFRAMEWORK_IMPEXP void callOnProfileAdded(hkUint32 addedProfileIndex, hkUint32 copySettingsFromProfileIndex) const;

public: // Data
  hkvCallback OnActiveProfileChanged;
  hkvCallback OnProfileAdded;
  hkvCallback OnProfileDeleted;
  hkvCallback OnProfilesLoaded;
  hkvCallback OnProfilesUnloaded;
  hkvCallback OnPropertiesChanged;

private:
  void updateTransformRuleTemplateMapping();
  void updateLockTimeStamp();
  hkvProfileManager(const hkvProfileManager& profile);
  void operator=(const hkvProfileManager& rhs);

private:
  hkStringPtr m_manifestFile;
  hkStringPtr m_transformRuleTemplateMapping;
  std::vector<hkvProfile::RefPtr> m_profiles;
  std::vector<hkvTransformRuleTemplate*> m_transformRuleTemplates;
  
  hkUint32 m_activeProfile;
  hkBool m_assetPreviewMode;
  hkBool m_ownsLock;
  hkBool m_dirty;

  hkStringPtr m_editorProfileName;

  // Locking
  VFileLock m_Lock;
  VFileTime m_FileTime;


public:
  ASSETFRAMEWORK_IMPEXP static const char* const s_manifestFileName;
  ASSETFRAMEWORK_IMPEXP static const char* const s_customTemplateName;
  ASSETFRAMEWORK_IMPEXP static const char* const s_defaultTemplateName;

private:
  static hkvProfileManager* s_globalInstance;
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
