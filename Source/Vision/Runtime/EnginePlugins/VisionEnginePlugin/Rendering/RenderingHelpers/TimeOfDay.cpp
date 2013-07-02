/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/Curve.hpp>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/RenderingHelpers/TimeOfDay.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


VTimeOfDayFactory VTimeOfDayFactory::g_Instance;


VTimeOfDay::VTimeOfDay(float fDayTime, float fSunRiseTime, float fSunSetTime)
{
  m_fFogRange = 10000.f; // backwards compatibility
  m_fFogStartDistance = 0.f;
  m_fDayTime = fDayTime;
  m_fSunRiseTime = fSunRiseTime;
  m_fSunSetTime = fSunSetTime;
  m_bDayNightSwitch = false;
  SetDefaultCurves();
  SetSunPathOrientation(0.f,0.f,-57.f);
  m_bControlSky = true;
  m_bOldOrientationStyle = true; // backwards compatibility (set after SetSunPathOrientation)

  m_fHeightFogRange = 10000.0f;
  m_fHeightFogStartDistance = 0.0f;
  m_fHeightFogDensityScale = 1.0f;
  m_fHeightFogHalfDensityHeightScale = 1.0f;
}

VTimeOfDay::~VTimeOfDay()
{
  m_spAmbientColor = NULL;
  m_spFullbrightColor = NULL;
  m_spDawnWeight = NULL;
  m_spDuskWeight = NULL;
  m_spNightWeight = NULL;

  m_spFogStart = NULL;
  m_spFogEnd = NULL;
  m_spFogColor = NULL;
  m_spHeightFogStart = NULL;
  m_spHeightFogEnd = NULL;
  m_spHeightFogColor = NULL;
  m_spHeightFogDensity = NULL;
  m_spHeightFogHalfDensityHeight = NULL;
}

void VTimeOfDay::SetSunPathOrientation(float fYaw, float fPitch, float fRoll)
{
  m_vSunPathEuler.set (fYaw, fPitch, fRoll);
  m_SunPathOrientation.setFromEulerAngles (fRoll, fPitch, fYaw);
}

void VTimeOfDay::SetDayTime(float fTime)
{ 
  m_fDayTime = fTime; 
  if (m_fDayTime>1.0f) 
    m_fDayTime = hkvMath::mod (m_fDayTime,1.0f); 
  else if (m_fDayTime<0.0f) 
    SetDayTime(hkvMath::mod (m_fDayTime,1.0f)+1.f);

  InvalidateLightGridColors();
}

float VTimeOfDay::GetDayTime()
{
  return m_fDayTime;
}

void VTimeOfDay::DeInit()
{
  m_spAmbientColor = NULL;
  m_spFullbrightColor = NULL;
  m_spDawnWeight = NULL;
  m_spDuskWeight = NULL;
  m_spNightWeight = NULL;

  m_spFogStart = NULL;
  m_spFogEnd = NULL;
  m_spFogColor = NULL;
  m_spHeightFogStart = NULL;
  m_spHeightFogEnd = NULL;
  m_spHeightFogColor = NULL;
  m_spHeightFogDensity = NULL;
  m_spHeightFogHalfDensityHeight = NULL;
}


void VTimeOfDay::EvaluateSunDir(hkvVec3& pos)
{
  EvaluateSunDirAtDaytime(pos, m_fDayTime);
}


void VTimeOfDay::EvaluateSunDirAtDaytime(hkvVec3& pos, float fTime)
{
  // Time of Day [0..1] converted for sinus / cosinus 
  float w = fTime * (hkvMath::pi () * 2.0f);

  // Parameter a: Shift the respective curve (here cosinus) on the X axis
  float a = -(m_fSunRiseTime + m_fSunSetTime) * 2.0f * hkvMath::pi ();

  // Parameter b: Shift the respective curve (here cosinus) on the Y axis
  float b = hkvMath::cosRad ((m_fSunRiseTime - m_fSunSetTime) * hkvMath::pi ());

  if (m_bOldOrientationStyle)
  {
    pos.x = hkvMath::sinRad (w);
    pos.y = hkvMath::cosRad (w);
    pos.z = -1.5f*hkvMath::cosRad (w) + 0.2f; // we want 12:00 to be high noon :-)

    // the direction should always be normalized so we can use it directly in vertex programs
    pos.normalizeIfNotZero();
  } 
  else
  {
    pos.set(hkvMath::sinRad (w),hkvMath::cosRad (w + a) + b,0.f);
    pos = m_SunPathOrientation * pos;
  }
}


