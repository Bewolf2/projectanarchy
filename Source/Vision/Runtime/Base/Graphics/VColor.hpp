/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VColor.hpp

#ifndef _VISION_VCOLOR_HPP
#define _VISION_VCOLOR_HPP

#define V_DIV255 (1.0f/255.0f)

/// \brief
///   Structure that represents a 32bit RGBA color
/// 
/// Each color component is represented by a single unsigned byte [0..255].
/// When using this class, DO NOT assume that the components will be laid out in memory
/// in any particular order, since it will vary across platforms.
/// 
/// This structure also provides some functionality to convert the color value
/// to 32bit integer values and floats.
struct VColorRef
{
  V_DECLARE_SERIALX( VColorRef, VBASE_IMPEXP )
public:

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor, sets the color to black
  inline VColorRef() {_colorValue=0;}

  /// \brief
  ///   Constructor, sets the color to red, green, blue and alpha value
  inline VColorRef(UBYTE red,UBYTE green,UBYTE blue,UBYTE alpha=255) {r=red; g=green; b=blue; a=alpha;}

  /// \brief
  ///   This constructor version creates an uninitialized value
  explicit inline VColorRef(bool bUninitialized) {}

  /// \brief
  ///   Serializes the color components taking byte order into account
  VBASE_IMPEXP void SerializeX( VArchive &ar );


  ///
  /// @}
  ///

  ///
  /// @name Set Color Functions
  /// @{
  ///

  /// \brief
  ///   Sets the color to black
  inline void Clear() {_colorValue=0;}

  /// \brief
  ///   Sets the color portions without modifying the alpha value
  inline void SetNoAlpha(VColorRef color) {r=color.r; g=color.g; b=color.b;}


  ///
  /// @}
  ///

  ///
  /// @name Conversion Functions
  /// @{
  ///

  /// \brief
  ///   Converts the RGB values into a 0..1 based float vector
  inline void ToFloat(hkvVec3& vDest) const {RGB_To_Float(*this,vDest);}

  /// \brief
  ///   Converts the RGB values into a 0..1 based float vector
  inline hkvVec3 ToFloat() const {hkvVec3 vColor(hkvNoInitialization); RGB_To_Float(*this, vColor); return vColor;}

  /// \brief
  ///   Converts the RGB values into a 0..1 based float vector
  inline void ToFloat4(hkvVec4& vDest) const {RGBA_To_Float(*this,vDest);}

  /// \brief
  ///   Converts the RGB values into a 0..1 based float vector
  inline const hkvVec4 ToFloat4() const {hkvVec4 vColor(hkvNoInitialization); RGBA_To_Float(*this, vColor); return vColor;}

  /// \brief
  ///   Gets the RGB values from a 0..1 based float vector. Clamps if necessary.
  inline void FromFloat(const hkvVec3& vColor) {*this = Float_To_RGB(vColor);}

  /// \brief
  ///   Gets the RGB values from a 0..1 based float vector. Clamps if necessary.
  inline void FromFloat4(const hkvVec4& vColor) {*this = Float_To_RGBA(vColor);}

  /// \brief
  ///   Sets all 4 color components (without clamping)
  inline void SetRGBA(UINT red, UINT green, UINT blue, UINT alpha) {r=red; g=green; b=blue; a=alpha;}

  /// \brief
  ///   Sets 3 color components (without clamping) and leaves alpha unmodified
  inline void SetRGB(UINT red, UINT green, UINT blue){r=red; g=green; b=blue;}

  /// \brief
  ///   Converts color to a 32bit integer value. This value can be set via SetRGBA
  inline UINT GetRGBA() const { return (UINT) ((UINT)((UINT)r) + (((UINT)((UINT)g)) << 8) + (((UINT)((UINT)b))<<16) + (((UINT)((UINT)a))<<24)); }

