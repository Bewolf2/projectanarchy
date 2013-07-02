/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/VScriptIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

//-----------------------------------------------------------------------------------
//
// *** VScriptResourceSerializationProxy ***
//
// Synopsis:
//    - Serialization proxy class to serialize a script resource
//

class VScriptResourceSerializationProxy : public IVSerializationProxy
{
public:
  VScriptResourceSerializationProxy(VScriptResource *pOwner=NULL) {m_pOwner=pOwner;}
  virtual IVSerializationProxyCreator *GetInstance() {return m_pOwner;}
  // serialization
  V_DECLARE_SERIAL_DLLEXP( VScriptResourceSerializationProxy, SCRIPT_IMPEXP )
  virtual void Serialize( VArchive &ar );
  VScriptResource *m_pOwner;
};

//-----------------------------------------------------------------------------------

V_IMPLEMENT_SERIAL( VScriptResourceSerializationProxy, VTypedObject, 0, &g_VisionEngineModule );
void VScriptResourceSerializationProxy::Serialize( VArchive &ar )
{
  VTypedObject::Serialize( ar );
  if ( ar.IsLoading() )
  {
    char iVersion;
    char szFilename[FS_MAX_PATH];
    ar >> iVersion; VVERIFY(iVersion==0 && "invalid version number");
    ar.ReadStringBinary(szFilename,FS_MAX_PATH);
    // lookup in the resource manager
    m_pOwner = VScriptResourceManager::GlobalManager().LoadScriptFile(szFilename);
  }
  else
  {
    // write out script filename
    ar << (char)0; // version
    ar << m_pOwner->GetFilename();
  }
}

//-----------------------------------------------------------------------------------

VScriptResource::VScriptResource(VScriptResourceManager *pManager) 
  : VManagedResource(pManager), m_pResourceState(NULL), m_iKey(LUA_NOREF)
{
//  m_iScriptLen = 0;
  SetResourceFlag(VRESOURCEFLAG_ALLOWUNLOAD);
}

VScriptResource::~VScriptResource()
{
  //discard the thread, just to be sure
  VScriptResourceManager::DiscardThread(m_pResourceState);
  m_pResourceState = NULL;
}

IVSerializationProxy *VScriptResource::CreateProxy()
{
  return new VScriptResourceSerializationProxy(this);
}

//-----------------------------------------------------------------------------------

BOOL VScriptResource::Reload()
{
  const char * pszFilename = GetFilename();
  IVFileInStream *pIn = GetParentManager()->CreateFileInStream(pszFilename, this);
  if (!pIn)
    return FALSE;

  int iScriptLen = pIn->GetSize();
  VMemoryTempBuffer<16*1024> buffer(iScriptLen+1);
  char *szBuffer = (char *)buffer.GetBuffer();
  pIn->Read(szBuffer,iScriptLen);
  szBuffer[iScriptLen] = 0;
  pIn->Close();

  // Test if file is UTF-8 and if so remove the byte order mark
  szBuffer = StripUTF8BOM(szBuffer, iScriptLen);
  
  lua_State *pParentState = GetScriptManager()->GetMasterState();
  VASSERT(pParentState && "Script manager not initialized");
  if (m_iKey == LUA_NOREF)
  {
    VASSERT(m_pResourceState == NULL);
    m_pResourceState = lua_newthread(pParentState);
    m_iKey = luaL_ref(pParentState, LUA_REGISTRYINDEX); // reference the thread from the registry for GC

    // Create a new locals table for this resource's state. Loaded functions will then be in this state's scope.
    // Links the new table to the original global table so scripts can find the global functions
    LUA_CreateLocalsTable(m_pResourceState);
  }
  VASSERT(m_pResourceState != NULL && m_iKey != LUA_NOREF);
  
  // load the string
  if (!LUA_ERRORCHECK(m_pResourceState, 
    luaL_loadbuffer(m_pResourceState, szBuffer, iScriptLen, pszFilename)))
  {
    Unload();   
    return false;
  }

  // run once so function names are known
  if (!LUA_ERRORCHECK(m_pResourceState, lua_pcall (m_pResourceState, 0, LUA_MULTRET, 0)))
  {
    Unload();
    return false;
  }

  SetNewMemSize(VRESOURCEMEMORY_SYSTEM, iScriptLen + 1);
  
  return TRUE;
}

