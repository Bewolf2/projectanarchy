/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/VisionAssetsPCH.h>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetManager.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvStateSync.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvAnimationAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvCollectionAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvCollisionMeshAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvModelAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvParticleAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvPrefabAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvStaticMeshAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvCollectionTransformationRule.hpp>
#include <Vision/Editor/vForge/AssetManagement/VisionAssets/hkvTextureTransformationRule.hpp>

static bool s_bIsInitialized = false;


void OneTimeInit()
{
  if (s_bIsInitialized)
    return;

  hkvStateSync::notifyModuleLoaded();

  s_bIsInitialized = true;
}

extern "C" __declspec(dllexport) void InitAssetPlugin()
{
  OneTimeInit();

  hkvTextureAsset::StaticInit();
  hkvStaticMeshAsset::StaticInit();
  hkvModelAsset::StaticInit();
  hkvParticleAsset::StaticInit();
  hkvPrefabAsset::StaticInit();
  hkvAnimationAsset::StaticInit();
  hkvCollisionMeshAsset::StaticInit();
  hkvCollectionAsset::StaticInit();
  //hkvShaderLibAsset::StaticInit();

  hkvTextureTransformationRule::staticInit();
  hkvCollectionTransformationRule::staticInit();
}

extern "C" __declspec(dllexport) void DeInitAssetPlugin()
{
  hkvTextureAsset::StaticDeInit();
  hkvStaticMeshAsset::StaticDeInit();
  hkvModelAsset::StaticDeInit();
  hkvParticleAsset::StaticDeInit();
  hkvPrefabAsset::StaticDeInit();
  hkvAnimationAsset::StaticDeInit();
  hkvCollisionMeshAsset::StaticDeInit();
  hkvCollectionAsset::StaticDeInit();
  //hkvShaderLibAsset::StaticDeInit();

  hkvTextureTransformationRule::staticDeInit();
  hkvCollectionTransformationRule::staticDeInit();

  hkvStateSync::notifyModuleUnloading();
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
