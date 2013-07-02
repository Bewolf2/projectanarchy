/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once
using namespace System;
using namespace TerrainBase::Config;
using namespace TerrainBase::Editing;

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Terrain/Application/EditableTerrain.hpp>


class VTerrainConfig;

namespace TerrainManaged
{
  public ref class TerrainConversionHelpers
  {
  public:
    static void ConfigToNative(TerrainBase::Config::TerrainConfig ^pSrc, VTerrainConfig *pDest);
    static void ConfigFromNative(VTerrainConfig *pSrc, TerrainBase::Config::TerrainConfig ^pDest);
    static bool V3DHasTerrain();

    static TerrainBrush ^ CreateBrush(String ^filename);
    static void FreeBrush(TerrainBrush ^ brush);
    static void CheckBrushModified(TerrainBrush ^ brush);

    static void DetailTexturePropertiesToNative(VTextureWeightmapChannelResource::DetailTextureProperties_t *pDest, DetailTextureResource ^pSrc);
  private:
    TerrainConversionHelpers(); // prevent creation
  };
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
