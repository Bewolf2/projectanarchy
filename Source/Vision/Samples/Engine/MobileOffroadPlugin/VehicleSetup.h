/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef HK_VEHICLE_SETUP_H
#define HK_VEHICLE_SETUP_H

#include <Physics2012/Vehicle/hkpVehicleInstance.h>

#include <Physics2012/Vehicle/AeroDynamics/Default/hkpVehicleDefaultAerodynamics.h>
#include <Physics2012/Vehicle/DriverInput/Default/hkpVehicleDefaultAnalogDriverInput.h>
#include <Physics2012/Vehicle/Brake/Default/hkpVehicleDefaultBrake.h>
#include <Physics2012/Vehicle/Engine/Default/hkpVehicleDefaultEngine.h>
#include <Physics2012/Vehicle/VelocityDamper/Default/hkpVehicleDefaultVelocityDamper.h>
#include <Physics2012/Vehicle/Steering/Default/hkpVehicleDefaultSteering.h>
#include <Physics2012/Vehicle/Suspension/Default/hkpVehicleDefaultSuspension.h>
#include <Physics2012/Vehicle/Transmission/Default/hkpVehicleDefaultTransmission.h>
#include <Physics2012/Vehicle/WheelCollide/RayCast/hkpVehicleRayCastWheelCollide.h>
#include <Physics2012/Vehicle/TyreMarks/hkpTyremarksInfo.h>

/// This class is a sample setup for a cruiser vehicle.
class VehicleSetup
{
public:

  VehicleSetup() :
    m_up(hkVector4(0, 0, 1)),
    m_forward(hkVector4(1, 0, 0)),
    m_right(hkVector4(0, -1, 0))
    {}

  VehicleSetup(hkVector4& up, hkVector4& forward, hkVector4& right) :
    m_up(up),
    m_forward(forward),
    m_right(right)
    {}

  void buildVehicle( const hkpWorld* world, hkpVehicleInstance& vehicle ); 

public:
  void setupVehicleData( const hkpWorld* world, hkpVehicleData& data );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultSuspension& suspension );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultSteering& steering );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultAnalogDriverInput& driverInput );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultEngine& engine );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultTransmission& transmission );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultBrake& brake );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultAerodynamics& aerodynamics );
  void setupComponent( const hkpVehicleData& data, hkpVehicleDefaultVelocityDamper& velocityDamper );
  void setupWheelCollide( const hkpWorld* world, const hkpVehicleInstance& vehicle, hkpVehicleRayCastWheelCollide& wheelCollide );
  void setupTyremarks( const hkpVehicleData& data, hkpTyremarksInfo& tyremarkscontroller );

private:
  hkReal m_wheelRadius;
  hkVector4 m_up;
  hkVector4 m_forward;
  hkVector4 m_right;
};

#endif // HK_VEHICLE_SETUP_H

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
