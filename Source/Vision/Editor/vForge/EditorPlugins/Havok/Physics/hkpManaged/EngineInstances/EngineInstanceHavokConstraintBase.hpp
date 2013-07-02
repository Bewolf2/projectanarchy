/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#pragma once

using namespace System;
using namespace CSharpFramework;
using namespace CSharpFramework::Math;
using namespace CSharpFramework::Shapes;
using namespace CSharpFramework::Scene;
using namespace ManagedFramework;
using namespace VisionManaged;

namespace HavokManaged
{
  ref struct AnchorDescription;

  /// Managed C++ representation of Havok constraint constructs. 
  /// Used for managed/native communication.
  public ref class EngineInstanceHavokConstraintBase abstract : public IEngineShapeInstance
  {
  public:
    /// @name Construction/Destruction
    //@{
    EngineInstanceHavokConstraintBase();

    virtual void DisposeObject() override;
    //@}

    /// @name Overridden IEngineShapeInstance functions
    //@{
    virtual void SetVisible(bool bStatus) override {}
    virtual void SetPosition(float x,float y,float z) override;
    virtual void SetOrientation(float yaw,float pitch,float roll) override;
    virtual void SetScaling(float x,float y, float z) override {}
    virtual void TraceShape(Shape3D ^ownerShape, Vector3F rayStart, Vector3F rayEnd, ShapeTraceResult ^%result) override;
    virtual bool GetLocalBoundingBox(BoundingBox ^%bbox) override;
    virtual void SetObjectKey(String ^key) override;
    //@}


    /// @name Anchor functions
    //@{

    /// Connects our constraint with the passed static world position (static world anchor).
    /// \param iAnchorId: unique ID of the anchor. Used to identify the anchor on removal.
    /// \param pStaticAnchorShape: pointer to the shape which represents the anchor
    /// \return true on success
    bool AddWorldAnchor(System::Int64 iAnchorId, Shape3D ^pStaticAnchorShape);

    /// Connects our constraint with the passed entity anchor.
    /// Fails if both connections are already occupied.
    /// \param iAnchorId: unique ID of the anchor. Used to identify the anchor on removal.
    /// \param pEntity: entity instance representing the physics-enabled entity to connect this constraint to
    /// \param pLocalSpacePosition: position of the anchor in local space of the entity
    /// \return true on success.
    bool AddEntityAnchor(System::Int64 iAnchorId, EngineInstanceEntity ^pEntity, Vector3F ^pLocalSpacePosition);

    /// Disconnects the passed anchor from our constraint
    /// Fails if the passed anchor is not connected with our constraint
    /// \param iAnchorId: unique ID of the anchor.
    /// \return true on success.
    bool RemoveAnchor(System::Int64 iAnchorId);

    /// Indicates whether the both connecting anchors have been defined.
    /// \return true if both anchors are connected.
    bool AreBothAnchorsDefined();

    //@}


    /// @name Configuration Functions
    //@{

    /// Indicates whether this constraint (or constraint chaing) engine instance is valid.
    /// An engine instance is not valid (and thus could not be created) if the attached
    /// entities are not compatible (e.g. not physics-enabled) or the configuration contains 
    /// invalid data.
    /// If the constraint doesn't have both anchors attached yet, then the configuration is
    /// still treated as valid (since there are no illegal values set).
    ///
    /// The validation of the configuration is only performed on recreation of the native 
    /// constraint.
    bool IsConfigurationValid();

    //@}

  protected:
    /// Gets the native entity instance from the passed engine instance.
    VisBaseEntity_cl *GetNativeEntityFromEngineInstance(EngineInstanceEntity ^pEntity);

    /// Removes the current native constraint instance and recreates it
    virtual void RecreateNativeObject() = 0;

    /// Updates the key of the native object from the cached data
    virtual void UpdateObjectKey() = 0;

  protected:
    float m_posX, m_posY, m_posZ;           ///< Current world space position of the constraint shape in the scene
    float m_dirX, m_dirY, m_dirZ;           ///< Current orientation of the constraint shape in the scene
    float m_oriY, m_oriP, m_oriR;           ///< Current orientation as Euler angles

    AnchorDescription ^m_pFirstAnchor;      ///< information for the first actor (NULL if not defined)        
    AnchorDescription ^m_pSecondAnchor;     ///< information for the first actor (NULL if not defined)        

    bool m_bConfigurationValid;             ///< Indicates whether the configuration is valid (i.e. complete and valid). Only updated on recreation of native constraint.
    bool m_bDisposed;                       ///< Indicates whether this object has already been disposed

    String ^m_sCachedObjectKey;             ///< Cached object key; needed to re-set it when the native instance is recreated.
  };

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
