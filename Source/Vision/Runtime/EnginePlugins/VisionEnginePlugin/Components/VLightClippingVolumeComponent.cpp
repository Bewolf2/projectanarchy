/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Components/VLightClippingVolumeComponent.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Entities/VCustomVolumeObject.hpp>

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(VLightClippingVolumeComponent,IVObjectComponent,0,&g_VisionEngineModule);

VLightClippingVolumeComponent::VLightClippingVolumeComponent(int iComponentFlags /*=VIS_OBJECTCOMPONENTFLAG_NONE*/) 
: IVObjectComponent(0, iComponentFlags), ClipHandedness(VCH_Outside)
{

}

VLightClippingVolumeComponent::~VLightClippingVolumeComponent()
{

}

void VLightClippingVolumeComponent::Serialize( VArchive &ar )
{
  if(ar.IsLoading())
  {
    int iVersion = -1;
    ar >> iVersion;
    VASSERT(iVersion>0 && iVersion<=VV_VersionCurrent);
    ar >> Volume;
    if (iVersion >= VV_Version2)
    {
      int iClipHandedness;
      ar >> iClipHandedness;
      ClipHandedness = (VClipHandedness_e)iClipHandedness;
    }
  }
  else
  {
    ar << (int)VV_VersionCurrent;
    ar << Volume;
    ar << (int)ClipHandedness; 
  }
}

BOOL VLightClippingVolumeComponent::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
  if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
    return FALSE;

  if (!pObject->IsOfType(V_RUNTIME_CLASS(VisLightSource_cl)))
  {
    sErrorMsgOut = "Component can only be added to instances of VisLightSource_cl or derived classes.";
    return FALSE;
  }

  return TRUE;
}

/// =========================================================================== ///
/// VLightClippingVolumeComponent Variable Table                                ///
/// =========================================================================== ///

START_VAR_TABLE(VLightClippingVolumeComponent,IVObjectComponent, "Light Clipping Volume Component. Can be attached to light sources in deferred rendering to limit their extend.", VVARIABLELIST_FLAGS_NONE, "Light Clipping Volume" )
  DEFINE_VAR_OBJECT_REFERENCE(VLightClippingVolumeComponent, Volume, "The volume to be used for clipping the lights extend", NULL, 0, 0);
  DEFINE_VAR_ENUM(VLightClippingVolumeComponent, ClipHandedness, "This setting determines whether geometry outside or inside the light clipping volume will be clipped. 'Outside' is the more common option.", "Outside", "Outside,Inside", 0, 0);
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
