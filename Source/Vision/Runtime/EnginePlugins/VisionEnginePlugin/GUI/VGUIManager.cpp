/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

VStrList VGUIManager::g_IDList;
int VGUIManager::ID_STATIC = 0;
int VGUIManager::ID_OK     = 0;
int VGUIManager::ID_CANCEL = 0;
int VGUIManager::ID_YES    = 0;
int VGUIManager::ID_NO     = 0;
int VGUIManager::ID_ACCEPT = 0;
int VGUIManager::ID_IGNORE = 0;


int VGUIManager::PROFILING_RENDERING     = 0;
int VGUIManager::PROFILING_FONTRENDERING = 0;
int VGUIManager::PROFILING_TICKFUNCTIONS = 0;
int VGUIManager::PROFILING_BUILD         = 0;

VGUIManager VGUIManager::g_GlobalManager;
VisCallback_cl VGUIManager::OnTextLabelCallback;

static VSimpleRenderState_t g_DefaultState(VIS_TRANSP_ALPHA,RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_USESCISSORTEST|RENDERSTATEFLAG_USEADDITIVEALPHA);

const VSimpleRenderState_t& VGUIManager::DefaultGUIRenderState(VIS_TransparencyType transp)
{
  g_DefaultState.SetTransparency(transp);
  return g_DefaultState;
}


VGUIManager& VGUIManager::GlobalManager()
{
  return g_GlobalManager;
}

VModule& VGUIManager::GUIModule() 
{
  return g_VisionEngineModule;
}


VGUIManager::VGUIManager(const char *szManagerName) : 
  VisResourceManager_cl(
    szManagerName,
    VRESOURCEMANAGERFLAG_SHOW_IN_VIEWER|
    VRESOURCEMANAGERFLAG_IGNORE_SURVIVING_RESOURCES|
    VRESOURCEMANAGERFLAG_NO_LOADWORLD_PURGING // don't purge GUI resources at all, otherwise the main menu dialog couldn't be opened again
    )
{
}

void VGUIManager::OneTimeInit()
{
  Vision::ResourceSystem.RegisterResourceManager(this,VColorRef(255,255,0));
  Vision::Callbacks.OnEngineDeInit += this;
  Vision::Callbacks.OnEngineInit += this;

  // register some IDs
  ID_STATIC  = GetID("STATIC");  VASSERT(ID_STATIC==0);
  ID_OK      = GetID("OK");
  ID_CANCEL  = GetID("CANCEL");
  ID_YES     = GetID("YES");
  ID_NO      = GetID("NO");
  ID_ACCEPT  = GetID("ACCEPT");
  ID_IGNORE  = GetID("IGNORE");


  // profiling
  if (!PROFILING_RENDERING)
  {
    PROFILING_RENDERING     = Vision::Profiling.GetFreeElementID();
    PROFILING_FONTRENDERING = Vision::Profiling.GetFreeElementID();
    PROFILING_TICKFUNCTIONS = Vision::Profiling.GetFreeElementID();
    PROFILING_BUILD         = Vision::Profiling.GetFreeElementID();

    VProfilingNode *pGUIGroup = Vision::Profiling.AddGroup("GUI");
    VProfilingNode *pRenderingGroup = Vision::Profiling.AddElement(PROFILING_RENDERING, "rendering", TRUE, pGUIGroup);
    Vision::Profiling.AddElement(PROFILING_FONTRENDERING, "font rendering", TRUE,pRenderingGroup);
    Vision::Profiling.AddElement(PROFILING_TICKFUNCTIONS, "tick functions", TRUE,pGUIGroup);
    Vision::Profiling.AddElement(PROFILING_BUILD,     "build", TRUE,pGUIGroup);
  }

  FORCE_LINKDYNCLASS(VCheckBox)
  FORCE_LINKDYNCLASS(VTextControl)
  FORCE_LINKDYNCLASS(VPushButton)
  FORCE_LINKDYNCLASS(VTextLabel)
  FORCE_LINKDYNCLASS(VImageControl)
  FORCE_LINKDYNCLASS(VDialogTitleBar)
  FORCE_LINKDYNCLASS(VDialogResizeCtrl)
  FORCE_LINKDYNCLASS(VDialogCloseButton)
  FORCE_LINKDYNCLASS(VListControl)
  FORCE_LINKDYNCLASS(VMapLookupControl)
  FORCE_LINKDYNCLASS(VModelPreviewControl)
  FORCE_LINKDYNCLASS(VItemContainer)
}

