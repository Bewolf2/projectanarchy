/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// ManagedPlugin.cpp : Defines the entry point for the DLL application.

// Note: the dependency on _vcclrit.h has been removed (during the migration to VS2005).
// See the following page for details: http://msdn2.microsoft.com/en-us/library/ms173267(VS.80).aspx
// The _vcclrit.h header file was used in VS2003 for fixing static member initialisation/deinitialisation (MS-KB #814472).

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#using <mscorlib.dll>
using namespace System;


#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>

VIMPORT IVisPlugin_cl* GetEnginePlugin_vHavok();


namespace HavokManaged
{
  // maps defines to C# enum
  public enum class HavokCollisionGroups_e
  {
    Dynamic       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DYNAMIC,
    Static        = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_STATIC,
    Terrain       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_TERRAIN,
    Controller    = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER,
    Ragdoll       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_RAGDOLL,
    Attachments   = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_ATTACHMENTS,
    Foot_IK       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_FOOT_IK,
    Debris        = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_DEBRIS,
    Custom0       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM0,
    Custom1       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM1,
    Custom2       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM2,
    Custom3       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM3,
    Custom4       = vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CUSTOM4
  };

  public ref struct WorldSetupSettings
  {
    float m_havokToVisionScale; //eg 50x, 100x
    int m_staticGeomMode;
    int m_mergedStaticWeldingType;

};

  public ref struct WorldRuntimeSettings
  {
    WorldRuntimeSettings()
    {
      m_collisionGroupMasks = gcnew array<unsigned int>(32);
    }
  
    bool m_broadPhaseSizeAuto;
	  bool m_disableConstrainedBodiesCollisions;
	  bool m_enableLegacyCompoundShapes;
    float m_broadPhaseSizeManual; // in  Vision units, only if above false;
    float m_gravityX;
    float m_gravityY;
    float m_gravityZ;
    bool m_shapeCaching;
    array<unsigned int>^ m_collisionGroupMasks;
  
    int m_solverIterations; // 2,4,8
    int m_solverHardness; // 0,1,2
  };


