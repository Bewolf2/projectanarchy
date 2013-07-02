/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vHavokFileStreamAccess.hpp

// ***********************************************************************************************
// vHavok binding for Vision that uses Havok for physics
// Copyright (C) Trinigy GmbH. All rights reserved.
// ***********************************************************************************************

#ifndef VHAVOKFILESTREAMACCESS_HPP_INCLUDED
#define VHAVOKFILESTREAMACCESS_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>
#include <Common/Base/System/Io/Writer/hkStreamWriter.h>
#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/System/Io/IArchive/hkIArchive.h>
#include <Common/Base/System/Io/OArchive/hkOArchive.h>

/// \brief
///   This class implements the Havok stream writer interface.
class vHavokStreamWriter : public hkStreamWriter
{
public:
  /// \fn vHavokStreamWriter(const char *pFileName)
  ///
  /// \brief  Constructor. 
  ///
  /// \param pFileName
  ///  The destination filename
  ///
  VHAVOK_IMPEXP vHavokStreamWriter(const char* pFileName);

  /// \fn vHavokStreamWriter(IVFileOutStream* pStream)
  ///
  /// \brief  Constructor.
  ///
  /// \param pStream
  ///  The destination stream
  ///
  VHAVOK_IMPEXP vHavokStreamWriter(IVFileOutStream* pStream);

  VHAVOK_IMPEXP virtual ~vHavokStreamWriter();

  /// \brief
  ///   Writes data to the stream and returns the number of bytes written.
	///
  /// \param pBuffer
  ///  The buffer containing the data
  ///
  /// \param iByteCount
  ///  The number of bytes to write
  ///
  VHAVOK_IMPEXP virtual int write(const void* pBuffer, int iByteCount);

	/// \brief
  ///   Returns false if EOF has been reached or some other error.
  ///
  VHAVOK_IMPEXP virtual hkBool isOk() const;

  
protected:

  IVFileOutStream* m_pOutStream;

};

/// \brief
///   This class implements the Havok stream reader interface.
class vHavokStreamReader : public hkStreamReader
{
public:
  /// \fn vHavokStreamReader(const char *pFileName)
  ///
  /// \brief  Constructor. 
  ///
  /// \param pFileName
  ///  The source filename
  ///
  VHAVOK_IMPEXP vHavokStreamReader(const char* pFileName);

  /// \fn vHavokStreamReader(IVFileInStream* pStream)
  ///
  /// \brief  Constructor.
  ///
  /// \param pStream
  ///  The source stream
  ///
  VHAVOK_IMPEXP vHavokStreamReader(IVFileInStream* pStream);

  VHAVOK_IMPEXP virtual ~vHavokStreamReader();

  /// \brief
  ///   Reads data from the stream and returns the number of bytes read.
	///
  /// \param pBuffer
  ///   The destination buffer
  ///
  /// \param iByteCount
  ///   The number of bytes to write
  ///
  VHAVOK_IMPEXP virtual int read(void* pBuffer, int iByteCount);

	/// \brief
  ///   Returns false if EOF has been reached or some other error.
  ///
  VHAVOK_IMPEXP virtual hkBool isOk() const;

protected:

  IVFileInStream* m_pInStream;

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
