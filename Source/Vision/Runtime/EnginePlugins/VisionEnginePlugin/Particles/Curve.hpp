/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file Curve.hpp

#ifndef CURVE_HPP_INCLUDED
#define CURVE_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Particles/ParticleModule.hpp>

// forward declarations
class TiXmlElement;
class VColorCurve;
class VCurve2D;
class VPositionCurve;

typedef VSmartPtr<VCurve2D> VCurve2DPtr;
typedef VSmartPtr<VColorCurve> VColorCurvePtr;
typedef VSmartPtr<VPositionCurve> VPositionCurvePtr;


///\brief
///Defines one point in a curve definition
///
///\see
///VCurve2DBase
struct VCurvePoint2D
{
public:

  ///\brief
  ///Helper function for XML serialization
  ///
  ///\param pPointNode
  ///XML node
  ///
  ///\param bWrite
  ///write flag
  ///
  ///\returns
  ///true if successful
  ///
  bool DataExchangeXML(TiXmlElement *pPointNode, bool bWrite);

  hkvVec2 m_vPos; ///< point position
  hkvVec2 m_vTanIn; ///< in-tangent
  hkvVec2 m_vTanOut; ///< out-tangent
};


///\brief
///Base class for 2D curve definitions
///
class VCurve2DBase
{
public:
  ///\brief
	///Constructor
	///
	///\param iNumPoints
	///Number of points to allocate
	PARTICLE_IMPEXP VCurve2DBase(int iNumPoints=0);

  ///\brief
	///Destructor
	PARTICLE_IMPEXP virtual ~VCurve2DBase() {FreeLookup();FreePoints();}

  ///\brief
	///Indicates whether this curve is valid
	///
	///\returns
	///Valid status
	///
	inline bool IsValid() const {return m_iNumCurvePoints>0;}

  ///\brief
	///Allocates curve points for this curve
	///
	///\param iCount
	///Number of points to allocate
	///
	PARTICLE_IMPEXP void AllocatePoints(int iCount);

  ///\brief
	///Release allocated points. Called by the destructor
	///
	PARTICLE_IMPEXP void FreePoints();

  ///\brief
	///Internal function that must be called after any point changed
	///
	PARTICLE_IMPEXP void UpdateCurve();

  ///\brief
	///Calculate actual curve value (slow but accurate)
	///
	///\param x
	///Curve parameter - must be between the x coordinates of first and last position. See GetDuration
	///
	///\returns
	///Curve value
	///
	PARTICLE_IMPEXP float GetValue(float x) const;

  ///\brief
	///Fast non-interpolated lookup in pre-calculated table
	///
	///\param x
  ///Curve parameter, must be [0..1]
	///
	///\returns
	///Curve value
	///
	inline float GetValueFast(float x)
  {
    VASSERT(m_pLookupValues);
    VASSERT(x>=0.f && x<=1.f); ///< 1.f is still OK since m_fLookupCount takes care
    return m_pLookupValues[(int)(m_fLookupCount*x)];
  }

  ///\brief
	///Fast interpolated lookup in pre-calculated table
	///
	///\param x
  ///Curve parameter, must be [0..1]
	///
	///\returns
	///Curve value
  inline float GetValueFastInterpolated(float x)
  {
    VASSERT(m_pLookupValues);
    VASSERT(x>=0.f && x<=1.f); ///< 1.f is still OK since m_fLookupCount takes care
    int lowerIndex = (int)((m_iLookupCount-1)*x);
    int upperIndex = lowerIndex + 1;
    
    if (upperIndex >= m_iLookupCount) 
        return m_pLookupValues[lowerIndex];

    float y1 = m_pLookupValues[lowerIndex];
    float y2 = m_pLookupValues[upperIndex];

    return y1 + (y2 - y1) * ((m_iLookupCount-1)*x - lowerIndex);
  }

  ///\brief
	///Returns the x-coordinate of the last curve point
	///
	///\returns
	///x-coordinate of the last curve point
	///
	inline float GetDuration() const {return m_fDuration;}

