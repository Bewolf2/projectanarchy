/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/VisionEnginePluginPCH.h>
#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/Curve.hpp>
#include <Vision/Runtime/Base/ThirdParty/tinyXML/TinyXMLHelper.hpp>
#include <Vision/Runtime/Base/System/Memory/VMemDbg.hpp>

VCurve2DBase::VCurve2DBase(int iNumPoints)
{
  m_fDuration = -1.f;
  m_fMax = -1000000000.f;
  m_iNumCurvePoints = 0;
  m_pPoints = NULL;
  m_fLookupCount = 0.f;
  m_iLookupCount = 0;
  m_pLookupValues = NULL;

  AllocatePoints(iNumPoints);
}

void VCurve2DBase::AllocatePoints(int iCount)
{
  FreePoints();
  m_iNumCurvePoints = iCount;
  if (iCount>0)
    m_pPoints = new VCurvePoint2D[iCount];
}

void VCurve2DBase::FreePoints()
{
  m_iNumCurvePoints = 0;
  V_SAFE_DELETE_ARRAY(m_pPoints);
}


void VCurve2DBase::UpdateCurve()
{
  m_fDuration = -1.f;
  m_fMax = -1000000000.f;
  if (!m_pPoints)
    return;

  // duration from the last point
  m_fDuration = m_pPoints[m_iNumCurvePoints-1].m_vPos.x;

  VCurvePoint2D *p = m_pPoints;
  for (int i=0;i<m_iNumCurvePoints;i++,p++)
  {
    if (i==0||p->m_vPos.y>m_fMax)
      m_fMax = p->m_vPos.y;
  }

  if (m_iLookupCount>0)
    CreateLookup(m_iLookupCount);
}


float VCurve2DBase::GetValue(float x) const
{
  // no points defined
  if (!IsValid())
    return 0.f;

  // only one curve point defined
  if (m_iNumCurvePoints==1)
    return m_pPoints[0].m_vPos.y;

  if (x<0.f) 
    x = 0.f;
  else if 
    (x>m_fDuration) x = m_fDuration;

  int iIndex2 = 1;
  // find the upper limit
  while (iIndex2<m_iNumCurvePoints-1 && x>=m_pPoints[iIndex2].m_vPos.x)
    iIndex2++;
  int iIndex1 = iIndex2-1;

  // interpolate between the two curve points
  const VCurvePoint2D *p1 = &m_pPoints[iIndex1];
  const VCurvePoint2D *p2 = &m_pPoints[iIndex2];

  float fdx = p2->m_vPos.x - p1->m_vPos.x;
  VASSERT(fdx>0);

  float t = (x-p1->m_vPos.x) / fdx; // now between 0 and 1
  VASSERT(t > -HKVMATH_LARGE_EPSILON && t < 1.0f + HKVMATH_LARGE_EPSILON);
  float ct = 1.f-t;

  float y = ct*ct*ct *  p1->m_vPos.y +
            3.f*t*ct*ct * (p1->m_vPos.y + p1->m_vTanOut.y) + 
            3.f*t*t*ct * (p2->m_vPos.y + p2->m_vTanIn.y) +
            t*t*t*  p2->m_vPos.y;

  return y;
}


