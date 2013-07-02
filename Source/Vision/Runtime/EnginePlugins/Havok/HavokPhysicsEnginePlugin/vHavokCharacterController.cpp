/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/HavokPhysicsEnginePluginPCH.h>

#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokShapeFactory.hpp>
#include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokStaticMesh.hpp>

#include <Physics2012/Utilities/CharacterControl/StateMachine/Util/hkpCharacterMovementUtil.h>
#include <Physics2012/Collide/Shape/Convex/Capsule/hkpCapsuleShape.h> 
#include <Physics2012/Internal/Collide/BvCompressedMesh/hkpBvCompressedMeshShape.h>

#include <Physics2012/Dynamics/Phantom/hkpSimpleShapePhantom.h>
#include <Physics2012/Utilities/CharacterControl/CharacterProxy/hkpCharacterProxy.h>
#include <Physics2012/Collide/Query/Collector/PointCollector/hkpAllCdPointCollector.h>
#include <Physics2012/Collide/Agent/Collidable/hkpCdPoint.h>
#include <Physics2012/Collide/Query/CastUtil/hkpLinearCastInput.h>
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastInput.h>
#include <Physics2012/Collide/Query/CastUtil/hkpWorldRayCastOutput.h>
#include <Common/GeometryUtilities/Misc/hkGeometryUtils.h>

#if defined(HAVOK_SDK_VERSION_MAJOR) && (HAVOK_SDK_VERSION_MAJOR >= 2012)
#include <Common/Internal/SimplexSolver/hkSimplexSolver.h>
#else
#include <Physics/ConstraintSolver/Simplex/hkpSimplexSolver.h>
#endif

#include <Vision/Runtime/Engine/System/Vision.hpp>


#define DEBUG_HAVOK_CHARACTER_CONTROLLER
#undef DEBUG_HAVOK_CHARACTER_CONTROLLER


class CharacterBestStepCollector : public hkpCdPointCollector
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT, CharacterBestStepCollector);

	CharacterBestStepCollector( )
		: hkpCdPointCollector()
		, m_matchPos(hkVector4::getZero())
		, m_matchDir(hkVector4::getZero())
		, m_useMatch(false)
	{
		reset();
	}

	CharacterBestStepCollector( hkVector4Parameter matchPos, hkVector4Parameter matchDir )
		: hkpCdPointCollector()
		, m_matchPos(matchPos)
		, m_matchDir(matchDir)
		, m_useMatch(true)
	{
		HK_ASSERT2(0x13372008, m_matchDir.isNormalized<3>(), "The match vector must be normalized!");
		reset();
	}

	virtual ~CharacterBestStepCollector( )
	{}

	virtual inline void reset()
	{
		m_hitPoint.m_rootCollidableA = HK_NULL;
		m_hitPoint.m_contact.setDistance( HK_REAL_MAX );
		m_best = m_useMatch ? -1.f : HK_REAL_MAX;
		hkpCdPointCollector::reset();
	}

	hkBool hasHit( ) const
	{
		return m_hitPoint.m_rootCollidableA != HK_NULL;
	}

	const hkpRootCdPoint& getHit() const
	{
		return m_hitPoint;
	}

	const hkContactPoint& getHitContact() const
	{
		return m_hitPoint.m_contact;
	}

protected:

	virtual void addCdPoint( const hkpCdPoint& event )
	{
		// We ignore dynamic bodies all along.
		const hkpRigidBody* body = hkpGetRigidBody( event.m_cdBodyB.getRootCollidable() );
		const bool isDynamic = body && !body->isFixedOrKeyframed();
		if(isDynamic) return;

		// Use special step match method which selects the cp which is on the same half space as (m_matchPos, m_matchDir) and has its normal closest to m_matchDir.
		const hkSimdReal currentBest = hkSimdReal::fromFloat(m_best);
		if(m_useMatch)
		{
			hkVector4 dir; dir.setSub(event.getContact().getPosition(), m_matchPos);
			const hkSimdReal halfSpace = dir.dot<3>(m_matchDir);
			const hkSimdReal cosAng = event.getContact().getNormal().dot<3>(m_matchDir);
			if ( halfSpace.isLessZero() | cosAng.isLessEqual(currentBest) ) return;
			cosAng.store<1>(&m_best);
		}
		// Defaults to closest point wrt distance.
		else
		{
			if(event.getContact().getDistanceSimdReal().isGreaterEqual(currentBest)) return;
			event.getContact().getDistanceSimdReal().store<1>(&m_best);
		}

		// Update the contact point.
		{
			m_hitPoint.m_contact = event.getContact();
			m_hitPoint.m_rootCollidableA = event.m_cdBodyA.getRootCollidable();
			m_hitPoint.m_shapeKeyA = event.m_cdBodyA.getShapeKey();

			m_hitPoint.m_rootCollidableB = event.m_cdBodyB.getRootCollidable();
			m_hitPoint.m_shapeKeyB = event.m_cdBodyB.getShapeKey();
			event.getContact().getDistanceSimdReal().store<1>(&m_earlyOutDistance);
		}
	}

protected:

	const hkVector4& m_matchPos;
	const hkVector4& m_matchDir;
	hkpRootCdPoint m_hitPoint;
	hkReal m_best;
	const bool m_useMatch;

};

#ifdef DEBUG_HAVOK_CHARACTER_CONTROLLER

#define DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_LINES     1024
#define DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_CAPSULES  16

class DebugHavokCharacterController
{
public:

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR(HK_MEMORY_CLASS_AGENT, DebugHavokCharacterController);

	DebugHavokCharacterController() {}
	virtual ~DebugHavokCharacterController()
	{
		cleanUp();
	}

	void cleanUp()
	{
		m_lines.clear();
		m_capsules.clear();
	}

public:
	struct DebugLine
	{
		DebugLine(hkVector4Parameter pos, hkVector4Parameter vec, bool hit, bool line = false) : m_pos(pos), m_vec(vec), m_hit(hit), m_line(line) {}
		hkVector4 m_pos;
		hkVector4 m_vec;
		bool m_hit;
		bool m_line;
	};
	hkArray<DebugLine> m_lines;

	struct DebugCapsule
	{
		DebugCapsule(hkVector4Parameter pos, bool hit) : m_pos(pos), m_hit(hit) {}
		hkVector4 m_pos;
		bool m_hit;
	};
	hkArray<DebugCapsule> m_capsules;

	void drawDebugLines()
	{
		for(int i = 0; i < m_lines.getSize(); ++i)
		{
      if (i >= DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_LINES)
      {
        m_lines.clear();
        break;
      }

			const DebugLine& line = m_lines[i];

			hkvVec3 beg = HK2VIS_VECTOR_SCALED(line.m_pos);
			hkvVec3 end;
			VColorRef col;
			if(line.m_line)
			{
				end = HK2VIS_VECTOR_SCALED(line.m_vec);
				col = line.m_hit ? VColorRef(255, 0, 255) : VColorRef(0, 0, 255);
			}
			else
			{
				Vision::Game.DrawCube(beg, 5.f, VColorRef(225, 225, 225));
				end = beg + HK2VIS_VECTOR_SCALED(line.m_vec);
				col = line.m_hit ? VColorRef(0, 255, 0) : VColorRef(255, 0, 0);
			}
			Vision::Game.DrawSingleLine(beg, end, col);
		}
	}

