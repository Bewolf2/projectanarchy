
#ifndef VLUA_APIDOC

%immutable;
const VColorRef  V_RGBA_WHITE;
const VColorRef  V_RGBA_GREY;
const VColorRef  V_RGBA_BLACK;

const VColorRef  V_RGBA_RED;
const VColorRef  V_RGBA_YELLOW;
const VColorRef  V_RGBA_GREEN;
const VColorRef  V_RGBA_CYAN;
const VColorRef  V_RGBA_BLUE;
const VColorRef  V_RGBA_PURPLE;
%mutable;

struct VColorRef
{
public:

  VColorRef();
  VColorRef(UBYTE red,UBYTE green,UBYTE blue,UBYTE alpha=255);

  void Clear();

  void SetRGBA(UINT red, UINT green, UINT blue, UINT alpha);
  void SetRGB(UINT red, UINT green, UINT blue);

  bool operator == (const VColorRef& other);

  VColorRef operator+ (const VColorRef& other) const;
  VColorRef operator- (const VColorRef& other) const;
  VColorRef operator* (const VColorRef& other) const;
  VColorRef operator* (float factor) const;

  bool IsZero() const;
  bool IsBlack() const;

  float GetIntensity() const;

  void Lerp(const VColorRef &iCol1, const VColorRef &iCol2, float fFactor);

  UBYTE r,g,b,a;
  
  %extend{
    VColorRef(const VColorRef &other)
    {
      return new VColorRef(other.r, other.g, other.b, other.a);
    }
    
    //deprecated with 2013.1
    VColorRef Clone()
    {
      return VColorRef(self->r, self->g, self->b, self->a);
    }
    
    VColorRef clone()
    {
      return VColorRef(self->r, self->g, self->b, self->a);
    }
  }
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VColorRef, 32, "[%d,%d,%d,%d]", self->r,self->g,self->b,self->a)
VSWIG_CREATE_TOSTRING(VColorRef, "VColorRef: R %d, G %d, B %d, A %d", self->r,self->g,self->b,self->a)


#else

/// \brief Simple RGBA color wrapper representation.
/// \note Since all wrappers classes are handled as pointers in Lua you
///   cannot copy it's content by assignment. Please take a look at the code samples.
/// \par Example
///   \code
///     -- create a new color:
///     local red = Vision.VColorRef(255,0,0)
///
///     -- this will not copy the color`s content (just another pointer to it):
///     local color = red
///
///     -- this will affect access to 'red' and 'color' since the pointers share the same object (turining to yellow):
///     color.g = 255
///     -- this assertion will not fail
///     Vision.Assert(red==color, "When this assertions fails 'red' and 'color' are pointing to different colors.")
///
///     -- in order to copy the content, you have to clone:
///     local c1 = red:clone()
///
///     -- or create a new color using the copy constructor
///     local c2 = Vision.VColorRef(red)
///
///     -- be aware that Lua has no concept for 'const'...
///     -- if you acceess a predefined value, you could accidentally change it!
///     local anotherColor = Vision.V_RGBA_BLUE -- this is a pointer to the global 'blue'
///
///     anotherColor.r = 255 -- now we accidentally changed the global blue constant!
///
///     -- in order to avoid this you should clone the color
///     -- (at least if you know that you will change it`s value):
///     local anotherColorNew = Vision.V_RGBA_BLUE:clone()
///
///     -- now we are only affecting our own color instance:
///     anotherColorNew:SetRGB(130,90,40)
///   \endcode
/// \see clone
class VColorRef {
public:

  /// @name Constructors
  /// @{
  
  /// \brief Default constructor (black color).
  VColorRef();
  
  /// \brief Constructor with defined color value.
  /// \param red amount of red color from 0 to 255.
  /// \param green amount of green color from 0 to 255.
  /// \param blue amount of blue color from 0 to 255.
  /// \param alpha (\b optional) amount of transparency color from 0 (invisible) to 255 (opaque).
  /// \par Example
  ///   \code
  ///     local color = Vision.VColorRef(255,0,255)
  ///   \endcode
  VColorRef(number red, number green, number blue, number alpha=255);

