/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiDataHistory.hpp

#ifndef DEFINE_VISAPIDATAHISTORY
#define DEFINE_VISAPIDATAHISTORY

#include <Vision/Runtime/Base/Math/Vector/hkvVec3.h>
#include <Vision/Runtime/Base/Math/Vector/hkvVec4.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat3.h>
#include <Vision/Runtime/Base/Math/Matrix/hkvMat4.h>
#include <Vision/Runtime/Base/Graphics/VColor.hpp>

/// \brief 
///   A data entry for the VisApiDataHistory.
///
/// This is the data entry to be interpolated. dataType must have to following operators:
/// - dataType * float
/// - dataType + dataType
/// - dataType - dataType
/// .
///
/// \sa VisDataHistory_cl
template <typename dataType>
struct VisDataHistoryCell
{
  /// \brief
  ///   Constructor
  VisDataHistoryCell ()
    : m_iTime(0), m_data() {}

  /// \brief
  ///   Constructor
  VisDataHistoryCell (__int64 iTime, const dataType& data)
    : m_iTime(iTime), m_data(data) {}

  __int64 m_iTime;
  dataType m_data;
};


/// \brief 
///   The default implementation of the Mix function in VisDataHistory_cl
///
template<class dataType>
class VLerp
{
public:
  dataType operator() (const dataType& data1, const dataType& data2, float fFactor)
  {
    return data1 * (1.0f - fFactor) + data2 * fFactor;
  }
};


/// \brief 
///   A specialized implementation of the Mix function in VisDataHistory_cl for non interpolatable data.
template<class dataType>
class VStepFunction
{
public:
  dataType operator() (const dataType& data1, const dataType& data2, float fFactor)
  {
    if (fFactor < 1.0)
      return data1;
    else
      return data2;
  }
};


/// \brief 
///   A specialized implementation of the Mix function in VisDataHistory_cl for Euler angles
class VLerpEulerAngles
{
public:

  hkvVec3 operator() (const hkvVec3& data1, const hkvVec3& data2, float fFactor)
  {
    hkvVec3 diff = data2 - data1;
    hkvVec3 diff2 = data2 - (data1 + hkvVec3(360.0f, 360.0f, 360.0f));
    hkvVec3 diff3 = data2 - (data1 - hkvVec3(360.0f, 360.0f, 360.0f));

    if (hkvMath::Abs (diff.x) > hkvMath::Abs (diff2.x))
      diff.x = diff2.x;
    if (hkvMath::Abs (diff.y) > hkvMath::Abs (diff2.y))
      diff.y = diff2.y;
    if (hkvMath::Abs (diff.z) > hkvMath::Abs (diff2.z))
      diff.z = diff2.z;

    if (hkvMath::Abs (diff.x) > hkvMath::Abs (diff3.x))
      diff.x = diff3.x;
    if (hkvMath::Abs (diff.y) > hkvMath::Abs (diff3.y))
      diff.y = diff3.y;
    if (hkvMath::Abs (diff.z) > hkvMath::Abs (diff3.z))
      diff.z = diff3.z;

    return data1 + diff * fFactor;
  }
};

/// \brief 
///   A specialized implementation of the Mix function in \sa VisDataHistory_cl for bitmasks
class VLerpBool
{
public:
  bool operator() (const bool& data1, const bool& data2, float fFactor)
  {
    if (fFactor < 1)
      return data1;
    else
      return data2;
  }
};

/// \brief
///   Return value of the Read function, defines the way the returned value was computed.
enum VReadResult_e
{
  VRR_None,         ///< There is no value in the buffer or the values lie to far in the past to savely extrapolate.
  VRR_Oldest,       ///< We are reading to far in the past, entires are either not there yet or already fallen out of the ring buffer, taking earliest known value.
  VRR_Extrapolated, ///< We are reading further ahead in time as we have data, data is extrapolated.
  VRR_Interpolated  ///< We have a previous and a next entry for this time point, so the data is interpolated between them.
};


