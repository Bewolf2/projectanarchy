/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VProfiling.hpp

#ifndef VPROFILING_HPP_INCLUDED
#define VPROFILING_HPP_INCLUDED

#include <Vision/Runtime/Base/Container/VBitField.hpp>

class IVRenderInterface;
class VProfilingNode;
typedef VSmartPtr<VProfilingNode> VProfilingNodePtr;

#define MAX_PROFILING_ID_COUNT      4096

#define PROFILING_ID_NOPROFILING    -1
#define PROFILING_ID_ASSIGNNEW      -2

#define PROFILING_FLAGS_SHOW     0x00000001
#define PROFILING_FLAGS_DEFAULT  (PROFILING_FLAGS_SHOW)

/// \brief
///   Container class for reference-counted VProfilingNode instances
class VProfilingNodeCollection : public VRefCountedCollection<VProfilingNode>
{
};


/// \brief
///   Class that represents one profiling item inside the profiling graph
/// 
/// The profiling items are arranged in a hierarchical profiling tree. For instance, one node can
/// measure the "overall AI time" and its child nodes measure a finer-grained profile.
/// 
/// See the Vision engine's Vision::Profiling functionality for specifics on how to build a profiling tree.
/// 
/// Furthermore, please refer to the Programmer's Manual for further information on the Profiling functionality
/// provided by the Vision Engine.
class VProfilingNode : public VRefCounter
{
public:

  ///
  /// @name Constructor/Destructor
  /// @{
  ///

  /// \brief
  ///   Constructor of a new profiling node
  /// 
  /// \param pParentNode
  ///   Optional parent node this node is attached to. Can be NULL
  /// 
  /// \param szName
  ///   Name to display this profiling node with in the graph
  /// 
  /// \param iID
  ///   Integer profiling ID this node is registered with. There are the following reserved values:
  ///   \li PROFILING_ID_ASSIGNNEW: The class finds a valid new ID that can be queried with VProfilingNode::GetID.
  /// 
  ///   \li PROFILING_ID_NOPROFILING: This item does not perform profiling; for instance it is just
  ///     used as a group headline.
  /// 
  /// \param iFlags
  ///   Flags for the profiling node. The following flags are supported:
  ///   \li PROFILING_FLAGS_SHOW: The item is shown in the graph.
  /// 
  ///   \li PROFILING_FLAGS_DEFAULT: combination of default bits (currently PROFILING_FLAGS_SHOW).
  VBASE_IMPEXP VProfilingNode(VProfilingNode *pParentNode, const char *szName, int iID=PROFILING_ID_ASSIGNNEW, int iFlags=PROFILING_FLAGS_DEFAULT);

  
  /// \brief
  ///   Destructor
  VBASE_IMPEXP virtual ~VProfilingNode();


  ///
  /// @}
  ///

  ///
  /// @name Query Information
  /// @{
  ///

  /// \brief
  ///   Returns the name of the node that was passed to the constructor
  inline const char* GetName() const
  {
    return m_sName;
  }

  /// \brief
  ///   Returns the profiling ID of this node. Valid IDs are >=0, otherwise it is e.g. PROFILING_ID_NOPROFILING.
  inline int GetID() const
  {
    return m_iProfilingID;
  }

  /// \brief
  ///   Indicates whether the ID is a valid ID for measuring performance.
  inline bool HasValidID() const
  {
    return m_iProfilingID>=0;
  }


  /// \brief
  ///   Returns the number of CPU cycles / ticks spent in this item during the last frame.
  inline unsigned int GetTimeInCycles() const
  {
    return m_iCycles;
  }

  /// \brief
  ///   Returns the time spent in this item during the last frame, measured in milliseconds.
  inline float GetTimeInMS() const
  {
    return m_fTimeInMS;
  }

  /// \brief
  ///   Returns the maximum time spent in this item (measured in milliseconds) since the last call
  ///   of ResetMaximum. Can be globally reset by the profiling class.
  inline float GetMaximumTimeInMS() const
  {
    return m_fMaxTimeMS;
  }

  /// \brief
  ///   Returns number of calls of this item during the last frame.
  inline unsigned int GetCallCount() const
  {
    return m_iCallCount;
  }

  /// \brief
  ///   Returns the contribution percentage of this item to the overall frame duration, where the
  ///   overall time is represented by the root node.
  inline float GetPercentage() const
  {
    return m_fPercentage;
  }


  ///
  /// @}
  ///

  ///
  /// @name Graph Hierarchy
  /// @{
  ///

  /// \brief
  ///   Accesses the collection of child nodes. Add/Remove children via AddChild/RemoveChild.
  inline const VProfilingNodeCollection& Children() const
  {
    return m_Children;
  }

