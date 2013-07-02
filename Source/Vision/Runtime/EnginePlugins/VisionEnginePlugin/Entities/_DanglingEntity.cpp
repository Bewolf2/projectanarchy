/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

// Class for a dangling entity

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/_DanglingEntity.hpp>
#include <Vision/Runtime/Engine/Animation/VisApiVertexAnimDeformer.hpp>
#include <Vision/Runtime/Engine/SceneElements/VisApiPath.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

extern VModule g_VisionEngineModule;

V_IMPLEMENT_SERIAL( DanglingEntity_cl, VisBaseEntity_cl, 0, &g_VisionEngineModule );


DanglingEntity_cl::DanglingEntity_cl()
{
  Vision::Callbacks.OnAfterSceneLoaded += this;
}

DanglingEntity_cl::~DanglingEntity_cl()
{
  Vision::Callbacks.OnAfterSceneLoaded -= this;
}


// *** The InitFunction will be called by the engine after the entity has been created
void DanglingEntity_cl::InitFunction()
{
  //init the time with a random value to a void syncronized
  //movement of different dangling objects
  m_fTime = Vision::Game.GetFloatRand();

  m_fTimeOffset = 0.1f + Vision::Game.GetFloatRand() * 0.3f;   

  //get information about the object
  m_vInitialOrientation = GetOrientation();
  m_vInitialPosition = GetPosition();

  //consider the dimension
  hkvAlignedBBox bBox;
  GetCurrentVisBoundingBox(bBox);
  m_vHookPosition = hkvVec3(0, 0, -bBox.getSizeZ() * 0.5f);

  CommonInit();
}

void DanglingEntity_cl::CommonInit() 
{
  m_fInfluenceY = hkvMath::sinDeg (Angle);
  m_fInfluenceX = hkvMath::cosDeg (Angle);
}

void DanglingEntity_cl::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{
  if ( iID == VIS_MSG_EDITOR_PROPERTYCHANGED )
  {
    CommonInit(); // the editor changed parameter
    return;
  }

  VisBaseEntity_cl::MessageFunction( iID, iParamA, iParamB );
}


void DanglingEntity_cl::ThinkFunction()
{
  m_fTime += Vision::GetTimer()->GetTimeDifference() * Speed;

  //set back the time (from time to time ;-) )
  if(m_fTime > hkvMath::pi () * 32.f) 
    m_fTime -= hkvMath::pi () * 32.f;

  //wave_1 * (1-Random) + wave_2 * Random
  float fWave = hkvMath::sinRad (m_fTime)*(1-Random) + hkvMath::sinRad (m_fTimeOffset+m_fTime*m_fTimeOffset)*Random;
  
  //set the current orientation
  SetOrientation (m_vInitialOrientation + hkvVec3 (fWave*Rotation, m_fInfluenceY*fWave*Deflection, m_fInfluenceX*fWave*Deflection));

  //and adjust the position according to the made rotation
  hkvMat3 rotMat;
  GetRotationMatrix(rotMat);
  SetPosition(m_vInitialPosition + (rotMat * m_vHookPosition - m_vHookPosition));
}

void DanglingEntity_cl::Serialize( VArchive &ar )
{
  VisBaseEntity_cl::Serialize(ar);
  if (ar.IsLoading())
  {
    char iVersion;
    ar >> iVersion; VVERIFY (iVersion==1 && "Invalid entity version number. Please re-export scene");
    ar >> Speed;
    ar >> Deflection;
    ar >> Angle;
    ar >> Random;
    ar >> Rotation;
    m_vInitialOrientation.SerializeAsVisVector (ar);
    m_vInitialPosition.SerializeAsVisVector (ar);

    SetPosition(m_vInitialPosition);
    SetOrientation(m_vInitialOrientation);

    InitFunction();
  } 
  else
  {
    ar << (char )1; // version
    ar << Speed;
    ar << Deflection;
    ar << Angle;
    ar << Random;
    ar << Rotation;
    m_vInitialOrientation.SerializeAsVisVector (ar);
    m_vInitialPosition.SerializeAsVisVector (ar);

  }
}



// *** The variable table is used by the plugin system of the engine and editor. In that case we do not have
// *** variables which have to be initialised or which have to be available in the editor.
START_VAR_TABLE(DanglingEntity_cl,VisBaseEntity_cl,"dangling entity",VFORGE_HIDECLASS, NULL )
  DEFINE_VAR_FLOAT(DanglingEntity_cl, Speed, "Animation Speed", "1.0", 0, "Slider(0,30)");
  DEFINE_VAR_FLOAT(DanglingEntity_cl, Deflection, "Max. deflection", "3.0",  0, "Slider(0,30)");
  DEFINE_VAR_FLOAT(DanglingEntity_cl, Angle, "Defines the dangle direction (z axis fix)", "0.0", 0, "Slider(0,90)");
  DEFINE_VAR_FLOAT(DanglingEntity_cl, Rotation, "Amount of rotation around the z axis", "0.0", 0, "Slider(-90,90)");
  DEFINE_VAR_FLOAT(DanglingEntity_cl, Random, "Random motion influence", "0.25", 0, "Slider(0,1)");
END_VAR_TABLE

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