	void drawDebugCapsules(const vHavokCharacterController* character) 
	{
		for(int i = 0; i < m_capsules.getSize(); ++i)
		{
      if (i >= DEBUG_HAVOK_CHARACTER_CONTROLLER_MAX_NUM_CAPSULES)
      {
        m_capsules.clear();
        break;
      }

			const DebugCapsule& cap = m_capsules[i];

			hkGeometry geom;
			hkTransform t; t.setIdentity();
			t.setTranslation(cap.m_pos);
			hkGeometryUtils::createCapsuleGeometry(VIS2HK_VECTOR_SCALED(character->Character_Top), VIS2HK_VECTOR_SCALED(character->Character_Bottom), VIS2HK_FLOAT_SCALED(character->Capsule_Radius), 4, 4, t, geom);

			//VColorRef color(128, 128, 128);
			VColorRef color = cap.m_hit ? VColorRef(128, 228, 128) : VColorRef(228, 128, 128);
			for(int t = 0; t < geom.m_triangles.getSize(); ++t)
			{
				hkvVec3 v0 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 0));
				hkvVec3 v1 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 1));
				hkvVec3 v2 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 2));

				Vision::Game.DrawSingleLine(v0, v1, color);
				Vision::Game.DrawSingleLine(v1, v2, color);
				Vision::Game.DrawSingleLine(v2, v0, color);
			}
		}
	}

};
#define DEBUG_ADD_VISION_LINE(pos, vec, hit, line) //m_visDebug->m_lines.pushBack(DebugHavokCharacterController::DebugLine(pos, vec, hit, line))
#define DEBUG_ADD_VISION_LINE2(pos, vec, hit, line) m_visDebug->m_lines.pushBack(DebugHavokCharacterController::DebugLine(pos, vec, hit, line))
#define DEBUG_ADD_VISION_CAPSULE(pos, hit) //m_visDebug->m_capsules.pushBack(DebugHavokCharacterController::DebugCapsule(pos, hit))
#define DEBUG_DRAW_VISION_LINES() m_visDebug->drawDebugLines()
#define DEBUG_DRAW_VISION_CAPSULES(character) m_visDebug->drawDebugCapsules(character)
#define DEBUG_DRAW_CREATE() m_visDebug = new DebugHavokCharacterController()
#define DEBUG_DRAW_DESTROY() delete m_visDebug; m_visDebug = HK_NULL
#else
#define DEBUG_ADD_VISION_LINE(pos, vec, hit, line)
#define DEBUG_ADD_VISION_LINE2(pos, vec, hit, line) 
#define DEBUG_ADD_VISION_CAPSULE(pos, hit) 
#define DEBUG_DRAW_VISION_LINES() 
#define DEBUG_DRAW_VISION_CAPSULES(character) 
#define DEBUG_DRAW_CREATE() 
#define DEBUG_DRAW_DESTROY() 
#endif

vHavokCharacterPushableProxy::vHavokCharacterPushableProxy( const hkpCharacterProxyCinfo& info, const vHavokCharacterController *pOwner, hkReal strength )
	: hkpCharacterProxy( info )
	, m_strength( strength )
	, m_onStepCnt( 0 )
	, m_visDebug( HK_NULL )
{
  // Set user data to identify this Havok component during collision detection
	info.m_shapePhantom->setUserData((hkUlong)vHavokUserDataPointerPair_t::CombineTypeAndPointer((void*)pOwner, V_USERDATA_OBJECT));

	this->addCharacterProxyListener(this);
	DEBUG_DRAW_CREATE();
}

vHavokCharacterPushableProxy::~vHavokCharacterPushableProxy()
{
	DEBUG_DRAW_DESTROY();
}

void vHavokCharacterPushableProxy::drawDebug(const vHavokCharacterController* character) const
{
	DEBUG_DRAW_VISION_LINES();
	DEBUG_DRAW_VISION_CAPSULES(character);
#ifdef DEBUG_HAVOK_CHARACTER_CONTROLLER
	// Draw capsule
	if(1)
	{
		hkGeometry geom;
		hkTransform t; t.setIdentity();
		t.setTranslation(VIS2HK_VECTOR_SCALED(character->GetPosition()));
		hkGeometryUtils::createCapsuleGeometry(VIS2HK_VECTOR_SCALED(character->Character_Top), VIS2HK_VECTOR_SCALED(character->Character_Bottom), VIS2HK_FLOAT_SCALED(character->Capsule_Radius), 6, 6, t, geom);

		//VColorRef color(128, 128, 128);
		VColorRef color = isOnStep() ? VColorRef(128, 228, 228) : VColorRef(228, 228, 228);
		for(int t = 0; t < geom.m_triangles.getSize(); ++t)
		{
			hkvVec3 v0 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 0));
			hkvVec3 v1 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 1));
			hkvVec3 v2 = HK2VIS_VECTOR_SCALED(geom.getVertex(t, 2));

			Vision::Game.DrawSingleLine(v0, v1, color);
			Vision::Game.DrawSingleLine(v1, v2, color);
			Vision::Game.DrawSingleLine(v2, v0, color);
		}
	}
#endif
}