/// \brief 
///   Interpolates structs of type VisDataHistoryCell over time, used for network interpolation of entity properties.
///
/// \param dataType
///   The template struct that is interpolated over time.
///
/// \param iHistoryLength
///   How many entries should be kept in the history ring buffer.
///
/// \param Mix
///   Functor to compute lerp between two dataType variables \sa Lerp
template <typename dataType, int iHistoryLength, class Mix = VLerp<dataType> >
class VisDataHistory_cl
{
public:
  /// \brief
  ///   Constructor
  VisDataHistory_cl()
  {
    Init();
  }

  /// \brief
  ///   Destructor
  ~VisDataHistory_cl() {}

  /// \brief
  ///   Adds an entry to the history.
  /// 
  /// \param data
  ///   The data at the given time.
  ///
  /// \param iCurrentTime
  ///   The point in time at which the data was valid (in ms).
	void Write (const dataType &data, __int64 iCurrentTime);
  
  /// \brief
  ///   Reads an interpolated value from the history.
  /// 
  /// \param[out] out_pData
  ///   Returns the data computed for the given time 'iInterpolationPoint', \sa VReadResult_e .
  ///
  /// \param iInterpolationPoint
  ///   The point in time at which the data should be computed (in ms).
  ///
  /// \return
  ///   Enum defining how the value was computed \sa VReadResult_e
  VReadResult_e Interpolate (dataType* out_pData, __int64 iInterpolationPoint);

  /// \brief
  ///   Reads a value from the history.
  /// 
  /// \param[out] out_pData
  ///   Returns the data at iIndex.
  ///
  /// \param[out] out_pTime
  ///   Returns the time at iIndex.
  ///
  /// \param iIndex
  ///   The index of the data. 0 is the last added entry, 1 the previous etc.
  ///
  /// \return
  ///   Returns true if the data was retrieved. False, if the index does not exist.
  bool Read (const dataType*& out_pData, const __int64*& out_pTime, int iIndex);

  /// \brief
  ///   Returns the last entry's timestamp. Returns zero if none is present.
  __int64 GetLastEntryTime()
  {
    if (m_iHistoryEntries != 0)
      return m_history[(m_iHistoryStart + (m_iHistoryEntries - 1)) % iHistoryLength].m_iTime;
    else
      return 0;
  }

  /// \brief
  ///   Inits the class. 
  ///
  /// This class is used on preallocated memory and thus has to init itself when used.
  /// \sa VisTypedEngineObject_cl::GetSynchronizationGroupList, VNetworkEntityVisibilityGroup::VHistoryDataVisibility::Init()
  void Init()
  {
    m_iHistoryStart = 0;
    m_iHistoryEntries = 0;
  }

private:
  VisDataHistoryCell<dataType> m_history[iHistoryLength];
  unsigned short m_iHistoryStart;
  unsigned short m_iHistoryEntries;

  
};


template <typename dataType, int iHistoryLength, class Mix>
void VisDataHistory_cl<dataType, iHistoryLength, Mix>::Write (const dataType &data, __int64 iCurrentTime)
{
  //Don't add entires that are older than the last one in the buffer
  if (m_iHistoryEntries && iCurrentTime <= m_history[(m_iHistoryStart + m_iHistoryEntries-1) % iHistoryLength].m_iTime)
    return;

  //Is the history already full?
  if (m_iHistoryEntries == iHistoryLength)
  {
    m_history[(m_iHistoryStart + m_iHistoryEntries) % iHistoryLength].m_iTime = iCurrentTime;
    m_history[(m_iHistoryStart + m_iHistoryEntries) % iHistoryLength].m_data = data;
    m_iHistoryStart = (m_iHistoryStart + 1) % iHistoryLength;
  }
  else
  {
    m_history[(m_iHistoryStart + m_iHistoryEntries) % iHistoryLength].m_iTime = iCurrentTime;
    m_history[(m_iHistoryStart + m_iHistoryEntries) % iHistoryLength].m_data = data;
    m_iHistoryEntries++;
  }
}

