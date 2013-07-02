/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VScriptRenderer_wrapper.hpp

#ifndef __VSCRIPTUTIL_WRAPPER_HPP
#define __VSCRIPTUTIL_WRAPPER_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VScriptConstants.hpp>

/// 
/// \brief
///   Small wrapper to unify utility functions inside the scripting binding
/// 
class VScriptUtil_wrapper
{
public:
  VScriptUtil_wrapper() {}
  ~VScriptUtil_wrapper() {}

  inline int GetRandInt(int iMaxRand = VRAND_MAX) 
  { 
    VASSERT_MSG(iMaxRand>=-VRAND_MAX && iMaxRand<=VRAND_MAX, "You exceeded RAND_MAX, random number will be invalid!");

    if(iMaxRand>0)      return Vision::Game.GetRand()%iMaxRand;
    else if(iMaxRand<0) return -Vision::Game.GetRand()%iMaxRand;
 
    return 0;
  }

  inline int GetRandIntSym(int iMaxRand = VRAND_MAX_2) 
  { 
    VASSERT_MSG(iMaxRand>=-VRAND_MAX_2 && iMaxRand<=VRAND_MAX_2, "Your number range exceeded RAND_MAX, random number will be invalid!");
    int iRange = iMaxRand<<1; //multiply by 2

    if(iMaxRand>0)      return Vision::Game.GetRand()%iRange-iMaxRand;
    else if(iMaxRand<0) return -Vision::Game.GetRand()%iRange-iMaxRand;

    return 0;
  }

  inline float GetRandFloat(float fMaxRand = 1.0f) 
  {
    return ((float)Vision::Game.GetRand())*fMaxRand*VRAND_MAX_INV;
  }

  inline float GetRandFloatSym(float fMaxRand = 1.0f) 
  { 
    return ((float)Vision::Game.GetRand())*fMaxRand*2.f*VRAND_MAX_INV - fMaxRand;
  }

  inline hkvVec3 GetRandVector(float fMaxX, float fMaxY, float fMaxZ)
  {
    return hkvVec3(GetRandFloat(fMaxX),GetRandFloat(fMaxY),GetRandFloat(fMaxZ));
  }

  inline hkvVec3 GetRandVectorSym(float fMaxX, float fMaxY, float fMaxZ)
  {
    return hkvVec3(GetRandFloatSym(fMaxX),GetRandFloatSym(fMaxY),GetRandFloatSym(fMaxZ));
  }

  inline int Round(float fNumber)
  {
    return (fNumber >= 0) ? (int)(fNumber + 0.5f) : (int)(fNumber - 0.5f);
  }

  inline float Round(float fNumber, int iDecimalPlaces)
  {
    if(iDecimalPlaces<1) return (float)Round(fNumber);
    if(iDecimalPlaces>9) iDecimalPlaces = 9; //otherwise we will exceed our number limits

    int iMultiplier = (int)pow(10.f, iDecimalPlaces);
    double dNumber = fNumber;

    long lNumber = (fNumber >= 0) ? (long)(iMultiplier*dNumber + 0.5) : (long)(iMultiplier*dNumber - 0.5);

    return (float)((double)lNumber/(double)iMultiplier);
  }

  inline float RadToDeg(float radValue)
  {
    return hkvMath::Rad2Deg(radValue);
  }

  inline float DegToRad(float degValue)
  {
    return hkvMath::Deg2Rad(degValue);
  }

  inline float AngleBetweenVectors(const hkvVec3& vec1, const hkvVec3& vec2)
  {
    return vec1.getAngleBetween (vec2);
  }

  inline hkvMat3 CreateLookAtMatrix( const hkvVec3* pPos, const hkvVec3* pTarget, const hkvVec3* pUp = NULL )
  {
    if(pPos && pTarget)
    {
      hkvMat3 matrix (hkvNoInitialization);

      if(pUp) 
        matrix.setLookInDirectionMatrix (*pTarget - *pPos, *pUp);
      else    
        matrix.setLookInDirectionMatrix (*pTarget - *pPos);

      return hkvMat3 (matrix);
    }

    return hkvMat3 ();
  }

};

#endif // __VSCRIPTUTIL_WRAPPER_HPP

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
