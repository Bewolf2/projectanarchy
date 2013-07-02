/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vLightGrid.hpp

#ifndef VLIGHTGRID_HPP_INCLUDED
#define VLIGHTGRID_HPP_INCLUDED

#include <Vision/Runtime/Base/System/Resource/VResource.hpp>
#include <Vision/Runtime/Base/System/Resource/VResourceManager.hpp>
#include <Vision/Runtime/Base/System/IO/Serialization/VChunkFile.hpp>
#include <Vision/Runtime/Base/Graphics/Video/VRenderInterface.hpp>
#include <Vision/Runtime/Base/Graphics/Textures/VTextureObject.hpp>


class VLightGridLoader_cl;
class VLightGrid_cl;
class VLightGridNodeIterator_cl;
class IVLightGridVisitor;
typedef VSmartPtr<VLightGrid_cl> VLightGridPtr;
  
#define NODE_CHILD_FLAG     0x80000000

#define CHILDNODE_POSX_FLAG   1
#define CHILDNODE_POSY_FLAG   2
#define CHILDNODE_POSZ_FLAG   4

#define CUBEMAPFACE_BIT_POS_X     (1<<VTM_CUBEMAPFACE_POS_X)
#define CUBEMAPFACE_BIT_NEG_X     (1<<VTM_CUBEMAPFACE_NEG_X)
#define CUBEMAPFACE_BIT_POS_Y     (1<<VTM_CUBEMAPFACE_POS_Y)
#define CUBEMAPFACE_BIT_NEG_Y     (1<<VTM_CUBEMAPFACE_NEG_Y)
#define CUBEMAPFACE_BIT_POS_Z     (1<<VTM_CUBEMAPFACE_POS_Z)
#define CUBEMAPFACE_BIT_NEG_Z     (1<<VTM_CUBEMAPFACE_NEG_Z)
#define CUBEMAPFACE_BIT_ALL       (CUBEMAPFACE_BIT_POS_X|CUBEMAPFACE_BIT_NEG_X|CUBEMAPFACE_BIT_POS_Y|CUBEMAPFACE_BIT_NEG_Y|CUBEMAPFACE_BIT_POS_Z|CUBEMAPFACE_BIT_NEG_Z)

#define GRID_RENDERFLAG_NODEBOX           0x00000001
#define GRID_RENDERFLAG_LINECOLORFACES    0x00000002
#define GRID_RENDERFLAG_SOLIDCOLORFACES   0x00000004
#define GRID_RENDERFLAG_NOROOTCELLS       0x00000010
#define GRID_RENDERFLAG_NOCHILDCELLS      0x00000020


class VLightGridDetailBox_cl;
typedef VSmartPtr<VLightGridDetailBox_cl> VLightGridDetailBoxPtr;

/// \brief
///   Class for specifying regions in space with higher lightgrid density. Internal use only.
class VLightGridDetailBox_cl : public VRefCounter
{
public:
  VLightGridDetailBox_cl() {m_iSubDiv[0]=m_iSubDiv[1]=m_iSubDiv[2]=2;}
  VLightGridDetailBox_cl(const hkvAlignedBBox &bbox, int iSubDivX, int iSubDivY, int iSubDivZ) {m_BBox=bbox; m_iSubDiv[0]=iSubDivX;m_iSubDiv[1]=iSubDivY;m_iSubDiv[2]=iSubDivZ;}

  VBASE_IMPEXP void Render(IVRenderInterface *pRenderer, VColorRef iColor, const VSimpleRenderState_t &state, const VLightGrid_cl *pGrid=NULL, int iFlags=RENDERSHAPEFLAGS_LINES|RENDERSHAPEFLAGS_SOLID);

  int m_iSubDiv[3];
  hkvAlignedBBox m_BBox;
};


/// \brief
///   A collection of lightgrid detail boxes. Used for subdividing a lightgrid in the preprocessing step. Internal use only.
class VLightGridDetailBoxCollection_cl : public VPList
{
public:
  VBASE_IMPEXP VLightGridDetailBoxCollection_cl() {}
  inline ~VLightGridDetailBoxCollection_cl() {RemoveAll();}

  inline int GetCount() const {return GetLength();}
  inline VLightGridDetailBox_cl** GetList() const {return (VLightGridDetailBox_cl**)GetPtrs();}
  inline VLightGridDetailBox_cl* GetAt(int iIndex) const {VASSERT(iIndex>=0 && iIndex<GetCount());return (VLightGridDetailBox_cl*)GetPtrs()[iIndex];}

  inline void Add(VLightGridDetailBox_cl *pBox)
  {
    Append(pBox);
    pBox->AddRef();
  }

  inline void Add(const VLightGridDetailBoxCollection_cl *list)
  {
    for (int i=0;i<list->GetLength();i++)
      Add(list->GetAt(i));
  }

  inline void RemoveAt(int iIndex)
  {
    VASSERT(iIndex>=0 && iIndex<GetCount());
    VLightGridDetailBox_cl **pBoxes = GetList();
    pBoxes[iIndex]->Release();
    VPList::RemoveAt(iIndex);
  }

  inline void Remove(VLightGridDetailBox_cl *pBox) {RemoveAt(Find(pBox));}

  inline void RemoveAll()
  {
    while (GetLength()>0) RemoveAt(GetLength()-1);
  }
};


// flags passed to the EvaluateLightColor function of the IVLightGridTracer_cl interface
#define LIGHTGRIDTRACE_BASELIGHTING       0x00000001
#define LIGHTGRIDTRACE_RADIOSITY          0x00000002

#define LIGHTGRIDTRACE_SEPARATERADIOSITY  0x00010000

/// \brief
///   Interface for tracing result colors for a lightgrid in a pre-processing step
class IVLightGridTracer_cl
{
public:
  virtual ~IVLightGridTracer_cl() {}

  ///
  /// @name Overridables
  /// @{
  ///

