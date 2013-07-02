/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodSoundResource.hpp

#ifndef VFMODSOUNDRESOURCE_HPP_INCLUDED
#define VFMODSOUNDRESOURCE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>


/// \brief
///   Fmod sound-resource manager class
///
/// This manager manages simply sound resources (e.g. wave files)
class VFmodSoundResourceManager : public VisResourceManager_cl
{
public:
  VFmodSoundResourceManager() : VisResourceManager_cl("FmodSoundResource", VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0)
  {
  }

  FMOD_IMPEXP VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);
};


///\brief
///  This class represents a sound resource, e.g. a wave file in memory with resource specific properties, 
///  such as 2D/3D flag.
///
///  From this resource, sound instances can be created.
///\see
///  VFmodSoundObject
class VFmodSoundResource : public VManagedResource
{
protected:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  FMOD_IMPEXP VFmodSoundResource(VFmodSoundResourceManager *pManager, int iUsageFlags);

public:
  FMOD_IMPEXP virtual ~VFmodSoundResource();

  ///
  /// @}
  ///


  ///
  /// @name Instances
  /// @{
  ///

  /// \brief
  ///   Create a sound instance from this resource
  FMOD_IMPEXP VFmodSoundObject* CreateInstance(const hkvVec3 &vPos, int iFlags=VFMOD_FLAG_DEFAULT, int iPriority=128); 
  
  ///
  /// @}
  ///


  ///
  /// @name Resource property functions
  /// @{
  ///

  ///\brief
  ///  Indicates whether this is a 2D sound resource
  inline bool Is2D() const { return (m_iSoundFlags&VFMOD_RESOURCEFLAG_3D)==0; }

  ///\brief
  ///  Indicates whether this is a positionable 3D sound resource
  inline bool Is3D() const { return (m_iSoundFlags&VFMOD_RESOURCEFLAG_3D)>0; }

  ///\brief
  ///  Indicates whether this is sound resource is flagged for streaming
  inline bool IsStreaming() const {return (m_iSoundFlags&VFMOD_RESOURCEFLAG_STREAM)>0; }

  ///\brief
  ///  Indicates whether this is sound resource data is loaded into memory in compressed state (MP3/OGG/XMA etc).
  inline bool IsCompressedData() const {return (m_iSoundFlags&VFMOD_RESOURCEFLAG_COMPRESSED_DATA)>0; }

  ///\brief
  ///  Indicates whether linear attenuation is used for this resource
  inline bool LinearAttenuation() const { return (m_iSoundFlags&VFMOD_RESOURCEFLAG_LOG_ATTENUATION)==0; }

  ///\brief
  ///  Indicates whether logarithmic attenuation is used for this resource
  inline bool LogAttenuation() const { return (m_iSoundFlags&VFMOD_RESOURCEFLAG_LOG_ATTENUATION)>0; }

  ///\brief
  ///  Indicates whether sound is opened and prepared in the background, or asynchronously.
  ///  This allows the main application to execute without stalling on audio loads.
  inline bool LoadsAsyncronous() const { return (m_iSoundFlags&VFMOD_RESOURCEFLAG_ASYNCRONOUS)>0; }

  ///\brief
  ///  Indicates whether sound is loaded completely
  FMOD_IMPEXP bool IsReady() const;

  ///\brief
  ///  Returns all resource bitflags
  inline int GetResourceFlags() const { return m_iSoundFlags; }

  /// \brief
  ///   Gets the internal Fmod sound instance
  inline FMOD::Sound* GetFmodSoundInstance() const { return m_pSound; }

  /// \brief
  ///   Returns whether the sound resource has a valid Fmod sound instance
  inline bool IsValid() const { return m_pSound!=NULL; }

  ///
  /// @}
  ///


  ///
  /// @name Overridden functions
  /// @{
  ///

#ifdef SUPPORTS_SNAPSHOT_CREATION
  FMOD_IMPEXP VOVERRIDE void GetDependencies(VResourceSnapshot &snapshot);
#endif

  FMOD_IMPEXP VOVERRIDE BOOL Reload();

  FMOD_IMPEXP VOVERRIDE BOOL Unload();

  FMOD_IMPEXP VOVERRIDE int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars);

  inline VOVERRIDE void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
  {
    iUniqueSys = m_iSysMem;
  }

  ///
  /// @}
  ///
 
private:  
  friend class VFmodManager;     
  friend class VFmodSoundObject;

  int m_iSoundFlags,m_iSysMem;
  FMOD::Sound *m_pSound;

};

#endif // VFMODSOUNDRESOURCE_HPP_INCLUDED

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
