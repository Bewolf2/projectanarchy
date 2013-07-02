// ===========================================================
// Havok Physics Lua Module - SWIG Interface
// ===========================================================
// This interface file generates PhysicsLuaModule_wrapper.cpp
// and is represented in Lua as the "Physics" module.
 
#ifndef VLUA_APIDOC
 
//get access to all vision specific things...
%include <windows.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/vision_types.i>

//dependencies of ObjectComponent:
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VColor.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VTypedObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiTypedEngineObject.i>
%include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VisApiObjectComponent.i>

//custom headers for generated source file
%module Physics
%{
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokPhysicsModule.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConstraint.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokConversionUtils.hpp>
  
  #include <Vision/Runtime/EnginePlugins/VisionEnginePlugin/Scripting/Lua/VBitmask.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokCharacterController.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRigidBody.hpp>
  #include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/vHavokRagdoll.hpp>
  
  #define MOTIONTYPE_DYNAMIC hkpMotion::MOTION_DYNAMIC
  #define MOTIONTYPE_KEYFRAMED hkpMotion::MOTION_KEYFRAMED
  #define MOTIONTYPE_FIXED hkpMotion::MOTION_FIXED
  #define MOTIONTYPE_SPHERE_INERTIA hkpMotion::MOTION_SPHERE_INERTIA
  #define MOTIONTYPE_BOX_INERTIA hkpMotion::MOTION_BOX_INERTIA
  #define MOTIONTYPE_THIN_BOX_INERTIA hkpMotion::MOTION_THIN_BOX_INERTIA
  #define MOTIONTYPE_CHARACTER hkpMotion::MOTION_CHARACTER

  #define QUALITY_FIXED HK_COLLIDABLE_QUALITY_FIXED
  #define QUALITY_KEYFRAMED HK_COLLIDABLE_QUALITY_KEYFRAMED
  #define QUALITY_DEBRIS HK_COLLIDABLE_QUALITY_DEBRIS
  #define QUALITY_DEBRIS_SIMPLE_TOI HK_COLLIDABLE_QUALITY_DEBRIS_SIMPLE_TOI
  #define QUALITY_MOVING HK_COLLIDABLE_QUALITY_MOVING
  #define QUALITY_CRITICAL HK_COLLIDABLE_QUALITY_CRITICAL
  #define QUALITY_BULLET HK_COLLIDABLE_QUALITY_BULLET
  #define QUALITY_CHARACTER HK_COLLIDABLE_QUALITY_CHARACTER
  #define QUALITY_KEYFRAMED_REPORTING HK_COLLIDABLE_QUALITY_KEYFRAMED_REPORTING

%}

%immutable;

//motion types
static const int MOTIONTYPE_DYNAMIC;
static const int MOTIONTYPE_KEYFRAMED;
static const int MOTIONTYPE_FIXED;
static const int MOTIONTYPE_SPHERE_INERTIA;
static const int MOTIONTYPE_BOX_INERTIA;
static const int MOTIONTYPE_THIN_BOX_INERTIA;
static const int MOTIONTYPE_CHARACTER;


//quality types
static const int QUALITY_FIXED;
static const int QUALITY_KEYFRAMED;
static const int QUALITY_DEBRIS;
static const int QUALITY_DEBRIS_SIMPLE_TOI;
static const int QUALITY_MOVING;
static const int QUALITY_CRITICAL;
static const int QUALITY_BULLET;
static const int QUALITY_CHARACTER;
static const int QUALITY_KEYFRAMED_REPORTING;

%mutable;
  
  
%include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/Lua/vHavokCharacterController.i>;
%include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/Lua/vHavokRigidBody.i>;
%include <Vision/Runtime/EnginePlugins/Havok/HavokPhysicsEnginePlugin/Lua/vHavokRagdoll.i>;

/*
%native(EnableDebugRendering) int PhysicsLuaModule_EnableDebugRendering(lua_State *L);
%{
  int PhysicsLuaModule_EnableDebugRendering(lua_State *L) {
    if(lua_isboolean(L,-1))
    {
      bool bEnable = lua_toboolean(L,-1)==TRUE;
      lua_pop(L,1);
      if(Vision::GetApplication()->GetPhysicsModule())
        ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->SetEnabledDebug(bEnable);
    } 
    return 0;
  }
%}
*/

