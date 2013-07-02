/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file ITerrainFilter.hpp

#ifndef ITERRAINFILTER_HPP_INCLUDED
#define ITERRAINFILTER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/TerrainIncludes.hpp>

class VEditableTerrain;


/// \brief
///   Simple interface that can apply a filter action on a terrain
class IVTerrainFilter
{
public:
  ///\brief
	///Constructor
	///
	inline IVTerrainFilter()
  {
    m_pLog = NULL;
    m_pTerrain = NULL;
  }

  virtual ~IVTerrainFilter(){}

  ///\brief
	///Pure virtual that initializes the filter instance before applying it
	///
	///\param affectedArea
	///absolute rectangle in world space coordinates
	///
	///\param pUserData
	///Not used
	///
	///\returns
	///True if the filter can proceed
	///
	virtual bool Init(const VLargeBoundingBox& affectedArea, void *pUserData) = 0;

  ///\brief
	///Runs the actual filter
	///
	///\param pUserData
	///Not used, same as passed to Init function
	///
	///\returns
	///True if operation was successful
	virtual bool Do(void *pUserData) = 0;

  ///\brief
	///Called after running the filter
	///
	///\param pUserData
	///Not used, same as passed to Init function
	///
	///\returns
	///True if operation was successful
	///
	virtual bool DeInit(void *pUserData) = 0;

public:

  // members get set when the filter is applied
  IVLog *m_pLog;  ///< stores the log instance
  VEditableTerrain *m_pTerrain; ///< Owner terrain
  VLargeBoundingBox m_AffectedArea; ///Stores the affected area
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
