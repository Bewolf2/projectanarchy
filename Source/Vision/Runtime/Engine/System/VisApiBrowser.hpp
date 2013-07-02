/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiBrowser.hpp

#ifndef VIS_API_BROWSER_HPP
#define VIS_API_BROWSER_HPP

#ifdef SUPPORTS_BROWSER

#include <Vision/Runtime/Engine/System/VisApiCallbacks.hpp>


/// \brief
///   Pure virtual interface of an object that describes a file package. A file package provides a list of names of contained files.
class IVFilePackage
{
public:
  /// \brief
  ///   Pure virtual function to return the name of the package. Can be an arbitrary string to identify the package (e.g. name of the ZIP file)
  virtual const char *GetPackageFileName() = 0;

  /// \brief
  ///   Returns the number of file name entries
  virtual int GetFileEntryCount() = 0;

  /// \brief
  ///   Returns the file name with specified index, where iIndex must be in valid [0..GetFileEntryCount()-1] range
  virtual const char* GetFileName(int iIndex) = 0;
};


/// \brief
///   Wrapper class that allows to use a ZIP file as a data package. Useful to pass a ZIP to the VisBrowserManager_cl::TriggerPackageFileLoaded function
class VFilePackageZIPWrapper : public IVFilePackage
{
public:
  /// \brief
  ///   Constructor; Takes the ZIP file as an input
  inline VFilePackageZIPWrapper(VZipFileStreamManager &zip) : m_ZIP(zip)
  {
  }

  /// \brief
  ///   IVFilePackage implementation: Returns the name of the ZIP file
  virtual const char *GetPackageFileName() HKV_OVERRIDE
  {
    VASSERT(m_ZIP.GetNumDataDirectories()==1);
    return m_ZIP.GetDataDirectory(0);
  }
  /// \brief
  ///   IVFilePackage implementation - returns the number of entries in the ZIP
  VOVERRIDE int GetFileEntryCount()
  {
    return m_ZIP.GetNumZipFileEntries();
  }
  /// \brief
  ///   IVFilePackage implementation - returns the file name of the entry
  VOVERRIDE const char* GetFileName(int iIndex)
  {
    const VZipFileInfo& entry(m_ZIP.GetZipFileEntry(iIndex));
    return entry.m_szFilename;
  }

protected:
  VZipFileStreamManager &m_ZIP;
};

/// \brief
///   Data object class passed by the Vision::Browser.OnNewPackageFileLoaded callback
class VisFilePackageDataObject_cl : public IVisCallbackDataObject_cl 
{
public:
  VisFilePackageDataObject_cl(VisCallback_cl *pSender, IVFilePackage &package, bool bIsNewFile)
    : IVisCallbackDataObject_cl(pSender),
      m_Package(package), m_bIsNewFile(bIsNewFile)
  {
  }

  bool m_bIsNewFile;        ///< Indicates whether the package is a new download or just loaded from cache
  IVFilePackage &m_Package; ///< File package instance. See IVFilePackage
};

/// \brief
///   Singleton class for accessing the browser related states and functionality. It can be globally accessed via Vision::Browser
class VisBrowserManager_cl
{
public:
  VisBrowserManager_cl();
  ~VisBrowserManager_cl();

  /// \brief
  ///   This function triggers the OnNewPackageFileLoaded member callback
  /// 
  /// Custom implementations of a browser app that load file packages should trigger this callback, because extern plugins might need to respond to new
  /// data packages. See default implementation for exact usage.
  ///
  /// \param package
  ///   Reference to the new data package. There is a helper class VFilePackageZIPWrapper to pass ZIP file manager directly.
  ///
  /// \param bIsNewFile
  ///   Flag that indicates whether the package is a new download or whether it was just loaded from existing cache. For plugins this might make a difference. 
  inline void TriggerPackageFileLoaded(IVFilePackage &package, bool bIsNewFile)
  {
    VisFilePackageDataObject_cl data(&OnNewPackageFileLoaded, package, bIsNewFile);
    data.Trigger();
  }

  /// \brief
  ///   Callback that is triggered for every new package that is loaded. The passed data structure can be casted to VisFilePackageDataObject_cl which contains relevant information
  VisCallback_cl OnNewPackageFileLoaded;
};


#endif // SUPPORTS_BROWSER

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
