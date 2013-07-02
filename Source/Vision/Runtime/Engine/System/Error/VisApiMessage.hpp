/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiMessage.hpp

#ifndef DEFINE_VISAPIMESSAGE
#define DEFINE_VISAPIMESSAGE

#include <Vision/Runtime/Engine/System/VisApiFont.hpp>

/// \brief
///   Class for debug output message handling. The global instance of this class can be accessed
///   via Vision::Message
/// 
/// This class offers both functions to directly render text messages on the screen and to render
/// messages by adding them to a message queue (with having the possibility to replace and delete
/// messages as well).
/// 
/// For displaying text messages the engine always uses its internal small font.
class VisMessage_cl
{
public :

  VisMessage_cl();
  ~VisMessage_cl();


  ///
  /// @name Printing Text Messages
  /// @{
  ///


  /// \brief
  ///   Prints a text at a certain position on the screen.
  /// 
  /// This function adds a text to an internal stack which will be processed during the
  /// Vision::RenderScene function. There is neither a limitation in the number of text messages
  /// nor a restriction in the position of the text message. The text will be rendered and deleted
  /// in the next call of the Vision::RenderScene function.
  /// 
  /// You can use this function to place a text message at an arbitrary position on the screen
  /// without caring about the message queue handling and restrictions.
  /// 
  /// Use this text output for debugging purposes only. For retail versions of the application, use
  /// the VisFont_cl class.
  ///
  /// If the passed szText contains an \@ (at) character then this character is treated as tabulator. For example,
  /// you can use \@320 to let the succeeding text start at x position 320.
  /// 
  /// \param fontHandle
  ///   Ignored. Always uses small system font.
  /// 
  /// \param xPos
  ///   On screen position of the text in x direction (upper left corner has x = 0).
  /// 
  /// \param yPos
  ///   On screen position of the text in y direction (upper left corner has y = 0).
  /// 
  /// \param s
  ///   Text to print (printf style).
  /// 
  /// \sa VisMessage_cl::Add
  /// \sa VisMessage_cl::LoadFont
  /// 
  /// \example
  ///   \code
  ///   Vision::Message.Print(1, 10, 10, "Entity Position %s", entity->GetPos().ToString());
  ///   \endcode
  VISION_APIFUNC void Print(int fontHandle, int xPos, int yPos, const char *s, ...);

  /// \brief
  ///   Prints a text at a certain position on the screen.
  /// 
  /// This function adds a text to an internal stack which will be processed during the
  /// Vision::RenderScene function. There is neither a limitation in the number of text messages
  /// nor a restriction in the position of the text message. The text will be rendered and deleted
  /// in the next call of the Vision::RenderScene function.
  /// 
  /// You can use this function to place a text message at an arbitrary position on the screen
  /// without caring about the message queue handling and restrictions.
  /// 
  /// Use this text output for debugging purposes only. For retail versions of the application, use
  /// the VisFont_cl class.
  ///
  /// If the passed szText contains an \@ (at) character then this character is treated as tabulator. For example,
  /// you can use \@320 to let the succeeding text start at x position 320.
  /// 
  /// 
  /// \param xPos
  ///   On screen position of the text in x direction (upper left corner has x = 0).
  /// 
  /// \param yPos
  ///   On screen position of the text in y direction (upper left corner has y = 0).
  ///
  /// \param iState
  ///   Render state/properties to render this text with.
  /// 
  /// \param s
  ///   Text to print (printf style).
  /// 
  /// \sa VisMessage_cl::Add
  /// \sa VisMessage_cl::LoadFont
  /// 
  /// \example
  ///   \code
  ///   Vision::Message.Print(1, 10, 10, "Entity Position %s", entity->GetPos().ToString());
  ///   \endcode
  VISION_APIFUNC void Print(int xPos, int yPos, const VSimpleRenderState_t &iState, const char *s, ...);

  /// \brief
  ///   Prints a text at a certain position on the screen.
  /// 
  /// This function adds a text to an internal stack which will be processed during the
  /// Vision::RenderScene function. There is neither a limitation in the number of text messages
  /// nor a restriction in the position of the text message. The text will be rendered and deleted
  /// in the next call of the Vision::RenderScene function.
  /// 
  /// You can use this function to place a text message at an arbitrary position on the screen
  /// without caring about the message queue handling and restrictions.
  /// 
  /// Use this text output for debugging purposes only. For retail versions of the application, use
  /// the VisFont_cl class.
  ///
  /// If the passed szText contains an \@ (at) character then this character is treated as tabulator. For example,
  /// you can use \@320 to let the succeeding text start at x position 320.
  /// 
  /// 
  /// \param xPos
  ///   On screen position of the text in x direction (upper left corner has x = 0).
  /// 
  /// \param yPos
  ///   On screen position of the text in y direction (upper left corner has y = 0).
  ///
  /// \param zDepth
  ///   zDepth position of the text. For 2D Text use -1.
  ///
  /// \param iState
  ///   Render state/properties to render this text with.
  /// 
  /// \param s
  ///   Text to print (printf style).
  /// 
  /// \sa VisMessage_cl::Add
  /// \sa VisMessage_cl::LoadFont
  /// 
  /// \example
  ///   \code
  ///   Vision::Message.Print(1, 10, 10, "Entity Position %s", entity->GetPos().ToString());
  ///   \endcode
  VISION_APIFUNC void Print(int xPos, int yPos, float zDepth, const VSimpleRenderState_t &iState, const char *s, ...);



