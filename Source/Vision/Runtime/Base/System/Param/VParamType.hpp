/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VParamType.hpp

#ifndef DC_VPARAMTYPE_HPP
#define DC_VPARAMTYPE_HPP

// parameter types - If you change this, remember to update the ParamSize table in the VParam.cpp

// these classes should not be used by the engine
#ifndef _VISION_DOC

/// \brief
///   enum type for the different types of parameters.
enum VParamType 
{
    //Simple parameters
  
    V_TYPE_NONE = -1,     ///< undefined type of parameter
    V_TYPE_BOOL = 0,      ///< boolean parameter, the value of the parameter could be true or false
    V_TYPE_INT,           ///< integer parameter, the value is a whole number like  123 or -456
    V_TYPE_FLOAT,         ///< the parameter has floating point value
    V_TYPE_FLOAT_VECTOR,  ///< three floating point values
    V_TYPE_COLOR,         ///< the parameter is a color, defined with three beyts for RGB
    
    V_TYPE_ENUM,          ///< the parameter is of the type enum - the value could be one of a defined multitude
    V_TYPE_FLAGS,
    
    V_TYPE_FLOAT_VEC2,    ///< the parameter has two floating point values
    V_TYPE_FLOAT_VEC4,    ///< the parameter has four floating values
    
    // add new parameter types here at the end
    // ...
    
    
    // Complex (pointer based) parameters start here
    V_TYPE_FIRST_COMPLEX = 100, 
    
    V_TYPE_STRING = V_TYPE_FIRST_COMPLEX,  ///< the parameter values is a character string
    V_TYPE_COMPOSITE,                      ///< the parameter is a parameterblock which has also parameters
    V_TYPE_PC_OBJECT,                      ///< the parameter is a parameter container for other parameters
    V_TYPE_ARRAY,                          ///< the parameter is an array of other parameters
    
    // Future types
    V_TYPE_ENTITY_KEY,
    V_TYPE_REFERENCE,
    V_TYPE_SURFACE,
    
    //animatable controllers (future)
    V_TYPE_FIRST_CTRL = 200,
    
    V_TYPE_BOOL_CTRL = V_TYPE_FIRST_CTRL,
    V_TYPE_INT_CTRL,
    V_TYPE_FLOAT_CTRL,
    V_TYPE_FLOAT_VECTOR_CTRL,
    V_TYPE_COLOR_CTRL,
    
    V_TYPE_LAST_CTRL = V_TYPE_COLOR_CTRL,
    
    V_TYPE_LAST_COMPLEX = V_TYPE_LAST_CTRL,
    
    V_TYPE_MAX
};

#endif // _VISION_DOC

#endif // DC_VPARAMTYPE_HPP

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