void VTimeOfDay::EvaluateColorValue(float falloff, VColorRef &iColor, float &fDawnWeight, float &fDuskWeight, float &fNightWeight)
{
  VASSERT(falloff>=0.f && falloff<=1.f);

  float fx1 = 0.5f + 0.5f*hkvMath::cosRad (falloff * hkvMath::pi ());
  float fx2 = 1.f-fx1;

  float fColor[3];
  fColor[0] = m_spAmbientColor->GetCurveR().GetValue(m_fDayTime) * fx1 +
    m_spFullbrightColor->GetCurveR().GetValue(m_fDayTime) * fx2;
  fColor[1] = m_spAmbientColor->GetCurveG().GetValue(m_fDayTime) * fx1 +
    m_spFullbrightColor->GetCurveG().GetValue(m_fDayTime) * fx2;
  fColor[2] = m_spAmbientColor->GetCurveB().GetValue(m_fDayTime) * fx1 +
    m_spFullbrightColor->GetCurveB().GetValue(m_fDayTime) * fx2;

  iColor.SetRGBA((UBYTE)(fColor[0]*255.0f), (UBYTE)(fColor[1]*255.0f),
    (UBYTE)(fColor[2]*255.0f), 255);

  fDawnWeight   = m_spDawnWeight->GetValue(m_fDayTime);
  fDuskWeight   = m_spDuskWeight->GetValue(m_fDayTime);
  fNightWeight  = m_spNightWeight->GetValue(m_fDayTime);
}

void VTimeOfDay::UpdateFogParameters()
{
  VFogParameters fog = Vision::World.GetFogParameters();
  if (fog.depthMode == VFogParameters::TimeOfDay)
  {
    EvaluateDepthFog(fog.iDepthColor, fog.fDepthStart, fog.fDepthEnd);
  }
  if (fog.heightFogMode == VFogParameters::TimeOfDay)
  {
    EvaluateHeightFog(fog.iHeightFogColor, fog.fHeightFogStart, fog.fHeightFogEnd, fog.fHeightFogDensity, fog.fHeightFogHalfDensityHeight);
  }
  Vision::World.SetFogParameters(fog);
}

void VTimeOfDay::EvaluateDepthFog(VColorRef &iFogColor, float &fogStart, float &fogEnd)
{
  const int sampleCount = 24; // number of daytime samples
  const float timeMult = (float)(sampleCount);

  float fTime = m_fDayTime;

  fTime *= timeMult;
  int sample = ((int)fTime)%sampleCount;
  int nextSample = (sample+1)%sampleCount;

  // linearely interpolate in y-direction
  float fy2 = hkvMath::mod (fTime,1.f);
  float fy1 = 1.f-fy2;

  VisBitmap_cl* bitmap = m_spFogColor->GetLookupBitmap();
  VColorRef fc1 = *bitmap->GetDataPtr(sample, 0);
  VColorRef fc2 = *bitmap->GetDataPtr(nextSample, 0);
  
  iFogColor = fc1*fy1 + fc2*fy2;

  //TODO: fast lookup?
  fogStart = m_fFogStartDistance + m_spFogStart->GetValue(m_fDayTime) * m_fFogRange;
  fogEnd = m_fFogStartDistance + m_spFogEnd->GetValue(m_fDayTime) * m_fFogRange;
}

