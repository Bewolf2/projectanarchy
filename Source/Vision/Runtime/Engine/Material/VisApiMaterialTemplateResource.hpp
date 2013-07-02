/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef MATERIALTEMPLATERESOURCE_HPP_INCLUDED
#define MATERIALTEMPLATERESOURCE_HPP_INCLUDED

class TiXmlElement;

/// \brief
///   This class stores the information for every profile present in the material template.
class VMaterialTemplate
{
public:
  VISION_APIFUNC VMaterialTemplate();
  VISION_APIFUNC ~VMaterialTemplate();

  void operator= (const VMaterialTemplate& rhs);

public:
  /// \brief
  ///   Storage struct for the VMaterialTemplate class.
  struct ProfileSettings
  {
    VString m_sEffectLibrary;
    VString m_sEffectName;
    VString m_sParamString;

    /// \brief
    ///   Writes the settings to the xml node.
    VISION_APIFUNC bool WriteToXML(TiXmlElement* pTemplate);

    /// \brief
    ///   Reads the settings from the xml node.
    VISION_APIFUNC bool ReadFromXML(TiXmlElement* pTemplate);
  };

public:
  /// \brief
  ///   Returns the stored ProfileSettings for the given profile name. If no entry exists, NULL is returned.
  VISION_APIFUNC const ProfileSettings* GetProfileSettings(const char* szProfileName);

  /// \brief
  ///   Sets the ProfileSettings for the given profile name. Setting pProfilSettings to NULL will remove the profile from the VMaterialTemplate.
  VISION_APIFUNC void SetProfileSettings(const char* szProfileName, const ProfileSettings* pProfilSettings);

  /// \brief
  ///   Returns the VDictionary of ProfileSettings.
  VISION_APIFUNC const VDictionary<ProfileSettings>& GetAllProfileSettings();

  /// \brief
  ///   Writes all ProfileSettings into an XML 'MaterialTemplates' node.
  VISION_APIFUNC bool WriteToXML(TiXmlElement* pMaterialTemplates);

  /// \brief
  ///   Reads all ProfileSettings from an XML 'MaterialTemplates' node.
  VISION_APIFUNC bool ReadFromXML(TiXmlElement* pMaterialTemplates);

  /// \brief
  ///   Writes the VMaterialTemplate contents to a *.vmtx file.
  VISION_APIFUNC bool WriteToFile(const char* szFileName);

  /// \brief
  ///   Reads the VMaterialTemplate contents from a *.vmtx file.
  VISION_APIFUNC bool ReadFromFile(const char* szFileName);

private:
  VDictionary<ProfileSettings> m_sProfileSettings;
};


class VMaterialTemplateResource;
typedef VSmartPtr<VMaterialTemplateResource> VMaterialTemplateResourcePtr;

/// \brief
///   This resource is used to VisSurface_cl to provide platform specific
///   shader assignments and settings.
class VMaterialTemplateResource : public VManagedResource
{
public:
  /// \brief
  ///   Constructor: takes the parent manager instance as argument.
  VMaterialTemplateResource(VisResourceManager_cl* pParentManager);

  /// \brief
  ///   Destructor.
  virtual ~VMaterialTemplateResource();

  /// \brief
  ///   Returns the stored material template, only available in vForge.
  VISION_APIFUNC VMaterialTemplate* GetMaterialTemplate();

  /// \brief
  ///   Overrides the stored material template with the passed argument, only available in vForge.
  VISION_APIFUNC bool SetMaterialTemplate(const VMaterialTemplate* pTemplate);

  /// \brief
  ///   Returns whether for the current asset profile the material template has a manual shader assignment (instead of an automatic one).
  VISION_APIFUNC bool HasManualAssignment() const;

  /// \brief
  ///   Updates the shader assignment of the given surface with the once defined in this material template for the current asset profile.
  VISION_APIFUNC void UpdateSurface(VisSurface_cl* pSurface) const;

protected:
  virtual BOOL Reload() HKV_OVERRIDE;
  virtual BOOL Unload() HKV_OVERRIDE;

private:
  void ApplyProfileSettings(const VMaterialTemplate::ProfileSettings* settings) const;

private:
  mutable VisEffectConfig_cl m_TemplateFX;
  mutable VStaticString<128> m_sLoadedForProfile;  
  mutable VMaterialTemplate* m_pMaterialTemplate;
};

#endif // MATERIALTEMPLATERESOURCE_HPP_INCLUDED

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