  /// \brief
  ///   Gets called by VLightGrid_cl::TraceColors to evaluate a color at a specific position and
  ///   orientation
  /// 
  /// This is the version for the VLIGHTGRIDTYPE_6COLORS version. This function is called 6 times
  /// for every cell (once for every of the six directions). See VLightGridType_e for further information.
  /// 
  /// \param vPos
  ///   World position to evaluate the color at
  /// 
  /// \param vNormal
  ///   Normal vector
  /// 
  /// \param destColor
  ///   Receives the float-based destination color
  /// 
  /// \param iFlags
  ///   additional flags:
  ///   \li LIGHTGRIDTRACE_BASELIGHTING : The base lighting should be considered
  /// 
  ///   \li LIGHTGRIDTRACE_RADIOSITY : The radiosity contribution should be considered
  /// 
  /// \sa VLightGrid_cl::TraceColors
  VBASE_IMPEXP virtual void EvaluateLightColor(const hkvVec3& vPos,const hkvVec3& vNormal, hkvVec3& destColor, int iFlags) = 0;

  /// \brief
  ///   Gets called by VLightGrid_cl::TraceColors to evaluate colors at a specific position
  /// 
  /// This is the version for the VLIGHTGRIDTYPE_AMBIENTDIRECTIONAL version.
  /// See VLightGridType_e for further information.
  /// 
  /// \param vPos
  ///   World position to evaluate the color at
  /// 
  /// \param destAmbient
  ///   Receives the ambient color at the position.
  /// 
  /// \param destDirColor
  ///   Color of a single light
  /// 
  /// \param destDir
  ///   Direction of a single light
  /// 
  /// \param iFlags
  ///   additional flags:
  ///   \li LIGHTGRIDTRACE_BASELIGHTING : The base lighting should be considered
  /// 
  ///   \li LIGHTGRIDTRACE_RADIOSITY : The radiosity contribution should be considered
  /// 
  /// \sa VLightGrid_cl::TraceColors
  VBASE_IMPEXP virtual void EvaluateLightColorSimple(const hkvVec3& vPos, hkvVec3& destAmbient, hkvVec3& destDirColor, hkvVec3& destDir, int iFlags) = 0;

  /// \brief
  ///   Decides whether a light grid cell needs to be split up into smaller subcells.
  ///
  /// Gets called by VLightGrid_cl::SplitCells. This function is responsible for deciding whether a light grid cell should be split
  /// further, and if so, into how many subcells. The decision is made by evaluating the overlapping light grid detail boxes and
  /// comparing the current node resolution against the resolution of the detail boxes. The output values are set in such a way that
  /// the resulting resolution corresponds to the resolution of the detail boxes.
  /// 
  /// \param node
  ///   The node to test
  /// 
  /// \param nodebox
  ///   Bounding box of the node
  /// 
  /// \param iSplitX
  ///   Output: Receives the number of cell subdivision in x-direction
  /// 
  /// \param iSplitY
  ///   Output: Receives the number of cell subdivision in y-direction
  /// 
  /// \param iSplitZ
  ///   Output: Receives the number of cell subdivision in z-direction
  /// 
  /// \return
  ///   BOOL bSplit : if TRUE, the node should be split into subdivisions specified by iSplitX,
  ///   iSplitY and iSplitZ
  /// 
  /// \sa VLightGrid_cl::SplitCells
  VBASE_IMPEXP virtual BOOL GetSplitCount(const VLightGridNodeIterator_cl &node, const hkvAlignedBBox &nodebox, int &iSplitX, int &iSplitY, int &iSplitZ);

  /// \brief
  ///   Iteration function that gets called when using the Iterate function on the grid
  /// 
  /// \param node
  ///   The current cell node. Can be used to get or set colors
  /// 
  /// \param nodebox
  ///   The bounding box of the node
  /// 
  /// \sa VLightGrid_cl::Iterate
  VBASE_IMPEXP void OnIterate(VLightGridNodeIterator_cl &node, const hkvAlignedBBox &nodebox) {}

  /// \brief
  ///   Gets called while tracing. Useful to display a progress bar.
  /// 
  /// \param fPercentage
  ///   The percentage progress of the tracing
  /// 
  /// \sa VLightGrid_cl::SplitCells
  /// \sa VLightGrid_cl::TraceColors
  VBASE_IMPEXP virtual void OnProgress(float fPercentage) {VASSERT(fPercentage>=0.f && fPercentage<=100.f);}

  /// \brief
  ///   Virtual callback that is called every time a new color has been traced. Called for direct lighting,
  ///   radiosity, and the combined result
  VBASE_IMPEXP virtual void OnColorTraced(VLightGridNodeIterator_cl &node, const hkvVec3& vColor, int iType = LIGHTGRIDTRACE_BASELIGHTING) {}

  VLightGridDetailBoxCollection_cl m_DetailBoxes; ///< can be filled to automatically pre-split the cells

  inline int GetCurrentColorIndex() const {return m_iCurrentNodeIndex*6+m_iCurrentFace;}

  int m_iCurrentNodeIndex;
  int m_iCurrentFace;
  ///
  /// @}
  ///

};



/// \brief
///   Defines the type of light grid to be used.
enum VLightGridType_e
{
  VLIGHTGRIDTYPE_6COLORS  = 0,                ///< Light grid storing six color values at each sampling point, representing the incoming light from the six axis-directions. More accurate than VLIGHTGRIDTYPE_AMBIENTDIRECTIONAL, but also more expensive to evaluate. Default on PCs and console platforms.
  VLIGHTGRIDTYPE_AMBIENTDIRECTIONAL  = 1,     ///< Light grid storing one ambient color and one directional light to encode the incoming light at a sampling point. Less accurate than VLIGHTGRIDTYPE_6COLORS, but also more efficient to evaluate. Default on mobile platforms.

#ifdef SUPPORTS_ORTHOGONAL_LIGHTGRID
  VLIGHTGRIDTYPE_PLATFORMDEFAULT = VLIGHTGRIDTYPE_6COLORS
  #define VGLOBALLIGHTGRIDTYPE_6COLORS
#else
  VLIGHTGRIDTYPE_PLATFORMDEFAULT = VLIGHTGRIDTYPE_AMBIENTDIRECTIONAL
  #define VGLOBALLIGHTGRIDTYPE_AMBIENTDIRECTIONAL
#endif

};