void VTimeOfDay::EvaluateHeightFog(VColorRef &iFogColor, float &fogStart, float &fogEnd, float &fogDensity, float &halfDensityHeight)
{
  const int sampleCount = 24; // number of daytime samples
  const float timeMult = (float)(sampleCount);

  float fTime = m_fDayTime;

  fTime *= timeMult;
  int sample = ((int)fTime)%sampleCount;
  int nextSample = (sample+1)%sampleCount;

  // linearely interpolate in y-direction
  float fy2 = hkvMath::mod (fTime,1.f);
  float fy1 = 1.f-fy2;

  VisBitmap_cl* bitmap = m_spHeightFogColor->GetLookupBitmap();
  VColorRef fc1 = *bitmap->GetDataPtr(sample, 0);
  VColorRef fc2 = *bitmap->GetDataPtr(nextSample, 0);
  
  iFogColor = fc1*fy1 + fc2*fy2;

  //TODO: fast lookup?
  fogStart = m_fHeightFogStartDistance + m_spHeightFogStart->GetValue(m_fDayTime) * m_fHeightFogRange;
  fogEnd = m_fHeightFogStartDistance + m_spHeightFogEnd->GetValue(m_fDayTime) * m_fHeightFogRange;
  fogDensity = m_spHeightFogDensity->GetValue(m_fDayTime) * m_fHeightFogDensityScale;
  halfDensityHeight = m_spHeightFogHalfDensityHeight->GetValue(m_fDayTime) * m_fHeightFogHalfDensityHeightScale;
}