void VScriptResource::UnloadAndReload(BOOL bUnload, BOOL bReload)
{
  if(bUnload && bReload)
  {
    TriggerResourceChangedCallback(VRESOURCECHANGEDFLAG_BEFORE_FILEMODIFIED);
    Reload();
    TriggerResourceChangedCallback(VRESOURCECHANGEDFLAG_AFTER_FILEMODIFIED);
  }
  else
  {
    VManagedResource::UnloadAndReload(bUnload, bReload);
  }
}

void VScriptResource::ReloadAndReplace(char* pNewContent /*= NULL */)
{
  VASSERT(m_pResourceState);
  if(!m_pResourceState)
    return;

  // Just execute the script again from the file or the given new content as we only want to replace the functions and not the local variables.
  const char * pszFilename = GetFilename();
  int iScriptLen = 0;

  if(!pNewContent || pNewContent[0] == '\0')
  {
    IVFileInStream *pIn = GetParentManager()->CreateFileInStream(pszFilename, this);
    if (!pIn)
      return;
    iScriptLen = pIn->GetSize();
    VMemoryTempBuffer<16*1024> buffer(iScriptLen+1);
    char *szBuffer = (char *)buffer.GetBuffer();
    pIn->Read(szBuffer,iScriptLen);
    szBuffer[iScriptLen] = 0;
    pIn->Close();

    // Test if file is UTF-8 and if so remove the byte order mark
    pNewContent = StripUTF8BOM(szBuffer, iScriptLen);

    // load the string
    if (!LUA_ERRORCHECK(m_pResourceState, 
      luaL_loadbuffer(m_pResourceState, pNewContent, iScriptLen, pszFilename)))
    {
      return;
    }

    // szBuffer gets deallocated here
  }
  else
  {
    iScriptLen = static_cast<int>(strlen(pNewContent));

    // load the string
    if (!LUA_ERRORCHECK(m_pResourceState, 
      luaL_loadbuffer(m_pResourceState, pNewContent, iScriptLen, pszFilename)))
    {
      return;
    }
  }

  // run once so function names are known
  if (!LUA_ERRORCHECK(m_pResourceState, lua_pcall (m_pResourceState, 0, LUA_MULTRET, 0)))
    return;

  SetNewMemSize(VRESOURCEMEMORY_SYSTEM, iScriptLen + 1);
}

BOOL VScriptResource::Unload()
{
  //clear the light user data entry for this state
  VScriptResourceManager::DiscardThread(m_pResourceState);
  m_pResourceState = NULL;

  lua_State *pParentState = GetScriptManager()->GetMasterState();
  luaL_unref(pParentState, LUA_REGISTRYINDEX, m_iKey);
  m_iKey = LUA_NOREF;

  SetNewMemSize(VRESOURCEMEMORY_ALLTYPES,0);

  return TRUE;
}

//-----------------------------------------------------------------------------------

VScriptInstance* VScriptResource::CreateScriptInstance()
{
  if (!IsLoaded())
    return NULL;
    
  VScriptResourceManager *pManager = (VScriptResourceManager *)GetParentManager();

  VScriptInstance *pObj = NULL;
  IVObjectInstanceFactory *pFactory = pManager->GetScriptObjectFactory();
  if (pFactory!=NULL)
    pObj = (VScriptInstance *)pFactory->CreateObjectInstance(V_RUNTIME_CLASS(VScriptInstance));
  if (pObj==NULL)
    pObj = new VScriptInstance();
  else
  {
    VASSERT(pObj->IsOfType(V_RUNTIME_CLASS(VScriptInstance)));
  }

  pObj->Init();
  pObj->SetResource(this);

  pManager->Instances().Add(pObj);
  
  return pObj;
}

//-----------------------------------------------------------------------------------

/*static*/ char* VScriptResource::StripUTF8BOM(char* szScriptIn, int& iScriptLenInOut)
{
  const unsigned char utf8BOM[3] = { 0xef, 0xbb, 0xbf };

  // Check for heading byte order mark
  if(!memcmp(szScriptIn, utf8BOM, sizeof(utf8BOM)))
  {
    // script file is UTF8-encoded
    iScriptLenInOut -= sizeof(utf8BOM);
    szScriptIn += sizeof(utf8BOM);
  }

  return szScriptIn;
}

//-----------------------------------------------------------------------------------

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
