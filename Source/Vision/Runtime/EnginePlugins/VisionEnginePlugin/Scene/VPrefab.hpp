/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VPrefab.hpp

#ifndef VPREFAB_HPP_INCLUDED
#define VPREFAB_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scene/VSceneLoader.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiBaseEntity.hpp>

/// \brief
///   Structure that is passed to the VPrefab at instantiation time. It contains information about the positioning and it optionally outputs the resulting shapes
struct VPrefabInstanceInfo
{
  VPrefabInstanceInfo() : m_Instances(0,NULL)
  {
    m_iInstanceCount = 0;
    m_bOutputInstances = false;
    m_pParentObject = NULL;
  }

  /// \brief
  ///   Helper function to remove objects from the scene again. Only makes sense if m_bOutputInstances was 'true' at instantiation time
  inline void DisposeObjects()
  {
    const int iCount = hkvMath::Min(m_iInstanceCount, (int)m_Instances.GetSize()); // this handles m_bOutputInstances==false gracefully
    // delete in reverse order. This ensures children get deleted first which avoids deletion issues
    for (int i=iCount-1;i>=0;i--)
      V_SAFE_DISPOSEOBJECT(m_Instances.GetDataPtr()[i]);
  }

  bool m_bOutputInstances;                  ///< if true, instantiated shapes are written to the m_Instances array
  int m_iInstanceCount;                     ///< Receives the number of instances. Actually the same as VPrefab::m_Header.m_iRootObjectCount
  DynArray_cl<VTypedObject *> m_Instances;  ///< array that receives the created instances if the m_bOutputInstances flag is set. The number of elements in the array is set in the m_iInstanceCount member
  VisObject3D_cl *m_pParentObject;          ///< If this pointer is specified, then instances of type VisObject3D_cl will be attached to this object. That way it is possible to move the prefab instances through one object. For this feature it is not mandatory that m_bOutputInstances is set
  hkvVec3 m_vInstancePos;              ///< defines the world space position where the instances are created. This absolute position is also used when a m_pParentObject object is specified
  hkvVec3 m_vInstanceEuler;              ///< defines the orientation of the shapes in the prefab (xyz = Euler angles in degrees)
};

#define VPREFAB_BINARY_VERSION_0        0
#define VPREFAB_BINARY_VERSION_CURRENT  VPREFAB_BINARY_VERSION_0


/// \brief
///   Represents a prefab type (vprefab binary file). Once loaded into memory, it can be instantiated arbitrarily
class VPrefab : public VManagedResource
{
public:
  SCENE_IMPEXP VPrefab(VResourceManager *pParentManager);

  SCENE_IMPEXP virtual ~VPrefab()
  {
  }

  SCENE_IMPEXP virtual BOOL Unload();
  SCENE_IMPEXP virtual BOOL Reload();

  /// \brief
  ///   Key function of this class: Instantiate this prefab using the transformation that is set in the passed structure
  SCENE_IMPEXP BOOL Instantiate(VPrefabInstanceInfo &info);

  /// \brief
  ///   Header information of the prefab
  struct VPrefabHeader
  {
    inline VPrefabHeader()
    {
      memset(this, 0, sizeof(VPrefabHeader));
    }

    int m_iArchiveVersion;  ///< VArchive version used to save the shapes
    int m_iLocalVersion;    ///< Version of the prefab file
    int m_iObjectCount;     ///< Overall number of objects in this archive (not relevant)
    int m_iNonNullCount;    ///< Number of non-null object serializations in the archive (not relevant)
    int m_iRootObjectCount; ///< Number of object serializations at root level. This is the actual number of shapes in the prefab
    int m_iReserved;
  } m_Header;

protected:

  int m_iSize;
  VMemoryTempBuffer<2048> m_BinaryBlock;
};


/// \brief
///   Resource manager for the VPrefab resources. A global instance of this manager can be accessed via VPrefabManager::GlobalManager()
class VPrefabManager : public VisResourceManager_cl
{
public:
  SCENE_IMPEXP VPrefabManager(const char *szName) : VisResourceManager_cl(szName,VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER)
  {
  }

  /// \brief
  ///   Create or return an existing prefab type with the passed filename.
  inline VPrefab *LoadPrefab(const char *szFilename) {return (VPrefab *)LoadResource(szFilename);}

  SCENE_IMPEXP virtual VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);

  /// \brief
  ///   Access the global manager that handles all prefabs
  static SCENE_IMPEXP VPrefabManager& GlobalManager(); 

  void OneTimeInit();
  void OneTimeDeInit();

protected:

  static VPrefabManager g_GlobalManager;
};

/// \brief
/// Instance of a prefab in the engine.
/// 
/// This class is used for the nested binary prefabs, 
/// where inner prefabs are stored separately in files.
///   
class VPrefabInstance : public VisBaseEntity_cl
{
public:
	SCENE_IMPEXP VPrefabInstance();

  /// \brief
  ///   Fully serializes this VPrefabInstance instance from archive or to archive. For more information
  ///   about serialization, see the Vision API Documentation.
  SCENE_IMPEXP virtual void Serialize( VArchive& ar );

  /// \brief
  /// Gets the filename of the prefab represented by this instance
  /// 
  /// \return pointer to the string containing the filename
  ///
  SCENE_IMPEXP const char* GetFileName() const;

  /// \brief
  /// Sets the filename of the prefab repesented by this class.
  ///
  /// \param file pointre to the string containing the file name of the prefab
  ///
  SCENE_IMPEXP void SetFileName(const char* file);

  V_DECLARE_SERIAL_DLLEXP( VPrefabInstance,  SCENE_IMPEXP );
  
  /// \brief
  ///   RTTI macro to add a variable table
  V_DECLARE_VARTABLE( VPrefabInstance, SCENE_IMPEXP );



protected:
  /// \brief
  /// Member storing the prefab's filename
  ///
	VString m_sFilename;
	
};


#endif // VPREFAB_HPP_INCLUDED

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