void VGUIManager::OneTimeDeInit()
{
  Vision::ResourceSystem.UnregisterResourceManager(this);
  Vision::Callbacks.OnEngineDeInit -= this;
  Vision::Callbacks.OnEngineInit -= this;
  g_IDList.Reset();
}


int VGUIManager::GetID(const char *szIDName)
{
  if (!szIDName || !szIDName[0])
    return 0;
  return g_IDList.AddUniqueString(szIDName);
}


const char* VGUIManager::GetIDName(int iID)
{
  VASSERT(iID>=0);
  return g_IDList[iID];
}


VisFont_cl *VGUIManager::GetDefaultFont() const 
{
  if (m_spDefaultFont==NULL)
    return &Vision::Fonts.DebugFont();
  return m_spDefaultFont;
}


void VGUIManager::CleanupResources()
{
  m_spDefaultCursor = NULL;

  m_spDefaultFont = NULL;
  m_LoadedFonts.Clear();
  
  //deactivate every context
  for(int i=0;i<m_Contexts.Count();i++)
    m_Contexts.GetAt(i)->SetActivate(false);
  
  //clear context list
  m_Contexts.Clear();
  PurgeUnusedResources();
}


void VGUIManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
{
  if (pData->m_pSender==&Vision::Callbacks.OnEngineDeInit)
  {
    CleanupResources();
    return;
  }

  if (pData->m_pSender==&Vision::Callbacks.OnEngineInit)
  {
  }
}



VManagedResource *VGUIManager::FindResource(const char *szFilename, ResourceType type, const char *szPath) const
{
  if (type==FONT) // go via engine
    return Vision::Fonts.FindFont(szFilename,szPath);

  char szPathname[FS_MAX_PATH];
  VFileHelper::CombineDirAndFile(szPathname,szPath,szFilename);

  int iCount = GetResourceCount();
  for (int i=0;i<iCount;i++)
  {
    IVMenuSystemResource *pRes = (IVMenuSystemResource *)GetResourceByIndex(i);
    if (!pRes || pRes->GetResourceType()!=type)
      continue;

    // does the resource name (which is not necessarily filename) match?
    if (!pRes->m_sResourceName.IsEmpty() && !_stricmp(pRes->m_sResourceName,szFilename))
      return pRes;

    // otherwise check for filename (with path!)
    if (pRes->GetFilename() && !_stricmp(pRes->GetFilename(),szPathname))
      return pRes;
  }
  return NULL;
}




VManagedResource *VGUIManager::CreateResource(TiXmlElement *pNode, const char *szPath)
{
  if (!pNode)
    return NULL;
  const char *szType = pNode->Value();
  ResourceType type = GetResourceType(szType);
  if (type!=CURSOR && type!=DIALOG) // this is the only type that can be created directly
    return NULL;

  // already loaded?
  VManagedResource *pRes = NULL;
  const char *szName = XMLHelper::Exchange_String(pNode,"name",NULL,false);
  if (szName)
  {
    pRes = FindResource(szName,type);
    if (pRes)
      return pRes;
  }

  switch (type)
  {
    case CURSOR:
    {
      VCursor *pCursor = new VCursor(this,NULL);
      pCursor->m_sResourceName = szName;
      pCursor->Parse(pNode,szPath);
      pRes = pCursor;
    } break;
    case DIALOG:
    {
      VDialogResource *pDlg = new VDialogResource(this,NULL);
      pDlg->m_sResourceName = szName;
      pDlg->Parse(pNode,szPath);
      pRes = pDlg;
    } break;

      //    case FONT:pRes = new VisFont_cl(this,NULL);break;
    default:
      VASSERT(!"This resource type cannot be created directly");
      return NULL;
  }

  return pRes;
}