VCurvePoint2D VCurve2DBase::GetBezierPoint(float x) const
{
  // no points defined
  if (!IsValid())
  {
    VCurvePoint2D point;
    point.m_vPos.x = 0.0f;
    point.m_vPos.y = 0.0f;
    return point;
  }

  // only one curve point defined
  if (m_iNumCurvePoints==1)
    return m_pPoints[0];

  if (x<0.f) 
    x = 0.f;
  else if 
    (x>m_fDuration) x = m_fDuration;

  int iIndex2 = 1;
  // find the upper limit
  while (iIndex2<m_iNumCurvePoints-1 && x>=m_pPoints[iIndex2].m_vPos.x)
    iIndex2++;
  int iIndex1 = iIndex2-1;

  // interpolate between the two curve points
  const VCurvePoint2D *p1 = &m_pPoints[iIndex1];
  const VCurvePoint2D *p2 = &m_pPoints[iIndex2];

  float fdx = p2->m_vPos.x - p1->m_vPos.x;
  VASSERT(fdx>0);

  float t = (x-p1->m_vPos.x) / fdx; // now between 0 and 1
  VASSERT(t>-HKVMATH_LARGE_EPSILON && t<1.f+HKVMATH_LARGE_EPSILON);
  float ct = 1.f-t;

  VCurvePoint2D point;

  point.m_vPos.x = ct*ct*ct *  p1->m_vPos.x +
            3.f*t*ct*ct * (p1->m_vPos.x + p1->m_vTanOut.x) + 
            3.f*t*t*ct * (p2->m_vPos.x + p2->m_vTanIn.x) +
            t*t*t*  p2->m_vPos.x;

  point.m_vPos.y = ct*ct*ct *  p1->m_vPos.y +
            3.f*t*ct*ct * (p1->m_vPos.y + p1->m_vTanOut.y) + 
            3.f*t*t*ct * (p2->m_vPos.y + p2->m_vTanIn.y) +
            t*t*t*  p2->m_vPos.y;

  return point;
}


float VCurve2DBase::GetValue(VCurvePoint2D *points, int iPointCount, float x) const
{
  // no points defined
  if (!IsValid())
    return 0.f;

  // only one curve point defined
  if (m_iNumCurvePoints==1)
    return points[0].m_vPos.y;

  if (x<0.f) x = 0.f;
    else if (x>m_fDuration) x = m_fDuration;

  int iIndex2 = 1;
  // find the upper limit
  while (iIndex2<iPointCount-1 && x>=points[iIndex2].m_vPos.x)
    iIndex2++;
  int iIndex1 = iIndex2-1;

  // interpolate between the two curve points
  VCurvePoint2D p1 = points[iIndex1];
  VCurvePoint2D p2 = points[iIndex2];

  // Delta between current and next curve point
  float fdx = p2.m_vPos.x - p1.m_vPos.x;
  float fdy = p2.m_vPos.y - p1.m_vPos.y;
  
  // Get relative x position
  float frx = (x - p1.m_vPos.x) / fdx;
  
  // Calculate y position (approximation)
  float y = p1.m_vPos.y + (fdy * frx);

  return y;
}



void VCurve2DBase::FreeLookup()
{
  m_iLookupCount = 0;
  V_SAFE_DELETE_ARRAY(m_pLookupValues);
}


int VCurve2DBase::GetSuitableTableSize(float fDuration) const
{
  if (fDuration<=1.f)
    return 64;
  if (fDuration<=4.f)
    return 256;
  if (fDuration<=16.f)
    return 1024;

  return 2048;
}


void VCurve2DBase::CreateLookup(int iSampleCount, float *pArray, int iValueStride, float fScaling, float fBias)
{

  // lookup bezier points
  float x = 0.0f;                                       // X-Coordinate of Lookup
  float fdx = GetDuration()/(float)(iSampleCount-1);  // Step size of x

  // Iterate bezier curve and create PointList
  VMemoryTempBuffer<512*sizeof(VCurvePoint2D)> buffer(iSampleCount*sizeof(VCurvePoint2D));
  VCurvePoint2D *pPoints = (VCurvePoint2D *)buffer.GetBuffer();
  for (int i = 0; i < iSampleCount; i++, x += fdx)
    pPoints[i] = GetBezierPoint(x);

  // Fill lookup table
  x = 0.0f;
  for (int i = 0; i < iSampleCount; i++, x += fdx)
    pArray[i*iValueStride] = GetValue(pPoints, iSampleCount, x)*fScaling + fBias;
}

void VCurve2DBase::CreateLookup(int iSampleCount, float fScaling)
{
  VASSERT(iSampleCount>1);

  // Lookup values
  if (iSampleCount!=m_iLookupCount)
  {
    FreeLookup();
    m_iLookupCount = iSampleCount;
    m_fLookupCount = ((float)iSampleCount)-0.001f;
    m_pLookupValues = new float[iSampleCount];
  }
  if (!m_pLookupValues)
    return;

  CreateLookup(iSampleCount,m_pLookupValues,1,fScaling);
}



