/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

/// \file VisApiPathNode.inl

#if !defined(VISAPIPATHNODE_INL_INCLUDED_BY_VISAPIPATHNODE_H)
#error Do not include this file directly - include VisApiPathNode.h instead.
#endif

inline void VisPathNode_cl::EvalPoint(const VisPathNode_cl &next, float t, hkvVec3& pos, hkvVec3* firstDerivative /*= NULL*/, hkvVec3* secondDerivative /*= NULL*/) const
{

  if (OUT_VECTOR_TYPE(inoutType) == LINEAR && IN_VECTOR_TYPE(next.inoutType) == LINEAR)
  { 
    // linear line
    hkvVec3 velocity = next.m_vPosition - m_vPosition;
    pos = m_vPosition + velocity * t;

    if(firstDerivative)
    {
      *firstDerivative = velocity;
    }

    if (secondDerivative)
    {
      secondDerivative->setZero();
    }
    return;
  }

  float s = 1.0f - t;
  float s2 = s * s;
  float s3 = s2 * s;

  float t2 = t * t;
  float t3 = t2 * t;

  // The following allows us to make "half bezier" splines - for linear inputs/outputs, we just reuse the position as the control vertex
  const hkvVec3& controlVertex0 = (OUT_VECTOR_TYPE(inoutType) == BEZIER) ? m_vControlVertices[OUT_VECTOR] : m_vPosition;
  const hkvVec3& controlVertex1 = (IN_VECTOR_TYPE(next.inoutType) == BEZIER) ? next.m_vControlVertices[IN_VECTOR] : next.m_vPosition;

  // Standard cubic Bezier formula
  pos = next.m_vPosition * t3 + (controlVertex1 * (t2 * s) + controlVertex0 * (t * s2)) * 3.0f + m_vPosition * s3;

  if(firstDerivative)
  {
    *firstDerivative = (next.m_vPosition * t2 + controlVertex1 * (2.0f * t - 3.0f * t2) - controlVertex0 * (2.0f * s - 3.0f * s2) - m_vPosition * s2) * 3.0f;
  }

  if(secondDerivative)
  {
    *secondDerivative = (next.m_vPosition * t + controlVertex1 * (1.0f - 3.0f * t) + controlVertex0 * (1.0f - 3.0f * s) + m_vPosition * t) * 6.0f;
  }
}



inline void VisPathNode_cl::EvalPointSmooth(const VisPathNode_cl &next, float distance, hkvVec3& pos, hkvVec3* firstDerivative /*= NULL*/, hkvVec3* secondDerivative /*= NULL*/, float segLen/*=-1.0f*/ ) const
{
  hkvVec3 last = m_vPosition;
  hkvVec3 diff;
  hkvVec3 current;

  segLen = segLen < 0.0f ? GetLen(next) : segLen;

  // is the distance equal or greater as the length between this node and the next node?
  if( distance >= segLen )
  {
    if ((firstDerivative==NULL) && (secondDerivative==NULL))
    {
      // yes, return the position of the next node
      pos = next.m_vPosition;
    }else
    {
      // We need the derivatives. Thus do a full eval point on the end of the path.
      EvalPoint(next, 1.0f, pos, firstDerivative, secondDerivative);
    }
    return;
  }

  float length = 0.0f;
  float stepping = 0.01f;
  float f = 0.0f;

  // if our cached distance is smaller than the one we are looking for, then use it
  if ( m_fLastLen < distance )
  {
    length = m_fLastLen;
    f = m_iLastDiv * stepping;
    // our last position must to be computed now, we can't start from the node position
    EvalPoint( next, f, last, firstDerivative, secondDerivative );
  }else
  {
    // if our cached distance is smaller then we better start from the beginning
    m_iLastDiv = 0;
    m_fLastLen = 0.0f;
  }

  bool topLevel = true; // true when we are in the first level of subdivisions
  int iDiv = m_iLastDiv;

  // Evaluate a point between this node and the next node.
  // This point has the given distance to this node.
  while(stepping > PATH_DELTA)
  {
    EvalPoint(next, f+stepping, current, firstDerivative, secondDerivative);

    diff = current;
    diff -= last; ///< segment
    last = current;
    float diffLen = diff.getLength();

    if( (length+diffLen) > distance)
    {
      stepping *= 0.5f;
      if ( topLevel )
      {
        // if we need to deep into another level, cached the top level division and length we reached
        m_iLastDiv = iDiv;
        m_fLastLen = length;
        topLevel = false;
      }
      continue;
    }
    ++iDiv;
    length += diffLen;
    f += stepping;
  }
  pos = current;
}

inline float VisPathNode_cl::GetLen(const VisPathNode_cl &next) const
{
  //calculates the length of the spline
  //returns and stores in 'length'
  float length = 0.f;

  hkvVec3 last = m_vPosition;
  hkvVec3 diff;
  hkvVec3 current;
  float f = PATH_STARTVALUE;
  for (int i=0; i<PATH_STEPS; i++)
  {
    EvalPoint(next, f, current);
    diff = current;
    diff -= last; ///< segment
    last = current;
    length += diff.getLength(); ///< length of the segment
    f += PATH_STEPVALUE;
  }
  return length;
}


inline void VisPathNode_cl::GetControlVertices(hkvVec3& cv1, hkvVec3& cv2) const
{
  cv1 = m_vControlVertices[0];
  cv2 = m_vControlVertices[1];
}

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