  ///\brief
	///Returns the number of allocated curve points
	///
	///\returns
	///Number of curve points
	///
	inline int GetNumPoints() const {return m_iNumCurvePoints;}

  ///\brief
	///Returns a pointer to the array of curve points
	///
	///\returns
	///Pointer to the first point
	///
	inline VCurvePoint2D* GetPoints() const {return m_pPoints;}

  ///\brief
	///Copies the passed array of points
	///
	///\param pPoints
	///SOurce array to copy
	///
	///\param iNumPoints
	///Number of source points, will be allocated by this operation
	///
	inline void SetPoints(VCurvePoint2D* pPoints, int iNumPoints) 
  { 
    AllocatePoints(iNumPoints);
    memcpy(m_pPoints, pPoints, iNumPoints*sizeof(VCurvePoint2D));
  }

  ///\brief
	///Release memory allocated by the lookup table. Called automatically
	///
	PARTICLE_IMPEXP void FreeLookup();

  ///\brief
	///Creates a lookup table for fast lookups
	///
	///\param iSampleCount
	///Number of samples
	///
	///\param pArray
	///NULL to use own array, otherwise pass a pointer to an array that holds enough values to store iSampleCount values
  ///
	///\param iValueStride
  ///Stride (in measures of float) to fill the array
  ///
	///\param fScaling
	///Additional scaling factor for curve values
	///
	///\param fBias
	///Additional bias value for curve values
  ///
	///\see
	///GetValueFast
	PARTICLE_IMPEXP void CreateLookup(int iSampleCount, float *pArray, int iValueStride=1, float fScaling=1.f, float fBias=0.f);

  ///\brief
	///Overloaded version that always uses its own array
  PARTICLE_IMPEXP void CreateLookup(int iSampleCount, float fScaling=1.f);

  ///\brief
	/// Indicates whether this curve has a lookup already
  inline bool HasLookup() const
  {
    return m_iLookupCount>0;
  }

  ///\brief
	///Helper function for XML serialization
	///
	///\param pCurveNode
	///XML node
	///
	///\param bWrite
	///write flag
	///
	///\returns
	///true, if successful
	///
	PARTICLE_IMPEXP bool DataExchangeXML(TiXmlElement *pCurveNode, bool bWrite);

  V_DECLARE_SERIALX( VCurve2DBase, PARTICLE_IMPEXP )

  ///\brief
	///Binary serialization function
	///
	///\param ar
	///Binary archive
	PARTICLE_IMPEXP void SerializeX( VArchive &ar );

  ///\brief
  /// Internal helper function to determine a suitable size for lookup tables, based on the duration (in seconds) of the curve
  PARTICLE_IMPEXP int GetSuitableTableSize(float fDuration) const;
protected:

  ///\brief
	///Calculate actual curve value
	///
  ///\param points
	///Point list of Bezier Curve that is used to determine an approximate y curve value
	///
  ///\param iPointCount
  ///Length of the points array
  ///
	///\param x
	///Curve parameter - must be between the x coordinates of first and last position. See GetDuration
	///
	///\returns
	///Curve value
	///
  float GetValue(VCurvePoint2D *points, int iPointCount, float x) const;

  ///\brief
	///Calculates point x/y coordinates of a bezier curve point(slow but accurate)
	///
	///\param x
	///Curve parameter - must be between the x coordinates of first and last position. See GetDuration
	///
	///\returns
	///Curve point of bezier curve at relative value x 
	///
	VCurvePoint2D GetBezierPoint(float x) const;

  // curve
  int m_iNumCurvePoints;
  VCurvePoint2D *m_pPoints;

  // curve properties
  float m_fDuration, m_fMax;

  // lookup
  int m_iLookupCount;
  float m_fLookupCount; ///< float version of m_fLookupCount
  float *m_pLookupValues;
};