bool VCurvePoint2D::DataExchangeXML(TiXmlElement *pPointNode, bool bWrite)
{
  if (!pPointNode)
    return false;
      
  XMLHelper::Exchange_Floats(pPointNode,"pos",m_vPos.data,2,bWrite);
  XMLHelper::Exchange_Floats(pPointNode,"t_in",m_vTanIn.data,2,bWrite);
  XMLHelper::Exchange_Floats(pPointNode,"t_out",m_vTanOut.data,2,bWrite);

  return true;
}


bool VCurve2DBase::DataExchangeXML(TiXmlElement *pCurveNode, bool bWrite)
{
  if (!pCurveNode)
    return false;
  const char *szPointNodeName = "point";

  // curve points
  int iCount = GetNumPoints();
  XMLHelper::Exchange_Int(pCurveNode,"numpoints",iCount,bWrite);
  if (bWrite)
  {
    // write out points
    VCurvePoint2D *p = m_pPoints;
    for (int i=0;i<iCount;i++,p++)
    {
      TiXmlElement *pPointNode = XMLHelper::SubNode(pCurveNode,szPointNodeName,bWrite);
      p->DataExchangeXML(pPointNode,bWrite);
    }
  }
  else
  {
    // allocate and read points
    AllocatePoints(iCount);
    int iIndex = 0;   
    TiXmlNode *pFirstPoint = pCurveNode->FirstChild(szPointNodeName);
    if (pFirstPoint)
    {
	    for (TiXmlElement *pPoint=pFirstPoint->ToElement(); pPoint!=NULL; pPoint=pPoint->NextSiblingElement(szPointNodeName),iIndex++)
      {
        VASSERT(iIndex<iCount && "More point nodes defined than defined via 'numpoints' attribute");
        // First point should have x = 0.f
        if (iIndex == 0)
          m_pPoints[iIndex].m_vPos.x = 0.f;
        if (iIndex<iCount)
          m_pPoints[iIndex].DataExchangeXML(pPoint,bWrite);
      }
    }
    UpdateCurve();
  }

  return true;
}


