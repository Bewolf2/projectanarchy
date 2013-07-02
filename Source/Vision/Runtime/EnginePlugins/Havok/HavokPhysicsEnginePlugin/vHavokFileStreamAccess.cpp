/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokFileStreamAccess.hpp>

//-----------------------------------------------------------------------------------

vHavokStreamWriter::vHavokStreamWriter(const char* pFileName)
  : m_pOutStream(NULL)
{
  if (pFileName != NULL && pFileName[0] != '\0')
    m_pOutStream = Vision::File.Create(pFileName);
}

vHavokStreamWriter::vHavokStreamWriter(IVFileOutStream* pStream)
  : m_pOutStream(pStream)
{
  VASSERT(m_pOutStream);
}

vHavokStreamWriter::~vHavokStreamWriter()
{
  if(m_pOutStream)
    m_pOutStream->Close();

  m_pOutStream = NULL;
}

int vHavokStreamWriter::write(const void* pBuffer, int iByteCount)
{
  return m_pOutStream ? (int)m_pOutStream->Write(pBuffer, iByteCount) : 0;
}

hkBool vHavokStreamWriter::isOk() const
{
  return m_pOutStream != NULL;
}

//-----------------------------------------------------------------------------------

vHavokStreamReader::vHavokStreamReader(const char* pFileName)
  : m_pInStream(NULL)
{
  if (pFileName != NULL && pFileName[0] != '\0')
    m_pInStream = Vision::File.Open(pFileName);
}

vHavokStreamReader::vHavokStreamReader(IVFileInStream* pStream)
{
  m_pInStream = pStream;
  VASSERT(m_pInStream);
}

vHavokStreamReader::~vHavokStreamReader()
{
  if(m_pInStream)
    m_pInStream->Close();

  m_pInStream = NULL;
}

int vHavokStreamReader::read(void* pBuffer, int iByteCount)
{
  return m_pInStream ? (int)m_pInStream->Read(pBuffer, iByteCount) : 0;
}

hkBool vHavokStreamReader::isOk() const
{
  return (m_pInStream && !m_pInStream->IsEOF());
}

//-----------------------------------------------------------------------------------

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