// array with same dimension as VLightGridType_e
const int VLIGHTGRIDTYPE_NUMCOMPONENTS[2] = {6,3};

/// \brief
///   Light grid class
/// 
/// An instance of the VLightGrid_cl class represents a grid of cells that provide pre-computed
/// lighting contribution for a scene.
/// 
/// This lighting information can be used to light dynamic objects in a scene (entities) using static lights.
/// 
/// A light grid can be computed with the vLux tool.
class VLightGrid_cl : public VManagedResource
{
public:

  ///
  /// @name Constructor/destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of the lightgrid. Takes a resource manager, which can be NULL.
  VBASE_IMPEXP VLightGrid_cl(VResourceManager *pParentManager);

  /// \brief
  ///   Destructor of the lightgrid.
  VBASE_IMPEXP virtual ~VLightGrid_cl();


  ///
  /// @}
  ///

  ///
  /// @name Cloning and Copying
  /// @{
  ///


  /// \brief
  ///   Returns a deep copy of the grid including the node structure and color data
  VBASE_IMPEXP VLightGrid_cl* Clone() const;

  ///
  /// @}
  ///

  ///
  /// @name Initialization and Serialization
  /// @{
  ///

  /// \brief
  ///   Initializes the grid using a bounding box and the number of subdivisions
  VBASE_IMPEXP void SetGridDimension(const hkvAlignedBBox &bbox, int iSubDivX,int iSubDivY,int iSubDivZ);

  /// \brief
  ///   Saves the lightgrid to a binary file. Uses the VLightGridLoader_cl class.
  VBASE_IMPEXP BOOL SaveToFile(const char* szAbsFilename, const char* szResourceFileName,
    IVFileStreamManager* pManager=NULL);

  /// \brief
  ///   Loads the lightgrid from a binary file. Uses the VLightGridLoader_cl class.
  VBASE_IMPEXP BOOL LoadFromFile(const char *szFilename,IVFileStreamManager* pManager=NULL);

  /// \brief
  ///   Loads a light grid from a binary file and returns a new light grid instance. Uses the
  ///   VLightGridLoader_cl class.
  static VBASE_IMPEXP VLightGrid_cl* LoadFromFile(const char *szFilename,VResourceManager *pParentManager, IVFileStreamManager* pManager=NULL);

  /// \brief
  ///   Creates a dummy light grid with all faces set to white
  static VBASE_IMPEXP VLightGrid_cl* CreateFullbrightLightGrid(const hkvAlignedBBox &bbox, VColorRef iColor = V_RGBA_WHITE);

  /// \brief
  ///   Non-static function to fill the grid with a solid color
  VBASE_IMPEXP void InitWithDefaultColor(const hkvAlignedBBox &bbox, VColorRef iColor = V_RGBA_WHITE);

  ///
  /// @}
  ///

  ///
  /// @name Manually generating a Grid
  /// @{
  ///

  /// \brief
  ///   Performs the cell splits according to the results from the trace interface's GetSplitCount function
  /// 
  /// \param pTracer
  ///   The trace interface to determine the splits
  /// 
  /// \return
  ///   BOOL bResult : TRUE if successful
  VBASE_IMPEXP BOOL SplitCells(IVLightGridTracer_cl *pTracer);

  /// \brief
  ///   Generates the light grid information by evaluating the colors via the trace interface.
  /// 
  /// It calls either EvaluateLightColor or EvaluateLightColorSimple on the interface, according to
  /// the light grid type.
  /// 
  /// \param pTracer
  ///   The trace interface which will be used to evaluate the actual colors
  /// 
  /// \param iSuperSample
  ///   Number of cell subdivision steps for super-sampling the lighting information. If >0, the cells will be subdivided iSuperSample times and the final
  ///   color will be averaged from the resulting samples.
  /// 
  /// \param iTraceFlags
  ///   Bit combinations that determines what should be considered while tracing:
  ///   \li LIGHTGRIDTRACE_BASELIGHTING: Consider the base lighting from all static lights
  ///   \li LIGHTGRIDTRACE_RADIOSITY: Consider radiosity contribution
  ///   \li LIGHTGRIDTRACE_SEPARATERADIOSITY: Add the radiosity in a separate step without super-sampling. When using radiosity, this should generally be enabled for performance reasons.
  /// 
  /// \return
  ///   BOOL bResult : TRUE if successful
  VBASE_IMPEXP BOOL TraceColors(IVLightGridTracer_cl *pTracer, int iSuperSample=0, int iTraceFlags=LIGHTGRIDTRACE_BASELIGHTING|LIGHTGRIDTRACE_RADIOSITY|LIGHTGRIDTRACE_SEPARATERADIOSITY);

  /// \brief
  ///   Plain iteration function that calls OnIterate on each node
  /// 
  /// \param pTracer
  ///   The trace interface to call the OnIterate function
  VBASE_IMPEXP void Iterate(IVLightGridTracer_cl *pTracer);

  /// \brief
  ///   Implementation of the visitor pattern. See IVLightGridVisitor
  /// 
  /// \param visitor
  ///   The visitor interface. IVLightGridVisitor::VisitNode gets called on every node
  VBASE_IMPEXP bool RunVisitor(IVLightGridVisitor &visitor);

  /// \brief
  ///   Finalizes a lightgrid. Should be called after creating a new lightgrid, but isn't required when loading a lightgrid from file.
  VBASE_IMPEXP BOOL Finalize();

