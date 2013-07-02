/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_DEBUG_CONSTRAINT_CHAIN_RENDERER_H_INCLUDED
#define V_HAVOK_DEBUG_CONSTRAINT_CHAIN_RENDERER_H_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraintChainRenderer.hpp>

// ****************************************************************************************************
// ***** PLEASE NOTE: THIS COMPONENT IS DEPRECATED; USE PATH RENDERER OF VISION ENGINE PLUGIN INSTEAD!
// ****************************************************************************************************

// ----------------------------------------------------------------------------
// vHavokDebugConstraintChainRenderer
// ----------------------------------------------------------------------------
/// \brief
///   A simple constraint chain renderer that draws a line for each link of
///   the constraint chain
class vHavokDebugConstraintChainRenderer
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
  VHAVOK_IMPEXP vHavokDebugConstraintChainRenderer(
    int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);
  //@}

  /// @name Base Class Overrides
  //@{
protected:
  void OnRender();
  //@}

  /// @name Serialization
  //@{
public:
  V_DECLARE_SERIAL_DLLEXP( vHavokDebugConstraintChainRenderer, VHAVOK_IMPEXP)
  V_DECLARE_VARTABLE(vHavokDebugConstraintChainRenderer, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
  //@}

  /// @name Attributes exposed to vForge
  //@{
public:  
  VColorRef Render_Color; ///< Color in which the constraint chain is visualized
  //@}

  /// @name Internal Attributes
  //@{
private:
  static const unsigned int s_iSerialVersion; ///< The current serialization version for objects of this class.
  //@}
};

#endif //V_HAVOK_DEBUG_CONSTRAINT_CHAIN_RENDERER_H_INCLUDED

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