///\brief
///Refcounted version of VCurve2DBase
///
class VCurve2D : public VCurve2DBase, public VRefCounter
{
public:
  ///\brief
	///Static helper function for writing to or creating from binary archive
	///
	///\param ar
	///Binary archive
	///
	///\param pSource
  ///Source curve (for write mode)
	///
	///\returns
	///A new curve if successfully read from archive
	///
	static PARTICLE_IMPEXP VCurve2D* DoArchiveExchange( VArchive &ar, VCurve2D *pSource);
	static PARTICLE_IMPEXP VCurve2D* DoArchiveLookupExchange( VArchive &ar, VCurve2D *pSource);

  ///\brief
	///Static helper function for writing to or creating from XML
	///
	///\param pParent
	///Parent XML node
	///
	///\param szAttribName
	///Name of the curve's XML attribute
	///
	///\param pCurve
  ///Source curve (for writing)
	///
	///\param bWrite
	///Write flag
	///
	///\returns
	///A new curve if successfully read from XML
	///
	PARTICLE_IMPEXP static VCurve2D *Exchange_Curve(TiXmlElement *pParent,const char *szAttribName, VCurve2D *pCurve, bool bWrite);

};


///\brief
///Color curve class. Holds 4 instances of VCurve2DBase.
///
class VColorCurve : public VRefCounter
{
public:

  ///\brief
	///Constructor
	PARTICLE_IMPEXP VColorCurve();

  ///\brief
	///Destructor
	PARTICLE_IMPEXP virtual ~VColorCurve();

  ///\brief
	///Indicates whether the curve definition is valid
	///
	///\returns
	///Valid status
	inline bool IsValid() const {return m_Channel[0].IsValid() || m_Channel[1].IsValid() || m_Channel[2].IsValid() || m_Channel[3].IsValid();}

  ///\brief
	///Returns the duration which is the x-coordinate of the last curve point
	///
	///\returns
	///x-coordinate of the last curve point
	inline float GetDuration() const {return m_fDuration;}

  ///\brief
	///Internal function that must be called after modifying any point in the curve
	///
	///\param bUpdateChannels
	///If true, every single sub-channel is also updated.
	PARTICLE_IMPEXP void UpdateCurve(bool bUpdateChannels=true);

  ///\brief
	///Returns a curve reference of a subchannel with 0=red, 1=green, 2=blue and 3=alpha
	///
	///\param iChannel
  ///Channel index [0..3]
	///
	///\returns
	///Curve reference
	///
	inline VCurve2DBase &GetChannelCurve(int iChannel) {return m_Channel[iChannel];}

  ///\brief
	///Returns a curve reference to the red color component
	///
	///\returns
	///Curve reference
	inline VCurve2DBase &GetCurveR() {return m_Channel[0];}

  ///\brief
	///Returns a curve reference to the green color component
	///
	///\returns
	///Curve reference
  inline VCurve2DBase &GetCurveG() {return m_Channel[1];}

  ///\brief
	///Returns a curve reference to the blue color component
	///
	///\returns
	///Curve reference
  inline VCurve2DBase &GetCurveB() {return m_Channel[2];}

  ///\brief
	///Returns a curve reference to the alpha color component
	///
	///\returns
	///Curve reference
  inline VCurve2DBase &GetCurveA() {return m_Channel[3];}

  ///\brief
	///Creates a color lookup table for fast lookups. The lookup is represented by class VisBitmap_cl
	///
	///\param iSamples
	///Number of samples
	///
  ///\see
  ///VisBitmap_cl
	PARTICLE_IMPEXP void CreateLookup(int iSamples);

  ///\brief
	///Releases the color lookup
	///
	void FreeLookup() {m_spLookup=NULL;}

  ///\brief
	///Returns the color lookup or NULL if no lookup has been created
	///
	///\returns
	///The color lookup
	///
	///\see
	///CreateLookup|VisBitmap_cl
	inline VisBitmap_cl *GetLookupBitmap() const {return m_spLookup;}

  ///\brief
	/// Indicates whether this curve has a lookup already
  inline bool HasLookup() const
  {
    return m_spLookup!=NULL;
  }