bool vHavokCharacterPushableProxy::handleSteps( const vHavokCharacterController* character, const hkpCharacterInput& input, hkpCharacterOutput& output )
{
	// Verify that any change may take place
	if( character->GetCharacterContext()->getState() != HK_CHARACTER_ON_GROUND || output.m_velocity.lengthSquared3() <= hkSimdReal_EpsSqrd )
	{
		m_onStepCnt = 0;
		return false;
	}

	// Simple onStep delay
	if(m_onStepCnt > 0)
	{
		--m_onStepCnt;
	}

	hkSimdReal threshold = hkSimdReal::fromFloat( hkvMath::cosRad ( hkvMath::Deg2Rad(character->Step_Threshold) ) );
	threshold.setClamped(threshold, hkSimdReal_0, hkSimdReal_1);

	// 1) Detect step candidates
	// 2) Find the closest non-penetrating contact manifold with a normal opposite to the velocity
	int index = -1;
	{
		hkSimdReal dist = hkSimdReal_Max;
		for(int i = 0; i < m_manifold.getSize(); ++i)
		{
			const hkpRootCdPoint& manifold = m_manifold[i];
			bool isCharacterA;
			{
				const hkpPhantom* test = hkpGetPhantom( manifold.m_rootCollidableA );
				isCharacterA = test == m_shapePhantom;
			}
			const hkpRigidBody* body = hkpGetRigidBody( isCharacterA ? manifold.m_rootCollidableB : manifold.m_rootCollidableA );
			const bool isDynamic = body && !body->isFixedOrKeyframed();
			// Consider only static collision objects
			if(!isDynamic)
			{
				const hkSimdReal vert = manifold.m_contact.getNormal().dot<3>(m_up);
				//const bool vertical = manifold.m_contact.getDistance() > 0 && vert >= hkSimdReal_0 && vert < threshold;
				const hkBool32 vertical = vert.isGreaterEqualZero() && vert.isLess(threshold);
				// Step candidate found
				if(vertical)
				{
					if( manifold.getContact().getDistanceSimdReal().isLess(dist) && manifold.getContact().getNormal().dot<3>(output.m_velocity).isLessZero() )
					{
						index = i;
						dist = manifold.getContact().getDistanceSimdReal();
					}
				}

				// Debug
				if(0)
				{
					hkvVec3 beg; vHavokConversionUtils::PhysVecToVisVecWorld(manifold.m_contact.getPosition(),beg);
					hkvVec3 end; vHavokConversionUtils::PhysVecToVisVecLocal(manifold.m_contact.getNormal(),end); end += beg;
					Vision::Game.DrawSingleLine(beg, end, VColorRef((vertical ? 255 : 0), (vertical ? 0 : 255), 0), 10.f);
					Vision::Game.DrawCube(beg, 5.f, VColorRef((vertical ? 255 : 50), (vertical ? 50 : 255), 50));
				}
			}
		}
	}
	// If no step candidate that met the requirements was found then we're done!
	if(index < 0)
	{
		return false;
	}

	const hkpRootCdPoint& cp = m_manifold[index];

	const hkTransform& trans = m_shapePhantom->getTransform();

	DEBUG_ADD_VISION_LINE(cp.getContact().getPosition(), cp.getContact().getNormal(), false, false);

	// We backup the current position as we need to put back the character to this place if the step candidate was unsuccessful.
	const hkVector4 currentPosition = trans.getTranslation();

	hkSimdReal slope = hkSimdReal::fromFloat( hkvMath::cosRad ( hkvMath::Deg2Rad(character->Max_Slope) ) ); slope.setClamped(slope,hkSimdReal_0,hkSimdReal_1);
	hkSimdReal stepHeight = hkSimdReal::fromFloat( character->Step_Height ); stepHeight.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());
	hkSimdReal radius = hkSimdReal::fromFloat( character->Capsule_Radius ); radius.mul(vHavokConversionUtils::GetVision2HavokScaleSIMD());

	// Extra contact point validations checks.
	{
		hkVector4 ground;
		vHavokConversionUtils::VisVecToPhysVecLocal(character->Character_Bottom, ground);
		ground.add(currentPosition);
		ground.subMul(m_up, radius);

		hkVector4 diff; diff.setSub(cp.getContact().getPosition(), ground);
		const hkSimdReal height = m_up.dot<3>(diff);

		// Slope validation
		{
			// Get the true surface normal from the contact point to determine if this is really a slope.
			{
				hkpWorldRayCastInput input;
				{
					// From is inside the capsule at the same hight as the contact point
					input.m_from.setAddMul(ground, m_up, height);
					// To is inside the hit body 5 % (of the distance) along the hit reverse normal.
					input.m_to.setSubMul(cp.getContact().getPosition(), cp.getContact().getNormal(), hkSimdReal::fromFloat(0.05f)*cp.getContact().getDistanceSimdReal());
				}

				hkpWorldRayCastOutput hit;
				vHavokPhysicsModule::GetInstance()->GetPhysicsWorld()->castRay(input, hit);
				if(hit.hasHit())
				{
					const hkSimdReal angle = hit.m_normal.dot<3>(m_up);
					// Detect if we are on a slope and do not need to reduce the velocity
					if(angle >= slope)
					{
						m_onStepCnt = 0;
						return false;
					}
				}
			}
		}

		// Step_Height validation test, we need to make sure we climb at maximum Step_Height.
		// Note: this step will often remove false positives such as walls, and thus speed up performance.
		{
			// If not climbable then adjust velocity to prevent the step climbing.
			if(height > stepHeight)
			{
				// Project cp normal to the plane perpendicular to the up direction.
				hkVector4 normal = cp.getContact().getNormal();
				normal.subMul(m_up, m_up.dot<3>(normal));
				normal.normalize<3>();

				DEBUG_ADD_VISION_LINE(ground, m_up, false, false);

				// Cancel velocity in the forward normal direction
				output.m_velocity.subMul(normal, output.m_velocity.dot<3>(normal));
				// Cancel velocity in the upward direction
				output.m_velocity.subMul(m_up, output.m_velocity.dot<3>(m_up));

				return false;
			}
		}
	}

	// Start position is lifted Step_Height upwards.
	hkVector4 from; from.setAddMul(currentPosition, m_up, stepHeight);
	// Forward sweep magnitude depends on velocity and timestep, and the predefined keepDistance controller parameter.
	hkSimdReal keepDistance = hkSimdReal::fromFloat( m_keepDistance );
	//horiHelp.setZero();
	hkVector4 forward; forward.setMul(input.m_forward, keepDistance + output.m_velocity.length<3>()*hkSimdReal::fromFloat(input.m_stepInfo.m_deltaTime));
	hkVector4 to; to.setAdd(from, forward);

	hkpLinearCastInput lci;
	{
		lci.m_startPointTolerance = m_keepDistance + m_keepContactTolerance;
		lci.m_maxExtraPenetration = 0.01f;
		lci.m_to = to;
	}

	// 1. FORWARD SWEEP TEST (this will eliminate all walls, etc.)
	// If something is hit during the forward sweet we cannot climb the step (or it might not be a step at all).
	{
		CharacterBestStepCollector hit;
		m_shapePhantom->setPositionAndLinearCast(from, lci, hit, HK_NULL);
		if(hit.hasHit())
		{
			m_shapePhantom->setPosition(currentPosition);
			DEBUG_ADD_VISION_CAPSULE(from, false);
			DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), false, false);
			return false;
		}
		DEBUG_ADD_VISION_LINE(from, to, false, true);
	}

	// 2. UPWARD SWEEP TEST (this is merely used to prevent escaping small shaft, etc.)
	// If something is hit during the upward sweet we cannot reach the step.
	{
		CharacterBestStepCollector hit;
		lci.m_to = from;
		m_shapePhantom->setPositionAndLinearCast(currentPosition, lci, hit, HK_NULL);
		if(hit.hasHit())
		{
			DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), false, false);
			return false;
		}
	}

	// 3. DOWNWARD SWEEP TEST
	hkVector4 newPos = currentPosition;
	bool handledStep = false;
	{
		// end position is lowered Step_Height downwards.
		from = to;
		to.setSubMul(from, m_up, stepHeight);
		lci.m_to = to;
		// The match point should be a below the actual step candidate to cater for new cp adjacent to the candidate.
		//hkVector4 match; match.setSubMul(cp.getContact().getPosition(), m_up, 0.1f*stepHeight);
		//CharacterBestStepCollector hit(match, m_up);
		CharacterBestStepCollector hit;
		m_shapePhantom->setPositionAndLinearCast(from, lci, hit, HK_NULL);

		DEBUG_ADD_VISION_LINE(from, to, false, true);

		if(hit.hasHit())
		{
			// New additional ray check to identify if the character is allowed to be on step using the true surface normal.
			bool horizontal = false;
			{
				hkpWorldRayCastInput input;
				{
					input.m_from.setAddMul(hit.getHitContact().getPosition(), m_up, keepDistance);
					input.m_to.setSubMul(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), keepDistance);
				}

				hkpWorldRayCastOutput slopeHit;
				vHavokPhysicsModule::GetInstance()->GetPhysicsWorld()->castRay(input, slopeHit);
				if(slopeHit.hasHit())
				{
					// Detect if we are on a slope and not a step (if contact normal is the same as the hit normal!)
					const hkSimdReal angle = slopeHit.m_normal.dot<3>(m_up);
					if(angle > slope)
					{
						horizontal = true;
					}
				}
				DEBUG_ADD_VISION_LINE2(input.m_from, input.m_to, horizontal, true);
			}

			if(0)	// old check
			{
				hkSimdReal hori = hkSimdReal_1 - hit.getHitContact().getNormal().dot<3>(m_up);
				horizontal = hit.getHitContact().getDistance() > 0 && hori >= hkSimdReal_0 && hori < threshold;
				DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), horizontal, false);
			}
			DEBUG_ADD_VISION_LINE(hit.getHitContact().getPosition(), hit.getHitContact().getNormal(), horizontal, false);

			const hkSimdReal s = hit.getHitContact().getDistanceSimdReal();
			hkVector4 tmpPos; tmpPos.setMul(from, hkSimdReal_1-s);
			tmpPos.addMul(to, s);

			if(horizontal)
			{
				newPos = tmpPos;
				// Correct for the previously added keepDistance
				newPos.subMul(input.m_forward, keepDistance);
				// The velocity is reduced according to the surface normal even though the projection does cover the entire position time integration.
				output.m_velocity.setZero();
				//output.m_velocity.mul(hori);
				// We force the character to be on a step for 10 updates, this value is chosen experimentally and is bound to change.
				m_onStepCnt = 10;
				handledStep = true;
			}
			else
			{
				DEBUG_ADD_VISION_CAPSULE(tmpPos, horizontal);
			}
		}
	}

	m_shapePhantom->setPosition(newPos);
	return handledStep;
}

