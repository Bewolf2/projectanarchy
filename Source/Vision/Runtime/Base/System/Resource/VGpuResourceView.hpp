/*
 *
 * Confidential Information of Telekinesys Research Limited (t/a Havok). Not for disclosure or distribution without Havok's
 * prior written consent. This software contains code, techniques and know-how which is confidential and proprietary to Havok.
 * Product and Trade Secret source code contains trade secrets of Havok. Havok Software (C) Copyright 1999-2013 Telekinesys Research Limited t/a Havok. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 *
 */

#ifndef _VGPURESOURCEVIEW_HPP_INCLUDED
#define _VGPURESOURCEVIEW_HPP_INCLUDED

#if defined( _VISION_DOC )
  #define SUPPORTS_GPU_RESOURCE_VIEWS
#endif

#if defined( SUPPORTS_GPU_RESOURCE_VIEWS )

/// \brief
///   GPU Resource View - Base Class
/// 
/// Base class for encapsulating a generic, platform-specific resource view on DX11.
class VGpuResourceView : public VRefCounter
{
public:
  /// \brief
  ///   Destructor
  virtual ~VGpuResourceView() {}

  /// \brief
  ///   Retrieve the (generic) platform-specific resource view.
  ID3D11View* GetGenericView() const { return m_spView; }

  /// \brief
  ///   Set the platform-specific resource view to be encapsulated by this object.
  void SetView( ID3D11View* pView ) { m_spView = pView; }

protected:  
  VGpuResourceView() {}

private:
  VSmartPtr<ID3D11View> m_spView;
};


/// \brief
///   GPU Resource View - Template Class for Specific Resource Views (DX11-only)
template< typename TView, typename TViewDescription >
class VTypedResourceView : public VGpuResourceView
{
public:
  /// \brief
  ///   Constructor
  ///
  /// \param pDesc
  ///   Platform-specific and view-specific description of the resource view to be encapsulated.
  ///
  VTypedResourceView( const typename TViewDescription* pDesc )
    : VGpuResourceView()
	, m_pDesc( NULL )
  {
    if ( pDesc != NULL )
    {
      m_pDesc = new TViewDescription;
      memcpy( m_pDesc, pDesc, sizeof( typename TViewDescription ) );
    }
  }

  ~VTypedResourceView()
  {
    V_SAFE_DELETE( m_pDesc );
  }

  /// \brief
  ///   Retrieve the encapsulated specific view.
  typename TView* GetView() const { return static_cast< typename TView* >( GetGenericView() ); }

  /// \brief
  ///   Retrieve this view's description.
  const typename TViewDescription* GetViewDescription() const { return m_pDesc; }

private:
  typename TViewDescription* m_pDesc;
};

/// \brief
///   Typedef for encapsulating a shader resource view.
typedef VTypedResourceView< ID3D11ShaderResourceView, D3D11_SHADER_RESOURCE_VIEW_DESC > VShaderResourceView;

/// \brief
///   Typedef for encapsulating an unordered access view.
typedef VTypedResourceView< ID3D11UnorderedAccessView, D3D11_UNORDERED_ACCESS_VIEW_DESC > VUnorderedAccessView;

/// \brief
///   Typedef for encapsulating a depth stencil view.
typedef VTypedResourceView< ID3D11DepthStencilView, D3D11_DEPTH_STENCIL_VIEW_DESC > VDepthStencilView;

/// \brief
///   Typedef for encapsulating a render target view.
typedef VTypedResourceView< ID3D11RenderTargetView, D3D11_RENDER_TARGET_VIEW_DESC> VRenderTargetView;

typedef VSmartPtr< VShaderResourceView >  VShaderResourceViewPtr;
typedef VSmartPtr< VUnorderedAccessView > VUnorderedAccessViewPtr;
typedef VSmartPtr< VDepthStencilView >    VDepthStencilViewPtr;
typedef VSmartPtr< VRenderTargetView >    VRenderTargetViewPtr;


#endif  // defined( SUPPORTS_GPU_RESOURCE_VIEWS )


#endif // _VGPURESOURCEVIEW_HPP_INCLUDED

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