  /// \brief
  ///   Returns the items's parent node. Can be NULL if it is not attached.
  inline VProfilingNode* GetParentNode() const
  {
    return m_pParentNode;
  }

  /// \brief
  ///   Adds the passed node as a child to this node. The return value is the ID of this node for faster access in subsequent functions.
  VBASE_IMPEXP int AddChild(VProfilingNode *pNode);

  /// \brief
  ///   Overloaded version that takes the name, an ID (which is a reference that receives a valid
  ///   ID if iID was PROFILING_ID_ASSIGNNEW), and flags.
  VBASE_IMPEXP VProfilingNode* AddChild(const char *szName, int &iID, int iFlags=PROFILING_FLAGS_DEFAULT);

  /// \brief
  ///   Removes a node from the list of this node's children
  VBASE_IMPEXP void RemoveChild(VProfilingNode *pNode);

  /// \brief
  ///   Removes all child nodes
  VBASE_IMPEXP void RemoveAllChildren();

  /// \brief
  ///   Removes the current node from the graph. If bRecursive is set to true, all child nodes of this node are removed as well.
  VBASE_IMPEXP void Remove(bool bRecursive=true);

  /// \brief
  ///   Returns the first node (or NULL) with the specified ID that is found in the subtree headed by the node. 
  VBASE_IMPEXP VProfilingNode* FindByID(int iID);

  /// \brief
  ///   Returns the first node (or NULL) with the specified name that is found in the subtree headed by the node. 
  VBASE_IMPEXP VProfilingNode* FindByName(const char *szName);

  ///
  /// @}
  ///

  ///
  /// @name Update
  /// @{
  ///

  /// \brief
  ///   Updates the result once a frame. Called by the engine.
  VBASE_IMPEXP void UpdateFrame(bool bRecursive=true);

  /// \brief
  ///   Recursively resets the value returned by GetMaximumTimeInMS. Can be called on the root node
  VBASE_IMPEXP void ResetMaximum();

  /// \brief
  ///   Displays a sub-graph of the results as text overlay. Called by the engine
  VBASE_IMPEXP void Show(IVRenderInterface *pRenderer, int x, int &y, int iIndent, VColorRef iBaseColor, bool bRecursive);


  ///
  /// @}
  ///

  ///
  /// @name Static Functions
  /// @{
  ///

  /// \brief
  ///   Returns one global instance of the root node (set by InitProfiling).
  VBASE_IMPEXP static VProfilingNode& Root();

  /// \brief
  ///   Creates a new profiling ID integer value that can be used for measuring performance.
  VBASE_IMPEXP static int GetNewProfilingID();

  /// \brief
  ///   Frees a profiling ID again. Called by the destructor of a profiling node.
  VBASE_IMPEXP static void FreeProfilingID(int iID);

  /// \brief
  ///   Reserves a range that will not be returned by FreeProfilingID.
  VBASE_IMPEXP static void ReserveProfilingIDs(int iRangeStart,int iRangeEnd);

  /// \brief
  ///   Globally initializes the profiling system. Called by the engine at startup.
  VBASE_IMPEXP static void InitProfiling(int iCyclesPerMs, int iCycleScale, long *pCycleArray, long *pCallArray);

  /// \brief
  ///   Sets the root node of the graph. Called by the engine.
  VBASE_IMPEXP static void SetRootNode(VProfilingNode *pRootNode);

  /// \brief
  ///   Updates profiling properties. Called by the engine when the profiling mode changes.
  VBASE_IMPEXP static void SetProfilingParams(int iCyclesPerMs, int iCycleScale);

  /// \brief
  ///   Globally de-initializes the profiling. Called by the engine.
  VBASE_IMPEXP static void DeInitProfiling();

  /// \brief
  ///   Updates the profiling. Called by the engine once a frame.
  VBASE_IMPEXP static void UpdateProfiling();

protected:
  VBASE_IMPEXP unsigned int EvaluateTimeInCycles() const;
  VBASE_IMPEXP unsigned int EvaluateCallCount() const;
  VString m_sName;
  int m_iProfilingID;
  int m_iFlags;
  float m_fTimeInMS, m_fMaxTimeMS, m_fPercentage;
  unsigned int m_iCycles, m_iCallCount;

  VProfilingNode *m_pParentNode;
  VProfilingNodeCollection m_Children;

  ///
  /// @}
  ///

private:
  static VTBitfield<MAX_PROFILING_ID_COUNT> &GetUsedProfilingIDs();

  static bool g_bStructureChanged;
  static int g_iCyclesPerMs, g_iCycleScale;
  static long *g_pCycleArray;
  static long *g_pCallArray;
  static VTBitfield<MAX_PROFILING_ID_COUNT> *g_pUsedProfilingIDs;

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