VCursor* VGUIManager::LoadCursorResource(const char *szFilename, const char *szPath)
{
  VISION_PROFILE_FUNCTION(PROFILING_BUILD);
  if (!szFilename || !szFilename[0])
    return GetDefaultCursor();
  VCursor *pCursor = (VCursor *)FindResource(szFilename,CURSOR, szPath);
  if (pCursor)
    return pCursor;

  char szPathname[FS_MAX_PATH];
  VFileHelper::CombineDirAndFile(szPathname,szPath,szFilename);

  pCursor = new VCursor(this, szPathname);
  pCursor->EnsureLoaded();

  return pCursor;
}

 
VDialogResource *VGUIManager::LoadDialog(const char *szFilename, const char *szPath)
{
  VISION_PROFILE_FUNCTION(PROFILING_BUILD);
  VDialogResource *pDialog = (VDialogResource *)FindResource(szFilename,DIALOG, szPath);
  if (pDialog)
    return pDialog;

  char szPathname[FS_MAX_PATH];
  VFileHelper::CombineDirAndFile(szPathname,szPath,szFilename);

  pDialog = new VDialogResource(this, szPathname);
  pDialog->EnsureLoaded();
  return pDialog;
}


VDialog* VGUIManager::CreateDialogInstance(const char *szDialogResource, IVGUIContext *pContext, VDialog *pParent, int iDlgFlags)
{
  // no file extension? Then try with binary version first
  if (VFileHelper::GetExtensionPos(szDialogResource)<0)
  {
    char szFilenameExt[FS_MAX_PATH];
    VFileHelper::AddExtension(szFilenameExt, szDialogResource, DIALOG_BINARYFILEEXT);
    if (!Vision::File.Exists(szFilenameExt))
      VFileHelper::AddExtension(szFilenameExt, szDialogResource, "xml"); // fall back to XML version if binary does not exist
    return CreateDialogInstance(szFilenameExt, pContext, pParent, iDlgFlags);
  }

  if (VFileHelper::HasExtension(szDialogResource,"XML"))
  {
    VDialogResource *pRes = LoadDialog(szDialogResource);
    if (!pRes)
      return NULL;

    return pRes->CreateInstance(pContext, pParent, iDlgFlags);
  }

  // de-serialize from binary file
  VDialog* pDlg = VDialog::LoadFromBinaryFile(szDialogResource);
  if (!pDlg)
    return NULL;
  pDlg->SetContextRecursive(pContext);

  return pDlg;
}




VisFont_cl *VGUIManager::LoadFont(const char *szFilename, const char *szPath)
{
  VISION_PROFILE_FUNCTION(PROFILING_BUILD);
  if (!szFilename || !szFilename[0])
    return m_spDefaultFont;

  // might be the font name only:
  VisFont_cl *pFound = Vision::Fonts.FindFont(szFilename);
  if (!pFound) // otherwise load it
  {
    char szPathname[FS_MAX_PATH];
    VFileHelper::CombineDirAndFile(szPathname,szPath,szFilename);
    pFound = Vision::Fonts.LoadFont(szPathname);
    if (pFound)
    {
      pFound->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      m_LoadedFonts.Add(pFound); // keep them in separate list so the engine does not purge them
    }
    else
    {
      pFound = m_spDefaultFont;
      if (!pFound) // still not found? Then replace by debug font...
        pFound = &Vision::Fonts.DebugFont();
    }
  }
  return pFound;
}


VTextureObject *VGUIManager::LoadTexture(const char *szFilename, const char *szPath)
{
  VISION_PROFILE_FUNCTION(PROFILING_BUILD);
  if (!szFilename || !szFilename[0])
    return NULL;
  char szPathname[FS_MAX_PATH];
  VFileHelper::CombineDirAndFile(szPathname,szPath, szFilename);
  return Vision::TextureManager.Load2DTexture(szPathname,VTM_FLAG_NO_DOWNSCALE);
}


#define CHECK_TYPESTR(t)  {if (!_stricmp(szTypeStr,#t)) return t;}
#define GET_TYPESTR(t)    {if (eType==t) return #t;}

