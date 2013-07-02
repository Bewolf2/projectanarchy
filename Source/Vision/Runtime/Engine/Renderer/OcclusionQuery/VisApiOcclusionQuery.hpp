/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */



#ifndef DEFINE_VISAPIOCCLUSIONQUERY
#define DEFINE_VISAPIOCCLUSIONQUERY

#include <Vision/Runtime/Engine/System/VisApiDefs.hpp>

/// \brief
///  Defines for which kind of occlusion queries the hardware should be configured.
enum VisQueryRenderState_e
{
  VISQUERY_RENDERSTATE_NOTSET = 0,
  VISQUERY_RENDERSTATE_BOX = 1,
  VISQUERY_RENDERSTATE_BILLBOARD = 2,
  VISQUERY_RENDERSTATE_CUSTOMGEOMETRY = 3
};


/// \brief
///   This class encapsulates hardware occlusion query functionality. The class itself does not hold a hardware query
///   but instead holds an index to an internal hardware query pool as long as a query is in progress.
///
///  To use the query call 'BeginOcclusionQuery' and anything that is rendered until 'EndOcclusionQuery' will be added to the query's result.
///  Note that there can only be one query active at any time, so it is important to end a query before 
///  calling 'BeginOcclusionQuery' on another query.
///
///  Calling 'GetResult' or 'CheckQueryQueue' will result in a check to the queue of actual hardware queries. These will
///  be checked in chronological order and those that are already finished will have their result written back to their
///  corresponding 'VOcclusionQuery' object. This ensures that the actual hardware queries can be reused as soon as possible,
///  even if a 'VOcclusionQuery' never or rarely retrieves its results.
///
///  This functionality depends on the back pointer within the hardware queries to map them to the corresponding 'VOcclusionQuery' objects.
///  Therefore, it is important that active 'VOcclusionQuery' objects are not moved in memory. Do not store them in containers like VArray<VOcclusionquery>
///  which move the memory on reallocation.
class VOcclusionQuery
{
public:
  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor.
  ///
  /// \param iDefaultValue
  ///   Default value that is returned if no query was done yet.
	VISION_APIFUNC VOcclusionQuery (unsigned int iDefaultValue = MIN_HWOCCTEST_VISIBLE_PIXELS);

  /// \brief
  ///   Copy constructor.
  VISION_APIFUNC VOcclusionQuery (const VOcclusionQuery& cc);

  /// \brief
  ///   Destructor.
	VISION_APIFUNC ~VOcclusionQuery ();

  ///
  /// @}
  ///

  ///
  /// @name Public Query Functions
  /// @{
  ///

  /// \brief
  ///   Assignment operator.
  VISION_APIFUNC const VOcclusionQuery& operator= (const VOcclusionQuery& cc);

  /// \brief
  ///   Starts the hardware occlusion query.
  ///
  ///  Anything that is rendered between 'BeginOcclusionQuery' and 'EndOcclusionQuery' will add to this query.
  ///  Note that there can only be one query active at any time, so it is important to end a query before 
  ///  calling this function on another query.
  ///
  /// \sa VOcclusionQuery::EndOcclusionQuery
	VISION_APIFUNC void BeginOcclusionQuery (void);

  /// \brief
  ///   Ends the hardware occlusion query.
  ///
  ///  Anything that is rendered between 'BeginOcclusionQuery' and 'EndOcclusionQuery' will add to this query.
  ///  Note that there can only be one query active at any time, so it is important to end a query before 
  ///  calling this function on another query.
  ///
  /// \sa VOcclusionQuery::BeginOcclusionQuery
	VISION_APIFUNC void EndOcclusionQuery (void);

  /// \brief
  ///   Returns whether a query is currently in progress. Does not check if the query has finished.
  VISION_APIFUNC bool IsQueryInProgress (void);

  /// \brief
  ///   Returns true if the query is already available. Also returns true if no query was ever made.
	VISION_APIFUNC bool IsResultReady (void);

