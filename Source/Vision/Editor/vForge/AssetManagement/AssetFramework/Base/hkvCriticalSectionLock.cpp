/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFrameworkPCH.h>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvCriticalSectionLock.hpp>
#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

class hkvCriticalSectionLock::LockRef
{
protected:
  LockRef();
private:
  LockRef(const LockRef&);
  LockRef& operator=(const LockRef&);
protected:
  virtual ~LockRef();

public:
  void inc();
  void dec();

private:
  hkUint32 m_refCount;
};


class LockRef_hkCriticalSection : public hkvCriticalSectionLock::LockRef
{
public:
  LockRef_hkCriticalSection(hkCriticalSection& criticalSection);
private:
  LockRef_hkCriticalSection(const LockRef_hkCriticalSection&);
  LockRef_hkCriticalSection& operator=(const LockRef_hkCriticalSection&);
public:
  ~LockRef_hkCriticalSection();

private:
  hkCriticalSection& m_criticalSection;
};


class LockRef_Win32CriticalSection : public hkvCriticalSectionLock::LockRef
{
public:
  LockRef_Win32CriticalSection(CRITICAL_SECTION& criticalSection);
private:
  LockRef_Win32CriticalSection(const LockRef_Win32CriticalSection&);
  LockRef_Win32CriticalSection& operator=(const LockRef_Win32CriticalSection&);
public:
  ~LockRef_Win32CriticalSection();

private:
  CRITICAL_SECTION& m_criticalSection;
};


// ----------------------------------------------------------------------------
// hkvCriticalSectionLock
// ----------------------------------------------------------------------------
hkvCriticalSectionLock::hkvCriticalSectionLock()
: m_lockRef(NULL)
{
}


hkvCriticalSectionLock::hkvCriticalSectionLock(hkCriticalSection& criticalSection)
: m_lockRef(new LockRef_hkCriticalSection(criticalSection))
{
}


hkvCriticalSectionLock::hkvCriticalSectionLock(CRITICAL_SECTION& criticalSection)
: m_lockRef(new LockRef_Win32CriticalSection(criticalSection))
{
}


hkvCriticalSectionLock::hkvCriticalSectionLock(const hkvCriticalSectionLock& other)
: m_lockRef(other.m_lockRef)
{
  if (m_lockRef)
    m_lockRef->inc();
}


hkvCriticalSectionLock::~hkvCriticalSectionLock()
{
  if (m_lockRef)
    m_lockRef->dec();
}


hkvCriticalSectionLock& hkvCriticalSectionLock::operator=(const hkvCriticalSectionLock& other)
{
  if (other.m_lockRef != NULL)
  {
    other.m_lockRef->inc();
  }

  LockRef* oldLockRef = m_lockRef;
  m_lockRef = other.m_lockRef;

  if (oldLockRef != NULL)
  {
    oldLockRef->dec();
  }

  return *this;
}


// ----------------------------------------------------------------------------
// hkvCriticalSectionLock::LockRef
// ----------------------------------------------------------------------------
hkvCriticalSectionLock::LockRef::LockRef()
: m_refCount(1)
{
}


hkvCriticalSectionLock::LockRef::~LockRef()
{
}


void hkvCriticalSectionLock::LockRef::inc()
{
  ++m_refCount;
}


void hkvCriticalSectionLock::LockRef::dec()
{
  if (--m_refCount == 0)
    delete this;
}


// ----------------------------------------------------------------------------
// LockRef_hkCriticalSection
// ----------------------------------------------------------------------------
LockRef_hkCriticalSection::LockRef_hkCriticalSection(hkCriticalSection& criticalSection)
: m_criticalSection(criticalSection)
{
  m_criticalSection.enter();
}


LockRef_hkCriticalSection::~LockRef_hkCriticalSection()
{
  m_criticalSection.leave();
}


// ----------------------------------------------------------------------------
// LockRef_Win32CriticalSection
// ----------------------------------------------------------------------------
LockRef_Win32CriticalSection::LockRef_Win32CriticalSection(CRITICAL_SECTION& criticalSection)
: m_criticalSection(criticalSection)
{
  EnterCriticalSection(&m_criticalSection);
}


LockRef_Win32CriticalSection::~LockRef_Win32CriticalSection()
{
  LeaveCriticalSection(&m_criticalSection);
}

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
