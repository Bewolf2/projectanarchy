/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file vSimpleRenderState.hpp

#ifndef VSIMPLERENDERSTATE_HPP_INCLUDED
#define VSIMPLERENDERSTATE_HPP_INCLUDED

// flags for simple render state flags
#define RENDERSTATEFLAG_NONE                0x00000000
#define RENDERSTATEFLAG_FRONTFACE           0x00000001
#define RENDERSTATEFLAG_BACKFACE            0x00000002
#define RENDERSTATEFLAG_DOUBLESIDED         (RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_BACKFACE)
#define RENDERSTATEFLAG_ALWAYSVISIBLE       0x00000004
#define RENDERSTATEFLAG_WRITETOZBUFFER      0x00000008
#define RENDERSTATEFLAG_ALPHATEST           0x00000010
#define RENDERSTATEFLAG_USEFOG              0x00000020
#define RENDERSTATEFLAG_LUMINANCETOALPHA    0x00000040
#define RENDERSTATEFLAG_FILTERING           0x00000080
#define RENDERSTATEFLAG_USEADDITIVEALPHA    0x00000100
#define RENDERSTATEFLAG_SAMPLERCLAMPING     0x00000200
#define RENDERSTATEFLAG_NOWIREFRAME         0x00000400
#define RENDERSTATEFLAG_USESCISSORTEST      0x00000800
#define RENDERSTATEFLAG_NOMULTISAMPLING     0x00001000
#define RENDERSTATEFLAG_CUSTOMBIT           0x00008000

// helper flag combination that can be used when passed to IVRenderInterface::DrawLine
#define RENDERSTATEFLAG_LINE2D           (RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_ALWAYSVISIBLE|RENDERSTATEFLAG_CUSTOMBIT)

#define SIMPLERENDERSTATE_UNDEFINED   -1


/// \brief
///   A structure that defines a simple render state packed into a 32bit value
/// 
/// Only a small subset of render states is supported by this structure (e.g. alpha blending, backface culling, depth test/depth write,
/// texture clamping, scissoring).
/// 
/// This simple render state is typically used for rendering less complex geometry batches such as
/// 2D overlays.
struct VSimpleRenderState_t
{
public:

  /// \brief
  ///   Empty constructor. Sets defaults
  inline VSimpleRenderState_t() {iAllProperties=0;iRenderFlags=RENDERSTATEFLAG_FRONTFACE|RENDERSTATEFLAG_FILTERING|RENDERSTATEFLAG_USEADDITIVEALPHA;}

  /// \brief
  ///   Constructor that takes the transparency type and state flags.
  /// 
  /// \param iTransp
  ///   One of the blending modes defined in VIS_TransparencyType.
  /// 
  /// \param iFlags
  ///   bitflags; the following flags are supported:
  ///   \li RENDERSTATEFLAG_FRONTFACE: The front faces are rendered (default).
  /// 
  ///   \li RENDERSTATEFLAG_BACKFACE: The back faces are rendered.
  /// 
  ///   \li RENDERSTATEFLAG_DOUBLESIDED: doublesided; combination of RENDERSTATEFLAG_FRONTFACE and
  ///     RENDERSTATEFLAG_BACKFACE bit.
  /// 
  ///   \li RENDERSTATEFLAG_ALWAYSVISIBLE: No z-test is performed while rendering.
  /// 
  ///   \li RENDERSTATEFLAG_WRITETOZBUFFER: Rasterizer writes to z-buffer.
  /// 
  ///   \li RENDERSTATEFLAG_ALPHATEST: Alpha test is performed using the global alpha threshold
  ///     value. Only supported on platforms supporting the alpha test state (Windows DirectX9, Playstation 3, Xbox360).
  /// 
  ///   \li RENDERSTATEFLAG_LUMINANCETOALPHA: Used for fonts that need to copy the luminance value
  ///     into the alpha channel.
  /// 
  ///   \li RENDERSTATEFLAG_FILTERING: Bilinear filtering is enabled on the used sampler(s) (otherwise just point
  ///     sampling).
  /// 
  ///   \li RENDERSTATEFLAG_USEADDITIVEALPHA: Determines whether additive blending multiplies the
  ///     source color with source alpha.
  /// 
  ///   \li RENDERSTATEFLAG_SAMPLERCLAMPING: If enabled, used sampler(s) use clamping
  /// 
  ///   \li RENDERSTATEFLAG_NOWIREFRAME: Ignores the global wireframe mode (e.g. fonts should be
  ///     displayed correctly also in wireframe mode).
  /// 
  ///   \li RENDERSTATEFLAG_USESCISSORTEST: Enable scissor test.
  /// 
  ///   \li RENDERSTATEFLAG_CUSTOMBIT: For custom usage (ignored by Vision).
  inline VSimpleRenderState_t(int iTransp, int iFlags=RENDERSTATEFLAG_FRONTFACE) {iAllProperties=0;iTranspMode=iTransp;iRenderFlags=iFlags;}