V_IMPLEMENT_SERIALX( VCurve2DBase);
void VCurve2DBase::SerializeX( VArchive &ar )
{
  char iVersion = 0;
  if (ar.IsLoading())
  {
    int iLookupCount;
    ar >> iVersion; VASSERT(iVersion==0);
    ar >> m_iNumCurvePoints;
    AllocatePoints(m_iNumCurvePoints);
    if (m_iNumCurvePoints>0)
      ar.Read(m_pPoints,m_iNumCurvePoints*sizeof(VCurvePoint2D),"ffffff",m_iNumCurvePoints);
    ar >> m_fDuration >> m_fMax;
    ar >> iLookupCount;
    if (iLookupCount>0)
      CreateLookup(iLookupCount);
  } else
  {
    ar << iVersion;
    ar << m_iNumCurvePoints;
    if (m_iNumCurvePoints>0)
      ar.Write(m_pPoints,m_iNumCurvePoints*sizeof(VCurvePoint2D),"ffffff",m_iNumCurvePoints);
    ar << m_fDuration << m_fMax;
    ar << m_iLookupCount;
  }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////
// color curve
/////////////////////////////////////////////////////////////////////////////////////////////////////


VColorCurve::VColorCurve()
{
}

VColorCurve::~VColorCurve()
{
}


void VColorCurve::UpdateCurve(bool bUpdateChannels)
{
  m_fDuration = -1.f;
  for (int i=0;i<4;i++)
  {
    VCurve2DBase *pChannel = &m_Channel[i];
    if (bUpdateChannels)
      pChannel->UpdateCurve();
    if (!pChannel->IsValid())
      continue;
    if (pChannel->GetDuration()>m_fDuration)
      m_fDuration = pChannel->GetDuration();
  }
}


static inline UBYTE CLAMPED(float f)
{
  if (f<=0.f) return 0;
  if (f>=1.f) return 255;
  return (UBYTE)(f*255.99f);
}

void VColorCurve::CreateLookup(int iSamples)
{
  VASSERT(iSamples>1);
  if (!m_spLookup || m_spLookup->GetWidth()!=iSamples)
  {
    m_spLookup = new VisBitmap_cl("<CurveLookup>",iSamples,1);
    m_spLookup->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
  }

  // create the colors
  float fStep = GetDuration()/(float)(iSamples-1);
  float x = 0.f;
  hkvVec4 vColor;
  VColorRef *pCol = m_spLookup->GetDataPtr();
  for (int i=0;i<iSamples;i++,x+=fStep,pCol++)
  {
    vColor.r = m_Channel[0].IsValid() ? m_Channel[0].GetValue(x) : 0.f;
    vColor.g = m_Channel[1].IsValid() ? m_Channel[1].GetValue(x) : 0.f;
    vColor.b = m_Channel[2].IsValid() ? m_Channel[2].GetValue(x) : 0.f;
    vColor.a = m_Channel[3].IsValid() ? m_Channel[3].GetValue(x) : 1.f;

    // convert to 32 bit RGBA
    pCol->SetRGBA(CLAMPED(vColor.r),CLAMPED(vColor.g),CLAMPED(vColor.b),CLAMPED(vColor.a));
  }
}

bool VColorCurve::DataExchangeXML(TiXmlElement *pCurveNode, bool bWrite)
{
  if (!pCurveNode)
    return false;

  if (bWrite)
  {
    if (m_Channel[0].IsValid()) m_Channel[0].DataExchangeXML(XMLHelper::SubNode(pCurveNode,"curve_red",bWrite),bWrite);
    if (m_Channel[1].IsValid()) m_Channel[1].DataExchangeXML(XMLHelper::SubNode(pCurveNode,"curve_green",bWrite),bWrite);
    if (m_Channel[2].IsValid()) m_Channel[2].DataExchangeXML(XMLHelper::SubNode(pCurveNode,"curve_blue",bWrite),bWrite);
    if (m_Channel[3].IsValid()) m_Channel[3].DataExchangeXML(XMLHelper::SubNode(pCurveNode,"curve_alpha",bWrite),bWrite);
  } else
  {
    TiXmlElement *pChannel;

    pChannel = XMLHelper::SubNode(pCurveNode,"curve_red",bWrite);
    if (pChannel) m_Channel[0].DataExchangeXML(pChannel,bWrite);
    pChannel = XMLHelper::SubNode(pCurveNode,"curve_green",bWrite);
    if (pChannel) m_Channel[1].DataExchangeXML(pChannel,bWrite);
    pChannel = XMLHelper::SubNode(pCurveNode,"curve_blue",bWrite);
    if (pChannel) m_Channel[2].DataExchangeXML(pChannel,bWrite);
    pChannel = XMLHelper::SubNode(pCurveNode,"curve_alpha",bWrite);
    if (pChannel) m_Channel[3].DataExchangeXML(pChannel,bWrite);
    UpdateCurve(false);
  }

  return true;
}


V_IMPLEMENT_SERIALX( VColorCurve);
void VColorCurve::SerializeX( VArchive &ar )
{
  char iVersion = 0;
  short iLookupSamples = 0;
  if (ar.IsLoading())
  {
    ar >> iVersion; VASSERT(iVersion==0);
    ar >> m_fDuration;
    ar >> m_Channel[0] >> m_Channel[1] >> m_Channel[2] >> m_Channel[3];
    ar >> iLookupSamples;
    if (iLookupSamples>0)
      CreateLookup(iLookupSamples);
  } else
  {
    ar << iVersion;
    ar << m_fDuration;
    ar << m_Channel[0] << m_Channel[1] << m_Channel[2] << m_Channel[3];
    if (m_spLookup)
       iLookupSamples = m_spLookup->GetWidth();
    ar << iLookupSamples;
  }
}

  
// static helper function
VColorCurve* VColorCurve::DoArchiveExchange( VArchive &ar, VColorCurve *pSource )
{
  bool bDefined;
  if (ar.IsLoading())
  {
    ar >> bDefined;
    if (!bDefined) return NULL;
    if (!pSource)
      pSource = new VColorCurve();
    ar >> pSource;
  }
  else
  {
    bDefined = pSource!=NULL;
    ar << bDefined;
    if (bDefined)
      ar << pSource;
  }
  return pSource;
}


// static helper function
VColorCurve* VColorCurve::DoArchiveLookupExchange( VArchive &ar, VColorCurve *pSource )
{
  short iSamples;
  if (ar.IsLoading())
  {
    // Version 0
    ar >> iSamples;
    if (iSamples == 0)
      return NULL;

    unsigned char uiVersion = 0;

    // Mode added in Version 1
    if (iSamples < 0)
    {
      iSamples = -iSamples;
      ar >> uiVersion;

      VASSERT(uiVersion == 1);
    }

    ANALYSIS_IGNORE_WARNING_ONCE(6211)
    pSource = new VColorCurve();

    // In Version 0 we wrote the sample array, from Version 1 and up the array is simply recreated on load
    if (uiVersion == 0)
    {
      pSource->m_spLookup = new VisBitmap_cl("<CurveLookup>",iSamples,1);
      pSource->m_spLookup->SetResourceFlag(VRESOURCEFLAG_AUTODELETE);
      VColorRef *pData = pSource->m_spLookup->GetDataPtr();
      for (int i=0;i<iSamples;i++)
        ar >> pData[i];
    }

    if (uiVersion >= 1)
    {
      // Added in Version 1
      for(int i = 0; i < 4; ++i)
        ar >> pSource->m_Channel[i];

      pSource->UpdateCurve(false);
      pSource->CreateLookup(iSamples);
    }
  } 
  else
  {
    if (pSource!=NULL && pSource->m_spLookup!=NULL)
    {
      iSamples = (short)pSource->m_spLookup->GetWidth();

      VASSERT(iSamples > 0);

      // The great elders did not think that this code might ever change and therefore did not include a version number for the file format.
      // The number of samples cannot be negative. Thus the sign-bit of the samples number now indicates whether this is the old version zero, or at least version 1
      // In case of version 1, a version number is written, to ensure that future updates can be more easily done

      ar << (short) -iSamples; // a negative sample number indicates that this is version 1 and thus a version byte will follow
      const unsigned char uiVersion = 1;

      // Added in Version 1
      ar << uiVersion;

      // Version 0 wrote the sample array, in Version 1 it is simply recreated from the source data
      //VColorRef *pData = pSource->m_spLookup->GetDataPtr();
      //for (int i=0;i<iSamples;i++)
      //  ar << pData[i];

      // Version 1 stuff
      for(int i = 0; i < 4; ++i)
        ar << pSource->m_Channel[i];
    }
    else
    {
      iSamples = 0;
      ar << iSamples;
    }
  }
  return pSource;
}

VCurve2D* VCurve2D::DoArchiveExchange( VArchive &ar, VCurve2D *pSource)
{
  bool bDefined;
  if (ar.IsLoading())
  {
    ar >> bDefined;
    if (!bDefined) return NULL;
    if (!pSource)
      pSource = new VCurve2D();
    ar >> pSource;
  }
  else
  {
    bDefined = pSource!=NULL;
    ar << bDefined;
    if (bDefined)
      ar << pSource;
  }
  return pSource;
}


// static helper function
VCurve2D* VCurve2D::DoArchiveLookupExchange( VArchive &ar, VCurve2D *pSource )
{
  short iSamples;
  if (ar.IsLoading())
  {
    ar >> iSamples;
    if (iSamples == 0)
      return NULL;

    unsigned char uiVersion = 0;
    if (iSamples < 0)
    {
      iSamples = -iSamples;
      ar >> uiVersion;

      VASSERT(uiVersion == 1);
    }

    ANALYSIS_IGNORE_WARNING_ONCE(6211)
    pSource = new VCurve2D();

    if (uiVersion == 0)
    {
      pSource->m_iLookupCount = iSamples;
      pSource->m_fLookupCount = ((float)iSamples)-0.001f;
      pSource->m_pLookupValues = new float[iSamples];
      ar.Read(pSource->m_pLookupValues,iSamples*sizeof(float),"f",iSamples);
    }

    if (uiVersion >= 1)
    {
      pSource->SerializeX(ar);
      pSource->CreateLookup(iSamples);
    }
  } 
  else
  {
    if (pSource!=NULL && pSource->m_iLookupCount>0)
    {
      iSamples = (short)pSource->m_iLookupCount;
      VASSERT(iSamples > 0);

      // indicates that this is not Version 0
      ar << (short) -iSamples;

      // Added in Version 1
      const unsigned char uiVersion = 1;
      ar << uiVersion;

      //ar.Write(pSource->m_pLookupValues,iSamples*sizeof(float),"f",iSamples);

      // Added in Version 1
      pSource->SerializeX(ar);
    }
    else
    {
      iSamples = 0;
      ar << iSamples;
    } 
  }
  return pSource;
}


VCurve2D *VCurve2D::Exchange_Curve(TiXmlElement *pParent,const char *szAttribName, VCurve2D *pCurve, bool bWrite)
{
  if (!pParent)
    return pCurve;
  if (bWrite)
  {
    if (pCurve && pCurve->IsValid())
      pCurve->DataExchangeXML(XMLHelper::SubNode(pParent,szAttribName,true),true);
    return pCurve;
  }

  TiXmlElement *pCurveNode = XMLHelper::SubNode(pParent,szAttribName,false);
  if (!pCurveNode)
    return pCurve;
  if (!pCurve)
    pCurve = new VCurve2D();
  pCurve->DataExchangeXML(pCurveNode,bWrite);
  return pCurve;
}


  
void VColorCurve::CreateGradient(VColorRef iColor1,VColorRef iColor2)
{
  for (int i=0;i<4;i++)
  {
    VCurve2DBase *pChannel = &m_Channel[i];
    pChannel->AllocatePoints(2);
    VCurvePoint2D* p = pChannel->GetPoints();
    switch(i)
    {
      case 0:
        p[0].m_vPos.x = 0.f;
        p[0].m_vPos.y = (float)iColor1.r * V_DIV255;
        p[1].m_vPos.x = 1.f;
        p[1].m_vPos.y = (float)iColor2.r * V_DIV255;
        break;
      case 1:
        p[0].m_vPos.x = 0.f;
        p[0].m_vPos.y = (float)iColor1.g * V_DIV255;
        p[1].m_vPos.x = 1.f;
        p[1].m_vPos.y = (float)iColor2.g * V_DIV255;
        break;
      case 2:
        p[0].m_vPos.x = 0.f;
        p[0].m_vPos.y = (float)iColor1.b * V_DIV255;
        p[1].m_vPos.x = 1.f;
        p[1].m_vPos.y = (float)iColor2.b * V_DIV255;
        break;
      case 3:
        p[0].m_vPos.x = 0.f;
        p[0].m_vPos.y = (float)iColor1.a * V_DIV255;
        p[1].m_vPos.x = 1.f;
        p[1].m_vPos.y = (float)iColor2.a * V_DIV255;
        break;
    }
  }
}



VColorCurve *VColorCurve::Exchange_ColorCurve(TiXmlElement *pParent,const char *szAttribName, VColorCurve *pCurve, bool bWrite)
{
  if (!pParent)
    return pCurve;
  if (bWrite)
  {
    if (pCurve && pCurve->IsValid())
      pCurve->DataExchangeXML(XMLHelper::SubNode(pParent,szAttribName,bWrite),bWrite);
    return pCurve;
  }

  TiXmlElement *pCurveNode = XMLHelper::SubNode(pParent,szAttribName,bWrite);
  if (!pCurveNode)
    return pCurve;
  if (!pCurve)
    pCurve = new VColorCurve();
  pCurve->DataExchangeXML(pCurveNode,bWrite);
  return pCurve;
}

///////////////////////////////////////////////////////////////////////////
// class VPositionCurve
///////////////////////////////////////////////////////////////////////////

void VPositionCurve::CreateLookup(int iSampleCount, float fScale)
{
  VASSERT(iSampleCount>0);
  // Lookup values
  if (iSampleCount!=m_iLookupCount)
  {
    FreeLookup();
    m_iLookupCount = iSampleCount;
    m_fLookupCount = ((float)iSampleCount)-0.001f;
    m_pLookupValues = new hkvVec3[iSampleCount];
  }
  m_X.CreateLookup(iSampleCount,&m_pLookupValues->x,3,fScale); // fill array with stride 3
  m_Y.CreateLookup(iSampleCount,&m_pLookupValues->y,3,fScale); // fill array with stride 3
  m_Z.CreateLookup(iSampleCount,&m_pLookupValues->z,3,fScale); // fill array with stride 3
}

void VPositionCurve::UpdateCurve(bool bUpdateChannels)
{
  m_fDuration = -1.f;
  for (int i=0;i<4;i++)
  {
    VCurve2DBase &channel = GetChannel(i);
    if (bUpdateChannels)
      channel.UpdateCurve();
    if (!channel.IsValid())
      continue;
    if (channel.GetDuration()>m_fDuration)
      m_fDuration = channel.GetDuration();
  }
}

VPositionCurve *VPositionCurve::Exchange_PositionCurve(TiXmlElement *pParent,const char *szAttribName, VPositionCurve *pCurve, bool bWrite)
{
  if (!pParent)
    return pCurve;
  if (bWrite)
  {
    if (pCurve && pCurve->IsValid())
      pCurve->DataExchangeXML(XMLHelper::SubNode(pParent,szAttribName,bWrite),bWrite);
    return pCurve;
  }

  TiXmlElement *pCurveNode = XMLHelper::SubNode(pParent,szAttribName,bWrite);
  if (!pCurveNode)
    return pCurve;
  if (!pCurve)
    pCurve = new VPositionCurve();
  pCurve->DataExchangeXML(pCurveNode,bWrite);
  return pCurve;
}

bool VPositionCurve::DataExchangeXML(TiXmlElement *pCurveNode, bool bWrite)
{
  if (!pCurveNode)
    return false;

  if (bWrite)
  {
    if (m_X.IsValid()) m_X.DataExchangeXML(XMLHelper::SubNode(pCurveNode,"x",bWrite),bWrite);
    if (m_Y.IsValid()) m_Y.DataExchangeXML(XMLHelper::SubNode(pCurveNode,"y",bWrite),bWrite);
    if (m_Z.IsValid()) m_Z.DataExchangeXML(XMLHelper::SubNode(pCurveNode,"z",bWrite),bWrite);
  } else
  {
    TiXmlElement *pChannel;
    pChannel = XMLHelper::SubNode(pCurveNode,"x",bWrite);
    if (pChannel) m_X.DataExchangeXML(pChannel,bWrite);
    pChannel = XMLHelper::SubNode(pCurveNode,"y",bWrite);
    if (pChannel) m_Y.DataExchangeXML(pChannel,bWrite);
    pChannel = XMLHelper::SubNode(pCurveNode,"z",bWrite);
    if (pChannel) m_Z.DataExchangeXML(pChannel,bWrite);
    UpdateCurve(false);
  }

  return true;
}


// static helper function
VPositionCurve* VPositionCurve::DoArchiveLookupExchange( VArchive &ar, VPositionCurve *pSource, bool bBrokenCount )
{
  short iSamples;
  if (ar.IsLoading())
  {
    ar >> iSamples;
    if (iSamples<=0)
      return NULL;
    ANALYSIS_IGNORE_WARNING_ONCE(6211)
    pSource = new VPositionCurve();
    pSource->m_iLookupCount = iSamples;
    pSource->m_fLookupCount = ((float)iSamples)-0.001f;
    pSource->m_pLookupValues = new hkvVec3[iSamples];
    if (bBrokenCount)
      ar.Read(pSource->m_pLookupValues,iSamples*sizeof(float),"f",iSamples); // OLD version
    else
      ar.Read(pSource->m_pLookupValues,iSamples*sizeof(float)*3,"fff",iSamples);
  } else
  {
    if (pSource!=NULL && pSource->m_iLookupCount>0)
    {
      iSamples = (short)pSource->m_iLookupCount;
      ar << iSamples;
      ar.Write(pSource->m_pLookupValues,iSamples*sizeof(float)*3,"fff",iSamples);
    }
    else
    {
      iSamples = 0;
      ar << iSamples;
    } 
  }
  return pSource;
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
