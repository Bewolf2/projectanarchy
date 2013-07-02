/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VFileTime.hpp

#ifndef VBASE_VFILETIME_HPP
#define VBASE_VFILETIME_HPP


#define TIMESTAMP_INVALID       0

/// \brief
///   Structure for file time information
/// 
/// This class encapsulates time an date value in ANSI compatible form. This form is similar to
/// ANSI tm structure. However it removes redundancies and makes data shorter.
struct VBASE_IMPEXP_CLASS VFileTime
{

  ///
  /// @name Constructor
  /// @{
  ///

  /// \brief
  ///   Constructor. Clears everything.
  VFileTime() { m_year = m_month = m_day = m_hour = m_min = m_sec = 0; }

  /// \brief
  ///   Constructor that initialises the members. The passed "year" value is the absolute year, e.g. "2005"
  VFileTime( int year, int month, int day, int hour, int min, int sec )
  {
    m_year = year - 1900; 
    m_month = month; 
    m_day = day; 
    m_hour = hour; 
    m_min = min; 
    m_sec = sec;
  }
  
  /// \brief
  ///   Copy constructor
  VFileTime( const VFileTime& t ) { SetTime( t ); }

  /// \brief
  ///   Conversion constructor from the ANSI (UNIX) vtime_t value
  VFileTime( const vtime_t t ) { SetTime( t ); }


  ///
  /// @}
  ///

  ///
  /// @name Comparison Operators
  /// @{
  ///

  /// \brief
  ///   Operator to compare two file times
  bool operator <( const VFileTime& t ) const
  {
    if( m_year < t.m_year ) return true;
    if( m_year > t.m_year ) return false;
    if( m_month < t.m_month ) return true;
    if( m_month > t.m_month ) return false;
    if( m_day < t.m_day ) return true;
    if( m_day > t.m_day ) return false;
    if( m_hour < t.m_hour ) return true;
    if( m_hour > t.m_hour ) return false;
    if( m_min < t.m_min ) return true;
    if( m_min > t.m_min ) return false;
    if( m_sec < t.m_sec ) return true;
    if( m_sec > t.m_sec ) return false;
    return false;
  }

  /// \brief
  ///   Operator to compare two file times
  bool operator > ( const VFileTime& t ) const { return t < (*this); }

  /// \brief
  ///   Operator to compare two file times
  bool operator == (const VFileTime& t ) const { return !((*this) < t) && !((*this) > t); }

  /// \brief
  ///   Operator to compare two file times
  bool operator != ( const VFileTime& t ) const { return !((*this) == t); }

  /// \brief
  ///   Operator to compare two file times
  bool operator <= ( const VFileTime& t ) const { return (*this) < t || (*this) == t; }

  /// \brief
  ///   Operator to compare two file times
  bool operator >= ( const VFileTime& t ) const { return (*this) > t || (*this) == t; }

  
  ///
  /// @}
  ///

  ///
  /// @name Set Time
  /// @{
  ///


  /// \brief
  ///   set time by specified input time (copy time)
  void SetTime( const VFileTime& t )
  { 
    m_year = t.m_year;
    m_month = t.m_month;
    m_day = t.m_day;
    m_hour = t.m_hour;
    m_min = t.m_min;
    m_sec = t.m_sec;
  }
  
  /// \brief
  ///   set time by specified ANSI (UNIX) time value of type time_t
  void SetTime( const vtime_t t );
  
  ///
  /// @}
  ///


  ///
  /// @name Get Time
  /// @{
  ///

  /// \brief
  ///   return vtime_t value representing this time
  vtime_t GetTime() const;

  /// \brief
  ///   checks if the file time is valid, returns true if yes, false otherwise
  inline bool IsValid() const 
  {
    return (m_year != 0);
  }

  /// \brief
  ///   Returns the year component of the time.
  inline int GetYear() const 
  {
    return m_year + 1900;
  }

  /// \brief
  ///   Returns the month component of the time.
  inline BYTE GetMonth() const 
  {
    return m_month;
  }

  /// \brief
  ///   Returns the day component of the time.
  inline BYTE GetDay() const 
  {
    return m_day;
  }

  /// \brief
  ///   Returns the hour component of the time.
  inline BYTE GetHour() const 
  {
    return m_hour;
  }

  /// \brief
  ///   Returns the minute component of the time.
  inline BYTE GetMinute() const 
  {
    return m_min;
  }

  /// \brief
  ///   Returns the second component of the time.
  inline BYTE GetSecond() const 
  {
    return m_sec;
  }
 
  ///
  /// @}
  ///


  ///
  /// @name Assignment Operators
  /// @{
  ///

  /// \brief
  ///   Assigns the time
  VFileTime& operator = ( const vtime_t t ) { SetTime( t ); return *this; }

  /// \brief
  ///   Assigns the time
  VFileTime& operator = ( const VFileTime& t ) { SetTime( t ); return *this; }

  /// \brief
  ///   Converts a __time64_t into a vtime_t
  static vtime_t GetVTimeFromTime(const __time64_t &inTime);

  /// \brief
  ///   Converts a vtime_t into a __time64_t
  static __time64_t GetTimeFromVTime(const vtime_t &inTime);

protected:
  BYTE m_year;    ///< years since 1900
  BYTE m_month;   ///< months since January - [0,11]
  BYTE m_day;     ///< day of the month - [1,31]
  BYTE m_hour;    ///< hours since midnight - [0,23]
  BYTE m_min;     ///< minutes after the hour - [0,59]
  BYTE m_sec;     ///< seconds after the minute - [0,59]
  ///
  /// @}
  ///

};

#endif // VBASE_VFILETIME_HPP

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