void vHavokCharacterPushableProxy::processConstraintsCallback( const hkpCharacterProxy* proxy, const hkArray<hkpRootCdPoint>& manifold, hkpSimplexSolverInput& input )
{
	VASSERT( this == proxy );

	for( int i=0; i<manifold.getSize(); ++i )
	{
		hkpPhantom* otherPhantom = hkpGetPhantom( manifold[i].m_rootCollidableB );
		if( otherPhantom )
		{
      vHavokUserDataType_e eType = V_USERDATA_UNDEFINED;
      void *pObject = vHavokUserDataPointerPair_t::ExtractTypeAndPointer((void *)otherPhantom->getUserData(), eType);
      if (pObject && eType==V_USERDATA_OBJECT)
      {
        vHavokCharacterPushableProxy* otherCharacter = ((vHavokCharacterController*)pObject)->GetCharacterProxy();
			  if(otherCharacter)
			  {
				  const hkSimdReal strength = hkSimdReal::fromFloat(m_strength);
				  const hkSimdReal relativeStrength = strength / ( strength + hkSimdReal::fromFloat(otherCharacter->m_strength) );
				  input.m_constraints[i].m_velocity.addMul( relativeStrength, getLinearVelocity() );
				  input.m_constraints[i].m_velocity.addMul( (hkSimdReal_1 - relativeStrength), otherCharacter->getLinearVelocity() );
			  }
      }
		}
	}
}


// -------------------------------------------------------------------------- //
// Constructor / Destructor                                                   //
// -------------------------------------------------------------------------- //

vHavokCharacterController::vHavokCharacterController()
  : Capsule_Radius(0.5f * vHavokConversionUtils::GetHavok2VisionScale())
  , Character_Top(0.f, 0.f, (1.8f * vHavokConversionUtils::GetHavok2VisionScale()) - Capsule_Radius)
  , Character_Bottom(0.f, 0.f, Capsule_Radius)
  , Static_Friction(0.f)
  , Dynamic_Friction(0.8f)
  , Max_Slope(75.f)
  , Character_Mass(100.f)
  , Character_Strength(5000.f)
  , Gravity_Scale(1.f)
  , Jump_Height(1.5f)
  , Step_Height(30.f) // in cm
  , Step_Threshold(60.f) // in degrees
  , m_pCharacterProxy(NULL)
  , m_characterContext(NULL)
  , m_currentVelocity(0.f)
  , m_wantJump(false)
  , m_characterInput()
  , m_iCollisionFilter(hkpGroupFilter::calcFilterInfo(vHavokPhysicsModule::HK_LAYER_COLLIDABLE_CONTROLLER))
  , m_bEnabled(TRUE)
  , Debug(FALSE)
  , DebugColor(255, 50, 50, 255)
  , Fly_State(FALSE)
{
	m_characterInput.m_fly = false;
}

vHavokCharacterController::~vHavokCharacterController()
{
}

vHavokCharacterPushableProxy* vHavokCharacterController::GetCharacterProxy()
{
	return m_pCharacterProxy;
}

void vHavokCharacterController::SetEnabled(BOOL bEnabled)
{
  m_bEnabled = bEnabled;

  if (m_pCharacterProxy != NULL)
  {
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

    if (m_bEnabled && m_pCharacterProxy->getShapePhantom()->getWorld() == NULL)
    {
      pModule->AddCharacterController(this);

      // Set Position in case it has changed
      VisBaseEntity_cl* pOwnerEntity = static_cast<VisBaseEntity_cl*>(GetOwner());
      SetPosition(pOwnerEntity->GetPosition());
    }
    else if (!m_bEnabled && m_pCharacterProxy->getShapePhantom()->getWorld() != NULL)
    {
      pModule->RemoveCharacterController(this);
    }
  }
}

// -------------------------------------------------------------------------- //
// IVObjectComponent Virtual Overrides                                        //
// -------------------------------------------------------------------------- //

void vHavokCharacterController::SetOwner(VisTypedEngineObject_cl *pOwner)
{
	IVObjectComponent::SetOwner(pOwner);

	// Do not initialize the component in case our module is not active
	if (!vHavokPhysicsModule::GetInstance())
	{
		Vision::Error.Warning("Failed to initialize vHavokCharacterController since a non Havok physics module is currently active");
		return;
	}

	// Insert code here to respond to attaching this component to an object
	// This function is called with pOwner==NULL when de-attaching.
	if (pOwner!=NULL)
	{
		if (m_pCharacterProxy == NULL)
		{
			CreateHavokController();
			SetDebugRendering(Debug);
		}
		m_characterInput.m_fly = Fly_State;
	}
	else
	{ 
		DeleteHavokController();
	}
}

BOOL vHavokCharacterController::CanAttachToObject(VisTypedEngineObject_cl *pObject, VString &sErrorMsgOut)
{
	if (!IVObjectComponent::CanAttachToObject(pObject, sErrorMsgOut))
		return FALSE;

	// Define criteria here that allows the editor to attach this component to the passed object.
	// In our example, the object should be derived from VisObject3D_cl to be positionable.
	BOOL bIsValidClass = pObject->IsOfType(V_RUNTIME_CLASS(VisBaseEntity_cl));
	if (!bIsValidClass)
		sErrorMsgOut += "Component can only be added to instances of VisBaseEntity_cl or derived classes.";

	if ( !bIsValidClass)
		return FALSE;

  // vHavokCharacterController component can not be attached to objects, that already have a vHavokRigidBody component attached
  if (pObject->Components().GetComponentOfType<vHavokRigidBody>())
    return FALSE;

	return TRUE;
}

void vHavokCharacterController::OnVariableValueChanged(VisVariable_cl *pVar, const char * value)
{
	if (m_pCharacterProxy == NULL)
		return;

	if (!strcmp(pVar->name,"Gravity_Scale")) // this value is used directly and does not require the controller to be reinitialized
  {
		return;
  }
  else if(!strcmp(pVar->name, "m_bEnabled"))
  {
    SetEnabled(m_bEnabled);
		return;
  }

	// Recreate controller
	DeleteHavokController();
	CreateHavokController();

	// Check rendering
	if (m_pCharacterProxy)
		SetDebugRendering(Debug);
}


void vHavokCharacterController::MessageFunction( int iID, INT_PTR iParamA, INT_PTR iParamB )
{
	IVObjectComponent::MessageFunction(iID,iParamA,iParamB);

  if ( iID == VIS_MSG_EDITOR_PROPERTYCHANGED )
  {
    const char *szPropertyName = (const char *) iParamA;
    if (_stricmp(szPropertyName, "ModelFile") == 0)
    {
      // Recreate controller
      DeleteHavokController();
      CreateHavokController();

      // Check rendering
      if (m_pCharacterProxy)
        SetDebugRendering(Debug);

      return;
    }
    if (_stricmp(szPropertyName, "Scaling") == 0)
    {
      // update scaling
      UpdateBoundingVolume();
      return;
    }
  }
	else if ( iID == VIS_MSG_HAVOK_ONCOLLISION )
	{
		vHavokPhysicsModule::TriggerCollisionScriptFunction(this, (vHavokCollisionInfo_t *)iParamA);
		return;
	}
}


// -------------------------------------------------------------------------- //
// Serialization                                                              //
// -------------------------------------------------------------------------- //

// Register the class in the engine module so it is available for RTTI
V_IMPLEMENT_SERIAL(vHavokCharacterController,IVObjectComponent,0,&g_vHavokModule);

