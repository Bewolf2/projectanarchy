/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef VBASE_HKVMATH_HKVEULERUTILMODE_H
#define VBASE_HKVMATH_HKVEULERUTILMODE_H

#define EulerOrder(I,P,R,F) (F) | (R << 1) | (P << 2) | (I << 3)

/// \brief
///   This is a helper class to convert Euler angles to and from matrices and quaternions.
///   It works with every mode known to man.
///   The code was taken from "Graphics Gems IV", "Euler Angle Conversion" by Ken Shoemake, page 222.
class hkvEulerMode
{
public:
  /// \brief [internal]
  enum Frame      { Static = 0, Rotating = 1 };
  /// \brief [internal]
  enum Repetition { RepNo  = 0, RepYes   = 1 };
  /// \brief [internal]
  enum Parity     { Even   = 0, Odd      = 1 };
  /// \brief [internal]
  enum InnerAxis  { AxisX  = 0, AxisY    = 1, AxisZ  = 2 };

  /// \brief
  ///   Defines all the different modes that euler angles could be used like.
  ///
  /// 's' means 'static', ie. rotate around the GLOBAL axis.
  /// 'r' means 'rotating', ie. rotate around the LOCAL axis.
  ///
  /// For example XYZs means:
  /// rotate around the global Z axis, then
  /// rotate around the global Y axis, then
  /// rotate around the global X axis
  ///
  /// The Vision Engine uses 'XYZs' as its default mode (which is the same 3D Studio Max uses).
  /// Other classes like 'hkvQuat' use Euler Angles as 'Roll, Pitch, Yaw'. Since XYZs is then used,
  /// this translates to 'Roll' == 'X Axis', Pitch == 'Y Axis', 'Yaw' == 'Z Axis'
  enum Enum
  {
    XYZs = EulerOrder (AxisX, Even, RepNo , Static),    ZYXr = EulerOrder (AxisX, Even, RepNo , Rotating),
    XYXs = EulerOrder (AxisX, Even, RepYes, Static),    XYXr = EulerOrder (AxisX, Even, RepYes, Rotating),
    XZYs = EulerOrder (AxisX,  Odd, RepNo , Static),    YZXr = EulerOrder (AxisX,  Odd, RepNo , Rotating),
    XZXs = EulerOrder (AxisX,  Odd, RepYes, Static),    XZXr = EulerOrder (AxisX,  Odd, RepYes, Rotating),
    YZXs = EulerOrder (AxisY, Even, RepNo , Static),    XZYr = EulerOrder (AxisY, Even, RepNo , Rotating),
    YZYs = EulerOrder (AxisY, Even, RepYes, Static),    YZYr = EulerOrder (AxisY, Even, RepYes, Rotating),
    YXZs = EulerOrder (AxisY,  Odd, RepNo , Static),    ZXYr = EulerOrder (AxisY,  Odd, RepNo , Rotating),
    YXYs = EulerOrder (AxisY,  Odd, RepYes, Static),    YXYr = EulerOrder (AxisY,  Odd, RepYes, Rotating),
    ZXYs = EulerOrder (AxisZ, Even, RepNo , Static),    YXZr = EulerOrder (AxisZ, Even, RepNo , Rotating),
    ZXZs = EulerOrder (AxisZ, Even, RepYes, Static),    ZXZr = EulerOrder (AxisZ, Even, RepYes, Rotating),
    ZYXs = EulerOrder (AxisZ,  Odd, RepNo , Static),    XYZr = EulerOrder (AxisZ,  Odd, RepNo , Rotating),
    ZYZs = EulerOrder (AxisZ,  Odd, RepYes, Static),    ZYZr = EulerOrder (AxisZ,  Odd, RepYes, Rotating),
    VisionDefault = XYZs,
  };
};

#endif

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