  /// \brief
  ///   Retrieves color from a 32bit integer value. The passed value is compatible with the return
  ///   value of GetRGBA. It is also compatible across platforms
  inline void SetRGBA(UINT rgba) { r = ((rgba)&0xff); g = ((rgba>>8)&0xff); b = ((rgba>>16)&0xff); a = ((rgba>>24)&0xff); }

  /// \brief
  ///   Returns a color value as a DWORD. Note that the byte order in this DWORD may differ depending on platform endianness.
  inline DWORD GetNative() const {return _colorValue;}

  /// \brief
  ///   Returns the color value as a four-component float vector
  inline hkvVec4 getAsVec4() const { hkvVec4 retVal; RGBA_To_Float(*this, retVal); return retVal; }

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Assigns another color
  inline VColorRef& operator = (const VColorRef& other)  {_colorValue=other._colorValue; return *this;}

  /// \brief
  ///   Assigns another color
  inline VColorRef& operator = (const hkvVec3& other)  {*this = Float_To_RGB(other); return *this;}

  /// \brief
  ///   Checks whether two colors are identical
  inline bool operator == (const VColorRef& other) const {return _colorValue==other._colorValue;}

  /// \brief
  ///   Checks whether two colors are not identical
  inline bool operator != (const VColorRef& other) const {return _colorValue!=other._colorValue;}

  /// \brief
  ///   Adds two color values. Clamps the components appropriately.
  inline void operator += (const VColorRef& other)
  {
    int m;
    m = (int)r+(int)other.r; if (m>255) m=255; r=(UBYTE)m;
    m = (int)g+(int)other.g; if (m>255) m=255; g=(UBYTE)m;
    m = (int)b+(int)other.b; if (m>255) m=255; b=(UBYTE)m;
    m = (int)a+(int)other.a; if (m>255) m=255; a=(UBYTE)m;
  }

  /// \brief
  ///   Subtracts two color values. Clamps the components appropriately.
  inline void operator -= (const VColorRef& other)
  {
    int m;
    m = (int)r-(int)other.r; if (m<0) m=0; r=(UBYTE)m;
    m = (int)g-(int)other.g; if (m<0) m=0; g=(UBYTE)m;
    m = (int)b-(int)other.b; if (m<0) m=0; b=(UBYTE)m;
    m = (int)a-(int)other.a; if (m<0) m=0; a=(UBYTE)m;
  }

  /// \brief
  ///   Multiplies two color values
  inline void operator *= (const VColorRef& other)
  {
    int m;
    m = (int)r*(int)other.r; r=(UBYTE)(m>>8);
    m = (int)g*(int)other.g; g=(UBYTE)(m>>8);
    m = (int)b*(int)other.b; b=(UBYTE)(m>>8);
    m = (int)a*(int)other.a; a=(UBYTE)(m>>8);
  }

  /// \brief
  ///   Multiplies a color value with a float scalar factor. Clamps the components appropriately.
  inline void operator *= (float factor)
  {
    int m;
    if (factor<0.f) factor=0.f;
    m = (int)((float)r*factor); if (m>255) m=255; r=(UBYTE)m;
    m = (int)((float)g*factor); if (m>255) m=255; g=(UBYTE)m;
    m = (int)((float)b*factor); if (m>255) m=255; b=(UBYTE)m;
    m = (int)((float)a*factor); if (m>255) m=255; a=(UBYTE)m;
  }

  /// \brief
  ///   Adds two color values. Clamps the components appropriately.
  inline VColorRef operator+ (const VColorRef& other) const {VColorRef col=*this;col+=other;return col;}

  /// \brief
  ///   Subtracts two color values. Clamps the components appropriately.
  inline VColorRef operator- (const VColorRef& other) const {VColorRef col=*this;col-=other;return col;}

  /// \brief
  ///   Multiplies two color values
  inline VColorRef operator* (const VColorRef& other) const {VColorRef col=*this;col*=other;return col;}

  /// \brief
  ///   Multiplies the color value with a float scalar factor
  inline VColorRef operator* (float factor) const {VColorRef col=*this;col*=factor;return col;}

  ///
  /// @}
  ///