void vHavokCharacterController::Serialize(VArchive &ar)
{
	char iLocalVersion = VHAVOKCHARACTERCONTROLLER_VERSION_CURRENT;

	IVObjectComponent::Serialize(ar);
	if (ar.IsLoading())
	{
		char iFileVersion = 0;
		ar >> iFileVersion;
		VASSERT_MSG(iFileVersion <= iLocalVersion, "Invalid file version. Please re-export");

		// VHAVOKCHARACTERCONTROLLER_VERSION_0
		Character_Top.SerializeAsVec3 (ar);
        Character_Bottom.SerializeAsVec3 (ar);
		ar >> Capsule_Radius; 

		// VHAVOKCHARACTERCONTROLLER_VERSION_1
		if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_1)
		{
			ar >> Static_Friction;
			ar >> Dynamic_Friction;
			ar >> Max_Slope;
			ar >> Character_Mass;
			ar >> Character_Strength;
		}

		// VHAVOKCHARACTERCONTROLLER_VERSION_2
		if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_2)
		{
			ar >> Fly_State;
		}

		// VHAVOKCHARACTERCONTROLLER_VERSION_3
		if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_3)
		{
			ar >> Gravity_Scale;
		}

		// VHAVOKCHARACTERCONTROLLER_VERSION_4
		if ( iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_4 )
		{
			ar >> Jump_Height;
		}

		// VHAVOKCHARACTERCONTROLLER_VERSION_5
		if ( iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_5 )
		{
			ar >> Step_Height;
			ar >> Step_Threshold;
		}

    // VHAVOKCHARACTERCONTROLLER_VERSION_6
    if (iFileVersion >= VHAVOKCHARACTERCONTROLLER_VERSION_6)
    {
      ar >> m_bEnabled;
    }
	}
	else
	{
		ar << iLocalVersion;

		// VHAVOKCHARACTERCONTROLLER_VERSION_0
		Character_Top.SerializeAsVec3 (ar);
        Character_Bottom.SerializeAsVec3 (ar);
		ar << Capsule_Radius;

		// VHAVOKCHARACTERCONTROLLER_VERSION_1
		ar << Static_Friction;
		ar << Dynamic_Friction;
		ar << Max_Slope;
		ar << Character_Mass;
		ar << Character_Strength;

		// VHAVOKCHARACTERCONTROLLER_VERSION_2
		ar << Fly_State;

		// VHAVOKCHARACTERCONTROLLER_VERSION_3
		ar << Gravity_Scale;

		// VHAVOKCHARACTERCONTROLLER_VERSION_4
		ar << Jump_Height;

		// VHAVOKCHARACTERCONTROLLER_VERSION_5
		ar << Step_Height;
		ar << Step_Threshold;

    // VHAVOKCHARACTERCONTROLLER_VERSION_6
    ar << m_bEnabled;
	}
}


void vHavokCharacterController::OnDeserializationCallback(const VSerializationContext &context)
{
	if(!m_pCharacterProxy)
	{
		CreateHavokController();
		SetDebugRendering(Debug);
	}
}

// -------------------------------------------------------------------------- //
// Simulation Step                                                             //
// -------------------------------------------------------------------------- //

class CharacterPointCollector: public hkpAllCdPointCollector 
{ 
public:
	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_DEMO, CharacterPointCollector);

	CharacterPointCollector()
		: hkpAllCdPointCollector()
	{ }

	virtual void addCdPoint( const hkpCdPoint& event ) 
	{
		const hkpCollidable* rootCollidable = event.m_cdBodyB.getRootCollidable();
		hkpWorldObject* wo = hkpGetWorldObject(rootCollidable);
		if(m_objectsWeHit.indexOf(wo) == -1)
		{
			m_objectsWeHit.pushBack(wo);

			m_shapesWeHit.pushBack(event.m_cdBodyB.getShapeKey()); // support for reporting material type of the mesh we are on etc

			// Push back contact . N.B. The *distance* here is may be an actual (world ) distance,
			// or it may be a parameterization [0,1] of a linear cast, depending on what algorithm is using the collector.
			m_contacts.pushBack(event.getContact());
		}

		hkpAllCdPointCollector::addCdPoint(event); 
	}

	hkArray<hkpWorldObject*> m_objectsWeHit;
	hkArray<hkpShapeKey> m_shapesWeHit;
	hkArray<hkContactPoint> m_contacts;
}; 

void vHavokCharacterController::Step(float fTimeStep, int iNumSteps, float fDuration)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
	if ((!GetOwner()) || (!pModule) || (!pModule->GetPhysicsWorld()) || (!m_pCharacterProxy))
    return;

  VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *)GetOwner();
	hkpWorld* world = pModule->GetPhysicsWorld();

  // Rotation is not relevant for capsule so rotate just the entity
  if (pOwnerEntity->HasRotationDelta())
  {
    hkvVec3 vRot = pOwnerEntity->GetRotationDelta();  
    pOwnerEntity->IncOrientation(vRot);
    pOwnerEntity->ResetRotationDelta();
  }

  // We can not honor the motion delta as much as possible. Yet the Havok 
  // controller will have effects such as gravity in its previous velocity
  // too. So we really just want to take the planar part (XY) from the motion 
  // delta and the Z (gravity etc) from the previous Havok step. All use 
  // dependent etc. so feel free to change, this is a simple solution but 
  // will have artifacts.

  // Process delta
  const hkvVec3 vMotionDelta = pOwnerEntity->GetMotionDeltaWorldSpace() + 
    pOwnerEntity->GetRotationMatrix() * pOwnerEntity->GetMotionDeltaLocalSpace();

  hkvVec3 vMotionVelocity;
  vMotionVelocity.x = vMotionDelta.x / fDuration;
  vMotionVelocity.y = vMotionDelta.y / fDuration;
  vMotionVelocity.z = Fly_State ? (vMotionDelta.z / fDuration) : 0;

  m_pCharacterProxy->drawDebug(this);

  pOwnerEntity->ResetMotionDelta();

  if (iNumSteps < 1)
    return;

  world->lock();

  hkVector4 scaledGravity = world->getGravity();
  scaledGravity.mul(hkSimdReal::fromFloat(Gravity_Scale));

  // We backup the maxSlopeCosine parameter, as it will be reset if character is on a step
  const hkReal maxSlopeCosine = m_pCharacterProxy->m_maxSlopeCosine;

  CharacterPointCollector castContactsCollector;
  CharacterPointCollector startContactsCollector;
  for (int i=0; i<iNumSteps; i++)
  {
    // Apply the player character controller
    if( m_pCharacterProxy->isOnStep() )
    {
      // We set m_maxSlopeCosine to 0 which implicitly implies that the maximum slope that the character can climb is vertical.
      // This ultimately disables the additional vertical planes insertion, which is required when the character is resting on step edges.
      m_pCharacterProxy->m_maxSlopeCosine = 0.f;
    }

    // update the state
    {
      hkVector4 hVel;
      vHavokConversionUtils::VisVecToPhysVecLocal( vMotionVelocity, hVel );

      hkpCharacterInput input;
      hkpCharacterOutput output;

      hkVector4 down; down = world->getGravity(); down.normalize3();

      input.m_userData = hkUlong(&m_characterInput); //set additional characterinput (currently only fly_state)
      input.m_inputLR = hkReal(0);
      (-hVel.length<3>()).store<1>(&input.m_inputUD); // negative for always forward movement

      input.m_wantJump = m_wantJump;
      input.m_atLadder = false;

      input.m_up.setNeg4(down);	  

      // XXX, forward should not be zero
      if ( hkMath::equal(input.m_inputUD, 0.f) )
      {
        input.m_inputUD = hkReal(0);
        input.m_forward = hkVector4::getConstant<HK_QUADREAL_1000>();
      }
      else
      {
        input.m_forward = hVel;
        input.m_forward.normalize<3>();
      }

      input.m_stepInfo.m_deltaTime = fTimeStep;
      input.m_stepInfo.m_invDeltaTime = 1.0f / fTimeStep;
      input.m_characterGravity = scaledGravity;
      // For the fly state use the motiondelta as velocity, for non-fly state set the last velocity.
      input.m_velocity = Fly_State ? hVel : m_pCharacterProxy->getLinearVelocity();
      input.m_position = m_pCharacterProxy->getPosition();

      m_pCharacterProxy->checkSupport(down, input.m_surfaceInfo);

      m_characterContext->update(input, output);

      // Handle steps (can modify character position and output.m_velocity)
      if(Step_Height >= 0.f && Step_Threshold > 0.f && Step_Threshold <= 90.f)
      {
        m_pCharacterProxy->handleSteps(this, input, output);
      }

      m_pCharacterProxy->setLinearVelocity(output.m_velocity);
    }

    if( m_pCharacterProxy->isOnStep() )
    {
      // We set m_maxSlopeCosine to 0 which implicitly implies that the maximum slope that the character can climb is vertical.
      // This ultimately disables the additional vertical planes insertion, which is required when the character is resting on step edges.
      m_pCharacterProxy->m_maxSlopeCosine = hkReal(0);
    }

    hkStepInfo si( world->getCurrentTime(), world->getCurrentTime() + fTimeStep);
    m_pCharacterProxy->integrateWithCollectors( si, scaledGravity, castContactsCollector, startContactsCollector);

    // reset collectors
    castContactsCollector.reset();
    startContactsCollector.reset();
  }

  // Restore maxSlopeCosine
  m_pCharacterProxy->m_maxSlopeCosine = maxSlopeCosine;

  // May want to know some collision events (with the immediate hit objects, so just the start collector)
  for (int ci = 0; ci < startContactsCollector.m_contacts.getSize(); ++ci)
  {
    vHavokCollisionInfo_t info;

    // Fake some rb like contact info
    const hkContactPoint& contactPoint = startContactsCollector.m_contacts[ci];
    const hkpWorldObject* hitObject = startContactsCollector.m_objectsWeHit[ci];

    vHavokConversionUtils::PhysVecToVisVecWorld( contactPoint.getPosition(), info.m_vPoint );
    vHavokConversionUtils::PhysVecToVisVec_noscale( contactPoint.getNormal(), info.m_vNormal );
    info.m_fVelocity = 0; //Todo: compute this (See Physics/Dynamics/Constraint/Contact/hkpSimpleContactConstraintUtil)

    info.m_Collider[0].m_eType = V_USERDATA_OBJECT;
    info.m_Collider[0].m_pCharacter = this;

    info.m_Collider[1].SetInfo( (void *)(hitObject->getUserData()) ); // ptr back to the vHavokRigidBody, terrain etc
    switch (info.m_Collider[1].m_eType)
    {
    case V_USERDATA_OBJECT:
    case V_USERDATA_TERRAIN:
      {
        // Perform a check to be on the safe side, though the hitObject should never be a character here.
        if (!info.m_Collider[1].m_pCharacter)
        {
          hkpRigidBody *pRigidBody = hkpGetRigidBody( hitObject->getCollidable() ); 
          VASSERT(pRigidBody);
          info.m_Materials[1].fDynamicFriction = float(pRigidBody->getFriction());
          info.m_Materials[1].fRestitution = float(pRigidBody->getRestitution());
        }
      }
      break;

    case V_USERDATA_STATIC:
      {
        // Get the leaf shape key of the contact
        const hkpShapeKey& key = startContactsCollector.m_shapesWeHit[ci];

        // Get shape
        const hkpShape *pShape = info.m_Collider[1].m_pStaticMesh->GetHkShape();
        VASSERT(pShape);

        if (pShape->getClassType() == &hkvBvCompressedMeshShapeClass)
        {
          const hkvBvCompressedMeshShape *pMeshShape = (hkvBvCompressedMeshShape*)(pShape);
          const hkvMeshMaterialCache* matCache = (const hkvMeshMaterialCache*) ( pMeshShape->m_userData );
        if (matCache)
        {
          hkUint32 matId = pMeshShape->getPrimitiveUserData( key );
          VASSERT(matId <= (hkUint32) matCache->getSize());
            const hkvMeshMaterial& pMaterial = (*matCache)[matId];

          info.m_Materials[1].fDynamicFriction = pMaterial.m_fFriction;
          info.m_Materials[1].fRestitution = pMaterial.m_fRestitution;
            info.m_Materials[1].szUserData = pMaterial.m_userData.cString(); 

            break;
        }
        }
          
          hkpRigidBody *pRigidBody = info.m_Collider[1].m_pStaticMesh->GetHkRigidBody();
          info.m_Materials[1].fDynamicFriction = float(pRigidBody->getFriction());
          info.m_Materials[1].fRestitution = float(pRigidBody->getRestitution());
        }
      break;

    default:
      break;
    }

    info.m_pSenderTarget[0] = GetOwner();
    //Need to send to rb as well?
    if (info.m_Collider[1].m_pRigidBody && info.m_Collider[1].m_pRigidBody->GetOwner())
      info.m_pSenderTarget[1] = info.m_Collider[1].m_pRigidBody->GetOwner();
    vHavokPhysicsModule::GetInstance()->OnObjectCollision(info);
  }

  const hkVector4& hVel = m_pCharacterProxy->getLinearVelocity();
  vHavokConversionUtils::PhysVecToVisVecLocal( hVel, m_currentVelocity );

  world->unlock();

  m_wantJump = false;
}

