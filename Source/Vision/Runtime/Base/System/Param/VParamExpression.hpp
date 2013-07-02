/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamExpression.hpp

#ifndef _VISION_VPARAMEXPRESSION_HPP
#define _VISION_VPARAMEXPRESSION_HPP

// this class is not relevant for the engine
#ifndef _VISION_DOC

const int iOpsNr = 7;

/// \brief
///   Class to manage parameter expressions
class VParamExpression
{
public:
  /// \brief supported operation types
  enum eOpperationType
  {
    OPP_NOTSUPPORTED = 0,
    OPP_INCREMENT,
    OPP_DECREMENT,
    OPP_PLUSEQUAL,
    OPP_MINUSEQUAL,
    OPP_MULEQUAL,
    OPP_SUBEQUAL,
    OPP_DEFAULTVAL,
  };

  VBASE_IMPEXP VParamExpression(const char *pExpression);
  VBASE_IMPEXP ~VParamExpression();

  /// \brief
  ///   Tests for supported operations strings. Does not test for validity.
  /// 
  /// \return
  ///   VBool: TRUE if any supported expression is found.
  VBASE_IMPEXP VBool ContainSupportedOpperation();
  
  /// \brief
  ///   Float version to evaluate the expression.
  /// 
  /// \param val
  ///   resulting value
  /// 
  /// \param unit
  ///   unit value ( for increment/decrement )
  /// 
  /// \param defVal
  ///   default value
  /// 
  /// \return
  ///   VBool: TRUE if the expression was successfully evaluate.
  VBASE_IMPEXP VBool Evaluate(float &val, float unit = 1.f, float defVal = 0.f);

  /// \brief
  ///   Int version to evaluate the expression.
  /// 
  /// \param val
  ///   resulting value
  /// 
  /// \param unit
  ///   unit value ( for increment/decrement )
  /// 
  /// \param defVal
  ///   default value
  /// 
  /// \return
  ///   VBool: TRUE if the expression was successfully evaluate.
  VBASE_IMPEXP VBool Evaluate(int &val, int unit = 1, int defVal = 0);

private:
  /// \brief
  ///   Test the expression for validity
  /// 
  /// \return
  ///   VBool: TRUE if the expression is valid
  VBASE_IMPEXP VBool IsValid();

  /// \brief
  ///   Return the operation type for the expression
  /// 
  /// \return
  ///   eOpperationType: 
  VBASE_IMPEXP eOpperationType GetOpperationType();

  VString m_sExpression; ///< expression string
  float m_fReadedValue;  ///< value read from expression

  eOpperationType m_opp; ///< active operation
  
  static const char *ops[iOpsNr]; ///< supported operations strings
};

#endif // _VISION_DOC

#endif // _VISION_VPARAMEXPRESSION_HPP

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
