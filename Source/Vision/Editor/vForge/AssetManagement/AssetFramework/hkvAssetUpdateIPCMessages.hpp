/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef _HKV_ASSET_UPDATE_IPC_MESSAGES_HPP_
#define _HKV_ASSET_UPDATE_IPC_MESSAGES_HPP_

#include <Vision/Runtime/Base/IPC/VMessage.hpp>

enum AssetUpdateIPCMessages
{
  MSG_CLEAR_DATA_DIRECTORIES = Message::MSG_USER,
  MSG_ADD_DATA_DIRECTORY,
  MSG_PROCESS_ASSET,
  MSG_ASSET_DEPENDENCY,
  MSG_ASSET_PROCESSED,
};


/// \brief
///  Wrapper class for a MSG_CLEAR_DATA_DIRECTORIES
class hkvMsgClearDataDirectories : public Message
{
public:
  hkvMsgClearDataDirectories()
  : Message(MSG_CLEAR_DATA_DIRECTORIES)
  {
  }

  hkvMsgClearDataDirectories(const Message& msg)
  : Message(MSG_CLEAR_DATA_DIRECTORIES)
  {
    CopyFrom(msg);
  }
};


/// \brief
///  Wrapper class for a MSG_ADD_DATA_DIRECTORY
class hkvMsgAddDataDirectory : public Message
{
public:
  const char* m_path;

  hkvMsgAddDataDirectory(const char* path) 
  : Message(MSG_ADD_DATA_DIRECTORY)
  {
    m_path = AddData(path, (UINT)strlen(path) + 1);
  }

  hkvMsgAddDataDirectory(const Message& msg)
  : Message(MSG_ADD_DATA_DIRECTORY)
  {
    CopyFrom(msg);
    m_path = GetData<char>();
  }
};


/// \brief
///  Wrapper class for a MSG_PROCESS_ASSET
class hkvMsgProcessAsset : public Message
{
public:
  const char* m_resourceManager;
  const char* m_assetPath;
  const char* m_thumbnailPath;
  bool m_createThumbnail;
  bool m_determineDependencies;

  hkvMsgProcessAsset(const char* resourceManager, const char* assetPath, const char* thumbnailPath, 
    bool createThumbnail, bool determineDependencies) 
  : Message(MSG_PROCESS_ASSET), m_createThumbnail(createThumbnail), 
    m_determineDependencies(determineDependencies)
  {
    if (thumbnailPath == NULL)
    {
      thumbnailPath = "\0";
    }

    m_resourceManager = AddData(resourceManager, (UINT)strlen(resourceManager) + 1);
    m_assetPath = AddData(assetPath, (UINT)strlen(assetPath) + 1);
    m_thumbnailPath = AddData(thumbnailPath, (UINT)strlen(thumbnailPath) + 1);
    AddData(&m_createThumbnail, sizeof(bool));
    AddData(&m_determineDependencies, sizeof(bool));
  }

  hkvMsgProcessAsset(const Message& msg)
  : Message(MSG_PROCESS_ASSET)
  {
    CopyFrom(msg);
    m_resourceManager = GetData<char>();
    m_assetPath = GetData<char>();
    m_thumbnailPath = GetData<char>();
    m_createThumbnail = *GetData<bool>();
    m_determineDependencies = *GetData<bool>();
  }
};


/// \brief
///  Wrapper class for a MSG_ASSET_PROCESSED
class hkvMsgAssetProcessed : public Message
{
public:
  bool m_thumbnailCreated;
  bool m_dependenciesDetermined;

  hkvMsgAssetProcessed(bool thumbnailCreated, bool dependenciesDetermined)
  : Message(MSG_ASSET_PROCESSED),
    m_thumbnailCreated(thumbnailCreated), m_dependenciesDetermined(dependenciesDetermined)
  {
    AddData(&m_thumbnailCreated, sizeof(bool));
    AddData(&m_dependenciesDetermined, sizeof(bool));
  }

  hkvMsgAssetProcessed(const Message& msg)
  : Message(MSG_ASSET_PROCESSED)
  {
    CopyFrom(msg);
    m_thumbnailCreated = *GetData<bool>();
    m_dependenciesDetermined = *GetData<bool>();
  }
};


/// \brief
///  Wrapper class for a MSG_ASSET_DEPENDENCY
class hkvMsgAssetDependency : public Message
{
public:
  const char* m_dependency;

  hkvMsgAssetDependency(const char* dependency) 
  : Message(MSG_ASSET_DEPENDENCY)
  {
    m_dependency = AddData(dependency, (UINT)strlen(dependency) + 1);
  }

  hkvMsgAssetDependency(const Message& msg)
  : Message(MSG_ASSET_DEPENDENCY)
  {
    CopyFrom(msg);
    m_dependency = GetData<char>();
  }
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
