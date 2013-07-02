/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef V_HAVOK_CHAIN_ANIMATION_HPP_INCLUDED
#define V_HAVOK_CHAIN_ANIMATION_HPP_INCLUDED

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsIncludes.hpp>

class vHavokConstraintChain;


/// \brief
///   Component for animating the bones of an entity according to the
///   links of a Havok constraint chain.
class vHavokChainAnimation
  : public IVObjectComponent
  , public IVisCallbackHandler_cl
{

  /// @name Constructor / Destructor
  //@{
public:
  /// \brief
  ///   Constructor for this chain animation component.
  /// \param iComponentFlags
  ///   Component bit flags that define the behavior of the component.
  /// \see
  ///   IVObjectComponent::IVObjectComponent
  VHAVOK_IMPEXP vHavokChainAnimation(
    int iComponentFlags=VIS_OBJECTCOMPONENTFLAG_NONE);

  /// \brief
  ///   Destructor.
  VHAVOK_IMPEXP virtual ~vHavokChainAnimation();
  //@}

  /// @name Initialization / Deinitialization
  //@{
protected:
  /// \brief
  ///   Initializes this component after it has been added to an owner.
  void CommonInit();

  /// \brief
  ///   Deinitialization that is used both on DisposeObject and on destruction.
  void CommonDeinit();  
  //@}

  /// @name Base Class Overrides
  //@{
public:
  VHAVOK_IMPEXP VOVERRIDE BOOL CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut);
  VHAVOK_IMPEXP VOVERRIDE void DisposeObject();
  VHAVOK_IMPEXP VOVERRIDE void OnHandleCallback(IVisCallbackDataObject_cl *pData);
  VHAVOK_IMPEXP VOVERRIDE void OnVariableValueChanged(VisVariable_cl *pVar, const char * value);
  VHAVOK_IMPEXP VOVERRIDE void SetOwner(VisTypedEngineObject_cl *pOwner);
  //@}

  /// @name Serialization
  //@{
public:
  V_DECLARE_SERIAL_DLLEXP( vHavokChainAnimation, VHAVOK_IMPEXP)
  V_DECLARE_VARTABLE(vHavokChainAnimation, VHAVOK_IMPEXP)
  VHAVOK_IMPEXP VOVERRIDE void Serialize( VArchive &ar );
  VHAVOK_IMPEXP VOVERRIDE void OnDeserializationCallback(const VSerializationContext &context);
  VOVERRIDE VBool WantsDeserializationCallback(const VSerializationContext &context) { return context.m_eType != VSerializationContext::VSERIALIZATION_EDITOR; }
  //@}

  /// @name Helpers
  //@{
protected:
  /// \brief
  ///   Retrieves the constraint chain instance matching the \c ConstraintChainKey
  /// \return
  ///   A pointer to a constraint chain; or \c null, if the key is empty or if no 
  ///   constraint chain with that key exists.
  vHavokConstraintChain *GetConstraintChain();

  /// \brief
  ///   Updates the bones of the owner entity with the data of the constraint chain
  ///   this component refers to. Does nothing if no constraint chain is specified
  ///   or if the specified constraint chain does not exist.
  void UpdateBones();
  //@}

  /// @name Public Attributes
  //@{
public:
  VString ConstraintChainKey; ///< The key of the constraint chain used to animate the entity
  float BoneRoll; ///< The roll to apply to each bone in addition to the link transformation
  BOOL ReverseBoneOrder; ///< Whether to reverse the bone order before applying the transformations
  //@}

  /// @name Internal Attributes
  //@{
protected:
  unsigned int m_iConstraintChainIndex; ///< The last known index of the constraint chain this component refers to
  VisBaseEntity_cl *m_pOwnerEntity; ///< The owner of this component as a VisBaseEntity_cl

private:
  static const unsigned int s_iSerialVersion; ///< The current serialization version for objects of this class.
  //@}

  /// @name Constants
  //@{
protected:
  static const unsigned int InvalidIndex; ///< The invalid constraint chain index
  //@}
};

#endif //V_HAVOK_CHAIN_ANIMATION_HPP_INCLUDED

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