  /// \brief Copy constructor.
  /// \param color The color to be duplicated.
  /// \return The new duplicate.
  VColorRef(VColorRef color);
  
  /// \brief Clone the current color.
  /// \param color The color to be duplicated.
  /// \return The new duplicate.
  VColorRef clone(VColorRef color);
  
  /// @}
  /// @name Common Color Functions
  /// @{
  
  /// \brief Sets the color to black. 
  void Clear();

  /// \brief Set the RGBA values.
  /// \param red amount of red color from 0 to 255.
  /// \param green amount of green color from 0 to 255.
  /// \param blue amount of blue color from 0 to 255.
  /// \param alpha amount of transparency color from 0 (invisible) to 255 (opaque).
  void SetRGBA(number red, number green, number blue, number alpha);
  
  /// \brief Set the RGB values, without changing th alpha value.
  /// \param red amount of red color from 0 to 255.
  /// \param green amount of green color from 0 to 255.
  /// \param blue amount of blue color from 0 to 255.
  void SetRGB(number red, number green, number blue);
  
  /// \brief Indicates whether the color is (0,0,0,0). 
  /// \return true on match, otherwise false.
  /// \par Example
  ///   \code
  ///     local color = Vision.VColorRef()
  ///     local isZero = color:IsZero()
  ///   \endcode
  boolean IsZero();
  
  /// \brief Indicates whether the color is (0,0,0,X). 
  /// \return true on match, otherwise false.
  /// \par Example
  ///   \code
  ///     local color = Vision.VColorRef()
  ///     local isBlack = color:IsBlack()
  ///   \endcode
  boolean IsBlack();

  /// \brief Gets the color's intensity (average red/green/blue).
  /// \return tThe inbtensity in a range of [0..1]
  number GetIntensity();

  /// \brief Linearily interpolates between two colors. 
  /// \param color1 Blend color #1.
  /// \param color2 Blend color #2.
  /// \param factor Blend value from 0 to 1, on 0 the result is color1.
  /// \par Example
  ///   \code
  ///     local red = Vision.VColorRef(255,0,0)
  ///     local blue = Vision.VColorRef(0,0,255)
  ///     local purple = Vision.VColorRef()
  ///     purple:Lerp(red, blue, 0.5)
  ///   \endcode
  void Lerp(VColorRef color1, VColorRef color2, number factor);
  
  /// @}
  /// @name Operators
  /// @{
  
  /// \brief Compare with another VColorRef, e.g. color1 == color2
  /// \param other The other color.
  /// \return true if equal, otherwise false.
  boolean operator == (VColorRef other);

  /// \brief Add another color
  /// \param other The other color.
  /// \return The additive color value.
  /// \par Example
  ///   \code
  ///     local red = Vision.VColorRef(255,0,0)
  ///     local blue = Vision.VColorRef(0,0,255)
  ///     local purple = red + blue
  ///   \endcode
  VColorRef operator+ (VColorRef other);
  
  /// \brief Subtract another color
  /// \param other The other color.
  /// \return The subtractive color value.
  VColorRef operator- (VColorRef other);
  
  /// \brief Multiply with another color
  /// \param other The other color.
  /// \return The multiplicative color value.
  VColorRef operator* (VColorRef other);
  
  /// \brief Multiply all components with a factor.
  /// \param factor The multiplication factor.
  /// \return The multiplicative color value.
  /// \par Example
  ///   \code
  ///     local darkRed = Vision.VColorRef(127,64,64)
  ///     local lightRed = darkRed * 2
  ///   \endcode
  VColorRef operator* (number factor);
  
  /// @}
  /// @name Public Members
  /// @{
  
  /// \brief Member holding the red value.
  number r;
  
  /// \brief Member holding the green value.
  number g;
  
  /// \brief Member holding the blue value.
  number b;
  
  /// \brief Member holding the alpha value.
  number a;
  
  /// @}
};

#endif
