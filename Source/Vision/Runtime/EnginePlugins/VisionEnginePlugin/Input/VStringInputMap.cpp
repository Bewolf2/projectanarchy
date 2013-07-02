/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>         // precompiled header
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Input/VStringInputMap.hpp>

#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

V_ENGINE_PLUGIN_ELEMENT_MANAGER_INIT_STATICS(VStringInputMap)

VStringInputMap::VStringInputMap(const char * szMapName, int iNumTriggers, int iNumAlternatives) :
  VInputMap(iNumTriggers, iNumAlternatives),
  m_pMappedIndices(NULL),
  m_hashMap(iNumTriggers)
{
  VASSERT_MSG(!VStringUtil::IsEmpty(szMapName), "Specify a valid name!");

  VStringInputMap* pMap = FindByKey(szMapName);
  VASSERT_ALWAYS_MSG(pMap == NULL, "VStringInputMap with the same name already exists.");
  
  VisObjectKey_cl::SetObjectKey(szMapName);
  AddToElementManager();

  m_pMappedIndices = new int [iNumTriggers];
}

VStringInputMap::~VStringInputMap()
{
  //do a check just in case somebody removed the map manually
  if(ElementManagerIndexOf(this)>=0)
    RemoveFromElementManager();

  V_SAFE_DELETE(m_pMappedIndices);
}

void VStringInputMap::OneTimeInit()
{
  //Vision::Callbacks.OnEditorModeChanged += this;
}

void VStringInputMap::OneTimeDeInit()
{
  //Vision::Callbacks.OnEditorModeChanged -= this;

  int iCount = ElementManagerGetSize();

  for(int i=0;i<iCount;i++)
  {
    VStringInputMap *pMap = ElementManagerGetAt(i);
    if(pMap!=NULL)
    {
      //in case the user does not use smart pointers and did not clean up manually
      //(as it works in Lua scripts... input maps are just created, but not explicitly deleted!)
      if(pMap->GetRefCount()==0)
        pMap->AddRef();

      pMap->Release();
    }
  }

  VASSERT_MSG(ElementManagerDeleteAllUnRef()==0, "Found unreferenced VStringInputMap after calling Release()");
}

//void VScriptResourceManager::OnHandleCallback(IVisCallbackDataObject_cl *pData)
//{
//}


int VStringInputMap::MapTrigger(const char* szTriggerName, IVInputDevice &inputDevice, unsigned int uiControl, const VInputOptions &options, int iOptTriggerIndex)
{
  int iTriggerIndex = 0;
  int *pTriggerIndex = &iTriggerIndex;

  //check if the string key exists in the hash map
  if(!m_hashMap.Lookup(szTriggerName, pTriggerIndex))
  {
    if(iOptTriggerIndex!=-1)
    {
      iTriggerIndex = iOptTriggerIndex;
    }
    else
    {
      iTriggerIndex = GetNextFreeTriggerIndex();

      if(iTriggerIndex<0)
        return -1;
    }

    //since there are as many array elements as trigger we can directly map the
    //indices instead of tracking them separately in an additional counter
    m_pMappedIndices[iTriggerIndex] = iTriggerIndex;

    //store the new trigger index assignment in the hash map and map it
    m_hashMap.SetAt(szTriggerName, &m_pMappedIndices[iTriggerIndex]);
    return VInputMap::MapTrigger(iTriggerIndex, inputDevice, uiControl, options);
  }

  //we have to de-reference the pointer, since we are interested into the value and the hash map just handles pointers
  return VInputMap::MapTrigger((int)*pTriggerIndex, inputDevice, uiControl, options);
}

int VStringInputMap::MapTrigger(const char* szTriggerName, VTouchArea* pArea, unsigned int uiControl, const VInputOptions &options, int iOptTriggerIndex)
{
  int iTriggerIndex = 0;
  int *pTriggerIndex = &iTriggerIndex;

  //check if the string key exists in the hash map
  if(!m_hashMap.Lookup(szTriggerName, pTriggerIndex))
  {
    if(iOptTriggerIndex!=-1)
    {
      iTriggerIndex = iOptTriggerIndex;
    }
    else
    {
      iTriggerIndex = GetNextFreeTriggerIndex();

      if(iTriggerIndex<0)
        return -1;
    }

    //since there are as many array elements as trigger we can directly map the
    //indices instead of tracking them separately in an additional counter
    m_pMappedIndices[iTriggerIndex] = iTriggerIndex;
    m_hashMap.SetAt(szTriggerName, &m_pMappedIndices[iTriggerIndex]);

    //store the new trigger index assignment in the hash map and map it
    return VInputMap::MapTrigger(iTriggerIndex, pArea, uiControl, options);
  }

  //we have to de-reference the pointer, since we are interested into the value and the hash map just handles pointers
  return VInputMap::MapTrigger((int)*pTriggerIndex, pArea, uiControl, options);
}

int VStringInputMap::MapTriggerAxis(const char* szTriggerName, IVInputDevice &inputDevice, unsigned int uiControlNegative, unsigned int uiControlPositive, const VInputOptions &options, int iOptTriggerIndex)
{
  int iTriggerIndex = 0;
  int *pTriggerIndex = &iTriggerIndex;

  //check if the string key exists in the hash map
  if(!m_hashMap.Lookup(szTriggerName, pTriggerIndex))
  {
    if(iOptTriggerIndex!=-1)
    {
      iTriggerIndex = iOptTriggerIndex;
    }
    else
    {
      iTriggerIndex = GetNextFreeTriggerIndex();

      if(iTriggerIndex<0)
        return -1;
    }

    //since there are as many array elements as trigger we can directly map the
    //indices instead of tracking them separately in an additional counter
    m_pMappedIndices[iTriggerIndex] = iTriggerIndex;
    m_hashMap.SetAt(szTriggerName, &m_pMappedIndices[iTriggerIndex]);

    //store the new trigger index assignment in the hash map and map it
    return VInputMap::MapTriggerAxis(iTriggerIndex, inputDevice, uiControlNegative, uiControlPositive, options);
  }

  //we have to de-reference the pointer, since we are interested into the value and the hash map just handles pointers
  return VInputMap::MapTriggerAxis((int)*pTriggerIndex, inputDevice, uiControlNegative, uiControlPositive, options);
}

int VStringInputMap::GetNextFreeTriggerIndex() const
{
  //check all trigger indices
  for (int iTriggerIndex=0;iTriggerIndex<m_iNumMappedInputs;iTriggerIndex++)
  {
    bool bIsFree = true;

    //a trigger index is considered as free (available) if no alternative is in use
    for(int iAlt=0;iAlt<m_iNumAlternatives;iAlt++)
    {
      if(m_ppMappedInputs[iTriggerIndex*m_iNumAlternatives + iAlt]!=NULL)
      {
        bIsFree = false;
        break; //break inner loop
      }
    }

    if(bIsFree)
      return iTriggerIndex;
  }

  // No free trigger index was found.
  return -1;
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
