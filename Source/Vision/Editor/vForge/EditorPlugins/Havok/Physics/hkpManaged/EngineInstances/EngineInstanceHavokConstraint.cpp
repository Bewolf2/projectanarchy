/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/HavokPhysicsManagedPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokBallAndSocketConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokHingeConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStiffSpringConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdollConstraint.hpp> 
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokGenericConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPrismaticConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPointToPlaneConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPointToPathConstraint.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPulleyConstraint.hpp>

#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/EngineInstances/EngineInstanceHavokConstraint.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/BallAndSocketConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/HingeConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/StiffSpringConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/RagdollConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/FixedConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/PrismaticConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/PointToPlaneConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/PointToPathConstraintState.hpp>
#include <Vision/Editor/vForge/EditorPlugins/Havok/Physics/hkpManaged/ConstraintProperties/PulleyConstraintState.hpp>

#using <mscorlib.dll>
using namespace ManagedFramework;
using namespace System::Diagnostics;


namespace HavokManaged
{
  EngineInstanceHavokConstraint::EngineInstanceHavokConstraint()
  : EngineInstanceHavokConstraintBase()
  {
    m_constraintType = HavokConstraintType_e::BallAndSocket;
    m_pConstraint = nullptr;
    m_pConstraintState = nullptr;
  }

  void EngineInstanceHavokConstraint::DisposeObject()
  {
    V_SAFE_RELEASE(m_pConstraint);
    EngineInstanceHavokConstraintBase::DisposeObject();
  }

  bool EngineInstanceHavokConstraint::OnExport(SceneExportInfo ^info) 
  {
    if (m_pConstraint == NULL)
      return true;
    VArchive &ar = *((VArchive *)info->NativeShapeArchivePtr.ToPointer());
    ar << m_pConstraint;
    return true;
  }

  void EngineInstanceHavokConstraint::EnsureAnchorOrder()
  {
    if (!AreBothAnchorsDefined())
      return;

    if ((m_pFirstAnchor->GetEntity() == NULL) && (m_pSecondAnchor->GetEntity() != NULL))
    {
      AnchorDescription ^pTempAnchor = m_pFirstAnchor;
      m_pFirstAnchor = m_pSecondAnchor;
      m_pSecondAnchor = pTempAnchor;
    }
  }

  void EngineInstanceHavokConstraint::SetConstraintType(HavokConstraintType_e type, ConstraintStateBase ^constraintState)
  {
    // Remember the new constraint type
    m_pConstraintState = constraintState;
    m_constraintType = type;

    RecreateNativeObject();
  }

  void EngineInstanceHavokConstraint::RecreateNativeObject()
  {
    // First release the existing constraint
    V_SAFE_RELEASE(m_pConstraint);

    // Cancel if we don't have a pair of anchors
    if (!AreBothAnchorsDefined())
      return;

    // Make sure that in case of an entity and a world anchor, the entity
    // anchor comes first.
    EnsureAnchorOrder();

    // Also cancel if constraint state is NULL for some reason
    Debug::Assert(m_pConstraintState != nullptr);
    if (!m_pConstraintState)
      return;

    // Get the native constraint description from the constraint state
    Vector3F position(m_posX, m_posY, m_posZ);
    Vector3F direction(m_dirX, m_dirY, m_dirZ);
    Vector3F orientation(m_oriY, m_oriP, m_oriR);
    vHavokConstraintDesc *pConstraintDesc = m_pConstraintState->GetNativeDescription(m_pFirstAnchor, m_pSecondAnchor, position, direction, orientation);
    if (!pConstraintDesc)
      return;

    // Create the constraint according to the configured type
    switch (m_constraintType)
    {
    case HavokConstraintType_e::BallAndSocket:
      m_pConstraint = new vHavokBallAndSocketConstraint();
      break;
    case HavokConstraintType_e::Hinge:
      m_pConstraint = new vHavokHingeConstraint();
      break;
    case HavokConstraintType_e::Ragdoll:
      m_pConstraint = new vHavokRagdollConstraint();
      break;
    case HavokConstraintType_e::StiffSpring:
      m_pConstraint = new vHavokStiffSpringConstraint();
      break;
    case HavokConstraintType_e::Fixed:
      m_pConstraint = new vHavokGenericConstraint();
      break;
    case HavokConstraintType_e::Prismatic:
    case HavokConstraintType_e::Cylindrical:
      m_pConstraint = new vHavokPrismaticConstraint();
      break;
    case HavokConstraintType_e::PointToPlane:
      m_pConstraint = new vHavokPointToPlaneConstraint();
      break;
    case HavokConstraintType_e::PointToPath:
      m_pConstraint = new vHavokPointToPathConstraint();
      break;
    case HavokConstraintType_e::Pulley:
      m_pConstraint = new vHavokPulleyConstraint();
      break;
    default:
      VASSERT_MSG(false, "Unsupported constraint type!");
    }
    if (!m_pConstraint->Init(*pConstraintDesc))
      V_SAFE_DELETE(m_pConstraint);
    V_SAFE_DELETE(pConstraintDesc);

    // Verify that the constraint could be created. If this is the case, then most
    // likely the constraint configuration was invalid.
    m_bConfigurationValid = (m_pConstraint != NULL);

    if (m_pConstraint)
      m_pConstraint->AddRef();

    UpdateObjectKey();
  }

  void EngineInstanceHavokConstraint::SetConstraintProperties(ConstraintStateBase ^constraintState)
  {
    // Set the new constraint state, and recreate the constraint according to it.
    m_pConstraintState = constraintState;
    RecreateNativeObject();
  }

  void EngineInstanceHavokConstraint::VisualizeConstraintState(VisionViewBase ^view, ShapeRenderMode mode)
  {
    if (!m_pConstraintState)
      return;

    if (!AreBothAnchorsDefined())
      return;

    IVRenderInterface *pRenderer = (static_cast<VisionView^>(view))->GetRenderInterface();

    // Visualize the constraint
    hkvVec3 startPos = m_pFirstAnchor->GetWorldSpacePosition();
    hkvVec3 endPos = m_pSecondAnchor->GetWorldSpacePosition();
    hkvVec3 constraintPos(m_posX, m_posY, m_posZ);
    hkvVec3 constraintDir(m_dirX, m_dirY, m_dirZ);
    hkvVec3 constraintOri(m_oriY, m_oriP, m_oriR);
    m_pConstraintState->Visualize(pRenderer, startPos, endPos, constraintPos, constraintDir, constraintOri);
  }

  void EngineInstanceHavokConstraint::UpdateObjectKey()
  {
    if (!m_pConstraint)
      return;

    VString sKey;
    ConversionUtils::StringToVString(m_sCachedObjectKey, sKey);
    if (sKey.IsEmpty())
      m_pConstraint->SetObjectKey(NULL);
    else
      m_pConstraint->SetObjectKey(sKey);
  }

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