  /// \brief
  ///   Draws a text on a specific 3D worldspace position
  /// 
  /// The message position will be transformed to camera space and then to screen space with a
  /// perspective divide. No scaling according to distance will take place. The camera of the current render context will be used.
  /// 
  /// \param pszMsg
  ///   String to display.
  /// 
  /// \param pos
  ///   Position in world space to render to.
  /// 
  /// \param iFont
  ///   Ignored.
  ///
  /// \param iProperties
  ///   render state properties.
  /// 
  /// \return
  ///   bool retValue: TRUE if the string is valid (not NULL).
  VISION_APIFUNC bool DrawMessage3D ( const char *pszMsg, const hkvVec3& pos, short iFont=1, const VSimpleRenderState_t &iProperties = VisFont_cl::DEFAULT_STATE);

  
  /// \brief
  ///   Prints a matrix on screen.
  /// 
  /// Use this to display the numeric values of matrix elements on screen. Output is buffered, so
  /// you can call it anytime.
  /// 
  /// \param fontHandle
  ///   Ignored. Always uses small system font.
  /// 
  /// \param xPos
  ///   On screen position of the text in x direction (upper left corner has x = 0).
  /// 
  /// \param yPos
  ///   On screen position of the text in y direction (upper left corner has y = 0).
  /// 
  /// \param mat
  ///   The 3x3 matrix to print.
  /// 
  /// \note
  ///   There is an overloaded version of this function to print a hkvMat4.
  inline void PrintMatrix ( int fontHandle, int xPos, int yPos, const hkvMat3& mat )
  {  
    for ( int i=0; i<3; i++ )
      Print ( 1, xPos, yPos+i*14, "(%.2f, %.2f, %.2f)", mat.m_Column[0][i], mat.m_Column[1][i], mat.m_Column[2][i]);
  }


  /// \brief
  ///   Prints out a matrix on screen.
  /// 
  /// Use this to display the numeric values of matrix elements on screen. Output is buffered, so
  /// you can call it anytime.
  /// 
  /// \param fontHandle
  ///   Ignored. Always uses small system font.
  /// 
  /// \param xPos
  ///   On screen position of the text in x direction (upper left corner has x = 0).
  /// 
  /// \param yPos
  ///   On screen position of the text in y direction (upper left corner has y = 0).
  /// 
  /// \param mat
  ///   The 4x4 matrix to print.
  /// 
  /// \note
  ///   There is an overloaded version of this function to print a hkvMat3.
  inline void PrintMatrix ( int fontHandle, int xPos, int yPos, const hkvMat4& mat )
  {  
    for ( int i=0; i<4; i++ )
      Print ( 1, xPos, yPos+i*14, "(%.2f, %.2f, %.2f, %.2f)", mat.m_Column[0][i], mat.m_Column[1][i], mat.m_Column[2][i], mat.m_Column[3][i]);
  }


  ///
  /// @}
  ///


  ///
  /// @name Message Queue Related Functions
  /// @{
  ///


  /// \brief
  ///   Sets the duration (in seconds) of the messages in the queue.
  /// 
  /// Messages will disappear after the time has expired.
  /// 
  /// Affects mssages displayed via Vision::Message.Add.
  /// 
  /// \param duration
  ///   In seconds; a duration of 0 causes the messages to be resident.
  VISION_APIFUNC void SetDuration(float duration);


  /// \brief
  ///   Gets the duration of the messages in seconds.
  /// 
  /// \return
  ///   the currently set duration (in seconds; a duration of 0 causes the messages to be resident).
  VISION_APIFUNC float GetDuration();


  /// \brief
  ///   Sets the maximum number of messages on the screen.
  /// 
  /// If a new message exceeds this number, then the oldest message will automatically
  /// be removed.
  /// 
  /// \param maxNum
  ///   Maximum number of message on the screen.
  VISION_APIFUNC void SetMaxNum(short maxNum);


  /// \brief
  ///   Gets the maximum number of messages on the screen, that was set with SetMaxNum.
  /// 
  /// \return
  ///   short maxNum: Maximum number of message on the screen.
  /// 
  /// \sa SetMaxNum
  /// \sa GetPossibleMaxNum
  VISION_APIFUNC short GetMaxNum();


