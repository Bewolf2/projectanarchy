/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvAssetDecl.hpp

#ifndef HKV_ASSETDECL_HPP_INCLUDED
#define HKV_ASSETDECL_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/AssetFramework.hpp>

#define HKV_INVALID_INDEX ULONG_MAX

enum hkvAssetSortingCriterion
{
  HKV_ASC_NAME,
  HKV_ASC_TYPE,
  HKV_ASC_TAG,
  HKV_ASC_VARIANT,
  HKV_ASC_FOLDER,
};

enum hkvAssetLibraryLoadResult
{
  HKV_ALLR_SUCCESS,
  HKV_ALLR_PATH_NOT_FOUND,
  HKV_ALLR_LIBRARY_ALREADY_PRESENT,
  HKV_ALLR_LIBRARY_NOT_FOUND,
  HKV_ALLR_LIBRARY_LOAD_FAILED
};

enum hkvAssetLibraryMode
{
  HKV_ALM_AUTOMATIC,
  HKV_ALM_MANUAL
};

enum hkvAssetOperationResult
{
  HKV_AOR_SUCCESS,
  HKV_AOR_FAILURE,
  HKV_AOR_MAY_RETRY
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