  /// \brief
  ///   Translates the entire lightgrid by the specified number of world units. Internal use only.
  inline void Translate(const hkvVec3& vOfs) {m_BoundingBox.m_vMin+=vOfs;m_BoundingBox.m_vMax+=vOfs;m_vClampMax+=vOfs;}


  ///
  /// @}
  ///

  ///
  /// @name Accessing lightgrid properties
  /// @{
  ///

  /// \brief
  ///   Returns the grid type
  inline VLightGridType_e GetLightGridType() const {return m_eType;}

  /// \brief
  ///   Sets the light grid type. 
  inline void SetLightGridType(VLightGridType_e eType) {m_eType=eType;}

  /// \brief
  ///   Returns the number of grid subdivisions in x-direction.
  inline int GetSubdivX() const {return m_iSubDiv[0];}

  /// \brief
  ///   Returns the number of grid subdivisions in y-direction.
  inline int GetSubdivY() const {return m_iSubDiv[1];}

  /// \brief
  ///   Returns the number of grid subdivisions in z-direction.
  inline int GetSubdivZ() const {return m_iSubDiv[2];}

  /// \brief
  ///   Returns the number of root cells (just GetSubdivX()*GetSubdivY()*GetSubdivZ() ).
  inline int GetCellCount() const {return m_iSubDiv[0]*m_iSubDiv[1]*m_iSubDiv[2];}

  /// \brief
  ///   Gets the overall bounding box of the entire light grid. 
  inline void GetBoundingBox(hkvAlignedBBox &dest) const {dest=m_BoundingBox;}

  /// \brief
  ///   Returns true if any cell in the grid is subdivided (e.g. because it overlaps a light grid detail box)
  inline bool HasSubDivisions() const {return m_bHasSubDivisions;}

  /// \brief
  ///   Returns a vector that determines the grid spacing of a root cell, i.e. a diagonal in the
  ///   cell bounding box
  inline const hkvVec3& GetGridSpacing() const {return m_vSpacing;}

  /// \brief
  ///   Returns the recursion depth at a world position (can be 0 or 1, depending on whether the position is inside a light grid detail box)
  VBASE_IMPEXP int GetRecursionDepthAtPoint(const hkvVec3& vPos);

  ///
  /// @}
  ///

  ///
  /// @name Color Lookup Functions
  /// @{
  ///

  /// \brief
  ///   Evaluates the color array at a world position
  /// 
  /// This function takes a world position and looks up the colors in the lightgrid.
  /// 
  /// The result is an array of 6 float color vectors that represent the light contribution in the
  /// 6 cube directions.
  /// 
  /// \param vPos
  ///   input world position
  /// 
  /// \param pDestColors
  ///   destination array that will receive up to 6 color vectors (depending on iRequired)
  /// 
  /// \param iRequired
  ///   bitmask of cube faces (CUBEMAPFACE_BIT_POS_X,...) that should be filled in. Usually
  ///   CUBEMAPFACE_BIT_ALL.
  VBASE_IMPEXP void GetColorsAtPosition(const hkvVec3& vPos, hkvVec3* pDestColors, int iRequired=CUBEMAPFACE_BIT_ALL);


  ///\brief
  ///   Same as VLightGrid_cl::GetColorsAtPosition but trilinearily interpolates the colors in the grid
	///
	/// \param vPos
	///   World space position to sample the colors
	///
	/// \param pDestColors
	///   Location the color information should be written to
	///
	/// \param iRequired
  ///   Bitmask of directions to sample (e.g. CUBEMAPFACE_BIT_ALL)
	///
  ///\sa VLightGrid_cl::GetColorsAtPosition
	VBASE_IMPEXP void GetColorsAtPositionI(const hkvVec3& vPos, hkvVec3* pDestColors, int iRequired=CUBEMAPFACE_BIT_ALL);

  /// \brief
  ///   Calculates the average color intensity (i.e., componentwise arithmetic mean of the six colors) at the specified world position and stores the
  ///   average color in vDestColor. Samples from the closest sampling point; does not perform interpolation.
  VBASE_IMPEXP void GetAverageColorAtPosition(const hkvVec3& vPos, hkvVec3& vDestColor, int iRequired=CUBEMAPFACE_BIT_ALL);

  /// \brief
  ///   Calculates the average color intensity (i.e., componentwise arithmetic mean of the six colors) at the specified world position and stores the
  ///   average color in vDestColor. Samples at the 8 closest sampling points and performs trilinear interpolation between them.
  VBASE_IMPEXP void GetAverageColorAtPositionI(const hkvVec3& vPos, hkvVec3& vDestColor, int iRequired=CUBEMAPFACE_BIT_ALL);

  /// \brief
  ///   Calculates the maximum color at specified world position and stores the
  ///   average color in vDestColor. The maximum is computed component-wise for all 6 directions. Samples from the closest sampling point; does not perform interpolation.
  VBASE_IMPEXP void GetMaximumColorAtPosition(const hkvVec3& vPos, hkvVec3& vDestColor, int iRequired=CUBEMAPFACE_BIT_ALL);

  /// \brief
  ///   Calculates the maximum color at specified world position and stores the
  ///   average color in vDestColor. The maximum is computed component-wise for all 6 directions. Samples at the 8 closest sampling points and performs trilinear interpolation between them.
  VBASE_IMPEXP void GetMaximumColorAtPositionI(const hkvVec3& vPos, hkvVec3& vDestColor, int iRequired=CUBEMAPFACE_BIT_ALL);

  /// \brief
  ///   Evaluates the color at a world position using a position and a normal vector
  /// 
  /// \param vPos
  ///   input world position
  /// 
  /// \param vNormal
  ///   input normal used for falloff
  /// 
  /// \param destColor
  ///   Receives the destination color
  /// 
  /// \param bInterpolate
  ///   Determines whether interpolated or non-interpolated method should be used for color lookup
  VBASE_IMPEXP void EvaluateColorAtPosition(const hkvVec3& vPos, const hkvVec3& vNormal, hkvVec3& destColor, BOOL bInterpolate=TRUE);