VGUIManager::ResourceType VGUIManager::GetResourceType(const char *szTypeStr)
{
  if (!szTypeStr || !szTypeStr[0])
    return UNDEFINED;
  CHECK_TYPESTR(CURSOR);
  CHECK_TYPESTR(DIALOG);
  CHECK_TYPESTR(FONT);
  return UNDEFINED;
}


const char* VGUIManager::GetResourceTypeStr(ResourceType eType)
{
  GET_TYPESTR(UNDEFINED);
  GET_TYPESTR(CURSOR);
  GET_TYPESTR(DIALOG);
  GET_TYPESTR(FONT);
  VASSERT(FALSE);
  return NULL;
}


void VGUIManager::SetDefaultCursor(VCursor *pCursor)
{
  m_spDefaultCursor = pCursor;
}


VCursor* VGUIManager::GetDefaultCursor() const
{
  return m_spDefaultCursor;
}


bool VGUIManager::LoadResourceFile(const char *szFilename)
{
  TiXmlDocument doc;
  if (!doc.LoadFile(szFilename,Vision::File.GetManager()))
    return false;

  char szPath[FS_MAX_PATH];
  VFileHelper::GetFileDir(szFilename,szPath);

  // load all resource references (LOADRESOURCE) in there
  for (TiXmlElement *pNode=doc.RootElement()->FirstChildElement("LOADRESOURCE"); pNode; pNode=pNode->NextSiblingElement("LOADRESOURCE") )
  {
    const char *szType = XMLHelper::Exchange_String(pNode,"type",NULL,false);
    ANALYSIS_IGNORE_WARNING_ONCE(6246)
    const char *szFilename = XMLHelper::Exchange_String(pNode,"filename",NULL,false);
    const char *szName = XMLHelper::Exchange_String(pNode,"name",NULL,false);
    VASSERT(szType != NULL && szFilename != NULL)
    if (szType == NULL || szFilename == NULL) continue;
    ResourceType type = GetResourceType(szType);
    VASSERT(type!=UNDEFINED);
    if (type==UNDEFINED) continue;

    char szPathname[FS_MAX_PATH];
    VFileHelper::CombineDirAndFile(szPathname,szPath,szFilename);

//    VManagedResource *pRes = NULL;
    IVMenuSystemResource *pGUIRes = NULL;
    VisFont_cl *pFontRes = NULL;
    switch (type)
    {
      case CURSOR:pGUIRes = LoadCursorResource(szPathname);/*pRes=pGUIRes;*/break;
      case DIALOG:pGUIRes = LoadDialog(szPathname);/*pRes=pGUIRes;*/break;
      case FONT:pFontRes = LoadFont(szPathname);/*pRes=pFontRes;*/break;
    }
    // set the name of the resource (but don't replace old name)
    if (szName && szName[0])
    {
      if (pGUIRes && pGUIRes->m_sResourceName.IsEmpty())
        pGUIRes->m_sResourceName = szName;
      if (pFontRes)
        pFontRes->SetFontName(szName);
    }
  }

  // check for direct resources in this file
  for (TiXmlElement *pNode=doc.RootElement()->FirstChildElement(); pNode; pNode=pNode->NextSiblingElement() )
  {
    /*VManagedResource * pRes = */CreateResource(pNode,szPath);
    if (!pNode)
      continue;
  }

  // set the defaults
  TiXmlElement *pDefaults = XMLHelper::SubNode(doc.RootElement(),"defaults",false);
  if (pDefaults)
  {
    const char *szCursor = XMLHelper::Exchange_String(pDefaults,"cursor",NULL,false);
    if (szCursor)
      SetDefaultCursor(LoadCursorResource(szCursor,szPath));

    const char *szFont = XMLHelper::Exchange_String(pDefaults,"font",NULL,false);
    if (szFont)
      SetDefaultFont(LoadFont(szFont,szPath));
  }

  Vision::ResourceSystem.UpdateResourceViewer();
  return true;
}


const char *VGUIManager::TranslateString(VWindowBase *pItem, const char *szIn) const
{
  VTextLabelDataObject data(pItem, szIn);
  OnTextLabelCallback.TriggerCallbacks(&data);
  
  return data.m_szLabel;
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
