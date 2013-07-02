/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
#ifndef HKV_ASSET_UID_LOOK_UP_TABLE_HPP
#define HKV_ASSET_UID_LOOK_UP_TABLE_HPP

#include <Vision/Runtime/Base/Container/VMapStrToPtr.hpp>
#include <Vision/Runtime/Base/System/IO/Stream/IVFileStream.hpp>
#include <Vision/Runtime/Base/Math/hkvMath.h> //for hkvResult

struct VAssetResolveContext;
struct VAssetInfo;


/// \brief 
///   Helper class for loading an asset look-up table and for performing
///   asset look-ups.
/// \sa IVFileStreamManager#LookUpAsset
class hkvAssetLookUpTable
{
public:
  /// \brief 
  ///   Constructor; initializes an empty look-up table.
  VBASE_IMPEXP hkvAssetLookUpTable();

private:
  /// Disallow copy construction
  hkvAssetLookUpTable(const hkvAssetLookUpTable&);
  /// Disallow assignment
  void operator = (const hkvAssetLookUpTable&);

public:
  /// \brief 
  ///   Destructor; frees the loaded look-up table.
  VBASE_IMPEXP ~hkvAssetLookUpTable();

  /// \brief
  ///   Clears the current lookup table
  VBASE_IMPEXP void clear();

  /// \brief
  ///   Loads look-up table entries from a file input stream.
  ///
  /// Any existing look-up entries are cleared before the new entries are added.
  ///
  /// \param pLookupTableFile
  ///   a stream opened for reading the lookup table
  VBASE_IMPEXP hkvResult load(IVFileInStream* pLookupTableFile);

  /// \brief
  ///   Retrieves the information for the specified asset.
  ///
  /// \param pszAsset
  ///   the asset to look up. Assets are identified by their relative path within their
  ///   asset library.
  ///
  /// \param pszVariant
  ///   the variant of the asset. Can be \c NULL if the default variant is to be looked up.
  ///
  /// \return 
  ///   the data stored in this look-up table for the specified asset and variant, or \c NULL
  ///   if no information was found for the specified asset and variant.
  VBASE_IMPEXP const VAssetInfo* lookUp(const char* pszAsset, const char* pszVariant = NULL) const;

private:
  char* m_pStringData;
  VAssetInfo* m_pData; // array that contains all data structs
  VStrMap<VAssetInfo> m_LookUpTable;
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
