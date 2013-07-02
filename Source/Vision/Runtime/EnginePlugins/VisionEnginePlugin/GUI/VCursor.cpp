/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>        // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/GUI/VMenuIncludes.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>


///////////////////////////////////////////////////////////////////////////
// Cursor
///////////////////////////////////////////////////////////////////////////

VCursor::VCursor(VGUIManager *pManager, const char *szFilename) 
: IVMenuSystemResource(pManager, szFilename, VGUIManager::CURSOR)
{
}


BOOL VCursor::Reload()
{
  const char *szFilename = GetFilename();
  char szPath[FS_MAX_PATH];
  GetFilePath(szPath);

  // either load detailed definition from XML...
  if (VFileHelper::HasExtension(szFilename,"XML"))
  {
    TiXmlDocument doc;
    if (!doc.LoadFile(szFilename,Vision::File.GetManager()))
      return FALSE;
    return Build(doc.RootElement()->FirstChildElement("CURSOR"),szPath,false);
  }
  else // or just a texture filename...
  {
    VTextureObject *pTex = Vision::TextureManager.Load2DTexture(szFilename, VTM_FLAG_DEFAULT_NON_MIPMAPPED);
    for (int i=0;i<VGUIUserInfo_t::GUIMaxUser;i++)
    {
      m_UserProperties[i].SetTexture(pTex);
      m_UserProperties[i].SetSizeFromTexture();
    }
  }
  return TRUE;
}

BOOL VCursor::Unload()
{
  return TRUE;
}



bool VCursor::Build(TiXmlElement *pNode, const char *szPath, bool bWrite)
{
  if (!IVRenderableItem::Build(pNode,szPath,bWrite))
    return false;

  return Parse(pNode, szPath); // use the IVMenuSystemResource parsing code
}


bool VCursorProperties_t::Parse(VGUIManager *pManager, TiXmlElement *pNode, const char *szPath)
{
  const bool bWrite = false;
  const char *szFilename = XMLHelper::Exchange_String(pNode,"texture",NULL,bWrite);
  if (szFilename)
    SetTexture(pManager->LoadTexture(szFilename,szPath));
  else 
    SetTexture(Vision::TextureManager.Load2DTexture("plainwhite.dds"));

  // hotspot
  XMLHelper::Exchange_Floats(pNode,"hotspot",m_vHotSpot.data,2,bWrite);
  // size
  XMLHelper::Exchange_Floats(pNode,"size",m_vSize.data,2,bWrite);
  // texture coordinates
  float *texCoordData = (float*)(&texCoord.m_vMin);
  XMLHelper::Exchange_Floats(pNode,"texcoords",texCoordData,4,bWrite);
  // color
  XMLHelper::Exchange_Color(pNode,"color",iColor,bWrite);

  // transparency
  XMLHelper::Exchange_Transparency(pNode,"transparency",m_eTransp,bWrite);
  
  if (m_vSize.x<0.f)
    SetSizeFromTexture();
  return true;
}


bool VCursor::Parse(TiXmlElement *pNode, const char *szPath)
{
  if (!IVMenuSystemResource::Parse(pNode,szPath))
    return false;

  // user 0 parse from cursor root
  m_UserProperties[0].Parse(GetMenuManager(),pNode,szPath);

  // assign to all other user properties
  for (int i=1;i<VGUIUserInfo_t::GUIMaxUser;i++)
    m_UserProperties[i] = m_UserProperties[0];

  // see whether we have a subnode and parse
  for (TiXmlElement *pUserNode=pNode->FirstChildElement("user"); pUserNode; pUserNode=pUserNode->NextSiblingElement("user"))
  {
    int iUser = -1;
    XMLHelper::Exchange_Int(pUserNode,"ID",iUser,false);
    VASSERT_MSG(iUser>=0 && iUser<VGUIUserInfo_t::GUIMaxUser,"Missing or invalid user ID");
    m_UserProperties[iUser].Parse(GetMenuManager(),pUserNode,szPath);
  }

//  if (!GetFilename())
//    SetFilename(szFilename);

  return true; 
}



void VCursor::OnPaint(VGraphicsInfo &Graphics, const VItemRenderInfo &parentState)
{
  V_LOCK_RESOURCE(this);
  VASSERT(Graphics.m_pCurrentUser!=NULL);
  const VCursorProperties_t &cursor(m_UserProperties[Graphics.m_pCurrentUser->m_iID]); // user specific cursor properties
  if (!cursor.m_bVisible)
    return;

  VTextureObject* pTex = cursor.GetCurrentTexture();
  VASSERT(pTex!=NULL);
  hkvVec2 vPos1 = Graphics.m_pCurrentUser->m_vMousePos - cursor.m_vHotSpot;
  hkvVec2 vPos2 = vPos1 + cursor.m_vSize;
  VSimpleRenderState_t state = VGUIManager::DefaultGUIRenderState(cursor.m_eTransp);

  Graphics.Renderer.DrawTexturedQuad(vPos1,vPos2, pTex,
    cursor.texCoord.m_vMin,cursor.texCoord.m_vMax, parentState.iFadeColor*cursor.iColor,state);
}


void VCursor::OnTick(float dtime)
{
}


class VCursorSerializationProxy : public IVSerializationProxy
{
public:
  VCursorSerializationProxy(VCursor *pCursor=NULL)
  {
    m_spCursor = pCursor;
  }

  virtual IVSerializationProxyCreator *GetInstance() HKV_OVERRIDE
  {
    return m_spCursor;
  }

  V_DECLARE_SERIAL( VCursorSerializationProxy, GUI_IMPEXP);

  virtual void Serialize( VArchive &ar ) HKV_OVERRIDE
  {
    char iLocalVersion = 0;
    // TODO: load/save properties directly?
    if (ar.IsLoading())
    {
      ar >> iLocalVersion; VASSERT_MSG(iLocalVersion==0,"Invalid version");
      VMemoryTempBuffer<FS_MAX_PATH> buffer;
      const char *szCursor = buffer.ReadStringBinary(ar);
      if (szCursor)
        m_spCursor = VGUIManager::GlobalManager().LoadCursorResource(szCursor/*,szPath*/);
    } else
    {
      ar << iLocalVersion;
      const char *szCursor = (m_spCursor!=NULL && m_spCursor!=VGUIManager::GlobalManager().GetDefaultCursor()) ? m_spCursor->GetFilename() : NULL;
      ar << szCursor;
    }
  }

  VCursorPtr m_spCursor;
};

V_IMPLEMENT_SERIAL( VCursorSerializationProxy, VTypedObject, 0, &g_VisionEngineModule );



IVSerializationProxy *VCursor::CreateProxy() 
{
  return new VCursorSerializationProxy(this);
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
