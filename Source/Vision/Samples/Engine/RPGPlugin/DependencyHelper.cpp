/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */
#include <Vision/Samples/Engine/RPGPlugin/Precompiled.h>

#include <Vision/Samples/Engine/RPGPlugin/DependencyHelper.h>

#include <Vision/Samples/Engine/RPGPlugin/GameManager.h>
#include <Vision/Samples/Engine/RPGPlugin/VisionEffectHelper.h>

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>


#ifdef SUPPORTS_SNAPSHOT_CREATION

/// Adds the dependencies for a prefab to the resource snapshot
void RPG_DependencyHelper::AddPrefabDependency(const VString& prefabName, VResourceSnapshot &snapshot)
{
  if(prefabName.IsEmpty())
  {
    return;
  }

  if(Vision::Editor.IsPlayingTheGame())
  {
    // only check for already loaded assets when playing. Need to force a full dependency walk otherwise in order to make the "Show Assets" button work in the export window.
    VManagedResource* resource = VPrefabManager::GlobalManager().GetResourceByName(prefabName);

    if(resource && 
      resource->IsLoaded())
    {
      return;
    }
  }

  VPrefab* prefab = VPrefabManager::GlobalManager().LoadPrefab(prefabName);
  VASSERT(prefab);

  if(prefab)
  {
    prefab->GetDependencies(snapshot);

    // create the prefab entity, but don't do a full initialization, as we don't need it here.
    VisBaseEntity_cl* entity = RPG_GameManager::s_instance.CreateEntityFromPrefab(prefabName, hkvVec3(0.0f, 0.0f, 0.0f), hkvVec3(0.0f, 0.0f, 0.0f));
    VASSERT(entity);

    if(entity)
    {
      entity->GetDependencies(snapshot);

      entity->DisposeObject();
    }
  }
}

/// Adds the dependencies for a script created entity to the resource snapshot
void RPG_DependencyHelper::AddScriptEntityDependency(const VString& scriptName, VResourceSnapshot &snapshot)
{
  if(scriptName.IsEmpty())
  {
    return;
  }

  VScriptResourceManager* scriptManager = static_cast<VScriptResourceManager*>(Vision::GetScriptManager());

  if(Vision::Editor.IsPlayingTheGame())
  {
    // only check for already loaded assets when playing. Need to force a full dependency walk otherwise in order to make the "Show Assets" button work in the export window.
    VManagedResource* resource = scriptManager->GetResourceByName(scriptName);

    if(resource && 
      resource->IsLoaded())
    {
      return;
    }
  }

  VScriptResource* scriptRes = scriptManager->LoadScriptFile(scriptName);

  VASSERT(scriptRes);

  if(scriptRes)
  {
    scriptRes->GetDependencies(snapshot);

    // create the script entity, but don't do a full initialization, as we don't need it here.
    VisBaseEntity_cl* entity = RPG_GameManager::s_instance.CreateEntityFromScript(scriptName, hkvVec3(0.0f, 0.0f, 0.0f), hkvVec3(0.0f, 0.0f, 0.0f));
    VASSERT(entity);

    if(entity)
    {
      entity->GetDependencies(snapshot);
      
      entity->DisposeObject();
    }
  }
}

/// Adds the dependencies for a particle effect to the resource snapshot
void RPG_DependencyHelper::AddParticleEffectDependency(const VString& effectName, VResourceSnapshot &snapshot)
{
  if(effectName.IsEmpty())
  {
    return;
  }

  VResourceManager* resourceManager = Vision::ResourceSystem.GetResourceManagerByName("Particles");

  if(Vision::Editor.IsPlayingTheGame())
  {
    // only check for already loaded assets when playing. Need to force a full dependency walk otherwise in order to make the "Show Assets" button work in the export window.
    VManagedResource* resource = resourceManager->GetResourceByName(effectName);

    if(resource && 
      resource->IsLoaded())
    {
      return;
    }
  }

  VASSERT(resourceManager);

  if(!resourceManager)
  {
    return;
  }

  VisParticleEffectFile_cl* effectResource = static_cast<VisParticleEffectFile_cl*>(resourceManager->LoadResource(effectName));

  if(effectResource)
  {
    effectResource->GetDependencies(snapshot);
  }
}

void RPG_DependencyHelper::AddModelDependency(const VString& modelName, VResourceSnapshot& snapshot)
{
  if(modelName.IsEmpty())
  {
    return;
  }

  VResourceManager *resourceManager = Vision::ResourceSystem.GetResourceManagerByName(VIS_RESOURCEMANAGER_MESHES);

  if(Vision::Editor.IsPlayingTheGame())
  {
    // only check for already loaded assets when playing. Need to force a full dependency walk otherwise in order to make the "Show Assets" button work in the export window.
    VManagedResource* resource = resourceManager->GetResourceByName(modelName);

    if(resource && 
      resource->IsLoaded())
    {
      return;
    }
  }

  VMeshManager* meshManager = static_cast<VMeshManager*>(resourceManager);

  VDynamicMesh* mesh = meshManager->LoadDynamicMeshFile(modelName);

  if(mesh)
  {
    mesh->GetDependencies(snapshot);
  }
}

void RPG_DependencyHelper::AddTextureDependency(const VString& textureName, VResourceSnapshot& snapshot)
{
  if(textureName.IsEmpty())
  {
    return;
  }

  if(Vision::Editor.IsPlayingTheGame())
  {
    // only check for already loaded assets when playing. Need to force a full dependency walk otherwise in order to make the "Show Assets" button work in the export window.
    VManagedResource* resource = Vision::TextureManager.GetManager().GetResourceByName(textureName);

    if(resource && 
      resource->IsLoaded())
    {
      return;
    }
  }

  VTextureObject* texture = Vision::TextureManager.Load2DTexture(textureName);

  if(texture)
  {
    texture->GetDependencies(snapshot);
  }
}

#endif // SUPPORTS_SNAPSHOT_CREATION

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
