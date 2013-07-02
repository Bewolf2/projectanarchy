/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFmodEventGroup.hpp

#ifndef VFMODEVENTGROUP_HPP_INCLUDED
#define VFMODEVENTGROUP_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodIncludes.hpp>


/// \brief
///   Fmod event-group manager class
///
/// This manager manages event groups, which had been created by the Fmod Designer tool. 
class VFmodEventGroupManager : public VisResourceManager_cl
{
public:
  VFmodEventGroupManager() : VisResourceManager_cl("FmodEventGroup", VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER, 0)
  {
  }

  FMOD_IMPEXP VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);
};


/// \brief
///   This class represents an event group within an event project, which had been exported by the Fmod Designer tool.
///
///  An event group can contain itself nested event groups and as leaf-nodes the actual events.
///  Please note: when an event group is created, for all events within this group wave data is loaded and 
///  event instances are allocated.
///
///  From this resource, event instances can be created.
///\see
///  VFmodEvent
class VFmodEventGroup : public VManagedResource
{
protected:

  ///
  /// @name Constructor / Destructor
  /// @{
  ///

  FMOD_IMPEXP VFmodEventGroup(VFmodEventGroupManager *pManager);

public:
  FMOD_IMPEXP virtual ~VFmodEventGroup();

  ///
  /// @}
  ///


  ///
  /// @name Instances
  /// @{
  ///

  /// \brief
  ///   Creates an event instance by name
  /// 
  /// \param szEventName
  ///   Name of the event, relative to the parent event-group
  ///
  /// \param vPos
  ///   Position of the event
  /// 
  /// \param iFlags
  ///   Event instance flags
  FMOD_IMPEXP VFmodEvent* CreateEvent(const char *szEventName, const hkvVec3 &vPos, int iFlags=VFMOD_FLAG_NONE); 

  ///
  /// @}
  ///


  ///
  /// @name Resource property functions
  /// @{
  ///

  /// \brief
  ///   Gets a property of the event group by name
  ///  
  /// \param szPropertyName
  ///   Name of the property to retrieve. This is the name that was specified in Fmod Designer. 
  ///
  /// \param pValue
  ///   Address of a variable to receive the event group property
  /// 
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetProperty(const char *szPropertyName, void *pValue) const;

  /// \brief
  ///   Gets a property of the event group by index
  ///  
  /// \param iPropertyIndex
  ///   Index of the property to retrieve 
  ///
  /// \param pValue
  ///   Address of a variable to receive the event group property
  /// 
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetProperty(int iPropertyIndex, void *pValue) const;

  /// \brief
  ///   Gets the index and name of the event group
  /// 
  /// \param pGroupIndex
  ///   Address of a variable to receive the event group index
  /// 
  /// \param pszGroupName
  ///   Address of a variable to receive the event group name
  /// 
  /// \return
  ///   TRUE on success, otherwise FALSE
  FMOD_IMPEXP bool GetInfo(int *pGroupIndex, char **pszGroupName) const;

  /// \brief
  ///   Indicates whether event group is loaded completely
  FMOD_IMPEXP bool IsReady() const;

  /// \brief
  ///   Gets the internal Fmod event group
  inline FMOD::EventGroup* GetFmodEventGroup() const {return m_pEventGroup;}

  /// \brief
  ///   Returns whether the event group has a valid Fmod event group
  inline bool IsValid() const { return m_pEventGroup!=NULL; }

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

  inline VOVERRIDE void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU)
  {
    iUniqueSys = m_iSysMem;
  }

  ///
  /// @}
  ///

private:  
  friend class VFmodManager;     
  friend class VFmodEvent;
  friend class VFmodEventCollection;

  int m_iSysMem;
  FMOD::EventGroup *m_pEventGroup;

  VString m_sEventProjectPath; ///< path of the parent working event-project (.fdp)
  VString m_sEventGroupName;   ///< name of the event-group relative to the parent event-project 

};

#endif // VFMODEVENTGROUP_HPP_INCLUDED

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
