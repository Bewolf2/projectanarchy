/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiVideo.inl

inline int VisVideo_cl::GetXRes() const
{
  if (m_bSMInitialized)
    return currentConfig.m_iXRes;
  else
    return 0;
}


inline int VisVideo_cl::GetYRes() const
{
  if (m_bSMInitialized)
    return currentConfig.m_iYRes;
  else
    return 0;
}


inline int VisVideo_cl::GetXPos() const
{
  if (m_bSMInitialized)
    return currentConfig.m_iXPos;
  else
    return 0;
}


inline int VisVideo_cl::GetYPos() const
{
  if (m_bSMInitialized)
    return currentConfig.m_iYPos;
  else
    return 0;
}


inline bool VisVideo_cl::IsInitialized() const
{
  return m_bSMInitialized;
}


inline VVideoConfig *VisVideo_cl::GetCurrentConfig()
{
  return &currentConfig;
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