  ///\brief
	///Helper function for XML serialization
	///
	///\param pCurveNode
	///Parent XML node
	///
	///\param bWrite
	///Write flag
	///
	///\returns
	///true if successful
	///
	PARTICLE_IMPEXP bool DataExchangeXML(TiXmlElement *pCurveNode, bool bWrite);

  ///\brief
	///Static helper function for writing to or creating from XML
	///
	///\param pParent
	///parent XML node
	///
	///\param szAttribName
	///XML attribute name
	///
	///\param pCurve
  ///source curve (written if bWrite=true)
	///
	///\param bWrite
	///Write flag
	///
	///\returns
	///A new curve if successfully read from XML
	///
	PARTICLE_IMPEXP static VColorCurve *Exchange_ColorCurve(TiXmlElement *pParent,const char *szAttribName, VColorCurve *pCurve, bool bWrite);

  V_DECLARE_SERIALX( VColorCurve, PARTICLE_IMPEXP )

  ///\brief
	///Binary serialization function
	///
	///\param ar
	///Binary archive
	///
	PARTICLE_IMPEXP void SerializeX( VArchive &ar );

  ///\brief
	///Static helper function for writing to or creating from binary archive
	///
	///\param ar
	///Binary archive
	///
	///\param pSource
  ///Source curve (for write mode)
	///
	///\returns
	///A new curve if successfully read from archive
	///
	static PARTICLE_IMPEXP VColorCurve* DoArchiveExchange( VArchive &ar, VColorCurve *pSource );
	static PARTICLE_IMPEXP VColorCurve* DoArchiveLookupExchange( VArchive &ar, VColorCurve *pSource );

  ///\brief
	///Creates a simple gradient between two colors
	///
	///\param iColor1
	///Start color
	///
	///\param iColor2
	///End color
	///
	PARTICLE_IMPEXP void CreateGradient(VColorRef iColor1,VColorRef iColor2);

private:

  VCurve2DBase m_Channel[4];
  float m_fDuration;
  VisBitmapPtr m_spLookup;


};


///\brief
///Position (xyz) curve class.
class VPositionCurve : public VRefCounter
{
public:
  VPositionCurve()
  {
    m_pLookupValues = NULL;
    m_iLookupCount = 0;
    m_fLookupCount = 0.f;
    m_fDuration = -1.f;
  }

  virtual ~VPositionCurve()
  {
    FreeLookup();
  }

  inline bool IsValid() const
  {
    return m_X.IsValid() || m_Y.IsValid() || m_Z.IsValid();
  }

  void FreeLookup()
  {
    V_SAFE_DELETE_ARRAY(m_pLookupValues);
    m_iLookupCount = 0;
    m_fLookupCount = 0.f;
  }

  PARTICLE_IMPEXP static VPositionCurve *Exchange_PositionCurve(TiXmlElement *pParent,const char *szAttribName, VPositionCurve *pCurve, bool bWrite);
  PARTICLE_IMPEXP bool DataExchangeXML(TiXmlElement *pCurveNode, bool bWrite);

  PARTICLE_IMPEXP void CreateLookup(int iSamples, float fScale=1.0f);
  inline bool HasLookup() const
  {
    return m_iLookupCount>0;
  }
	PARTICLE_IMPEXP void UpdateCurve(bool bUpdateChannels=true);
  inline VCurve2DBase &GetChannel(int iIndex)
  {
    if (iIndex==1) return m_Y;
    if (iIndex==2) return m_Z;
    return m_X;
  }

	inline const hkvVec3& GetValueFast(float x)
  {
    VASSERT(m_pLookupValues);
    VASSERT(x>=0.f && x<=1.f); ///< 1.f is still OK since m_fLookupCount takes care
    return m_pLookupValues[(int)(m_fLookupCount*x)];
  }
	static PARTICLE_IMPEXP VPositionCurve* DoArchiveLookupExchange( VArchive &ar, VPositionCurve *pSource, bool bBrokenCount=false);


  VCurve2DBase m_X,m_Y,m_Z;
  hkvVec3* m_pLookupValues;
  int m_iLookupCount;
  float m_fLookupCount, m_fDuration;
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
