/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VClipboard.hpp

#ifndef VCLIPBOARD_HPP_INCLUDED
#define VCLIPBOARD_HPP_INCLUDED

#include <Vision/Runtime/Base/System/IO/Stream/VMemoryStream.hpp>

class VClipboard;
typedef VSmartPtr<VClipboard> VClipboardPtr;

/// \brief
///   Simple memory file manager that allows opening a memory stream for reading or writing.
///
/// This class is not related to the operating system's clipboard. If you need functionality for this,
/// have a look at VClipBoardHelper.
///
/// \note
///   Only one in-memory stream can be held at one time. It is not possible to create two
///   or more streams and access them at the same time.
class VClipboard : public IVFileStreamManager
{
  V_DECLARE_DYNAMIC_DLLEXP(VClipboard, VBASE_IMPEXP);

public:
  /// \brief
  ///   Returns whether the clipboard is empty.
  ///
  /// The clipboard is considered empty either if no memory stream has yet been created, or if the
  /// memory stream this clipboard holds has zero length.
  ///
  /// \return
  ///   \c true if the clipboard is empty; \c false if not
  inline bool IsEmpty() const {return (GetSize()==0);}

  /// \brief
  ///   Returns the amount of data held by this clipboard.
  /// \return
  ///   the number of bytes held by this clipboard. If no memory stream has yet been created, 
  ///   \c 0 is returned.
  inline int GetSize() const {if (m_spStream) return m_spStream->GetSize();return 0;}

  /// \brief
  ///   Empties this clipboard.
  inline void Clear() {m_spStream=NULL;}

  /// \brief
  ///   Returns the memory stream held by this clipboard as a readable file stream.
  /// \return
  ///   a file stream, or \c NULL if this clipboard does not hold a memory stream
  inline IVFileInStream* OpenClipboard()     {return Open(NULL);}

  /// \brief
  ///   Creates a new memory stream in this clipboard and returns a writable file
  ///   stream for it.
  ///
  /// If this clipboard already holds a memory stream, it is discarded and replaced
  /// by the new stream.
  ///
  /// \return
  ///   a writable file stream
  inline IVFileOutStream* CreateClipboard()  {return Create(NULL);}

  // overridden stream manager functions
  VBASE_IMPEXP virtual const char* GetName() HKV_OVERRIDE {return "VClipboard";}
  VBASE_IMPEXP virtual IVFileOutStream* Create(const char* pszFileName, int iFlags = 0) HKV_OVERRIDE;

  // overridden data directory function. Not supported.

  /// \brief
  ///   Not supported for VClipboard.
  /// \returns
  ///   always \c 0
  VBASE_IMPEXP virtual int GetNumDataDirectories() HKV_OVERRIDE {return 0;}

  /// \brief
  ///   Not supported for VClipboard.
  /// \param idx
  ///   ignored
  /// \returns
  ///   always \c NULL
  VBASE_IMPEXP virtual const char* GetDataDirectory(int idx = 0) HKV_OVERRIDE {return NULL;}

  /// \brief
  ///   Not supported for VClipboard; has no effect.
  /// \param bStatus
  ///   ignored
  /// \param idx
  ///   ignored
  VBASE_IMPEXP virtual void SetEnableDirectoryCaching(bool bStatus, int idx = 0) HKV_OVERRIDE { }

  /// \brief
  ///   Not supported for VClipboard.
  /// \param idx
  ///   ignored
  /// \returns
  ///   always \c false
  VBASE_IMPEXP virtual bool IsDirectoryCachingEnabled(int idx = 0) HKV_OVERRIDE { return false; }

protected:
  VBASE_IMPEXP virtual IVFileInStream* InternalOpen(const char* pszFileName, int iFlags) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL InternalExists(const char* pszFileName) HKV_OVERRIDE;
  VBASE_IMPEXP virtual BOOL InternalGetTimeStamp(const char *pszFileName, VFileTime &destTime) HKV_OVERRIDE { return FALSE; }
  VBASE_IMPEXP virtual bool InternalSetDataDirectory(int idx, const char* pszPath) HKV_OVERRIDE { return false; }

private:
  bool IsValidStreamName(const char* szName) const;

private:
  VString m_sStreamName;
  VMemoryStreamPtr m_spStream;
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