void vHavokCharacterController::UpdateOwner()
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if ((!GetOwner()) || (!pModule) || (!pModule->GetPhysicsWorld()))
    return;

	VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *)GetOwner();

	// Rotation is not relevant for capsule so rotate just the entity
	if (pOwnerEntity->HasRotationDelta())
	{
		hkvVec3 vRot = pOwnerEntity->GetRotationDelta();  
		pOwnerEntity->IncOrientation(vRot);
		pOwnerEntity->ResetRotationDelta();
	}

	if (m_pCharacterProxy)
	{
		hkvVec3 pos;
		// Get the transformation from Havok
		pModule->MarkForRead();
		const hkVector4& hPos = m_pCharacterProxy->getPosition();
		vHavokConversionUtils::PhysVecToVisVecWorld( hPos, pos ); 
		pModule->UnmarkForRead();

		// Rotation is not relevant as capsule
		pOwnerEntity->SetPosition(pos);
	}
}

void vHavokCharacterController::GetCurrentLinearVelocity(hkvVec3& currentVelocity) const
{
	currentVelocity = m_currentVelocity;
}

hkvMat4 vHavokCharacterController::ApplyEntityScale(hkVector4 &vTop, hkVector4 &vBottom, float &fRadius)
{
  VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *) GetOwner();
  VASSERT(pOwnerEntity);

  hkvMat4 worldTransfrom;
  pOwnerEntity->GetWorldMatrix(worldTransfrom);
  hkvVec3 fTransformScale = worldTransfrom.getScalingFactors();
  hkVector4 vTransformScale; vHavokConversionUtils::VisVecToPhysVec_noscale(fTransformScale, vTransformScale);
 
  vTop.mul(vTransformScale);
  vBottom.mul(vTransformScale);
  fRadius = fRadius*fTransformScale[1];

  // Check bounds (top and bottom)
  hkVector4 vd; 
  vd.setSub(vTop, vBottom);
  if (vd.lengthSquared<3>() < hkSimdReal::fromFloat(HKVMATH_LARGE_EPSILON*HKVMATH_LARGE_EPSILON))
  {
	  vTop.setAddMul(vBottom, hkVector4::getConstant<HK_QUADREAL_0010>(), hkSimdReal::fromFloat(HKVMATH_LARGE_EPSILON));
    Vision::Error.Warning("The points which define the vHavokCharacterController capsule shape are equal - capsule is auto-adjusted!");
  }

  // Check Radius
  if (fRadius < HKVMATH_LARGE_EPSILON)
  {
    fRadius = HKVMATH_LARGE_EPSILON;
    Vision::Error.Warning("The capsule shape radius of the vHavokCharacterController is too small - capsule is auto-adjusted!");
  }

  return worldTransfrom;
}

void vHavokCharacterController::CreateStateMachineAndContext()
{
  hkpCharacterState* state;
  hkpCharacterStateManager* manager = new hkpCharacterStateManager();

  state = new vCharacterStateOnGround();
  ((vCharacterStateOnGround*)state)->setSpeed(1.0f);
  manager->registerState( state,	HK_CHARACTER_ON_GROUND);
  state->removeReference();

  state = new vCharacterStateInAir();
  ((vCharacterStateInAir*)state)->setSpeed(1.0f);
  manager->registerState( state,	HK_CHARACTER_IN_AIR);
  state->removeReference();

  state = new vCharacterStateJumping();
  ((vCharacterStateJumping*)state)->setJumpHeight(Jump_Height);
  manager->registerState( state,	HK_CHARACTER_JUMPING);
  state->removeReference();

  state = new vCharacterStateClimbing();
  manager->registerState( state,	HK_CHARACTER_CLIMBING);
  state->removeReference();

  state = new vCharacterStateFlying();
  manager->registerState( state,	HK_CHARACTER_FLYING);
  state->removeReference();

  m_characterContext = new hkpCharacterContext(manager, HK_CHARACTER_ON_GROUND);
  m_characterContext->setCharacterType( hkpCharacterContext::HK_CHARACTER_PROXY );
  manager->removeReference();
}