  public ref class ManagedModule
  {
  public:
  
    static void InitManagedModule()
    {
      //Init the entity plugin directly since we link statically to it
      GetEnginePlugin_vHavok()->InitEnginePlugin();
    }

    static void DeInitManagedModule()
    {
      //Deinit the entity plugin directly since we link statically to it
      GetEnginePlugin_vHavok()->DeInitEnginePlugin();
    }

    static String^ GetVersionInfo()
    {
      return gcnew String( vHavokPhysicsModule_GetVersionString() );
    }

    static bool IsSimulationRunning()
    {
      vHavokPhysicsModule *pInst = vHavokPhysicsModule::GetInstance();
      if (pInst)
        return !pInst->IsPaused();
      else
        return false;
    }

    static bool ToggleSimulation()
    {
      vHavokPhysicsModule *pInst = vHavokPhysicsModule::GetInstance();
      if (pInst)
      {
        bool bPaused = pInst->IsPaused();
        pInst->SetPaused(!bPaused);
        return !bPaused;
      }
      return false;
    }

    static void EnableDebugRendering (bool bRigidBodies, bool bRagdolls, bool bCharacterControlers, bool bTriggerVolumes, bool bStaticMeshes)
    {
      vHavokPhysicsModule *pInst = vHavokPhysicsModule::GetInstance();

      if (pInst)
        pInst->EnableDebugRendering (bRigidBodies, bRagdolls, bCharacterControlers, bTriggerVolumes, bStaticMeshes);
    }

    static void ResetWorldSettings()
    {
      vHavokPhysicsModule_ResetWorldSettings();
    }

    //
    // Only applied after reloading scene 
    //
    static void GetWorldSetupSettings(WorldSetupSettings ^worldSettings)
    {
      vHavokPhysicsModuleDefaultSetupWorldParams& p = vHavokPhysicsModule_GetWorldSetupSettings();
      worldSettings->m_havokToVisionScale = p.m_fHavokToVisionScale;
      worldSettings->m_staticGeomMode = p.m_iStaticGeomMode;
      worldSettings->m_mergedStaticWeldingType = p.m_iMergedStaticWeldingType;
    }

    static void SetWorldSetupSettings(WorldSetupSettings ^worldSettings, bool bSync)
    {
      vHavokPhysicsModuleDefaultSetupWorldParams& p = vHavokPhysicsModule_GetWorldSetupSettings();
      p.m_fHavokToVisionScale = worldSettings->m_havokToVisionScale;
      p.m_iStaticGeomMode = worldSettings->m_staticGeomMode; 
      p.m_iMergedStaticWeldingType = worldSettings->m_mergedStaticWeldingType;
     
      if (bSync)
        vHavokConversionUtils::SetHavok2VisionScale(p.m_fHavokToVisionScale);
    }

    //
    // Can be set while simulation running
    //
    static void GetWorldRuntimeSettings(WorldRuntimeSettings ^worldSettings)
    {
      vHavokPhysicsModuleDefaultRuntimeWorldParams& p = vHavokPhysicsModule_GetDefaultWorldRuntimeSettings();
      worldSettings->m_broadPhaseSizeAuto = p.m_bBroadphaseAutoCompute;
      worldSettings->m_broadPhaseSizeManual = static_cast<float>(p.m_fBroadphaseManualSize);
      worldSettings->m_gravityX = p.m_vGravity.x;
      worldSettings->m_gravityY = p.m_vGravity.y;
      worldSettings->m_gravityZ = p.m_vGravity.z;
      worldSettings->m_shapeCaching = p.m_bEnableShapeCaching;
      worldSettings->m_disableConstrainedBodiesCollisions	= p.m_bEnableConstraintCollisionFilter;
	    worldSettings->m_enableLegacyCompoundShapes = p.m_bEnableLegacyCompoundShapes;

      for (int i=0;i<32;i++)
        worldSettings->m_collisionGroupMasks[i] = p.m_iCollisionGroupMasks[i];

      worldSettings->m_solverIterations = p.m_solverIterations;
      worldSettings->m_solverHardness = p.m_solverHardness;
   
    }

    static void SetWorldRuntimeSettings(WorldRuntimeSettings ^worldSettings)
    {
      // See if we have a running instance, as can just set on it to see immediate effect if there
      vHavokPhysicsModuleDefaultRuntimeWorldParams& p = vHavokPhysicsModule_GetDefaultWorldRuntimeSettings();
      p.m_bBroadphaseAutoCompute = worldSettings->m_broadPhaseSizeAuto;
      p.m_fBroadphaseManualSize = worldSettings->m_broadPhaseSizeManual;
      hkvVec3 g(worldSettings->m_gravityX, worldSettings->m_gravityY, worldSettings->m_gravityZ);
      p.m_vGravity = g;
      p.m_bEnableShapeCaching = worldSettings->m_shapeCaching; 
	    p.m_bEnableConstraintCollisionFilter	= worldSettings->m_disableConstrainedBodiesCollisions;
	    p.m_bEnableLegacyCompoundShapes	= worldSettings->m_enableLegacyCompoundShapes;
      for (int i=0;i<32;i++)
        p.m_iCollisionGroupMasks[i] = worldSettings->m_collisionGroupMasks[i];

      p.m_solverIterations = worldSettings->m_solverIterations;
      p.m_solverHardness = worldSettings->m_solverHardness;
   
      vHavokPhysicsModule *pInst = vHavokPhysicsModule::GetInstance();
      if (pInst)
      {
        // update broadphase size 
        if (p.m_bBroadphaseAutoCompute)
          pInst->SetBroadphaseSizeAuto();
        else
          pInst->SetBroadphaseSizeManual(p.m_fBroadphaseManualSize);

        // update gravity
        pInst->SetGravity(g);

        // update solver
        pInst->SetSolver(p.m_solverIterations, p.m_solverHardness);

        // update groups collision
        pInst->UpdateGroupsCollision();
      }
    }
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