  ///
  /// @name Misc
  /// @{
  ///

  /// \brief
  ///   Returns whether the color is (0,0,0,0)
  inline bool IsZero() const {return _colorValue==0;}

  /// \brief
  ///   Returns whether the color is (0,0,0,X)
  inline bool IsBlack() const {return ((r==0)&&(g==0)&&(b==0));}

  /// \brief
  ///   Returns the color's intensity (arithmetic mean of red/green/blue values). The range is a float value [0..1]
  inline float GetIntensity() const {return ((float)r+(float)g+(float)b) * (1.f/765.f);}

#ifndef SPU

  /// \brief
  ///   Set the color to a random value
  inline void SetRandomColor() {SetRGBA(rand()%255,rand()%255,rand()%255,rand()%255);}
#endif

  /// \brief
  ///   Linearly interpolates between two colors. If fFactor is zero, the result is iCol1
  inline void Lerp(const VColorRef &iCol1, const VColorRef &iCol2, float fFactor)
  {
    if (fFactor<0.f)
      fFactor = 0.f; 
    else
      if (fFactor>1.f) fFactor=1.f;
    float f2 = 1.f-fFactor;
    r = (UBYTE)((float)iCol1.r*f2+(float)iCol2.r*fFactor);
    g = (UBYTE)((float)iCol1.g*f2+(float)iCol2.g*fFactor);
    b = (UBYTE)((float)iCol1.b*f2+(float)iCol2.b*fFactor);
    a = (UBYTE)((float)iCol1.a*f2+(float)iCol2.a*fFactor);
  }

  /// \brief
  ///   Static conversion function to convert float-based RGB components to VColorRef
  inline static VColorRef Float_To_RGB(const hkvVec3& vColor)
  {
    VColorRef dest;
    int r = (int)(vColor.x*255.99f);if (r>255) dest.r=255; else dest.r=r;
    int g = (int)(vColor.y*255.99f);if (g>255) dest.g=255; else dest.g=g;
    int b = (int)(vColor.z*255.99f);if (b>255) dest.b=255; else dest.b=b;
    return dest;
  }

  /// \brief
  ///   Static conversion function to convert float-based RGBA components to VColorRef
  inline static VColorRef Float_To_RGBA(const hkvVec4& vColor)
  {
    VColorRef dest;
    int r = (int)(vColor.r*255.99f);if (r>255) dest.r=255; else dest.r=r;
    int g = (int)(vColor.g*255.99f);if (g>255) dest.g=255; else dest.g=g;
    int b = (int)(vColor.b*255.99f);if (b>255) dest.b=255; else dest.b=b;
    int a = (int)(vColor.a*255.99f);if (a>255) dest.a=255; else dest.a=a;
    return dest;
  }

  /// \brief
  ///   Static conversion function to convert VColorRef to float-based RGB components
  inline static void RGB_To_Float(VColorRef color, hkvVec3& vDest)
  {
    vDest.set ((float)color.r * V_DIV255, (float)color.g * V_DIV255, (float)color.b * V_DIV255);
  }

  /// \brief
  ///   Static conversion function to convert VColorRef to float-based RGBA components
  inline static void RGBA_To_Float(VColorRef color, hkvVec4& vDest)
  {
    vDest.set ((float)color.r * V_DIV255, (float)color.g * V_DIV255, (float)color.b * V_DIV255, (float)color.a * V_DIV255);
  }

  /// \brief
  ///   Color as union (r|g|b|a or _colorValue as DWORD)
  union
  {
#if (defined(WIN32) && defined(_VR_DX11)) || defined(_VISION_IOS) || defined(_VISION_ANDROID) || defined (_VISION_PS3) || defined (_VISION_POSIX) || defined (_VISION_PSP2) || defined(_VISION_WIIU)
    struct { UBYTE r,g,b,a; };
#elif defined (WIN32)
    struct { UBYTE b,g,r,a; };
#elif defined (_VISION_XENON)
    struct { UBYTE a,r,g,b; };
#else
  #error Undefined platform!
#endif