  /// \brief
  ///   Retrieves the result of the query.
  ///
  /// \param out_iDrawnPixels
  ///   If the return value is true, the new query result was written to this variable, otherwise the most recent result is written to it.
  ///
  /// \param bWait
  ///   Whether the process should stall to wait for the query to become available.
  ///
  /// \return
  ///   Returns true if a new value was written to 'out_iDrawnPixels'.
	VISION_APIFUNC bool GetResult (unsigned int& out_iDrawnPixels, bool bWait) const;

#ifdef SUPPORTS_DETAILED_OCCLUSION_QUERY
  /// \brief
  ///   Retrieves the result of the query. This function also returns the number of total queried pixels on platforms
  ///   that support this feature, which is currently only the XBOX360.
  ///
  /// \param out_iDrawnPixels
  ///   If the return value is true, the new query result was written to this variable, otherwise the most recent result is written to it.
  ///
  /// \param out_iTotalPixels
  ///   If the return value is true, the total number of queried pixels will be written to this variable.
  ///   This value is equal to 'out_iDrawnPixels' plus all pixels that were culled.
  ///
  /// \param bWait
  ///   Whether the process should stall to wait for the query to become available.
  ///
  /// \return
  ///   Returns true if a new value was written to 'out_iDrawnPixels'.
  VISION_APIFUNC bool GetResult (unsigned int& out_iDrawnPixels, unsigned int& out_iTotalPixels, bool bWait) const;
#endif
  ///
  /// @}
  ///


  ///
  /// @name Public Helper Functions
  /// @{
  ///

  /// \brief
  ///   Queries the visibility of a bounding box for visibility determination.
  /// 
  /// Simple helper method for rendering a bounding box defined by its min and max
  /// positions through the float[3] pointers fMin and fMax. Will call Begin/EndVisibilityQuery
  /// on its own.
  /// 
  /// \param fMin
  ///   Bounding box min corner.
  /// 
  /// \param fMax
  ///   Bounding box max corner.
  ///
  /// \sa VOcclusionQuery::EnableVisibilityQueryState
  VISION_APIFUNC void DoHardwareOcclusionTest_BBox(float* fMin, float* fMax);

  /// \brief
  ///   Queries the visibility of a bounding box for visibility determination.
  /// 
  /// Simple helper method for rendering a hkvAlignedBBox object that will call Begin/EndVisibilityQuery
  /// on its own.
  /// EnableVisibilityQueryState(VISQUERY_RENDERSTATE_BOX) needs to be called before this function can be used.
  /// 
  /// \param pBox
  ///   Bounding box to render.
  ///
  /// \sa VOcclusionQuery::EnableVisibilityQueryState
  VISION_APIFUNC void DoHardwareOcclusionTest_BBox(hkvAlignedBBox* pBox);

  /// \brief
  ///   Queries the visibility of a billboard for corona / lensflare visibility determination.
  /// 
  /// Simple helper method for rendering a billboard object that will call Begin/EndVisibilityQuery
  /// on its own.
  /// EnableVisibilityQueryState(VISQUERY_RENDERSTATE_BILLBOARD) needs to be called before this function can be used.
  ///
  /// \param fPos
  ///   pointer to 3 floats that define the position of the billboard.
  ///
  /// \param fPixelRadius
  ///   Radius in pixels of the billboard.
  ///
  /// \sa VOcclusionQuery::EnableVisibilityQueryState
  VISION_APIFUNC void DoHardwareOcclusionTest_Billboard(float* fPos, float fPixelRadius);

  /// \brief
  ///   Queries the visibility of a portal for visibility determination.
  /// 
  /// Simple helper method for rendering a portal that will call Begin/EndVisibilityQuery
  /// on its own.
  /// EnableVisibilityQueryState(VISQUERY_RENDERSTATE_CUSTOMGEOMETRY) needs to be called before this function can be used.
  ///
  /// \param pPortal
  ///   Portal to render.
  ///
  /// \sa VOcclusionQuery::EnableVisibilityQueryState
  VISION_APIFUNC void DoHardwareOcclusionTest_Portal(VisPortal_cl* pPortal);

  ///
  /// @}
  ///


