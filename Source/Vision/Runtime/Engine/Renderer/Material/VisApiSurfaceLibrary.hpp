/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiSurfaceLibrary.hpp

#ifndef VISAPISURFACELIBRARY_HPP_INCLUDED
#define VISAPISURFACELIBRARY_HPP_INCLUDED

#include <Vision/Runtime/Engine/Renderer/Material/VisApiSurface.hpp>


/// \brief
///   Resource Manager for surface libraries.
class VSurfaceLibraryManager : public VisResourceManager_cl
{
public:

  /// \brief Constructor of the surface library manager.
  VSurfaceLibraryManager(const char *szManagerName,int iFlags, int iMemLimit) : VisResourceManager_cl(szManagerName,iFlags,iMemLimit) {}

  /// \brief Returns a pointer to the global surface library manager.
  VISION_APIFUNC static VSurfaceLibraryManager* GetManager ();

protected:
  /// \brief Creates and loads a new VisSurfaceLibrary_cl from file.
  VISION_APIFUNC VOVERRIDE VManagedResource *CreateResource(const char *szFilename, VResourceSnapshotEntry *pExtraInfo);
};



/// \brief
///   Material Library class used by the Vision engine.
/// 
/// An instance of this class represents a set of Materials. Loading a VisSurfaceLibrary_cl makes all the surfaces in it available,
/// i.e. all Surfaces will be allocated, but their actual data (the textures) will only be loaded on demand.
/// 
class VisSurfaceLibrary_cl : public VManagedResource
{
public:
  /// \brief Constructor. Initializes the filename from which to load the library, but does not yet load anything.
  VISION_APIFUNC VisSurfaceLibrary_cl(const char* szFileName);
  VISION_APIFUNC ~VisSurfaceLibrary_cl();

  /// \brief Returns the number of Materials that are contained in this library.
  VISION_APIFUNC unsigned int GetNumberOfMaterials (void) const {return (m_uiMaterialCount);}

  /// \brief Returns the Material with the given index.
  VISION_APIFUNC VisSurface_cl* GetMaterial (unsigned int uiIndex) const {VASSERT(uiIndex<m_uiMaterialCount); return (m_pMaterials[uiIndex]);}

  /// \brief Returns the Material with the given name, or NULL if no such Material exists.
  VISION_APIFUNC VisSurface_cl* GetMaterialByName (const char* szName);

  /// \brief Adds a new Material to this library and copies its content from the given Material. Returns the pointer to the new material.
  ///
  /// Can only be executed during editing (from within vForge).
  ///
  /// \param pCopyFrom
  ///   The source material
  ///
  /// \returns
  ///   The cloned material
  VISION_APIFUNC VisSurface_cl* AddMaterialCopy (const VisSurface_cl* pCopyFrom);

  /// \brief Adds a new empty Material to this library. Returns the pointer to the new material.
  ///
  /// Can only be executed during editing (from within vForge).
  VISION_APIFUNC VisSurface_cl* AddMaterial (void);

  /// \brief Removes the Material with the given index from this library.
  ///
  /// No index range checks are done.
  /// Can only be executed during editing (from within vForge).
  ///
  /// \param uiIndex
  ///   The material index to remove
  VISION_APIFUNC void RemoveMaterial (unsigned int uiIndex);

  /// \brief Removes the Material with the given pointer, if it exists in this library.
  ///
  /// Can only be executed during editing (from within vForge).
  ///
  /// \param pMaterial
  ///   The material to remove
  VISION_APIFUNC void RemoveMaterial (VisSurface_cl* pMaterial);


  /// \brief (Re-)loads all the data of this library from the file specified during construction.
  VISION_APIFUNC VOVERRIDE BOOL Reload();

  /// \brief Unloads all data of this library.
  VISION_APIFUNC VOVERRIDE BOOL Unload();

  /// \brief Returns the global resource manager.
  VISION_APIFUNC static VSurfaceLibraryManager &GetResourceManager();

  // TODO: Not yet implemented.
  VISION_APIFUNC virtual void AccumulateMemoryFootprint (size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  // TODO: Not yet implemented.
  VISION_APIFUNC virtual void GetDependencies (VResourceSnapshot &snapshot);

  // TODO: Not yet implemented.
  VISION_APIFUNC virtual BOOL IsRelevantForSnapshot(VResourceSnapshot &snapshot) const;

  VISION_APIFUNC bool SaveToXML();
private:
  bool LoadFromBinaryFile (void);

  VisSurface_cl** m_pMaterials;
  unsigned int m_uiMaterialCount;
};

typedef VSmartPtr<VisSurfaceLibrary_cl> VisSurfaceLibraryPtr;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// inlines
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