  /// \brief
  ///   Evaluates the color at a world positions for multiple vertices, thus reducing the
  ///   color lookup overhead
  /// 
  /// It also calculates the normal falloff for an arbitrary array of vertices.
  /// 
  /// This is useful to simulate lightgrid lighting for the vertex colors of a static object (e.g.
  /// meshbuffer).
  /// 
  /// However it is not fast enough to do this every frame for an object. It should rather be used
  /// to evaluate the vertex colors once of a static object.
  /// 
  /// \param vPos
  ///   World position, used for all vertices
  /// 
  /// \param iVertexCount
  ///   Number of vertices
  /// 
  /// \param pFirstNormal
  ///   pointer to the first vertex normal
  /// 
  /// \param iNormalStride
  ///   Stride (in bytes) between two normals
  /// 
  /// \param pFirstColor
  ///   Pointer to first color that receives the destination color
  /// 
  /// \param iColorStride
  ///   Stride (in bytes) between two colors
  VBASE_IMPEXP void EvaluateColorArrayAtPosition(const hkvVec3& vPos, int iVertexCount, const hkvVec3* pFirstNormal, int iNormalStride, VColorRef *pFirstColor, int iColorStride);


  /// \brief
  ///   Evaluates the overall number of nodes including child nodes
  inline int GetOverallNodeCount() const {return CopyColorsToBuffer(NULL);}

  /// \brief
  ///   Copy all colors to a flat color array list. The destination buffer needs to be reserved for
  ///   hkvVec3[GetOverallNodeCount()*6].
  VBASE_IMPEXP int CopyColorsToBuffer(hkvVec3* pDestBuffer) const;

  /// \brief
  ///   Copy all colors from a flat color array list. The source buffer needs to be reserved for
  ///   hkvVec3[GetOverallNodeCount()*6].
  VBASE_IMPEXP void CopyColorsFromBuffer(const hkvVec3* pSrcBuffer);

  ///
  /// @}
  ///

  ///
  /// @name Debugging
  /// @{
  ///


  /// \brief
  ///   Renders the grid (or parts of it).
  /// 
  /// \param pRenderer
  ///   Render interface. Can be retrieved from the render context.
  /// 
  /// \param bbox
  ///   Only cells that touch this box are rendered
  /// 
  /// \param iFlags
  ///   Determines which parts of the grid should be rendered (GRID_RENDERFLAG_NODEBOX,
  ///   GRID_RENDERFLAG_LINECOLORFACES, GRID_RENDERFLAG_SOLIDCOLORFACES,
  ///   GRID_RENDERFLAG_NOROOTCELLS, GRID_RENDERFLAG_NOCHILDCELLS) 
  VBASE_IMPEXP void RenderGrid(IVRenderInterface *pRenderer, const hkvAlignedBBox &bbox, int iFlags = GRID_RENDERFLAG_NODEBOX);

  /// \brief
  ///   Renders the whole grid.
  VBASE_IMPEXP void RenderGrid(IVRenderInterface *pRenderer, int iFlags = GRID_RENDERFLAG_NODEBOX)
  {
    RenderGrid(pRenderer,m_BoundingBox,iFlags);
  }

  /// \brief
  ///   Renders a cross at the specified position that shows the light contribution from all 6
  ///   directions
  VBASE_IMPEXP void RenderAtPosition(IVRenderInterface *pRenderer, const hkvVec3& vPos, float fLen, float fLineWidth);

  ///
  /// @}
  ///

  ///
  /// @name Transform Functions
  /// @{
  ///

  /// \brief
  ///   Returns the node index in x-direction from world space position. Index can be out of range.
  inline int GetNodeIndexX(float fWorldPosX) const {return (int)((fWorldPosX-m_BoundingBox.m_vMin.x)*m_vInvSpacing.x);}

  /// \brief
  ///   Returns the node index in y-direction from world space position. Index can be out of range.
  inline int GetNodeIndexY(float fWorldPosY) const {return (int)((fWorldPosY-m_BoundingBox.m_vMin.y)*m_vInvSpacing.y);}

  /// \brief
  ///   Returns the node index in z-direction from world space position. Index can be out of range.
  inline int GetNodeIndexZ(float fWorldPosZ) const {return (int)((fWorldPosZ-m_BoundingBox.m_vMin.z)*m_vInvSpacing.z);}

  /// \brief
  ///   Returns the node index in x-direction from world space position and additionally the
  ///   weighting. Index can be out of range.
  inline void GetNodeIndexX(float fWorldPosX, int &iIndex, float &fWeight) const {float f=(fWorldPosX-m_BoundingBox.m_vMin.x)*m_vInvSpacing.x;iIndex=(int)f;fWeight=f-floorf(f);VASSERT(fWeight>=0.f && fWeight<1.f);}

  /// \brief
  ///   Returns the node index in y-direction from world space position and additionally the
  ///   weighting. Index can be out of range.
  inline void GetNodeIndexY(float fWorldPosY, int &iIndex, float &fWeight) const {float f=(fWorldPosY-m_BoundingBox.m_vMin.y)*m_vInvSpacing.y;iIndex=(int)f;fWeight=f-floorf(f);VASSERT(fWeight>=0.f && fWeight<1.f);}

  /// \brief
  ///   Returns the node index in z-direction from world space position and additionally the
  ///   weighting. Index can be out of range.
  inline void GetNodeIndexZ(float fWorldPosZ, int &iIndex, float &fWeight) const {float f=(fWorldPosZ-m_BoundingBox.m_vMin.z)*m_vInvSpacing.z;iIndex=(int)f;fWeight=f-floorf(f);VASSERT(fWeight>=0.f && fWeight<1.f);}