%native(Cast_vRigidBody_to_IVObjectComponent) int PhysicsLuaModule_Cast_vRigidBody_to_IVObjectComponent(lua_State *L);
%{
	int PhysicsLuaModule_Cast_vRigidBody_to_IVObjectComponent(lua_State *L)
	{
		int SWIG_arg = 0;
		vHavokRigidBody *arg1 = (vHavokRigidBody *) 0 ;
		IVObjectComponent *result = 0 ;
  
		SWIG_check_num_args("Cast_vRigidBody_to_IVObjectComponent",1,1)
		if ( !SWIG_isptrtype(L, 1) )
		{
			SWIG_fail_arg("Cast_vRigidBody_to_IVObjectComponent", 1, "vHavokRigidBody *");
		}
  
		if ( !SWIG_IsOK(SWIG_ConvertPtr(L,1,(void**)&arg1, SWIGTYPE_p_vHavokRigidBody,0)))
		{
			SWIG_fail_ptr("Cast_vRigidBody_to_IVObjectComponent", 1, SWIGTYPE_p_vHavokRigidBody);
		}
  
		result = (IVObjectComponent*)(arg1);
		SWIG_NewPointerObj(L, result, SWIGTYPE_p_IVObjectComponent, 0);
		SWIG_arg++; 
		return SWIG_arg;
		
		if ( 0 )
		{
			SWIG_fail;
		}
  
	fail:
		lua_error(L);
		return SWIG_arg;
	}
%}

%native(EnableVisualDebugger) int PhysicsLuaModule_EnableVisualDebugger(lua_State *L);
%{
  int PhysicsLuaModule_EnableVisualDebugger(lua_State *L) {
    if(lua_isboolean(L,-1))
    {
      bool bEnable = lua_toboolean(L,-1)==TRUE;
      lua_pop(L,1);
      if(Vision::GetApplication()->GetPhysicsModule())
        ((vHavokPhysicsModule*)Vision::GetApplication()->GetPhysicsModule())->SetEnabledVisualDebugger(bEnable);
    } 
    return 0;
  }
%}

%native(GetHavokToVisionScale) float PhysicsLuaModule_GetHavokToVisionScale(lua_State *L);
%{
  int PhysicsLuaModule_GetHavokToVisionScale(lua_State *L) 
  {
	float scale = float(vHavokPhysicsModule::GetInstance()->GetVisionWorldScale());
	lua_pushnumber(L, (lua_Number) scale);
	return 1;
  }
%}

%native(SetHavokToVisionScale) float PhysicsLuaModule_SetHavokToVisionScale(lua_State *L);
%{
  int PhysicsLuaModule_SetHavokToVisionScale(lua_State *L) 
  {
    if(lua_isnumber(L,-1))
	{
		  float scale = lua_tonumber(L,-1);
		  lua_pop(L,1);
		  vHavokPhysicsModule::GetInstance()->SetVisionWorldScale(scale);
	}
	return 0;
  }
%}

%native(SetGravity) float PhysicsLuaModule_SetGravity(lua_State *L);
%{
  int PhysicsLuaModule_SetGravity(lua_State *L) 
  {
	DECLARE_ARGS_OK
	GET_ARG(1, hkvVec3, gravity);
	vHavokPhysicsModule::GetInstance()->SetGravity( gravity );
	return 0;
  }
%}

%native(SetGroupsCollision) int PhysicsLuaModule_SetGroupsCollision(lua_State *L);
%{
  int PhysicsLuaModule_SetGroupsCollision(lua_State *L) 
  {
	DECLARE_ARGS_OK
	
	// Get arguments
	GET_ARG(1, int, group1);
	GET_ARG(2, int, group2);
	GET_ARG(3, bool, doEnable);
	
	vHavokPhysicsModule::GetInstance()->SetGroupsCollision(group1, group2, doEnable);
	return 0;
  }
%}

%native(RayCast) int PhysicsLuaModule_RayCast(lua_State *L);
%{
	int PhysicsLuaModule_RayCast(lua_State *L)
	{
		DECLARE_ARGS_OK;
	
		// Get arguments	
		GET_ARG(1, hkvVec3, origin);
		GET_ARG(2, hkvVec3, direction);
		
		// Get world
		hkpWorld* world = vHavokPhysicsModule::GetInstance()->GetPhysicsWorld();
		
		// Lock world
		world->lock();

		hkVector4 ray;
		vHavokConversionUtils::VisVecToPhysVec_noscale(direction, ray);
		ray.mul( hkSimdReal::fromFloat(500) );

		// Setup raycast
		hkpWorldRayCastInput input;
		vHavokConversionUtils::VisVecToPhysVecWorld( origin, input.m_from );
		input.m_to.setAdd( input.m_from, ray );
		input.m_enableShapeCollectionFilter = true;
		input.m_filterInfo = 0xffffffff;

		// Cast ray
		hkpClosestRayHitCollector output;
		world->castRay( input, output );

		// Unlock world
		world->unlock();

		// Check hit
		if ( !output.hasHit() )
		{
			lua_pushboolean(L,false);
			return 1;
		}

		// Get hit point
		const hkpWorldRayCastOutput& hit = output.getHit();
		hkVector4 hitPoint; hitPoint.setAddMul( input.m_from, ray, hit.m_hitFraction );

		// Get Vision hit point
    hkvVec3 visionHitPoint;
    vHavokConversionUtils::PhysVecToVisVecWorld( hitPoint, visionHitPoint );

		// Results
		lua_pushboolean(L,true);
		
		hkvVec3* resultPtr = new hkvVec3( (const hkvVec3&)visionHitPoint );
		SWIG_NewPointerObj( L, (void*)resultPtr, SWIGTYPE_p_hkvVec3, 1 );		
		
		return 2;
	}
%}