    DWORD _colorValue;
  };

  /// \brief The platform dependent swizzle mask as a string that can be used to map VColorRef => RGBA. This is NOT the component order.
  static const char* SwizzleMask;

  ///
  /// @}
  ///
};

const VColorRef  V_RGBA_WHITE  = VColorRef(255,255,255,255);
const VColorRef  V_RGBA_GREY   = VColorRef(127,127,127,255);
const VColorRef  V_RGBA_BLACK  = VColorRef(  0,  0,  0,255);

const VColorRef  V_RGBA_RED    = VColorRef(255,  0,  0,255);
const VColorRef  V_RGBA_YELLOW = VColorRef(255,255,  0,255);
const VColorRef  V_RGBA_GREEN  = VColorRef(  0,255,  0,255);
const VColorRef  V_RGBA_CYAN   = VColorRef(  0,255,255,255);
const VColorRef  V_RGBA_BLUE   = VColorRef(  0,  0,255,255);
const VColorRef  V_RGBA_PURPLE = VColorRef(255,  0,255,255);


/// \brief
///   Structure that represents a 32bit RGBE color.
/// 
/// VColorExpRef is a convenient class to store color values with extended range at a comparably low memory footprint. The 
/// alpha value is used as a shared exponent for all 3 color components. This assumes there are no big contrasts between the components
/// and if so the small values can be neglected as they do not contribute to the color perception.
/// This color structure does not support an alpha component. 
/// 
/// This structure also provides functionality to convert the color value to and from floating point colors.
struct VColorExpRef
{
  ///
  /// @name Constructors
  /// @{
  ///

  /// \brief
  ///   Empty constructor (black color)
  inline VColorExpRef() {_colorValue=0;e=127;}

  /// \brief
  ///   Constructor that takes a floating point vector as input
  inline VColorExpRef(const hkvVec3& vColor) {FromFloat(vColor);}
  /// \brief
  ///   Constructor that initializes the components and the exponent
  inline VColorExpRef(UBYTE red,UBYTE green,UBYTE blue,signed char exponent=0) {r=red;g=green;b=blue;e=exponent+127;}

  /// \brief
  ///   Copy constructor. Interprets the alpha channel of VColorRef as exponent(!)
  inline VColorExpRef(const VColorRef &color){r=color.r; g=color.g; b=color.b; e=color.a;}

  ///
  /// @}
  ///

  ///
  /// @name Static conversion functions
  /// @{
  ///

  /// \brief
  ///   Static conversion function to convert to floating point vector
  VBASE_IMPEXP static hkvVec3 RGBE_To_Float(VColorExpRef iColor);

  /// \brief
  ///   Static conversion function to convert to floating point vector (uses SetRGBE first)
  VBASE_IMPEXP static hkvVec3 RGBE_To_Float(unsigned int rgbe);

  /// \brief
  ///   Static conversion function to convert to floating point vector
  VBASE_IMPEXP static void RGBE_To_Float(VColorExpRef iColor, hkvVec3& vDestColor);

  /// \brief
  ///   Static conversion function to convert to floating point vector (uses SetRGBE first)
  VBASE_IMPEXP static void RGBE_To_Float(unsigned int rgbe, hkvVec3& vDestColor);

  /// \brief
  ///   Static conversion function to convert from floating point vector to RGBE format
  VBASE_IMPEXP static VColorExpRef Float_To_RGBE(const hkvVec3& vColor);

  /// \brief
  ///   Static conversion function to convert from floating point vector to VColorRef (uses clamping)
  VBASE_IMPEXP static VColorRef Float_To_RGB(const hkvVec3& vColor);

  /// \brief
  ///   Static conversion function to convert from RGBE format to VColorRef (uses clamping)
  VBASE_IMPEXP static VColorRef RGBE_To_RGB(VColorExpRef iColor);

