
#ifndef VLUA_APIDOC

class VBitmask
{
protected:
  unsigned int m_iBitMask;
public:

  VBitmask();
  VBitmask(unsigned int iMask);
  VBitmask(const char * szHexValue);
  VBitmask(bool bAllBitsSet);
  VBitmask(const VBitmask & bitmask);
  ~VBitmask();

  inline void Set();
  inline void Set(int iBitNum);
  inline void Set(const VBitmask * otherMask);
  inline void Set(const char * szHexValue);
  
  inline void Clear();
  inline void Clear(int iBitNum);
  inline void Clear(const VBitmask * otherMask);
  inline void Clear(const char * szHexValue);
  
  inline bool IsSet(int iBitNum) const;
  
  inline bool IsSet() const;
  
  inline unsigned int Get() const;

  void __setitem__(int iBitNum, bool bValue); // will allow 'mask[3] = true'
  bool __getitem__(int iBitNum) const; // will allow 'local bit3 = mask[3]'
  
  inline bool operator ==(const VBitmask &otherMask) const;
  
  %extend{  
    VBitmask Clone()
    {
      return VBitmask(*self);
    }
  }
};

//add lua tostring and concat operators
VSWIG_CREATE_CONCAT(VBitmask, 16, "0x%04x", self->Get())
VSWIG_CREATE_TOSTRING(VBitmask, "VBitmask [0x%04x]", self->Get())

#else

/// \brief Lua bitmask helper (wrapper) class.
/// \note Since all wrappers classes are handled as pointers in Lua you
///   cannot copy it's content by assignment. Please take a look at the code samples.
/// \par Example
///   \code
///     -- create a new bitmask:
///     local bitmask1 = Vision.VBitmask("0xFF")
///
///     -- this will not copy the bitmask`s content (just another pointer to it):
///     local bitmask2 = bitmask1
///
///     -- this will affect access to bitmask1 and bitmask2 since the pointers share the same object:
///     bitmask2:Clear(3)
///     -- this assertion will not fail
///     Vision.Assert(bitmask1==bitmask2, "When this assertions fails bitmask1 and bitmask2 are pointing to different bitmasks.")
///
///     -- in order to copy the content to another bitmask, you have to clone:
///     local bitmask3 = bitmask1:Clone()
///
///     -- or create a new bitmask using the copy constructor
///     local bitmask4 = Vision.VBitmask(bitmask1)
///
///     -- to make life easier, every Vision scripting method creates a bitmask copy for you - e.g:
///     local b1 = entity:GetVisibleBitmask()
///     local b2 = entity:GetVisibleBitmask()
///     b1:Set(1)
///     b2:Clear(1)
///
///     -- this assertion will not fail
///     Vision.Assert(b1~=b2, "When this assertion fails 'b1' and 'b2' are pointing to the same bitmask.")
///
///   \endcode
/// \see Clone
class VBitmask {
public:


  /// @name Constructors
  /// @{
  
  /// \brief Default constructor (all bits cleared).
  VBitmask();
  
  /// \brief Constructor with preset bitmask from a number.
  /// \param mask The mask in a numeric representation.
  VBitmask(number mask);
  
  /// \brief Constructor with preset bitmask from a hex formated string.
  /// \param hexValue The mask as a hexadecimal string, e.g. "09af".
  VBitmask(string hexValue);
  
  /// \brief Constructor with all bits set or cleared.
  /// \param allSet true = all bits set; false = all bits cleared.
  VBitmask(bool allSet);
  
  /// \brief Copy constructor.
  /// \param bitmask The bitmask to be duplicated.
  VBitmask(VBitmask bitmask);
  
  /// \brief Clone the current bitmask.
  /// \param bitmask The bitmask to be duplicated.
  /// \return The new duplicate.
  VBitmask Clone(VBitmask bitmask);

  /// @}
  /// @name Set & Clear Bits
  /// @{
  
  /// \brief Set all bits.
  void Set();
  
  /// \brief Set a single bit.
  /// \param bitNumber The bit to be set (added using bitwise or).
  /// \par Example
  ///   \code
  ///     local bitmask = Vision.VBitmask()
  ///     bitmask:Set(3)
  ///   \endcode
  void Set(number bitNumber);
  
  /// \brief Set bits defined by bitmask.
  /// \param bitmask The bits to be set (added using bitwise or).
  void Set(VBitmask bitmask);
  
  /// \brief Set bits defined by a hex string.
  /// \param hexValue The bits to be set, e.g. "ab02" (added using bitwise or).
  /// \par Example
  ///   \code
  ///     local bitmask = Vision.VBitmask()
  ///     bitmask:Set("ff")
  ///   \endcode
  void Set(string hexValue);
  
  /// \brief Clear all bits.
  void Clear();
 
  /// \brief Clear a single bit.
  /// \param bitNumber The bit to be cleared (removed using bitwise and).
  void Clear(number bitNumber);
  
  /// \brief Clear bits defined by bitmask.
  /// \param bitmask The bits to be cleared (removed using bitwise and).
  void Clear(VBitmask bitmask);
  
  /// \brief Clear bits defined by a hex string.
  /// \param hexValue The bits to be cleared, e.g. "c4f0" (removed using bitwise and).
  void Clear(string hexValue);

  /// @}
  /// @name Test Bits
  /// @{
  
  /// \brief Test if a bit is set.
  /// \param bitNumber The bit to be tested.
  /// \return true if set, otherwise false.
  boolean IsSet(number bitNumber);
  
  /// \brief Test if any bit is set.
  /// \return true if any bit is set, otherwise false.
  boolean IsSet();
  
  /// \brief Get the numeric representation of the bitmask.
  /// \return Numericvalue of the bitmask.
  number Get();

  /// @}
  /// @name Operators
  /// @{
  
  /// \brief Set a single bit, e.g: myBitmask[3] = true
  /// \param bitNumber The bit to be set or cleared.
  /// \param value The new boolean value of the bit.
  /// \par Example
  ///   \code
  ///     local bitmask = Vision.VBitmask()
  ///     bitmask[5] = true
  ///   \endcode
  void __setitem(number bitNumber, boolean value);
  
  /// \brief Get a single bit, e.g: local myBit = myBitmask[3]
  /// \param bitNumber The bit to receive.
  /// \return The boolean value of the bit.
  boolean __getitem(number bitNumber);
  
  /// \brief Compare with another bitmask, e.g. bitmask1 == bitmask2
  /// \param otherMask The other bitmask.
  /// \return true if equal, otherwise false.
  boolean operator ==(VBitmask otherMask);
  
  /// @}
};

#endif