template <typename dataType, int iHistoryLength, class Mix>
VReadResult_e VisDataHistory_cl<dataType, iHistoryLength, Mix>::Interpolate (dataType* out_pData, __int64 iInterpolationPoint)
{
  if (m_iHistoryEntries == 0)
    return VRR_None;

  __int64 iOldestTime = m_history[m_iHistoryStart].m_iTime;
  __int64 iNewestTime = m_history[(m_iHistoryStart + m_iHistoryEntries - 1) % iHistoryLength].m_iTime;
  
  VASSERT (iOldestTime <= iNewestTime);
  // 'iInterpolationPoint' is too old, taking earliest known value (extrapolate in the past?)
  if (iInterpolationPoint <= iOldestTime)
  {
    //Vision::Error.SystemMessage("  oldest = %d, newest = %d, iInterpolationPoint: %d", (int)iOldestTime, (int)iNewestTime, (int)iInterpolationPoint);

    *out_pData = m_history[m_iHistoryStart].m_data;
    return VRR_Oldest;
  }
  // 'iInterpolationPoint' is newer than the history, so we have to extrapolate the value
  else if (iInterpolationPoint >= iNewestTime)
  {
    // Taking the last value for the time being
    if (m_iHistoryEntries == 1)
    {
      *out_pData = m_history[(m_iHistoryStart + m_iHistoryEntries - 1) % iHistoryLength].m_data;
      return VRR_Extrapolated;
    }
    //extrapolate to the future
    else
    {
      const VisDataHistoryCell<dataType> &current = m_history[(m_iHistoryStart + m_iHistoryEntries - 1) % iHistoryLength];
      const VisDataHistoryCell<dataType> &prev = m_history[(m_iHistoryStart + m_iHistoryEntries - 2) % iHistoryLength];

      float fDivisor = float(current.m_iTime - prev.m_iTime);
      float fStep = float(iInterpolationPoint - current.m_iTime);

      //*out_pData = current.m_data + diff * (fStep / fDivisor);
      *out_pData = Mix() (prev.m_data, current.m_data, 1.0f + (fStep / fDivisor) );
      return VRR_Extrapolated;
    }
  }
  else
  {
    for (int i = m_iHistoryEntries-1;  i > 0; --i)
    {
      const VisDataHistoryCell<dataType> &current = m_history[(m_iHistoryStart + i) % iHistoryLength];
      const VisDataHistoryCell<dataType> &prev = m_history[(m_iHistoryStart + i - 1) % iHistoryLength];

      VASSERT (prev.m_iTime < current.m_iTime);

      if (iInterpolationPoint > prev.m_iTime)
      {
        float fDivisor = float(current.m_iTime - prev.m_iTime);
        float fLerp = float(iInterpolationPoint - prev.m_iTime);
        float fLerpFactor = fLerp / fDivisor;

        *out_pData = Mix()(current.m_data, prev.m_data, 1.0f - fLerpFactor);
        return VRR_Interpolated;
      }
      else
        continue;
    }
  }
  VASSERT_MSG (false, "Unreachable code reached!");
  return VRR_None; //impossible to reach
}


template <typename dataType, int iHistoryLength, class Mix>
bool VisDataHistory_cl<dataType, iHistoryLength, Mix>::Read (const dataType*& out_pData, const __int64*& out_pTime, int iIndex)
{
  // Is the index in range?
  if (m_iHistoryEntries > iIndex)
  {
    const VisDataHistoryCell<dataType> &dataCell = m_history[(m_iHistoryStart + (m_iHistoryEntries - 1) - iIndex) % iHistoryLength];
    out_pData = &dataCell.m_data;
    out_pTime = &dataCell.m_iTime;
    return true;
  }

  return false;
}

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