  /// \brief
  ///   Invalidates the simple render state to -1
  inline void Invalidate()
  {
    iAllProperties = -1;
  }

  /// \brief
  ///   Sets the blending mode, must be one of the values in VIS_TransparencyType.
  inline void SetTransparency(int iTransp)
  {
    iTranspMode=iTransp;
  }

  /// \brief
  ///   Returns the blending mode, must be one of the values in VIS_TransparencyType.
  inline int GetTransparency() const
  {
    return iTranspMode;
  }

  /// \brief
  ///   Translates the VCullMode value to the face culling bitflags and sets them in the simple render state object.
  inline void SetFaceCullMode(VCullMode iMode)
  {
    SetFlag(RENDERSTATEFLAG_DOUBLESIDED);
    if (iMode == CULL_FRONT) RemoveFlag(RENDERSTATEFLAG_FRONTFACE);
    if (iMode == CULL_BACK) RemoveFlag(RENDERSTATEFLAG_BACKFACE);
  }

  /// \brief
  ///   Translates the face culling bitflags into a VCullMode enum value and returns the result.
  inline VCullMode GetFaceCullMode() const
  {
    if (!IsFlagSet(RENDERSTATEFLAG_FRONTFACE)) return CULL_FRONT;
    if (!IsFlagSet(RENDERSTATEFLAG_BACKFACE)) return CULL_BACK;
    return CULL_NONE;
  }

  /// \brief
  ///   Sets all bitflags (see constructor for supported bitflags)
  inline void SetFlags(int iMask) {iRenderFlags=iMask;}

  /// \brief
  ///   Adds a specific bitflag (see constructor for supported bitflags)
  inline void SetFlag(int iMask)    {iRenderFlags|=iMask;}

  /// \brief
  ///   Removes a specific bitflag (see constructor for supported bitflags)
  inline void RemoveFlag(int iMask) {iRenderFlags&=(~iMask);}

  /// \brief
  ///   Gets the bitflag mask (see constructor for supported bitflags)
  inline int  GetFlags() const {return iRenderFlags;}

  /// \brief
  ///   Tests whether a specific flag is set (see constructor for supported bitflags)
  inline bool IsFlagSet(int iMask) const {return (iRenderFlags&iMask)==iMask;}

  /// \brief
  ///   Casts into a 32 bit integer
  inline operator int &() {return iAllProperties;}

  /// \brief
  ///   Assignment operator
  inline int &operator = (int other)  {iAllProperties=other;return iAllProperties;}

  /// \brief
  ///   Assignment operator
  inline int &operator = (const VSimpleRenderState_t &other)  {iAllProperties=other.iAllProperties;return iAllProperties;}

  /// \brief
  ///   Comparison operator
  inline bool operator == (const VSimpleRenderState_t &other) const {return iAllProperties==other.iAllProperties;}

  /// \brief
  ///   Comparison operator
  inline bool operator != (const VSimpleRenderState_t &other) const {return iAllProperties!=other.iAllProperties;}

  // data:
  union
  {
    struct
    {
      char iTranspMode;
      char unused;
      unsigned short iRenderFlags;
    };
    int iAllProperties;
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