void VTimeOfDay::SetDefaultCurves()
{
  m_spAmbientColor = new VColorCurve();
  VCurvePoint2D ambientColorRG[] =
  {
    {hkvVec2(0.0f, 45.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 96.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 45.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  VCurvePoint2D ambientColorB[] =
  {
    {hkvVec2(0.0f, 45.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 128.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 45.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spAmbientColor->GetCurveR().SetPoints(ambientColorRG, 3);
  m_spAmbientColor->GetCurveG().SetPoints(ambientColorRG, 3);
  m_spAmbientColor->GetCurveB().SetPoints(ambientColorB, 3);

  m_spFullbrightColor = new VColorCurve();
  VCurvePoint2D fullbrightColorR[] =
  {
    {hkvVec2(0.0f, 64.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 192.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 99.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  VCurvePoint2D fullbrightColorG[] =
  {
    {hkvVec2(0.0f, 64.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 192.0f/255.0f), hkvVec2(-0.2f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 88.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  VCurvePoint2D fullbrightColorB[] =
  {
    {hkvVec2(0.0f, 92.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 223.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 88.0f/255.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spFullbrightColor->GetCurveR().SetPoints(fullbrightColorR, 3);
  m_spFullbrightColor->GetCurveG().SetPoints(fullbrightColorG, 3);
  m_spFullbrightColor->GetCurveB().SetPoints(fullbrightColorB, 3);

  m_spDawnWeight = new VCurve2D();
  VCurvePoint2D dawnWeight[] = 
  {
    {hkvVec2(0.0f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.5f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.75f, 1.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spDawnWeight->SetPoints(dawnWeight, 4);

  m_spDuskWeight = new VCurve2D();
  VCurvePoint2D duskWeight[] = 
  {
    {hkvVec2(0.0f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.25f, 0.75f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.5f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spDuskWeight->SetPoints(duskWeight, 4);

  m_spNightWeight = new VCurve2D();
  VCurvePoint2D nightWeight[] = 
  {
    {hkvVec2(0.0f, 1.0f), hkvVec2(-0.2f, 0.0f), hkvVec2(0.2f, 0.0f)},
    {hkvVec2(0.35f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.65f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 1.0f), hkvVec2(-0.2f, 0.0f), hkvVec2(0.2f, 0.0f)}
  };
  m_spNightWeight->SetPoints(nightWeight, 4);

  m_spFogColor = new VColorCurve();
  VCurvePoint2D fogColor[] =
  {
    {hkvVec2(0.0f, 0.125f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 0.498f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 0.192f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spFogColor->GetCurveR().SetPoints(fogColor, 3);
  m_spFogColor->GetCurveG().SetPoints(fogColor, 3);
  m_spFogColor->GetCurveB().SetPoints(fogColor, 3);

  m_spHeightFogColor = new VColorCurve();
  m_spHeightFogColor->GetCurveR().SetPoints(fogColor, 3);
  m_spHeightFogColor->GetCurveG().SetPoints(fogColor, 3);
  m_spHeightFogColor->GetCurveB().SetPoints(fogColor, 3);

  m_spFogStart = new VCurve2D();
  VCurvePoint2D fogStart[] = 
  {
    {hkvVec2(0.0f, 0.1406f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.3f, 0.0044f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 0.5f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 0.1747f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };    
  m_spFogStart->SetPoints(fogStart, 4);

  m_spFogEnd = new VCurve2D();
  VCurvePoint2D fogEnd[] = 
  {
    {hkvVec2(0.0f, 0.25f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.3f, 0.1484f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.558f, 0.8828f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 0.3148f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spFogEnd->SetPoints(fogEnd, 4);

  m_spHeightFogStart = new VCurve2D();
  VCurvePoint2D heightFogStart[] = 
  {
    {hkvVec2(0.0f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.5f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 0.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spHeightFogStart->SetPoints(heightFogStart, 3);

  m_spHeightFogEnd = new VCurve2D();
  VCurvePoint2D heightFogEnd[] = 
  {
    {hkvVec2(0.0f, 1.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.5f, 1.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 1.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spHeightFogEnd->SetPoints(heightFogEnd, 3);

  VCurvePoint2D fogDensity[] =
  {
    {hkvVec2(0.0f, 0.001f),  hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.5f, 0.0001f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 0.001f),  hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spHeightFogDensity = new VCurve2D();
  m_spHeightFogDensity->SetPoints(fogDensity, 3);

  VCurvePoint2D fogHalfDensityHeight[] =
  {
    {hkvVec2(0.0f, 1500.0f),  hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(0.5f, 1000.0f), hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)},
    {hkvVec2(1.0f, 1500.0f),  hkvVec2(-0.1f, 0.0f), hkvVec2(0.1f, 0.0f)}
  };
  m_spHeightFogHalfDensityHeight = new VCurve2D();
  m_spHeightFogHalfDensityHeight->SetPoints(fogHalfDensityHeight, 3);
  
  m_spAmbientColor->UpdateCurve();
  m_spFullbrightColor->UpdateCurve();
  m_spDawnWeight->UpdateCurve();
  m_spDuskWeight->UpdateCurve();
  m_spNightWeight->UpdateCurve();

  m_spFogColor->UpdateCurve();
  m_spFogColor->CreateLookup(24);
  m_spFogStart->UpdateCurve();
  m_spFogEnd->UpdateCurve();

  m_spHeightFogColor->UpdateCurve();
  m_spHeightFogColor->CreateLookup(24);
  m_spHeightFogStart->UpdateCurve();
  m_spHeightFogEnd->UpdateCurve();
  m_spHeightFogDensity->UpdateCurve();
  m_spHeightFogHalfDensityHeight->UpdateCurve();
}


void VTimeOfDay::GetSunDirection(hkvVec3& vDirection)
{
  EvaluateSunDir(vDirection);
  vDirection *= -1.0f;
  vDirection.normalizeIfNotZero();
}

void VTimeOfDay::GetSunDirectionAtDayTime(hkvVec3& vDirection, float fTime)
{
  EvaluateSunDirAtDaytime(vDirection, fTime);
  vDirection *= -1.0f;
  vDirection.normalizeIfNotZero();
}

VColorRef VTimeOfDay::GetSunColor()
{
  VColorRef iColor;
  float fDummy;
  EvaluateColorValue(1.0f, iColor, fDummy, fDummy, fDummy);

  hkvVec3 vDirection(hkvNoInitialization);
  EvaluateSunDir(vDirection);
  vDirection *= -1.0f;
  vDirection.normalizeIfNotZero();
  float fBelowHorizonMultiplier = hkvMath::pow (hkvMath::Max(-vDirection.z, 0.0f), 0.65f);

  hkvVec3 vColor = iColor.ToFloat();
  vColor *= fBelowHorizonMultiplier;
  iColor.FromFloat(vColor);

  return iColor;
}

VColorRef VTimeOfDay::GetAmbientColor()
{
  VColorRef iColor;
  float fDummy;
  EvaluateColorValue(0.1f, iColor, fDummy, fDummy, fDummy);
  return iColor;
}





V_IMPLEMENT_SERIAL( VTimeOfDay, VisTypedEngineObject_cl, 0, &g_VisionEngineModule );

void VTimeOfDay::Serialize( VArchive &ar )
{
  char iLocalVersion = TIMEOFDAY_VERSION_CURRENT;
  if (ar.IsLoading())
  {
    ar >> iLocalVersion; VASSERT_MSG(iLocalVersion <= TIMEOFDAY_VERSION_CURRENT,"Invalid version number");

    ar >> m_fDayTime;
    ar >> m_fSunRiseTime;
    ar >> m_fSunSetTime;  
    ar >> m_bDayNightSwitch;
    if (iLocalVersion>=TIMEOFDAY_VERSION_3)
      ar >> m_bControlSky;

    if (iLocalVersion < TIMEOFDAY_VERSION_7)
    {
      int iWeatherType; // unused
      ar >> iWeatherType;
    }

    if (iLocalVersion>=TIMEOFDAY_VERSION_2)
      ar >> m_fFogRange;
    if (iLocalVersion>=TIMEOFDAY_VERSION_4)
      ar >> m_fFogStartDistance;
    if (iLocalVersion>=TIMEOFDAY_VERSION_5)
      ar >> m_bOldOrientationStyle;
    if (iLocalVersion>=TIMEOFDAY_VERSION_6)
    {
      ar >> m_fHeightFogRange;
      ar >> m_fHeightFogStartDistance;
      ar >> m_fHeightFogDensityScale;
      ar >> m_fHeightFogHalfDensityHeightScale;
    }
  }
  else
  {
    ar << iLocalVersion;
    ar << m_fDayTime;
    ar << m_fSunRiseTime;
    ar << m_fSunSetTime;  
    ar << m_bDayNightSwitch << m_bControlSky;
    ar << m_fFogRange;                         // version 2
    ar << m_fFogStartDistance;                 // version 4
    ar << m_bOldOrientationStyle;              // version 5
    ar << m_fHeightFogRange;                   // version 6
    ar << m_fHeightFogStartDistance;           // version 6
    ar << m_fHeightFogDensityScale;            // version 6
    ar << m_fHeightFogHalfDensityHeightScale;  // version 6
  }

  m_spAmbientColor->SerializeX(ar);
  m_spFullbrightColor->SerializeX(ar);

  // in theory we could skip this serialization block if m_bControlSky is false
  m_spDawnWeight->SerializeX(ar);
  m_spNightWeight->SerializeX(ar);
  if (iLocalVersion>=TIMEOFDAY_VERSION_3)
    m_spDuskWeight->SerializeX(ar);

  m_spFogStart->SerializeX(ar);
  m_spFogEnd->SerializeX(ar);
  m_spFogColor->SerializeX(ar);

  if ( iLocalVersion >= TIMEOFDAY_VERSION_6 )
  {
    m_spHeightFogStart->SerializeX(ar);
    m_spHeightFogEnd->SerializeX(ar);
    m_spHeightFogDensity->SerializeX(ar);
    m_spHeightFogHalfDensityHeight->SerializeX(ar);
    m_spHeightFogColor->SerializeX(ar);
  }

  if(iLocalVersion >= TIMEOFDAY_VERSION_1)
    m_spDuskWeight->SerializeX(ar);

  if(iLocalVersion >= TIMEOFDAY_VERSION_5 && !m_bOldOrientationStyle)
  {
    hkvVec4 vSun = m_vSunPathEuler.getAsVec4 (0.0f);
    SerializeX (ar, vSun);
    m_vSunPathEuler = vSun.getAsVec3 ();

    SetSunPathOrientation(m_vSunPathEuler.x, m_vSunPathEuler.y, m_vSunPathEuler.z);
  }
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
