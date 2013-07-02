/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file hkvTransformationBackgroundProcessor.hpp

#ifndef HKV_TRANSFORMATION_BACKGROUND_PROCESSOR_HPP_INCLUDED
#define HKV_TRANSFORMATION_BACKGROUND_PROCESSOR_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Assets/hkvAsset.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Base/hkvBackgroundProcessor.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/TransformationRules/hkvTransformationRule.hpp>

#include <Common/Base/Types/hkRefPtr.h>

#include <vector>


struct hkvBackgroundTransformationInput
{
  hkvBackgroundTransformationInput(hkvAsset* asset = NULL)
  : m_asset(asset)
  {
  }

  bool operator==(const hkvBackgroundTransformationInput& other) const
  {
    return (m_asset == other.m_asset);
  }

  bool operator<(const hkvBackgroundTransformationInput& other) const
  {
    return (m_asset < other.m_asset);
  }

  hkRefPtr<hkvAsset> m_asset;
};


struct hkvBackgroundTransformationOutput
{
  hkvBackgroundTransformationOutput() :
    m_asset(HK_NULL), m_sourceFileChanged(false), m_successful(false), m_numOutputsWritten(0)
  {
  }
  
  hkvBackgroundTransformationOutput(const hkvBackgroundTransformationOutput& rhs) :
    m_asset(rhs.m_asset), m_sourceFileChanged(rhs.m_sourceFileChanged), 
    m_successful(rhs.m_successful), m_numOutputsWritten(rhs.m_numOutputsWritten)
  {
    m_messages = rhs.m_messages;
  }

  hkvBackgroundTransformationOutput& operator=(const hkvBackgroundTransformationOutput& rhs)
  {
    m_asset = rhs.m_asset;
    m_sourceFileChanged = rhs.m_sourceFileChanged;
    m_successful = rhs.m_successful;
    m_numOutputsWritten = rhs.m_numOutputsWritten;
    m_messages = rhs.m_messages;
  }

  hkRefPtr<hkvAsset> m_asset;
  bool m_sourceFileChanged;
  bool m_successful;
  hkUint32 m_numOutputsWritten;
  hkArray<hkvTransformationMessage> m_messages;
};


class hkvTransformationBackgroundProcessor : 
  public hkvBackgroundProcessor<hkvBackgroundTransformationInput, hkvBackgroundTransformationOutput>
{
private:
  class TransformationControlHost;

public:
  hkvTransformationBackgroundProcessor(hkUint32 numThreads = 1, bool addTransformedToRCS = false);
private:
  hkvTransformationBackgroundProcessor(const hkvTransformationBackgroundProcessor&);
  hkvTransformationBackgroundProcessor& operator=(const hkvTransformationBackgroundProcessor&);
public:
  VOVERRIDE ~hkvTransformationBackgroundProcessor();

protected:
  virtual void onStopping() HKV_OVERRIDE;

  virtual hkvBackgroundProcessingResult process(const hkvBackgroundTransformationInput& input, hkvBackgroundTransformationOutput& output) HKV_OVERRIDE;

private:
  std::vector<TransformationControlHost*> m_transformationControlHosts;
  bool m_addTransformedToRCS;
};


class hkvTransformationBackgroundProcessor::TransformationControlHost : public hkvITransformationControlHost
{
public:
  TransformationControlHost(hkvTransformationBackgroundProcessor& owner);
private:
  TransformationControlHost(const TransformationControlHost&);
  TransformationControlHost& operator=(const TransformationControlHost&);

protected:
  virtual hkvCriticalSectionLock acquireLock() HKV_OVERRIDE;

private:
  hkvTransformationBackgroundProcessor& m_owner;
};

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
