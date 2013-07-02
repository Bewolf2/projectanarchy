/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_AI_STATIC_OBSTACLE_SOLVER_H
#define HK_AI_STATIC_OBSTACLE_SOLVER_H

#include <Ai/Pathfinding/Character/LocalSteering/hkaiAvoidanceSolver.h>

	/// The hkaiStaticObstacleSystem (SOS) class stores static velocity obstacles representation as a set of angle pairs and local coordinate system.
struct hkaiStaticObstacleSystem 
{
	// +version(4)

	HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, hkaiStaticObstacleSystem );

	// The constructor sets local system
	hkaiStaticObstacleSystem(const hkVector4& velocity, const hkVector4& up);

		/// Holds edge's type and angle in local system
	struct VelocityObstacleEdge
	{
		HK_DECLARE_NONVIRTUAL_CLASS_ALLOCATOR( HK_MEMORY_CLASS_AI_STEERING, VelocityObstacleEdge );
		
		enum EdgeType
		{
			OPEN = 0,
			CLOSE
		};

		hkReal m_angle;	 

		EdgeType m_type;		
	};

	// This function merges all velocity obstacles.
	void merge( hkArray<VelocityObstacleEdge>& edges );

	// This function finds closest edge to desired combination of dir and system forward
	bool findClosestEdge( const hkVector4& dir, const hkVector4& up, hkArray<VelocityObstacleEdge>& edges );

	// This utility functions returns idx of open edge
	static int HK_CALL isInsideAnyVO(const hkVector4& vec, const hkaiStaticObstacleSystem& sos, const hkArray<VelocityObstacleEdge>& edges );
	
	// The utility function for sorting
	inline static hkBool32 sideLessThan(const VelocityObstacleEdge& voeA, const VelocityObstacleEdge& voeB);

	/// The forward in SOS coordinate system
	hkVector4 m_forward;

	/// The left in SOS coordinate system
	hkVector4 m_left;

	/// The index of the best edge
	HK_PAD_ON_SPU(int) m_closestEdgeIdx;

	/// The number of VO cut by pi,-pi
	HK_PAD_ON_SPU(int)m_numCutVO;

	/// The narrow corridor multiple (0-1 narrow corridor, 1 boundary on one side, -1 no boundaries)
	HK_PAD_ON_SPU(hkReal) m_narrowMult;

	/// The value is true for merged system
	HK_PAD_ON_SPU(bool) m_merged;

};

	/// Local steering utility to handle static obstacles.
class hkaiStaticObstacleSolver
{
public:

	///
	static void HK_CALL solve( const hkaiAvoidanceSolver::WorldInfo& worldInfo,
							   const hkaiAvoidanceSolver::SteeringAgent& agent,
							   hkaiAvoidanceSolver::ControlGradient& gradient );

	///
	static void HK_CALL calcGradient( const hkaiAvoidanceSolver::SteeringAgent& agent,
								      const hkaiStaticObstacleSystem& sos,
									  const hkVector4& up,
									  const hkReal minCollisionTime,
									  hkaiAvoidanceSolver::ControlGradient& gradient,
									  const hkArray<hkaiStaticObstacleSystem::VelocityObstacleEdge>& edges );	

	/// This method process all nearby agents and adds velocity obstacle edges
	static void HK_CALL processNearbyAgents( const hkaiAvoidanceSolver::WorldInfo& worldInfo,
								  		  	 const hkaiAvoidanceSolver::SteeringAgent& agents, 
											 hkaiStaticObstacleSystem& sos, 
											 hkReal& minCollisionTime,
											 hkaiAvoidanceSolver::ControlGradient& gradient,
											 hkArray<hkaiStaticObstacleSystem::VelocityObstacleEdge>& edges );

	/// This method process all nearby sphere obstacles and returns maximal gradient of the velocity change
	static void HK_CALL processNearbyObstacles( const hkaiAvoidanceSolver::WorldInfo& worldInfo,
									 		    const hkaiAvoidanceSolver::SteeringAgent& agent, 
												hkaiStaticObstacleSystem& sos, 
												hkReal& minCollisionTime,
												hkaiAvoidanceSolver::ControlGradient& gradient,
												hkArray<hkaiStaticObstacleSystem::VelocityObstacleEdge>& edges ); 

	/// This method process all nearby boundaries and returns maximal gradient of the velocity change
	static void HK_CALL processNearbyBoundaries( const hkaiAvoidanceSolver::WorldInfo& worldInfo,
												 const hkaiAvoidanceSolver::SteeringAgent& agent, 
												 hkaiStaticObstacleSystem& sos,
												 hkReal& minCollisionTime,
												 hkaiAvoidanceSolver::ControlGradient& gradient,
												 hkArray<hkaiStaticObstacleSystem::VelocityObstacleEdge>& edges );
	
	/// This method detects possible narrow corridors near agent (0 - very narrow , 1 - wide, -1 - no boundaries)
	static hkReal HK_CALL detectNarrowCorridor( const hkVector4& up,
												const hkaiAvoidanceSolver::SteeringAgent& agent );

	///
	static void HK_CALL addSphereVelocityObstacleEdges( const hkReal radius,
														const hkVector4& agentToObstacle, 
														const hkVector4& up, 
														hkaiStaticObstacleSystem& sos,
														hkArray<hkaiStaticObstacleSystem::VelocityObstacleEdge>& edges );

	///
	static void HK_CALL addBoundaryVelocityObstacleEdges( bool insideFrontArea,
														  const hkReal radius,
														  const hkVector4& agentToStart,
														  const hkVector4& agentToEnd,
														  const hkVector4& up,				
														  hkaiStaticObstacleSystem& sos,
														  hkArray<hkaiStaticObstacleSystem::VelocityObstacleEdge>& edges );
};

#endif // HK_AI_STATIC_OBSTACLE_SOLVER_H

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