void vHavokCharacterController::CreateHavokController()
{
  // Do not initialize the component in case our module is not active
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  if (!pModule)
  {
    Vision::Error.Warning("Failed to initialize vHavokCharacterController since a non Havok physics module is currently active");
    return;
  }

	// allow reinit
  if ( m_pCharacterProxy) 
  {
    DeleteHavokController();
  }

  VisBaseEntity_cl *pOwnerEntity = (VisBaseEntity_cl *) GetOwner();
  VASSERT(pOwnerEntity != NULL);

	m_characterInput.m_fly = Fly_State;

	// The controller itself
	{
		// Construct a shape
		hkVector4 vTop; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Top, vTop);
		hkVector4 vBottom; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Bottom, vBottom);
    float fRadius = float(VIS2HK_FLOAT_SCALED(Capsule_Radius));
    
    hkvMat4 worldTransform = ApplyEntityScale(vTop, vBottom, fRadius);

    // remove scaling part of transformation matrix
    bool bMatrixValid = true;
    for (int i=0;i<3;i++)
    {
      const hkvVec3 vColumn = worldTransform.getColumn(i).getAsVec3();
      if (vColumn.isZero(HKVMATH_SMALL_EPSILON))
      {
        bMatrixValid = false;
        break;
      }
    }
    if (bMatrixValid)
      worldTransform.setScalingFactors (hkvVec3 (1));
    else
    {
      worldTransform.setIdentity();
      Vision::Error.Warning("vHavokCharacterController has invalid transformation - set to identity!");
    }

    // TODO:
    // Check Static_Friction, Dynamic_Friction, Max_Slope, Character_Mass, Character_Strength ?
    // -> Other Havok wrappers like vHavokRigidBody don't care about 'allowed' ranges (Havok clamps ?)

		// Create a capsule to represent the character standing
		hkpShape* standCapsule = new hkpCapsuleShape(vTop, vBottom, fRadius);

		// Construct a Shape Phantom
		hkTransform hkTfOut;
		vHavokConversionUtils::VisMatVecToPhysTransformWorld(worldTransform.getRotationalPart(), worldTransform.getTranslation(), hkTfOut);
		hkpShapePhantom* phantom = new hkpSimpleShapePhantom(standCapsule, hkTfOut, m_iCollisionFilter);
		standCapsule->removeReference();

		// Construct a character proxy
		hkpCharacterProxyCinfo cpci;
		vHavokConversionUtils::VisVecToPhysVecWorld( worldTransform.getTranslation(), cpci.m_position );
		vHavokConversionUtils::VisVecToPhysVec_noscale(worldTransform.getRotationalPart().getAxis(2), cpci.m_up);
		cpci.m_up.normalize<3>();

		// Controller properties
		cpci.m_staticFriction = Static_Friction;
		cpci.m_dynamicFriction = Dynamic_Friction;
		cpci.m_userPlanes = 4;
		cpci.m_keepDistance = .05f;
		cpci.m_maxSlope = hkvMath::Deg2Rad(Max_Slope);

		// This value will affect how much the character pushes down on objects it stands on. 
		cpci.m_characterMass = Character_Mass;

		// This value will affect how much the character is able to push other objects around.
		cpci.m_characterStrength = Character_Strength;

		cpci.m_shapePhantom = phantom;

		//cpci.m_penetrationRecoverySpeed = 0.1f;

		m_pCharacterProxy = new vHavokCharacterPushableProxy( cpci, this );
	}

	//
	// Create the Character state machine and context
	//
  CreateStateMachineAndContext();

  if (m_bEnabled)
	  pModule->AddCharacterController(this);
}

//-----------------------------------------------------------------------------------

void vHavokCharacterController::SetCollisionInfo(int iLayer, int iGroup,
	int iSubsystem, int iSubsystemDontCollideWith)
{
  int iFilter = hkpGroupFilter::calcFilterInfo(
    iLayer, iGroup, iSubsystem, iSubsystemDontCollideWith);

  SetCollisionInfo(iFilter);
}

void vHavokCharacterController::SetCollisionInfo(int iCollisionFilter)
{
	VVERIFY_OR_RET(m_pCharacterProxy);
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

  m_iCollisionFilter = iCollisionFilter;

  pModule->MarkForWrite();

  m_pCharacterProxy->getShapePhantom()->getCollidableRw()->setCollisionFilterInfo(iCollisionFilter);

  hkpWorld* pWorld = pModule->GetPhysicsWorld();
  if (pWorld != NULL)
  {
    pWorld->updateCollisionFilterOnPhantom(m_pCharacterProxy->getShapePhantom(), 
      HK_UPDATE_COLLECTION_FILTER_IGNORE_SHAPE_COLLECTIONS);
  }

  pModule->UnmarkForWrite();
}

//-----------------------------------------------------------------------------------

void vHavokCharacterController::SetPosition(const hkvVec3 &x)
{
	hkVector4 vec; vHavokConversionUtils::VisVecToPhysVecWorld(x,vec);

	if (m_pCharacterProxy)
	{
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

		pModule->MarkForWrite();
		m_pCharacterProxy->setPosition(vec);
		pModule->UnmarkForWrite();
	}
}

hkvVec3 vHavokCharacterController::GetPosition() const
{
	if (m_pCharacterProxy)
	{
		hkvVec3 v; 
        vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
        VASSERT(pModule != NULL);

		pModule->MarkForRead();
		const hkVector4& vec = m_pCharacterProxy->getPosition();
		vHavokConversionUtils::PhysVecToVisVecWorld(vec,v);
		pModule->UnmarkForRead();
		return v;
	}
	return hkvVec3::ZeroVector();
}

bool vHavokCharacterController::IsStanding() const
{
  return CheckSupport(hkvVec3(0,0,-1));
}

bool vHavokCharacterController::CheckSupport(const hkvVec3& v) const
{
	hkVector4 dir;
	vHavokConversionUtils::VisVecToPhysVec_noscale(v,dir);
	hkpSurfaceInfo pSurfaceInfo;

	if (m_pCharacterProxy)
	{
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);
		hkpWorld* world = pModule->GetPhysicsWorld();
		if (world)  world->lock();
		m_pCharacterProxy->checkSupport(dir, pSurfaceInfo);
		if (world) world->unlock();

		return (pSurfaceInfo.m_supportedState != hkpSurfaceInfo::UNSUPPORTED);
	}
	else
	{
		return false;
	}
}


void vHavokCharacterController::SetWantJump( bool wantJump )
{
	m_wantJump = wantJump;
}


void vHavokCharacterController::UpdateBoundingVolume()
{
	hkVector4 vTop; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Top, vTop);
	hkVector4 vBottom; vHavokConversionUtils::VisVecToPhysVecLocal(Character_Bottom, vBottom);
  float fRadius = float(VIS2HK_FLOAT_SCALED(Capsule_Radius)); 

  ApplyEntityScale(vTop, vBottom, fRadius);

	if (m_pCharacterProxy && m_pCharacterProxy->getShapePhantom())
	{
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

		hkpShapePhantom* phantom = m_pCharacterProxy->getShapePhantom();
		hkpShape* capsule = new hkpCapsuleShape(vTop, vBottom, fRadius);

		pModule->MarkForWrite();
		phantom->setShape(capsule);
    pModule->UnmarkForWrite();

		capsule->removeReference();
	}
}

