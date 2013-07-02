/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_EXTERNAL_TOOL_TRANSFORMATION_HPP_INCLUDED
#define HKV_EXTERNAL_TOOL_TRANSFORMATION_HPP_INCLUDED

#include <Common/Base/Thread/CriticalSection/hkCriticalSection.h>

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>
#include <Vision/Editor/vForge/AssetManagement/AssetFramework/Transformation/hkvFileTransformationStep.hpp>


class hkvProcessRunner;


/// \class hkvExternalToolTransformation
/// \brief Implements the running of an external tool (e.g., for running an asset transformation).
///
/// This class provides facilities for running the external command, capturing and checking the
/// exit code, and processing the tool stdout and stderr output into separate message items.
class hkvExternalToolTransformation : public hkvFileTransformationStep
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  ASSETFRAMEWORK_IMPEXP hkvExternalToolTransformation(const char* sourceFile, const char* targetFile);
private:
  hkvExternalToolTransformation(const hkvExternalToolTransformation&);
  hkvExternalToolTransformation& operator=(const hkvExternalToolTransformation&);

protected:
  virtual const char* getToolName() const = 0;

  virtual hkResult beforeRun() { return HK_SUCCESS; }
  virtual void afterRun(bool runWasSuccessful) { }

public:
  ASSETFRAMEWORK_IMPEXP virtual hkResult run() HK_OVERRIDE;
  ASSETFRAMEWORK_IMPEXP virtual void cancel() HK_OVERRIDE;

public:
  hkBool m_checkExitCode;
  hkBool m_checkTargetFile;
  hkvMessageSeverity m_stdOutSeveritySuccess;
  hkvMessageSeverity m_stdOutSeverityError;
  hkvMessageSeverity m_stdErrSeveritySuccess;
  hkvMessageSeverity m_stdErrSeverityError;

protected:
  hkStringPtr m_executable;
  hkStringPtr m_commandLine;
  hkStringPtr m_workingDirectory;
  bool m_idlePriority;

private:
  hkUint32 m_exitCode;
  
  hkCriticalSection m_protect;
  bool m_canceled;
  hkvProcessRunner* m_currentProcessRunner;
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
