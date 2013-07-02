/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace System::Runtime::Serialization;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace ManagedFramework;
using namespace CSharpFramework::Scene;
using namespace CSharpFramework::View;


#include <Vision/Runtime/EnginePlugins/ThirdParty/FmodEnginePlugin/VFmodManager.hpp>

namespace FmodManaged
{

  // maps defines to C# enum
  public enum class FmodCreationFlags_e
  {
    None            = VFMOD_FLAG_NONE,
    Looped          = VFMOD_FLAG_LOOPED,
    Paused          = VFMOD_FLAG_PAUSED,
    Mute            = VFMOD_FLAG_MUTE,
    NoDispose       = VFMOD_FLAG_NODISPOSE,
    Music           = VFMOD_FLAG_MUSIC,
    Is3D            = VFMOD_RESOURCEFLAG_3D,
    Streaming       = VFMOD_RESOURCEFLAG_STREAM,
    CompressedData  = VFMOD_RESOURCEFLAG_COMPRESSED_DATA,
    LogAttenuation  = VFMOD_RESOURCEFLAG_LOG_ATTENUATION
  };

  // reverb presets
  public enum class FmodReverbPresets_e
  {
    Off,
    Generic,
    PaddedCell,
    Room,
    BathRoom,
    LivingRoom,
    StoneRoom,
    Auditorium,
    ConcertHall,
    Cave,
    Arena,
    Hangar,
    CarpettedHallway,
    Hallway,
    StoneCorridor,
    Alley,
    Forest,
    City,
    Mountains,
    Quarry,
    Plain,
    ParkingLot,
    SewerPipe,
    Underwater,
    Custom,
  };

  // enum for passing through VFMOD_REVERBPRESET_CUSTOM_ENVIRONMENT from C++ to C# code
  public enum class FmodReverbPresetEnvironment_e
  {
    Custom = VFMOD_REVERBPRESET_CUSTOM_ENVIRONMENT
  };

  // maps VFmodReverbProps to FmodReverbProps
  [Serializable]
  public ref class FmodReverbProps: public ISerializable
  {
  public:
    FmodReverbProps()
    {
      m_presetType = FmodReverbPresets_e::Generic;
      m_iEnvironment = -1;                  
      m_iRoom = -1000;
      m_iRoomHF = -100;
      m_iRoomLF = 0; 
      m_fDecayTime = 1.49f;    
      m_fDecayHFRatio = 0.83f;
      m_iReflections = -2602;
      m_fReflectionsDelay = 0.007f;      
      m_iReverb = 200;
      m_fReverbDelay = 0.011f; 
      m_fHFReference = 5000.0f;
      m_fLFReference = 250.0f;          
      m_fDiffusion = 100.0f;
      m_fDensity = 100.0f;
    }

    FmodReverbProps(VFmodReverbProps &properties, FmodReverbPresets_e presetType)
    {
      m_presetType = presetType;
      m_iEnvironment = properties.m_iEnvironment;                  
      m_iRoom = properties.m_iRoom;
      m_iRoomHF = properties.m_iRoomHF;
      m_iRoomLF = properties.m_iRoomLF; 
      m_fDecayTime = properties.m_fDecayTime;    
      m_fDecayHFRatio = properties.m_fDecayHFRatio;
      m_iReflections = properties.m_iReflections;
      m_fReflectionsDelay = properties.m_fReflectionsDelay;      
      m_iReverb = properties.m_iReverb;
      m_fReverbDelay = properties.m_fReverbDelay; 
      m_fHFReference = properties.m_fHFReference;
      m_fLFReference = properties.m_fLFReference;          
      m_fDiffusion = properties.m_fDiffusion;
      m_fDensity = properties.m_fDensity;
    }


    VFmodReverbProps CreateNativeProps()
    {
      return  VFmodReverbProps(m_iEnvironment, m_iRoom, m_iRoomHF, m_iRoomLF, m_fDecayTime, m_fDecayHFRatio, m_iReflections,m_fReflectionsDelay, 
                               m_iReverb, m_fReverbDelay, m_fHFReference, m_fLFReference, m_fDiffusion, m_fDensity);
    }

    FmodReverbProps(SerializationInfo ^info, StreamingContext context)
    {
      m_presetType = (FmodReverbPresets_e)info->GetInt32("_presetType");
      m_iEnvironment = info->GetInt32("_iEnvironment");
      m_iRoom = info->GetInt32("_iRoom");
      m_iRoomHF = info->GetInt32("_iRoomHF");          
      m_iRoomLF = info->GetInt32("_iRoomLF");               
      m_fDecayTime = info->GetSingle("_fDecayTime");              
      m_fDecayHFRatio = info->GetSingle("_fDecayHFRatio");     
      m_iReflections = info->GetInt32("_iReflections");         
      m_fReflectionsDelay = info->GetSingle("_fReflectionsDelay");       
      m_iReverb = info->GetInt32("_iReverb");              
      m_fReverbDelay = info->GetSingle("_fReverbDelay");       
      m_fHFReference = info->GetSingle("_fHFReference");       
      m_fLFReference = info->GetSingle("_fLFReference");              
      m_fDiffusion = info->GetSingle("_fDiffusion");        
      m_fDensity = info->GetSingle("_fDensity");      
    }

    VOVERRIDE void GetObjectData(SerializationInfo ^info, StreamingContext context) = ISerializable::GetObjectData
    {
      info->AddValue("_presetType", m_presetType);
      info->AddValue("_iEnvironment", m_iEnvironment);
      info->AddValue("_iRoom", m_iRoom);
      info->AddValue("_iRoomHF", m_iRoomHF);
      info->AddValue("_iRoomLF", m_iRoomLF);
      info->AddValue("_fDecayTime", m_fDecayTime);
      info->AddValue("_fDecayHFRatio", m_fDecayHFRatio);
      info->AddValue("_iReflections", m_iReflections);
      info->AddValue("_fReflectionsDelay", m_fReflectionsDelay);      
      info->AddValue("_iReverb", m_iReverb);         
      info->AddValue("_fReverbDelay", m_fReverbDelay); 
      info->AddValue("_fHFReference", m_fHFReference); 
      info->AddValue("_fLFReference", m_fLFReference); 
      info->AddValue("_fDiffusion", m_fDiffusion); 
      info->AddValue("_fDensity", m_fDensity);
    }

    FmodReverbPresets_e m_presetType;      
    int m_iEnvironment;                     
    int m_iRoom;               
    int m_iRoomHF;             
    int m_iRoomLF;              
    float m_fDecayTime;             
    float m_fDecayHFRatio;     
    int m_iReflections;        
    float m_fReflectionsDelay;       
    int m_iReverb;             
    float m_fReverbDelay;       
    float m_fHFReference;      
    float m_fLFReference;             
    float m_fDiffusion;       
    float m_fDensity;          
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
