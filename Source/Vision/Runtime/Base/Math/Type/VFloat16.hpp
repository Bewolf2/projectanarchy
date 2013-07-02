/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFloat16.hpp


#ifndef __VBASE_FLOAT16__
#define __VBASE_FLOAT16__


/// \brief
///   This class represents a half float (also referred to as float16 as its memory footprint is 16bit).
/// 
/// You can set a half float from a float32 or from a bit representation. Since this class has assignment operators for float
/// it can be used almost interchangeably. The most common use case for this format is inside vertex buffers indicated by
/// the VERTEXDESC_FORMAT_HALF2 or VERTEXDESC_FORMAT_HALF4 format
class VFloat16
{
  public:

    /// \brief
    ///   Default constructor, initializes the value to 0.0f
    VFloat16()
    {
      FromFloat(0.0f);
    }

    /// \brief
    ///   Construtor from float value
    VFloat16( float Value )
    {
      FromFloat( Value );
    }

    /// \brief
    ///   Constructor from bit representation
    VFloat16( unsigned short Bits )
    {
      FromBits( Bits );
    }

    /// \brief
    ///   Copy constructor
    VFloat16( const VFloat16& Other )
    {
      m_Bits = Other.m_Bits;
    }

    /// \brief
    ///   Sets the internal bits of the float16
    /// 
    /// \param Bits
    ///   the internal bit representation (eg from a file)
    /// 
    /// \note
    ///   The bit representation used here is the same as the half data type on graphics card from
    ///   nvidia and amd
    void FromBits( unsigned short Bits )
    {
      m_Bits = Bits;
    }

    /// \brief
    ///   Returns the internal bits of the float16
    /// 
    /// \return
    ///   the internal bits of the float16
    /// 
    /// \note
    ///   The bit representation used here is the same as the half data type on graphics card from
    ///   nvidia and amd
    unsigned short GetBits() const
    {
      return m_Bits;
    }

    /// \brief
    ///   Sets the internal bits from a float32
    /// 
    /// \param FloatVal
    ///   the float value to set
    void FromFloat( float FloatVal )
    {
      union
      {
        float FloatVal;
        unsigned int IntVal;
      } FloatAndInt;
      FloatAndInt.FloatVal = FloatVal;
      m_Bits = ToFloat16Short( FloatAndInt.IntVal );
    }

    /// \brief
    ///   Gets a float32 value from the float16
    /// 
    /// \return
    ///   The float value (32bits) of the 16bit float
    float ToFloat() const
    {
      union
      {
        float FloatVal;
        unsigned int IntVal;
      } FloatAndInt;
      FloatAndInt.IntVal = ToFloatInteger(m_Bits);
      return FloatAndInt.FloatVal;
    }

    /// \brief
    ///   Gets the bit representation of the 32 bit float of the given bit representation of a 16
    ///   bit float
    /// 
    /// \param Bits
    ///   The bit representation of a 16 bit float
    /// 
    /// \return
    ///   The 32 bit representation of the 32bit float value
    /// 
    /// \note
    ///   Refer to Game Programming Gems 2 - Chapter 2.1 for the bit scheme
    unsigned int ToFloatInteger( unsigned short Bits ) const
    {
      int Sign = (Bits >> 15) & 0x00000001;
      int Exponent = (Bits >> 10) & 0x0000001f;
      int Mantissa =  Bits & 0x000003ff;

      if (Exponent == 0)
      {
        if (Mantissa == 0)
          return Sign << 31;
        else
        {
          while (!(Mantissa & 0x00000400))
          {
            Mantissa <<= 1;
            Exponent -=  1;
          }

          Exponent += 1;
          Mantissa &= ~0x00000400;
        }
      }
      else if (Exponent == 31)
      {
        if (Mantissa == 0)
          return (Sign << 31) | 0x7f800000;
        else
          return (Sign << 31) | 0x7f800000 | (Mantissa << 13);
      }

      Exponent = Exponent + (127 - 15);
      Mantissa = Mantissa << 13;

      return (Sign << 31) | (Exponent << 23) | Mantissa;
    }

    /// \brief
    ///   Gets the bit representation of the 16 bit float from the given bit representation of a 32
    ///   bit float
    /// 
    /// \param Bits
    ///   The bit representation of a 32 bit float
    /// 
    /// \return
    ///   The 16 bit representation of the 32bit float value
    /// 
    /// \note
    ///   Refer to Game Programming Gems 2 - Chapter 2.1 for the bit scheme
    unsigned short ToFloat16Short( unsigned int Bits ) const
    {
      int Sign =  (Bits >> 16) & 0x00008000;
      int Exponent = ((Bits >> 23) & 0x000000ff) - (127 - 15);
      int Mantissa = Bits & 0x007fffff;

      if (Exponent <= 0)
      {
        if (Exponent < -10)
          return 0;

        Mantissa = (Mantissa | 0x00800000) >> (1 - Exponent);
        return Sign | (Mantissa >> 13);
      }
      else if (Exponent == 0xff - (127 - 15))
      {
        if (Mantissa == 0)
          return Sign | 0x7c00;
        else
        {
          Mantissa >>= 13;
          return Sign | 0x7c00 | Mantissa | (Mantissa == 0);
        }
      }
      else
      {
        if (Exponent > 30)
          return Sign | 0x7c00;

        return Sign | (Exponent << 10) | (Mantissa >> 13);
      }
    }


    /// \brief
    ///   assigns this VFloat16 another VFloat16 value
    /// 
    /// \param other
    ///   The other VFloat16
    /// 
    /// \return
    ///   This VFloat16
    VFloat16& operator= (const VFloat16& other)
    {
      m_Bits = other.m_Bits;
      return *this;
    }

    /// \brief
    ///   returns the float value
    /// 
    /// a float value operator for simplicity
    /// 
    /// \return
    ///   The float32 value of this VFloat16
    operator float() const
    {
      return ToFloat();
    }

  private:

    // The internal bits of the 16 bit float
    unsigned short m_Bits;

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
