/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VJSONHANDLERS_HPP_INCLUDED
#define VJSONHANDLERS_HPP_INCLUDED

#include "hkvJsonMemoryHandler.hpp"
#include "hkvJsonFileHandler.hpp"

#include <Common/Base/System/Io/Reader/hkStreamReader.h>
#include <Common/Base/System/Io/Writer/hkStreamWriter.h>

/// \brief
///   JSON memory handler implementation
class hkvJsonMemoryHandlerImpl : public hkvJsonMemoryHandler
{
public:
  hkvJsonMemoryHandlerImpl ();

public: // Interface
  virtual void Free(void* ptr);
  virtual void* Malloc(size_t sz);
  virtual void* Realloc(void* ptr, size_t sz);
};


/// \brief
///   JSON file handler implementation
class hkvJsonFileHandlerImpl : public hkvJsonFileHandler
{
public:
  hkvJsonFileHandlerImpl ();

public: // Interface
  virtual void* Open(const char* pszFileName, JsonFileMode mode);
  virtual size_t Read(void* pBuffer, size_t iLen, void* pStream);
  virtual void Close(void* pStream);
  virtual size_t Write(void* pBuffer, size_t iLen, void* pStream);
  virtual bool IsEOF(void* pStream);

private:
  hkRefPtr<hkStreamReader> m_streamReader;
  hkRefPtr<hkStreamWriter> m_streamWriter;
  JsonFileMode m_mode;
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