  /// \brief
  ///   Static conversion function to convert from floating point vector to VColorRef (uses clamping)
  inline static void Float_To_RGB(const hkvVec3& vColor, UBYTE *pDest)
  {
    int r = (int)(vColor.x*255.99f);if (r>255) pDest[0]=255; else pDest[0]=r;
    int g = (int)(vColor.y*255.99f);if (g>255) pDest[1]=255; else pDest[1]=g;
    int b = (int)(vColor.z*255.99f);if (b>255) pDest[2]=255; else pDest[2]=b;
  }

  ///
  /// @}
  ///

  ///
  /// @name Conversion member functions
  /// @{
  ///

  /// \brief
  ///   Converts this color to float vector
  inline void ToFloat(hkvVec3& vDest) const {RGBE_To_Float(*this,vDest);}

  /// \brief
  ///   Returns clamped VColorRef version
  inline VColorRef ToRGB() const {return RGBE_To_RGB(*this);}

  /// \brief
  ///   Retrieves color values from float vector. Uses optimal exponent
  inline void FromFloat(const hkvVec3& vColor) {*this = Float_To_RGBE(vColor);}

  /// \brief
  ///   Retrieves color values from VColorRef
  inline void FromRGB(VColorRef color) {r=color.r;g=color.g;b=color.b;e=127;}

  /// \brief
  ///   Returns 32bit raw value (compatible with SetRGBE)
  inline UINT GetRGBE() { return (UINT) ((UINT)((UINT)r) + (((UINT)((UINT)g)) << 8) + (((UINT)((UINT)b))<<16) + (((UINT)((UINT)e))<<24)); }

  /// \brief
  ///   Sets 32bit raw value (compatible with GetRGBE)
  inline void SetRGBE(UINT rgbe) {r = ((rgbe)&0xff); g = ((rgbe>>8)&0xff); b = ((rgbe>>16)&0xff); e = ((rgbe>>24)&0xff);}

  /// \brief
  ///   Returns 32bit raw value (platform dependent, not compatible with GetRGBE/SetRGBE)
  inline DWORD GetNative(){return _colorValue;}

  ///
  /// @}
  ///

  ///
  /// @name Operators
  /// @{
  ///

  /// \brief
  ///   Assignment operator. Retrieves color values from float vector
  inline VColorExpRef operator = (const hkvVec3& vColor) {FromFloat(vColor);return (*this);}

  /// \brief
  ///   Assignment operator. Retrieves color values from VColorRef
  inline VColorExpRef operator = (VColorRef color) {FromRGB(color);return (*this);}

  /// \brief
  ///   Adds two colors.
  inline VColorExpRef operator+ (const VColorExpRef& other) const 
  {
    hkvVec3 col1,col2;
    other.ToFloat(col1);
    this->ToFloat(col2);
    return Float_To_RGBE(col1+col2);
  }

  /// \brief
  ///   Adds two colors.
  inline VColorExpRef operator+ (const hkvVec3& other) const 
  {
    hkvVec3 col;
    this->ToFloat(col);
    return Float_To_RGBE(col+other);
  }

  /// \brief
  ///   Adds a color to this one
  inline void operator += (const VColorExpRef& other)
  {
    (*this) = (*this)+other;
  }

  /// \brief
  ///   Adds a color to this one
  inline void operator += (const hkvVec3& other)
  {
    (*this) =(*this)+other;
  }

  /// \brief
  ///   Multiplies this color with a scalar.
  void operator *= (float factor)
  {
    hkvVec3 vColor;
    ToFloat(vColor);
    FromFloat(vColor*factor);
  }

  ///
  /// @}
  ///

  union
  {
#if defined(WIN32)
    struct { UBYTE b,g,r,e; };
#elif defined(_VISION_XENON)
    struct { UBYTE e,r,g,b; };
#elif defined(_VISION_PS3) || defined(_VISION_POSIX) || defined(_VISION_PSP2) || defined(_VISION_WIIU)
    struct { UBYTE r,g,b,e; };
#else
    #error Undefined platform!
#endif
    
    DWORD _colorValue;
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
