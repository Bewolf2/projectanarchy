/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/StochasticalParam.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>



///////////////////////////////////////////////////////////////////////////////////
// VisStochasticalParam_cl
///////////////////////////////////////////////////////////////////////////////////

bool VisStochasticalParam_cl::DataExchangeXML(const char *szName, TiXmlElement *pParent, bool bWrite)
{
  if (!pParent) return false;
  TiXmlElement *pNewNode = XMLHelper::SubNode(pParent,szName,bWrite);
  if (!pNewNode) return false;
  XMLHelper::Exchange_Float(pNewNode,"average",m_fAverage,bWrite);
  XMLHelper::Exchange_Float(pNewNode,"variation",m_fVariation,bWrite);
  return true;
}


V_IMPLEMENT_SERIALX( VisStochasticalParam_cl);
void VisStochasticalParam_cl::SerializeX( VArchive &ar )
{
  if (ar.IsLoading())
  {
    ar >> m_fAverage >> m_fVariation;
  } else
  {
    ar << m_fAverage << m_fVariation;
  }
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
