/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_CABLE_CONSTRAINT_CHAIN_RENDERER_H_INCLUDED
#define V_HAVOK_CABLE_CONSTRAINT_CHAIN_RENDERER_H_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChainRenderer.hpp>

class VisMeshBuffer_cl;

// ****************************************************************************************************
// ***** PLEASE NOTE: THIS COMPONENT IS DEPRECATED; USE PATH RENDERER OF VISION ENGINE PLUGIN INSTEAD!
// ****************************************************************************************************

// ----------------------------------------------------------------------------
// vHavokCableConstraintChainEntity
// ----------------------------------------------------------------------------

/// \brief
///   Entity class for holding the generated model of a constraint chain-based
///   cable.
///
/// This class is responsible for transferring the state of the constraint 
/// chain's link bodies to the bones used for animating the model.
///
class vHavokCableConstraintChainEntity
: public VisBaseEntity_cl
{
  V_DECLARE_SERIAL_DLLEXP(vHavokCableConstraintChainEntity, VHAVOK_IMPEXP);

public:
  /// \brief
  ///   Default constructor
  VHAVOK_IMPEXP vHavokCableConstraintChainEntity();

public:
  /// \brief
  ///   Sets the constraint chain used for updating the bone state
  /// \param pConstraintChain
  ///   the constraint chain to set
  VHAVOK_IMPEXP void SetConstraintChain(vHavokConstraintChain *pConstraintChain);

public:
  VHAVOK_IMPEXP VOVERRIDE void ThinkFunction();
  VHAVOK_IMPEXP VOVERRIDE void EditorThinkFunction() { ThinkFunction(); }

private:
  vHavokConstraintChain *m_pConstraintChain; ///< The constraint chain used to update the model
  unsigned int m_iLastKnownNumLinks; ///< The last known number of chain links
};


// ----------------------------------------------------------------------------
// vHavokCableConstraintChainRenderer
// ----------------------------------------------------------------------------

/// \brief
///   A constraint chain renderer that renders the constraint chain as a 
///   contiguous cable.
class vHavokCableConstraintChainRenderer
: public vHavokConstraintChainRendererBase
{
  /// @name Constructor / Destructor
  //@{
public:
  /// \brief
  ///   Constructor for this constraint chain renderer component.
  /// \param iComponentFlags
  ///   Component bit flags that define the behavior of the component.
  /// \see
  ///   IVObjectComponent::IVObjectComponent
  VHAVOK_IMPEXP vHavokCableConstraintChainRenderer(
    int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);
  //@}

  /// @name Base Class Overrides
  //@{
protected:
  VOVERRIDE void CommonDeinit();  
  VOVERRIDE bool DoInit();
  VOVERRIDE void OnUpdate();
  VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);
  //@}

  /// @name Model/Entity Handling
  //@{
private:
  bool RebuildModel();
  //@}

  /// @name Serialization
  //@{
public:
  V_DECLARE_SERIAL_DLLEXP( vHavokCableConstraintChainRenderer, VHAVOK_IMPEXP)
  V_DECLARE_VARTABLE(vHavokCableConstraintChainRenderer, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
  //@}

  /// @name Attributes exposed to vForge
  //@{
public:  
  int VerticesPerRing; ///< Number of vertices in each of the rings that form the cable's outline
  int RingsPerLink;    ///< Number of vertex rings defining the cable's outline for each chain link
  VString ModelFile;   ///< Model file from which the material definition for the cable's surface is taken
  BOOL CastDynamicShadows;   ///< If enabled dynamic shadow is rendered for cable
  //@}

  /// @name Internal Attributes
  //@{
private:
  VDynamicMeshPtr m_spChainMesh; ///< The generated cable mesh
  VSmartPtr<vHavokCableConstraintChainEntity> m_spChainEntity; ///< The entity for updating the cable bone state
  unsigned int m_iLastKnownNumLinks; ///< The last known number of chain links
  static const unsigned int s_iSerialVersion; ///< The current serialization version for objects of this class.
  //@}
};


#endif //V_HAVOK_CABLE_CONSTRAINT_CHAIN_RENDERER_H_INCLUDED

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