  /// \brief
  ///   Computes the node index in a flattened-out array from x,y and z index. Indices must be in valid range.
  inline int GetNodeIndex(int x, int y, int z) const
  {
    VASSERT(x>=0 && x<m_iSubDiv[0]);
    VASSERT(y>=0 && y<m_iSubDiv[1]);
    VASSERT(z>=0 && z<m_iSubDiv[2]);
    return (z*m_iSubDiv[1]+y)*m_iSubDiv[0] + x;
  }

  /// \brief
  ///   Computes the node index in a flattened-out array from x,y and z index.
  inline int GetNodeIndexClamped(int x, int y, int z) const
  {
    ClampCoordinates(x,y,z);
    return GetNodeIndex(x,y,z);
  }

  /// \brief
  ///   Computes the node index in a flattened-out array from a world position.
  inline int GetNodeIndexClamped(const hkvVec3& vWorldPos) const
  {
    return GetNodeIndexClamped(GetNodeIndexX(vWorldPos.x),GetNodeIndexY(vWorldPos.y),GetNodeIndexZ(vWorldPos.z));
  }

  /// \brief
  ///   Computes the clamped x,y,z node indices from a world position.
  inline void GetNodeIndicesClamped(const hkvVec3& vWorldPos,int &x, int &y, int &z) const
  {
    x = GetNodeIndexX(vWorldPos.x);
    y = GetNodeIndexY(vWorldPos.y);
    z = GetNodeIndexZ(vWorldPos.z);
    ClampCoordinates(x,y,z);
  }

  /// \brief
  ///   Clamps the passed node indices to valid range.
  inline void ClampCoordinates(int &x, int &y, int &z) const
  {
    if (x<0) x=0; else if (x>=m_iSubDiv[0]) x=m_iSubDiv[0]-1;
    if (y<0) y=0; else if (y>=m_iSubDiv[1]) y=m_iSubDiv[1]-1;
    if (z<0) z=0; else if (z>=m_iSubDiv[2]) z=m_iSubDiv[2]-1;
  }

  /// \brief
  ///   Gets the bounding box of a root cell with the given indices x,y,z.
  inline void GetNodeBoundingBox(hkvAlignedBBox &dest, int x, int y, int z) const
  {
    dest.m_vMin.x = m_BoundingBox.m_vMin.x + m_vSpacing.x*(float)x;
    dest.m_vMin.y = m_BoundingBox.m_vMin.y + m_vSpacing.y*(float)y;
    dest.m_vMin.z = m_BoundingBox.m_vMin.z + m_vSpacing.z*(float)z;
    dest.m_vMax = dest.m_vMin + m_vSpacing;
  }

  void GetNeighbourNodes(const hkvVec3& vPos, VLightGridNodeIterator_cl node[2][2][2], const unsigned int* vCol[2][2][2], float fWeight[3]) const;

  ///
  /// @}
  ///

  ///
  /// @name Static Helper Functions
  /// @{
  ///

  /// \brief
  ///   Static helper function to get the face direction 
  VBASE_IMPEXP static const hkvVec3& GetFaceNormal(int iFace=VTM_CUBEMAPFACE_POS_X) {return g_vFaceNormal[iFace];}

  /// \brief
  ///   Convenience helper function to generate 6 light grid colors that represent a directional
  ///   light
  VBASE_IMPEXP static void GetDirectionalLightColors(hkvVec3 pDestColors[6], const hkvVec3& vLightDir, const hkvVec3& vLightColor, const hkvVec3& vLightAmbientColor);

protected:
  // overridden resource functions
  VBASE_IMPEXP virtual BOOL Unload();
  VBASE_IMPEXP virtual BOOL Reload();
  VBASE_IMPEXP virtual void AccumulateMemoryFootprint(size_t &iUniqueSys, size_t &iUniqueGPU, size_t &iDependentSys, size_t &iDependentGPU);

  VBASE_IMPEXP virtual int GetAdditionalOutputString(char *szDestBuffer, int iMaxChars) 
  {
  #pragma warning(push)
  #pragma warning(disable:4996)
    return sprintf(szDestBuffer,"%i x %i x %i", (int)m_iSubDiv[0], (int)m_iSubDiv[1], (int)m_iSubDiv[2]);
  #pragma warning(pop)
  }

  ///
  /// @}
  ///

private:
  friend class VLightGridLoader_cl;
  friend class VLightGridNodeIterator_cl;
  friend class IVLightGridVisitor;

  void ReleaseGrid();
  void GetIterpolatedColorOnLine(float x, float y, float z, hkvVec3* pDestColors,int iRequired, int iDirection=CUBEMAPFACE_BIT_POS_X);
  
  inline int CreateColorHandle()    
  {
    return CreateEntries(GetComponentsPerCell());
  }

  inline int GetComponentsPerCell() const
  {
    switch (m_eType)
    {
      case VLIGHTGRIDTYPE_6COLORS:return 6;
      case VLIGHTGRIDTYPE_AMBIENTDIRECTIONAL:return 2; ///< 2x16bit for colors, 32bit for direction
    }
    VASSERT_MSG(FALSE,"Invalid lightgrid type");
    return 0;
  }

  inline int CreateChildrenHandle(int iSubDivX, int iSubDivY, int iSubDivZ) 
  {
    VASSERT(iSubDivX<256);
    VASSERT(iSubDivY<256);
    VASSERT(iSubDivZ<256);
    int iEntryCount = iSubDivX*iSubDivY*iSubDivZ * 6; ///< allocate color values directly
    int iStartIndex = CreateEntries(iEntryCount+1); ///< num entries + subdiv info (32bit)
    m_iNodes[iStartIndex] = iSubDivX | (iSubDivY<<8) | (iSubDivZ<<16); ///< store the number of subdivisions in the first entry
    return iStartIndex | NODE_CHILD_FLAG;
  }
  VBASE_IMPEXP int CreateEntries(int iCount);