  ///
  /// @name Static Functions
  /// @{
  ///

  /// \brief
  ///   Initializes the back end of the VOcclusionQuery class. This is done automatically by the engine.
  static void InitQuerySystem();

  /// \brief
  ///   De-initializes the back end of the VOcclusionQuery class. This is done automatically by the engine.
  static void DeInitQuerySystem();

  /// \brief
  ///   Resets the back end of the VOcclusionQuery class. Only relevant for lost device events on dx9.
  static void ResetQuerySystem();

  /// \brief
  ///   Enables the visibility query state
  /// 
  /// This method enables the basic states for performing custom visibility queries. Afterwards,
  /// rendering for visibility can be performed using the Begin-/EndOcclusionQuery methods.
  /// 
  /// The render states affected by this method include the vertex/pixel shaders, culling mode,
  /// depth comparison function, and the write mask for color and depth buffers.
  /// 
  /// After finishing rendering geometry for visibility query, the state has to be disabled using
  /// the VOcclusionQuery::DisableVisibilityQueryState method.
  ///
  /// \param eState
  ///   Defines for which kind of occlusion queries the hardware should be configured.
  ///
  /// \sa VisQueryRenderState_e
  /// \sa VOcclusionQuery::DoHardwareOcclusionTest_BBox
  /// \sa VOcclusionQuery::DoHardwareOcclusionTest_Billboard
  /// \sa VOcclusionQuery::DoHardwareOcclusionTest_Portal
  VISION_APIFUNC static void EnableVisibilityQueryState(VisQueryRenderState_e eState);

  /// \brief
  ///   Disables the visibility query states
  /// 
  /// This method disables the basic states for performing custom visibility query. 
  /// 
  /// After finishing rendering geometry for visibility query, the state has to be disabled using
  /// this method.
  VISION_APIFUNC static void DisableVisibilityQueryState();
  /// \brief
  ///   Returns the number of allocated hardware queries. Can not be higher than MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES.
  VISION_APIFUNC static unsigned short GetAllocatedQueryCount();

  /// \brief
  ///   Returns the number of allocated queries that are currently not in use.
  VISION_APIFUNC static unsigned short GetFreeAllocatedQueryCount();
  
  /// \brief
  ///   Returns the maximum number of allocated hardware queries. Equal to MAX_HWOCCTEST_OCCLUSIONQUERYHANDLES.
  VISION_APIFUNC static unsigned short GetMaximumQueryCount();

  /// \brief
  ///   Returns the number of still available hardware queries. Once this value reaches zero, the next query will fail.
  VISION_APIFUNC static unsigned short GetFreeQueryCount();

  /// \brief
  ///   Checks all pending occlusion queries and writes back their results.
  VISION_APIFUNC static void CheckQueryQueue (void);

  ///
  /// @}
  ///

private: // functions
	void DiscardQuery (void);

private: // static functions
  static void MoveUsedElementToFree (unsigned short iIndex);
  static unsigned short RemoveElementFromFree (unsigned char iQueueIndex);
  static unsigned short MoveFreeElementToUsed (unsigned char iQueueIndex);
  static void WarmupQuerySystem();


private: //data
	unsigned int m_iDrawnPixels;
#ifdef SUPPORTS_DETAILED_OCCLUSION_QUERY
  unsigned int m_iTotalPixels;
#endif
	unsigned short m_iQueryIndex;

private: //static data 
  static unsigned short s_iTotalQueries;
  static unsigned short s_iFreeQueries;
#ifdef SUPPORTS_OCCLUSION_QUERY
  static unsigned short s_iFreeBegin[HWOCCTEST_NUM_QUERY_QUEUES];
  static unsigned short s_iFreeEnd[HWOCCTEST_NUM_QUERY_QUEUES];
  static unsigned short s_iCurrentFreePos[HWOCCTEST_NUM_QUERY_QUEUES];
  static unsigned short s_iUsedBegin;
  static unsigned short s_iUsedEnd;
  static unsigned short s_iActiveQuery;
#endif
};

#pragma once

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