  /// \brief
  ///   Calculates the possible maximum number of message on the screen.
  /// 
  /// The possible maximum number of message is the result of screen resolution height divided by
  /// font height.
  /// 
  /// \param fontNr
  ///   Ignored.
  /// 
  /// \return
  ///   short maxNum: Maximum possible number of message on the screen.
  VISION_APIFUNC short GetPossibleMaxNum(short fontNr=1);


  /// \brief
  ///   Adds a printf style message to the message queue.
  ///
  /// If the passed szText contains an \c \@ character, this character is treated as tabulator. For example,
  /// you can use \@320 to let the following text start at x position 320.
  /// 
  /// \param fontHandle
  ///   Ignored.
  /// 
  /// \param s
  ///   printf-style message with optional arguments.
  /// 
  /// \sa VisMessage_cl::Print
  VISION_APIFUNC void Add(int fontHandle, const char *s, ...);


  /// \brief
  ///   Adds a message to the message queue.
  /// 
  /// If the passed szText contains an \c \@ character, this character is treated as tabulator. For example,
  /// you can use \@320 to let the following text start at x position 320.
  ///
  /// \param message
  ///   Message string.
  /// 
  /// \param fontHandle
  ///   Ignored.
  /// 
  /// \param iColor
  ///   Font color on screen. Note the alpha portion is used for transparency.
  /// 
  /// \sa VisMessage_cl::Print
  VISION_APIFUNC void Add(const char *message, short fontHandle = 1, VColorRef iColor=V_RGBA_WHITE);


  /// \brief
  ///   Replaces the last message string in the queue.
  /// 
  /// \param newMessage
  ///   New message string.
  VISION_APIFUNC void ChangeLast(const char *newMessage);


  /// \brief
  ///   Removes the oldest messages in the queue.
  /// 
  /// This function removes the oldest messages from the message queue.
  ///
  /// \param num
  ///   the number of messages to remove
  VISION_APIFUNC void erase(short num = 1);
  
  
  /// \brief
  ///   Resets the message queue.
  /// 
  /// This function resets the message queue and removes all the messages.
  VISION_APIFUNC void reset();


  /// \brief
  ///   Sets the text color for subsequent calls of Print functions
  /// 
  /// The color remains active until the next call of SetTextColor.
  /// 
  /// \param iTextColor
  ///   the text color (white by default).
  /// 
  /// \example
  ///   \code
  ///   Output all forthcoming text in olive:
  ///   Vision::Message.SetTextColor(VColorRef(186,218,85, 255));
  ///   semi-transparent red:
  ///   Vision::Message.SetTextColor(VColorRef(255,0,0, 127)); 
  ///   \endcode
  VISION_APIFUNC void SetTextColor(VColorRef iTextColor = V_RGBA_WHITE);

  ///
  /// @}
  ///

  /// \brief
  ///   Set the filter bitmask for rendering messages only in some render contexts
  /// 
  /// Messages will only be rendered in render contexts, if the result of the logical AND
  /// operation of this bitmask and the context's bitmask (see VisRenderContext_cl::SetRenderFilterMask) 
  /// is not zero.
  ///
  /// @param iMask
  ///   the visibility bitmask to set
  inline void SetVisibleBitmask(unsigned int iMask)
  {
    m_iVisibleBitmask = iMask;
  }

  /// \brief
  ///   Returns the filter bitmask that has been set via SetVisibleBitmask().
  inline unsigned int GetVisibleBitmask() const
  {
    return m_iVisibleBitmask;
  }

  // internal functions
  void Init();
  void DeInit();
  VISION_APIFUNC void HandleMessages();

private:
  struct VMessageData
  {
    VMessageData () : m_fTime(0), m_MsgColor(V_RGBA_WHITE) {}

    float m_fTime;
    VString m_sText;
    VColorRef m_MsgColor;
  };

  friend class VisProfiling_cl;

  void HandlePrintStack(IVRender2DInterface *pRI);            ///< prints all the messages on the print stack

  void vPrint(int xPos, int yPos, float zDepth, const VSimpleRenderState_t &iState, const char *s, va_list args);  ///< helper function to print text that takes argument va_list

  DynObjArray_cl<VMessageData> m_Messages;
  short m_iMessageBufferSize; ///< Maximum number of messages that can be stored
  short m_iMessageCount;      ///< Current number of messages stored
  short m_iMessageStart;      ///< The start position in the ring buffer 
  short m_iMessageDuration;   ///< duration of the messages
  VColorRef m_iCurrentColor;  ///< current print color

  int m_iPrintBufferLen;
  DynArray_cl<char> m_PrintBuffer;
  static const float m_fScaling;

  unsigned int m_iVisibleBitmask;
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