bool vHavokCharacterController::TryUpdateBoundingVolume(const hkvVec3 &capTop, const hkvVec3 &capBottom)
{
	hkVector4 vTop; vHavokConversionUtils::VisVecToPhysVecLocal(capTop, vTop);
	hkVector4 vBottom; vHavokConversionUtils::VisVecToPhysVecLocal(capBottom, vBottom);
  float fRadius = float(VIS2HK_FLOAT_SCALED(Capsule_Radius));

  ApplyEntityScale(vTop, vBottom, fRadius);

	if (m_pCharacterProxy && m_pCharacterProxy->getShapePhantom())
	{
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

		hkpShapePhantom* phantom = m_pCharacterProxy->getShapePhantom();
		hkpShape* capsule = new hkpCapsuleShape(vTop, vBottom, fRadius);

		pModule->MarkForWrite();

		phantom->setShape(capsule);

		struct Collector : hkpCdBodyPairCollector
		{
			virtual void addCdBodyPair(const hkpCdBody& bodyA, const hkpCdBody& bodyB)
			{
				m_earlyOut = true;
				success = false;
			}
			bool success;
		} coll;
		coll.success = true;

		phantom->getPenetrations(coll);

		if (coll.success)
		{
			Character_Top = capTop;
			Character_Bottom = capBottom;
		}
		else
		{
			vHavokConversionUtils::VisVecToPhysVecLocal(Character_Top, vTop);
			vHavokConversionUtils::VisVecToPhysVecLocal(Character_Bottom, vBottom);

      ApplyEntityScale(vTop, vBottom, fRadius);

			capsule->removeReference();
			capsule = new hkpCapsuleShape(vTop, vBottom, fRadius);
			phantom->setShape(capsule);
		}

		pModule->UnmarkForWrite();

		capsule->removeReference();

		return coll.success;
	}
	return false;
}

void vHavokCharacterController::DeleteHavokController()
{
	if (m_pCharacterProxy != NULL)
	{
    vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
    VASSERT(pModule != NULL);

    // Remove from world.
    if (m_pCharacterProxy->getShapePhantom()->getWorld() != NULL)
      pModule->RemoveCharacterController(this);

		pModule->MarkForWrite();

		m_pCharacterProxy->getShapePhantom()->removeReference();

		m_pCharacterProxy->removeReference();
		m_pCharacterProxy	= HK_NULL;

		pModule->UnmarkForWrite();
	}

  if (m_characterContext != NULL)
  {
    m_characterContext->removeReference();
    m_characterContext	= HK_NULL;
  }
}


// -------------------------------------------------------------------------- //
// Debug Rendering                                                            //
// -------------------------------------------------------------------------- //

void vHavokCharacterController::SetDebugRendering(BOOL bEnable)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

	Debug = bEnable;
  const bool bEnableDebug = Debug || (pModule->m_bDebugRenderCharacterControllers);

	// Get ID (cast from collidable pointer as its is used for display geometry ID)
	pModule->MarkForRead();
	hkUlong id = (hkUlong)m_pCharacterProxy->getShapePhantom()->getCollidable();
	pModule->UnmarkForRead();

	// Ensure debug display handler is created
	if (bEnableDebug)
		pModule->SetEnabledDebug(true);

	// Set display properties
	vHavokDisplayHandler* pDisplay = pModule->GetHavokDisplayHandler();
	if (!pDisplay)
		return;

	// Set debug color and visibility state
	pDisplay->SetVisible(id, bEnableDebug);
	pDisplay->SetColor(id, DebugColor);

	// In Editor attach the Havok Debug Shape to the owner object
	if (bEnableDebug)
		pDisplay->SetOwner(id, (VisObject3D_cl*)GetOwner());
}

void vHavokCharacterController::SetDebugColor(VColorRef color)
{
  vHavokPhysicsModule* pModule = vHavokPhysicsModule::GetInstance();
  VASSERT(pModule != NULL);

	DebugColor = color;

	// Get ID (cast from collidable pointer as its is used for display geometry ID)
  pModule->MarkForRead();
	hkUlong id = (hkUlong)m_pCharacterProxy->getShapePhantom()->getCollidable();
  pModule->UnmarkForRead();

	vHavokDisplayHandler* pDisplay = pModule->GetHavokDisplayHandler();
	if (!pDisplay)
		return;

	// Set debug color
	pDisplay->SetColor(id, DebugColor);
}


void vHavokCharacterController::SetFlyState(bool bIsFlying)
{
	Fly_State = bIsFlying;
	m_characterInput.m_fly = Fly_State;
}

// -------------------------------------------------------------------------- //
// Variable Attributes                                                        //
// -------------------------------------------------------------------------- //

#ifdef WIN32

void vHavokCharacterController::GetVariableAttributes(VisVariable_cl *pVariable, VVariableAttributeInfo &destInfo)
{
	// Check whether we need to display the debug color property
	if (!strcmp(pVariable->GetName(),"DebugColor"))
	{
		// Get debug display state
		BOOL b;
		GetVariable("Debug")->GetValueDirect(this, (void*) &b, true);

		if (b == false)
			destInfo.m_bHidden = true;
	}
}

#endif


START_VAR_TABLE(vHavokCharacterController,IVObjectComponent,"Havok Character Controller component. Can be attached to entities to enable physics processing of character control.", VVARIABLELIST_FLAGS_NONE, "Havok Character Controller" )
  DEFINE_VAR_BOOL_AND_NAME(vHavokCharacterController, m_bEnabled, "Enabled", "Enable or disable component", "True", 0, 0);
  DEFINE_VAR_VECTOR_FLOAT(vHavokCharacterController, Character_Top, "Top of character in local entity space.", "0/0/150", 0, 0); 
	DEFINE_VAR_VECTOR_FLOAT(vHavokCharacterController, Character_Bottom, "Bottom of character in local entity space.", "0/0/30", 0, 0); 
	DEFINE_VAR_FLOAT(vHavokCharacterController, Capsule_Radius, "Radius of the character in local entity space.", "30.0", 0, 0); 
	DEFINE_VAR_BOOL(vHavokCharacterController, Debug, "Enables/Disables Physics Debug Rendering.", "FALSE", 0, 0);
	DEFINE_VAR_COLORREF(vHavokCharacterController, DebugColor, "Color of this RB when Debug Rendering is active.", "255,50,50,255", 0, NULL);
	DEFINE_VAR_FLOAT(vHavokCharacterController, Static_Friction, "Default static friction for surfaces hit.", "0.0", 0, 0);
	DEFINE_VAR_FLOAT(vHavokCharacterController, Dynamic_Friction, "Default dynamic friction for surfaces hit.", "0.8", 0, 0);
	DEFINE_VAR_FLOAT(vHavokCharacterController, Max_Slope, "Maximum slope that the character can walk up (In degrees, Max: 90 deg.).", "75", 0, "Clamp(0, 90)");
	DEFINE_VAR_FLOAT(vHavokCharacterController, Character_Mass, "Mass of the character, standing on things.", "100", 0, 0);
	DEFINE_VAR_FLOAT(vHavokCharacterController, Character_Strength, "Maximum constant force that the character controller can impart onto moving objects.", "5000", 0, 0);
	DEFINE_VAR_BOOL(vHavokCharacterController, Fly_State, "Whether the character should fly. If set to true the gravity will be ignored and also the Z value of the motion delta is applied", "FALSE", 0, 0);
	DEFINE_VAR_FLOAT(vHavokCharacterController, Gravity_Scale, "Scalar factor to scale the global gravity strength. Use 0.0 for no gravity. Default is 1.0 (normal gravity)", "1.0", 0, 0); 
	DEFINE_VAR_FLOAT(vHavokCharacterController, Jump_Height, "Jump Height of the character.", "1.5", 0, 0); 
	DEFINE_VAR_FLOAT(vHavokCharacterController, Step_Height, "Maximum step height that the character can climb.", "30.0", 0, 0);
	DEFINE_VAR_FLOAT(vHavokCharacterController, Step_Threshold, "Minimum slope threshold for when to identify steps (In degrees, Max: 90 deg.).", "60.0", 0, "Clamp(0, 90)");
END_VAR_TABLE

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
