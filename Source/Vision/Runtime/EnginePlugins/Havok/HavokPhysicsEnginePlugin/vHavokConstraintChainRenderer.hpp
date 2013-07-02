/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_CONSTRAINT_CHAIN_RENDERER_HPP_INCLUDED
#define V_HAVOK_CONSTRAINT_CHAIN_RENDERER_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

class vHavokConstraintChain;


// ----------------------------------------------------------------------------
// vHavokConstraintChainRendererBase
// ----------------------------------------------------------------------------

/// \brief
///   Base class for all vHavok constraint chain renderers.
///
/// This base class provides functions common to all renderers, such as 
/// registration of relevant callbacks, and common component tasks.
class vHavokConstraintChainRendererBase
: public IVObjectComponent
, public IVisCallbackHandler_cl
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
  VHAVOK_IMPEXP vHavokConstraintChainRendererBase(
    int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Destructor; removes the renderer if it's still active.
  VHAVOK_IMPEXP virtual ~vHavokConstraintChainRendererBase();
  //@}

  /// @name Initialization / Deinitialization
  //@{
public:
  /// \brief
  ///   Removes the rigid body from the simulation without necessarily deleting 
  ///   this instance.
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();

protected:
  /// \brief
  ///   Initializes this component after it has been added to an owner.
  /// \note
  ///   Override the method DoInit() to perform initialization tasks in subclasses!
  void CommonInit();
  
  /// \brief
  ///   Overridable initialization method.
  /// \return
  ///   \c true if the initialization succeeded; \c false if it failed
  virtual bool DoInit();

  /// \brief
  ///   Deinitialization that is used both on DisposeObject and on destruction.
  virtual void CommonDeinit();  
  //@}

  /// @name Base Class Overrides
  //@{
public:
  VHAVOK_IMPEXP VOVERRIDE BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);
  VHAVOK_IMPEXP VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  VHAVOK_IMPEXP VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);
  //@}

  /// @name Serialization
  //@{
public:
  V_DECLARE_SERIAL_DLLEXP( vHavokConstraintChainRendererBase, VHAVOK_IMPEXP)
  V_DECLARE_VARTABLE(vHavokConstraintChainRendererBase, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
  VHAVOK_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);
  VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) { return context.m_eType != VSerializationContext::VSERIALIZATION_EDITOR; }
  //@}

  /// @name Rendering
  //@{
private:  
  /// \brief
  ///   Performs the rendering of the constraint chain.
  virtual void OnRender();
  /// \brief
  ///   Performs any updating necessary before the chain is rendered in this frame
  virtual void OnUpdate();
  //@}

  /// @name Internal Attributes
  //@{
protected:
  vHavokConstraintChain *m_pConstraintChain; ///< The constraint chain this renderer is attached to
private:
  static const unsigned int s_iSerialVersion; ///< The current serialization version for objects of this class.
  //@}
};

#endif //V_HAVOK_CONSTRAINT_CHAIN_RENDERER_HPP_INCLUDED

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
