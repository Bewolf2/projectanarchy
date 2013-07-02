/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HKV_FILE_TRANSFORMATION_STEP_HPP_INCLUDED
#define HKV_FILE_TRANSFORMATION_STEP_HPP_INCLUDED

#include <Vision/Editor/vForge/AssetManagement/AssetFramework/hkvAssetStructs.hpp>

class hkvFileTransformationStep : public hkReferencedObject
{
public:
  HK_DECLARE_CLASS_ALLOCATOR(HK_MEMORY_CLASS_TOOLS);

public:
  hkvFileTransformationStep(const char* sourceFile, const char* targetFile)
  : m_sourceFile(sourceFile), m_targetFile(targetFile)
  {
  }
private:
  hkvFileTransformationStep(const hkvFileTransformationStep&);
  hkvFileTransformationStep& operator=(const hkvFileTransformationStep&);

public:
  virtual hkResult run() = 0;

  virtual void cancel() = 0;

  const char* getSourceFile() const { return m_sourceFile; }
  const char* getTargetFile() const { return m_targetFile; }
  const hkArray<hkvTransformationMessage>& getMessages() const { return m_messages; }

protected:
  void addMessage(const hkvTransformationMessage& message) { m_messages.pushBack(message); }
  void setTargetFile(const char* targetFile) { m_targetFile = targetFile; }

private:
  hkArray<hkvTransformationMessage> m_messages;
  hkStringPtr m_sourceFile;
  hkStringPtr m_targetFile;
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