  inline void ClampPosition(hkvVec3& vPos) const
  {
    if (vPos.x<m_BoundingBox.m_vMin.x) vPos.x = m_BoundingBox.m_vMin.x;
      else if (vPos.x>m_vClampMax.x) vPos.x = m_vClampMax.x;
    if (vPos.y<m_BoundingBox.m_vMin.y) vPos.y = m_BoundingBox.m_vMin.y;
      else if (vPos.y>m_vClampMax.y) vPos.y = m_vClampMax.y;
    if (vPos.z<m_BoundingBox.m_vMin.z) vPos.z = m_BoundingBox.m_vMin.z;
      else if (vPos.z>m_vClampMax.z) vPos.z = m_vClampMax.z;
  }

  VLightGridType_e m_eType;
  bool m_bHasSubDivisions;
  hkvAlignedBBox m_BoundingBox;
  hkvVec3 m_vSpacing,m_vInvSpacing,m_vInvSpacingSafe;
  short m_iSubDiv[3];
  short m_iSubDivShift[3];

  int m_iRootCount;
  int m_iEntryCount;
  DynArray_cl<unsigned int>m_iNodes;
  hkvVec3 m_vClampMax; // max part of the bounding box minus some epsilon
  static hkvVec3 g_vFaceNormal[6];
};




/// \brief
///   Overridden chunk file loader for loading a light grid file.
class VLightGridLoader_cl : public VChunkFile
{
public:
  /// \brief
  ///   Constructor. Takes the destination light grid as input.
  VBASE_IMPEXP VLightGridLoader_cl(VLightGrid_cl *pGrid) {m_pGrid=pGrid;}

protected:
  virtual BOOL OnStartChunk(CHUNKIDTYPE chunkID, int iChunkLen);
  virtual void OnStartSaving();
  virtual void OnFinishLoading();

private:
  VLightGrid_cl *m_pGrid;
  int m_iFileVersion;
};




/// \brief
///   Helper class for recursively iterating through the lightgrid. For internal use only.
class VLightGridNodeIterator_cl
{
public:
  
  // empty constructor
  VLightGridNodeIterator_cl() {m_iParentIndex = -1;}

  // constructor for accessing root cell
  VLightGridNodeIterator_cl(VLightGrid_cl *pGrid, int iIndex)
  {
    m_iParentIndex = -1;
    m_pGrid = pGrid;
    m_iIndex = iIndex;
    VASSERT(IsRootElement());
  }

  // constructor for accessing root cell
  VLightGridNodeIterator_cl(VLightGrid_cl *pGrid, int x,int y,int z)
  {
    m_iParentIndex = -1;
    m_pGrid = pGrid;
    m_iIndex = pGrid->GetNodeIndexClamped(x,y,z);
    VASSERT(IsRootElement());
  }

  // constructor for accessing root cell
  VLightGridNodeIterator_cl(VLightGrid_cl *pGrid, const hkvVec3& vWorldPos)
  {
    m_iParentIndex = -1;
    m_pGrid = pGrid;
    m_iIndex = pGrid->GetNodeIndexClamped(vWorldPos);
    VASSERT(IsRootElement());
  }

  // constructor for child node
  VLightGridNodeIterator_cl(const VLightGridNodeIterator_cl *pParent, int x, int y, int z)
  {
    VASSERT(pParent && pParent->HasChildren());
    m_pGrid = pParent->m_pGrid;
    m_iParentIndex = pParent->m_iIndex;
    int dx,dy,dz;
    pParent->GetSubDivCount(dx, dy, dz);
    m_iIndex = GetChildIndex(dx,dy,dz, x,y,z);
  }

  void FindNode(VLightGrid_cl *pGrid, const hkvVec3& vWorldPos, hkvAlignedBBox &nodeBox);

  // indicates whether this is a root cell
  inline BOOL IsRootElement() const {return m_iParentIndex == -1;}

  // returns the value in in the grid array
  inline unsigned int GetNodeValue() const {return m_pGrid->m_iNodes[m_iIndex];}

  // sets the value in the grid array
  inline void SetNodeValue(unsigned int iVal) {m_pGrid->m_iNodes[m_iIndex]=iVal;}

  // indicates whether the node has child nodes or not
  inline BOOL HasChildren() const 
  {
    if (m_iParentIndex>=0)
      return false;
    return (GetNodeValue()&NODE_CHILD_FLAG)>0;
  }

  // stores the number of child subdivisions in the RGB values of the return value
  inline void GetSubDivCount(int &dx, int &dy, int &dz) const
  {
    VASSERT(IsRootElement());
    int iIndex = GetFirstChildIndex();
    VASSERT(iIndex>0 && sizeof(m_pGrid->m_iNodes[iIndex])==4);
    dx = (int)(m_pGrid->m_iNodes[iIndex]&255);
    dy = (int)((m_pGrid->m_iNodes[iIndex]>>8)&255);
    dz = (int)((m_pGrid->m_iNodes[iIndex]>>16)&255);
  }

  inline int GetNumChildren() const
  {
    int dx,dy,dz;
    GetSubDivCount(dx,dy,dz);
    return (dx*dy*dz);
  }

  // get the dimension of the child boxes
  inline void GetChildBoxSize(hkvVec3& vDestSize) const
  {
    int dx,dy,dz;
    GetSubDivCount(dx,dy,dz);
    vDestSize.x = m_pGrid->m_vSpacing.x / (float)dx;
    vDestSize.y = m_pGrid->m_vSpacing.y / (float)dy;
    vDestSize.z = m_pGrid->m_vSpacing.z / (float)dz;
  }

  // determines the relative child coordinates from a world position
  void GetChildIndicesFromPos(const hkvVec3& vPos, const hkvAlignedBBox &nodeBox, int &x, int &y, int &z) const
  {
    int dx,dy,dz;
    GetSubDivCount(dx,dy,dz);
    x = (int)((vPos.x - nodeBox.m_vMin.x) * m_pGrid->m_vInvSpacingSafe.x * (float)dx);
    y = (int)((vPos.y - nodeBox.m_vMin.y) * m_pGrid->m_vInvSpacingSafe.y * (float)dy);
    z = (int)((vPos.z - nodeBox.m_vMin.z) * m_pGrid->m_vInvSpacingSafe.z * (float)dz);
    VASSERT(x>=0 && x<dx);
    VASSERT(y>=0 && y<dy);
    VASSERT(z>=0 && z<dz);
  }

