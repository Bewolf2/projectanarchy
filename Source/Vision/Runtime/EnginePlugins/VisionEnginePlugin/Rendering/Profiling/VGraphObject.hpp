/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VGraphObject.hpp

#ifndef VGRAPH_OBJECT_HPP
#define VGRAPH_OBJECT_HPP

#include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Rendering/Effects/EffectsModule.hpp>
#include <Vision/Runtime/Engine/Mesh/VisApiVertexBuffer.hpp>

#define VCURVE_MAX_NAME 256 // maximum length of curve name

/// \brief simple vertex for displaying curves as a line list
struct VGraphVertex
{
  bool operator== (const VGraphVertex &rhs) const
  { 
    if (iColor!=rhs.iColor)
      return false;
    if (vPos!=rhs.vPos)
      return false;
    return false;
  }

  bool operator!= (const VGraphVertex &rhs) const
  {
    return (!(*this==rhs));
  }

  hkvVec3 vPos;
  VColorRef iColor;
};

/// \brief
///   Structure that describes the properties of a graph.
struct VGraphProps
{
  VGraphProps()
  {
    vPosition.set(0.0f, 0.0f);  
    fWidth = 0.5f;
    fHeight = 0.5f;  
    iResolution = 32; 
    fMinValue = 0.0f;
    fMaxValue = 1.0f;
    bRangeAdaptation = true;
  }

  hkvVec2 vPosition;          ///< left/ upper corner of graph in screen space [range: -1..1]
  float fWidth;               ///< relative to screen width [range: 0..1]
  float fHeight;              ///< relative to screen height [range: 0..1]
  unsigned int iResolution;   ///< number of subdivisions per curve
  float fMaxValue;            ///< maximum value that graph should display
  float fMinValue;            ///< minimum value that graph should display
  bool bRangeAdaptation;      ///< if true, value-range will be automatically adapted
};

/// brief
///  class that describes a curve within a graph.
class VCurveObject
{
public:
  friend class VGraphObject;

  VCurveObject()
  {
    m_szName[0] = 0;
    m_iColor = V_RGBA_WHITE;
    m_iVertexOffset = 0;
    m_spLegendMask = NULL;
  }

  ~VCurveObject()
  {
    m_spLegendMask = NULL;
  }
  
  bool operator== (const VCurveObject &rhs) const
  {
    if (m_iColor!=rhs.m_iColor)
      return false;
    if (m_iVertexOffset!=rhs.m_iVertexOffset)
      return false;
    if (strcmp(m_szName, rhs.m_szName)!=0)
      return false;
    return true;
  }

  bool operator!= (const VCurveObject &rhs) const
  {
    return (!(*this==rhs));
  }

private:
  char m_szName[VCURVE_MAX_NAME];    ///< name of curve
  VColorRef m_iColor;                ///< color of curve
  unsigned int m_iVertexOffset;      ///< offset into global vertex array of parent graph
  VisScreenMaskPtr m_spLegendMask;   ///< screen mask for legend
};

/// \brief 
///   Graph object for visualizing of profiling information.   
class VGraphObject : public IVisCallbackHandler_cl
{
public:

  /// \brief 
  ///   Constructor
  EFFECTS_IMPEXP VGraphObject(VGraphProps &props);

  /// \brief 
  ///   Destructor
  EFFECTS_IMPEXP virtual ~VGraphObject();

  /// \brief
  ///   Adds a curve to graph (must be called before Init()).
  /// 
  /// \param szName
  ///   Name of the curve that is displayed in the legend
  /// 
  /// \param iColor
  ///   Color of the curve and corresponding legend
  ///
  /// \return
  ///   Index of curve to be used for Update(). Will return -1 on failure.
  EFFECTS_IMPEXP virtual int AddCurve(const char *szName, VColorRef iColor);

  /// \brief
  ///   After curves have been added, this method has to be called to init graph.
  /// 
  /// \param szMaskTexture
  ///   Specifies texture to use for background of graph. Per default a grid texture
  ///   will be used.
  EFFECTS_IMPEXP virtual void Init(const char *szMaskTexture=NULL);

  /// \brief
  ///   Updates curve of specified index.
  EFFECTS_IMPEXP virtual void Update(unsigned int iCurveIndex, float fValue); 
  
  /// \brief
  ///   Returns whether graph is visible.
  inline bool IsVisible() const { return m_bVisible; }

  /// \brief
  ///   Toggles visibility of graph. 
  EFFECTS_IMPEXP void SetVisible(bool bVisible); 

  /// \brief 
  ///   Overrides the OnHandleCallback function of IVisCallbackHandler_cl
  EFFECTS_IMPEXP virtual void OnHandleCallback(IVisCallbackDataObject_cl *pData) HKV_OVERRIDE;

protected: 

  // creates effect for displaying graph
  virtual VCompiledTechnique* CreateEffect();

  // converts screen relative coords [range: 0..1] into pixel coordinates
  virtual hkvVec2 GetScreenPos(float fRelativePosX, float fRelativePosY);

  // resizes graph
  virtual void Resize();
  
  VisScreenMaskPtr m_spBackgroundMask;         ///< screen mask for graph background
  VisMeshBufferPtr m_spMeshBuffer;             ///< the mesh buffer data 
  VisMeshBufferObjectPtr m_spMeshBufferObject; ///< the mesh buffer object 

  DynArray_cl<VGraphVertex> m_vertexArray;     ///< global vertex array
  DynArray_cl<unsigned short> m_indexArray;    ///< global index array
  DynObjArray_cl<VCurveObject> m_curves;       ///< list of curves 
  
  int m_iManagerIndex;
  VGraphProps m_props;
  bool m_bDirty;
  bool m_bVisible;
  bool m_bInitialized;
  float m_fLastMinValue;
  float m_fLastMaxValue;

private:

  // copying not allowed
  VGraphObject(const VGraphObject& rhs);
  void operator=(const VGraphObject& rhs);

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