#else

/// \brief MODULE: The Lua \b Physics module contains all generated wrapper classes of the vHavokEnginePlugin.
/// \par Example
///   \code
///     function OnCreate(self)
///       self:AddComponentOfType("vHavokRigidBody", "RigidBody")
///       self.RigidBody:SetMass(65)
///       self.RigidBody:SetDebugRendering(true)
///     
///       Physics.EnableVisualDebugger(true)
///     end
///   \endcode
class Physics {
public:

  /// @name Static Module Functions
  /// @{
  
  /// \brief Setup possibility to attach the Havok visual debugger.
  /// \param enable Enable or disabe the visual debugger.
  /// \par Example
  ///   \code
  ///     function OnAfterSceneLoaded(self)
  ///       Physics.EnableVisualDebugger(true)
  ///     end
  ///   \endcode
  static void EnableVisualDebugger(boolean enable);
 
  /// note: temporary removed
  /// !brief Hide or show debug wireframe rendering.
  /// !param enable Enable or disabe debug rendering inside the viewport.
  /// !par Example
  ///   !code
  ///     ...
  ///     Physics.EnableDebugRendering(true)
  ///     ...
  ///   !endcode
  /// static void EnableDebugRendering(boolean enable);

  /// \brief Set the internal scaling from Havok to Vision.
  /// \param scale value to convert from Havok units to Vision units
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneLoaded(self)
  ///       Physics.SetHavokToVisionScale( 0.01 )
  ///     end
  ///   \endcode
  static void SetHavokToVisionScale(float scale = 1.0f / 50.0f);

  /// \brief Gets the internal scaling from Havok to Vision.
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneLoaded(self)
  ///       scale = Physics.GetHavokToVisionScale()
  ///     end
  ///   \endcode
  static float GetHavokToVisionScale();
  
  /// \brief Sets gravity
  /// \param gravity Gravity in Vision units
  /// \par Example
  ///   \code
  ///     function OnBeforeSceneLoaded(self)
  ///       Physics.SetGravity(0,0,-1000)
  ///     end
  ///   \endcode
  static void SetGravity( hkvVec3 gravity );
  
  /// \brief Enables / disables collisions between the given groups
  static void SetGroupsCollision(int group1, int group2, bool doEnable);
      
  /// @}
  /// @name Motion Type Constants
  /// @{
  
  /// \brief A fully-simulated, movable rigid body.
  static const int MOTIONTYPE_DYNAMIC;
  
  /// \brief Simulation is not performed as a normal rigid body, can be moved manually by code.
  static const int MOTIONTYPE_KEYFRAMED;

  /// \brief This motion type is used for the static elements of a game scene.
  static const int MOTIONTYPE_FIXED;

  /// \brief Simulation is performed using a sphere inertia tensor.
  static const int MOTIONTYPE_SPHERE_INERTIA;

  /// \brief Simulation is performed using a box inertia tensor.
  static const int MOTIONTYPE_BOX_INERTIA;

  /// \brief A box inertia motion which is optimized for thin boxes and has less stability problems.
  static const int MOTIONTYPE_THIN_BOX_INERTIA;
  
  /// \brief A specialized motion used for character controllers.
  static const int MOTIONTYPE_CHARACTER;
  
  /// @}
  /// @name Quality Constants
  /// @{
  
	/// \brief Use this for fixed bodies.
	static const number QUALITY_FIXED;

	/// \brief Use this for moving objects with infinite mass.
	static const number QUALITY_KEYFRAMED;

	/// \brief Use this for all your debris objects.
	static const number QUALITY_DEBRIS;

	/// \brief Use this for debris objects that should have simplified TOI collisions with fixed/landscape objects.
	static const number QUALITY_DEBRIS_SIMPLE_TOI;

	/// \brief Use this for moving bodies, which should not leave the world,
	/// but you rather prefer those objects to tunnel through the world than
	/// dropping frames because the engine.
	static const number QUALITY_MOVING;

	/// \brief Use this for all objects, which you cannot afford to tunnel through
	/// the world at all.
	static const number QUALITY_CRITICAL;

	/// \brief Use this for very fast objects.
	static const number QUALITY_BULLET;

  /// \brief Use this for rigid body character controllers.
	static const number QUALITY_CHARACTER;

	/// \brief Use this for moving objects with infinite mass which should report
	/// contact points and TOI-collisions against all other bodies, including other
	/// fixed and keyframed bodies.
  /// \note Note that only non-TOI contact points are reported in collisions against debris-quality objects.
	const number QUALITY_KEYFRAMED_REPORTING;
  
  /// @}
};

#endif