  // returns the first index of the child information. This is the 32bit subdivision count (RGB)
  inline int GetFirstChildIndex() const
  {
    VASSERT(HasChildren());
    return (GetNodeValue()&(~NODE_CHILD_FLAG));
  }

  // returns the index of the first color
  inline int GetFirstColorIndex() const 
  {
    VASSERT(!HasChildren());
    if (IsRootElement())
      return GetNodeValue();

    // for subdivided nodes, the index is relative to the parent
    VASSERT(m_iParentIndex>=0);
    VLightGridNodeIterator_cl parent(m_pGrid,m_iParentIndex);
    int iColorPos = parent.GetFirstChildIndex() + 1 + m_iIndex*6;
    return iColorPos;
  }

  // allocates colors for a root node
  inline void EnsureHasColors()
  {
    VASSERT(!HasChildren());
    if (GetFirstColorIndex()>0) return;
    SetNodeValue(m_pGrid->CreateColorHandle());
  }

  // create x*y*z sub-cells for a root node
  inline void CreateChildren(int iSubDivX, int iSubDivY, int iSubDivZ)
  {
    VASSERT(IsRootElement()); ///< Only root node is allowed to be subdivided
    VASSERT(!HasChildren())
    VASSERT(GetNodeValue()==0); ///< no color must be assigned yet
    SetNodeValue(m_pGrid->CreateChildrenHandle(iSubDivX,iSubDivY,iSubDivZ));
  }

  // get pointer to 6 RGBE colors. Valid also for sub nodes
  inline const unsigned int* GetColorPtr() const
  {
    VASSERT(!HasChildren());
    int iIndex = GetFirstColorIndex();
//    VASSERT(iIndex>0 && sizeof(m_pGrid->m_iNodes[iIndex])==sizeof(VColorRef));
    return (unsigned int* )&m_pGrid->m_iNodes[iIndex];
  }

  // store the colors in pDstColor array. Valid also for sub nodes
  void GetColors(hkvVec3* pDstColor, int iRequired=CUBEMAPFACE_BIT_ALL) const;

  // store the colors in this node
  VBASE_IMPEXP void SetColors(const hkvVec3* pColors);
  VBASE_IMPEXP void SetColors(const VColorRef *iColors);

  // get the color at world position
  void GetColorsRecursive(const hkvVec3& vPos, const hkvAlignedBBox &nodeBox, hkvVec3* pDstColor, int iRequired=CUBEMAPFACE_BIT_ALL) const;

  // checks that everything is allocated properly
  BOOL Finalize();

  // uses the tracer to trace the colors
  void TraceColors(IVLightGridTracer_cl *pTracer, const hkvAlignedBBox &nodeBox, int iSuperSample, int iTraceFlags);
  VBASE_IMPEXP bool RunVisitor(IVLightGridVisitor &visitor, const hkvAlignedBBox &nodeBox, float fProgress);

  // renders the node
  void Render(IVRenderInterface *pRenderer, const hkvAlignedBBox &nodeBox, int iFlags) const;

  void GetAverageColors(hkvVec3* pDest) const;
  float GetColorMeanSquareError() const;

  // helper function to convert x/y/z to linear index
  static inline int GetChildIndex(int dx, int dy, int dz, int x, int y, int z)
  {
    VASSERT(x>=0 && x<dx);
    VASSERT(y>=0 && y<dy);
    VASSERT(z>=0 && z<dz);
    return z*dx*dy + y*dx + x;
  }

  // helper function to create a node box of child node
  static inline void CreateChildBox(hkvAlignedBBox &dstbox, const hkvVec3& vStart, const hkvVec3& vChildDim, int x, int y, int z)
  {
    dstbox.m_vMin.x = vStart.x + (float)x*vChildDim.x;
    dstbox.m_vMin.y = vStart.y + (float)y*vChildDim.y;
    dstbox.m_vMin.z = vStart.z + (float)z*vChildDim.z;
    dstbox.m_vMax = dstbox.m_vMin+vChildDim;
  }


  // helper function to get face vertices of a box
  static void GetFaceVertices(const hkvAlignedBBox &dstbox, hkvVec3* pDest, int iFace=VTM_CUBEMAPFACE_POS_X);

  void CopyColorsToBuffer(int &iCount, hkvVec3* &pDestBuffer) const;
  int CopyColorsFromBuffer(const hkvVec3* pSrcBuffer);

  VLightGrid_cl *m_pGrid;
  int m_iParentIndex;
  int m_iIndex;
};


/// \brief
///   Interface to traverse all nodes in a lightgrid. See VLightGrid_cl::RunVisitor. Internal use only.
class IVLightGridVisitor
{
public:
  inline IVLightGridVisitor() {m_pLightGrid=NULL;}

  virtual ~IVLightGridVisitor() {}

  /// \brief
  ///   Virtual overridable that gets called at the beginning of VLightGrid_cl::RunVisitor
  VBASE_IMPEXP virtual bool OnVisitStart(VLightGrid_cl *pGrid) {return true;}

  /// \brief
  ///   Per-node callback from inside VLightGrid_cl::RunVisitor
  VBASE_IMPEXP virtual bool VisitNode(VLightGridNodeIterator_cl &nodeHandle, const hkvAlignedBBox &nodeBox, float fProgress) = 0;

  /// \brief
  ///   Virtual overridable that gets called when VLightGrid_cl::RunVisitor has finished
  VBASE_IMPEXP virtual void OnVisitEnd() {}

  VLightGrid_cl *m_pLightGrid;
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
