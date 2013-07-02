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


/////////////////////////////////////////////////////////////////////////////////////////////////
// Dialog resource
///////////////////////////////////////////////////////////////////////////


VDialogResource::VDialogResource(VGUIManager *pManager, const char *szFilename) 
: IVMenuSystemResource(pManager,szFilename,VGUIManager::DIALOG)
{
  m_pXMLNode = NULL;
}


bool VDialogResource::Parse(TiXmlElement *pNode, const char *szPath)
{
  if (!IVMenuSystemResource::Parse(pNode,szPath))
    return false;

  // creates a clone of the dialog XML node
  V_SAFE_DELETE(m_pXMLNode);
  m_pXMLNode = pNode->Clone()->ToElement();

  // since we need the path for creating the items later (GetFilePath), we create a dummy filename
  // that contains the path
  if (!GetFilename())
  {
    char szDummyName[FS_MAX_PATH];
    VFileHelper::CombineDirAndFile(szDummyName,szPath,"<XMLFile>");
    SetFilename(szDummyName);
  }
  return true;
}


BOOL VDialogResource::Reload()
{
  TiXmlDocument doc;

  char szPathName[FS_MAX_PATH];
  const char *szFilename = GetFilename();
  if (VFileHelper::GetExtensionPos(szFilename)<0)
  {
    VFileHelper::AddExtension(szPathName,szFilename,"XML");
    szFilename = szPathName;
  }

  bool bResult = doc.LoadFile(szFilename,Vision::File.GetManager());
  if (!bResult)
  {
    Vision::Error.Warning("GUI dialog resource '%s' failed to load : %s",szFilename,doc.ErrorDesc());
    return FALSE;
  }

  TiXmlElement *pDlgNode = XMLHelper::SubNode(doc.RootElement(),"DIALOG",false);
  if (!pDlgNode)
    return FALSE;

  return Parse(pDlgNode,NULL);
}

BOOL VDialogResource::Unload()
{
  V_SAFE_DELETE(m_pXMLNode);
  return TRUE;
}


VDialog *VDialogResource::CreateInstance(IVGUIContext *pContext, VDialog *pParent, int iFlags)
{
  VISION_PROFILE_FUNCTION(VGUIManager::PROFILING_BUILD);
  EnsureLoaded();

  if (!m_pXMLNode || !IsLoaded()) // couldn't load XML file?
  {
    VASSERT(!"VDialogResource::CreateInstance failed because dialog resource is invalid");
    return NULL;
  }

  char szPath[FS_MAX_PATH];
  GetFilePath(szPath);

  const char *szClass = XMLHelper::Exchange_String(m_pXMLNode,"class",NULL,false);
  if (!szClass || !szClass[0])
    szClass = "VDialog";

  VType *pType = Vision::GetTypeManager()->GetType(szClass);
  if (!pType)
  {
    VASSERT(!"VDialogResource::CreateInstance failed because dialog class does not exist in type manager");
    return NULL;
  }
  VDialog *pDlg = (VDialog *)pType->CreateInstance();
  
  // sanity check
  if (!pDlg->IsOfType(Vision::GetTypeManager()->GetType("VDialog")))
    Vision::Error.FatalError("class '%s' is not derived from base class VDialog",szClass);

  VASSERT(pParent==NULL || pParent->m_pContext==pContext);

  pDlg->InitDialog(pContext, this, pParent, iFlags);
  if (!pDlg->Build(m_pXMLNode, szPath,false))
  {
    VASSERT(!"VDialogResource::CreateInstance failed because Build from XML node failed");
    return NULL;
  }
  pDlg->OnBuildFinished();
  pDlg->OnInitDialog();

  return pDlg;
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
