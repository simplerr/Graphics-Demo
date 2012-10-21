#include <windows.h>
//#include "Gwen/Renderers/DirectX10.h"
#include "Gwen/Utility.h"
#include "Gwen/Font.h"
#include "Gwen/Texture.h"
#include <D3d9types.h>
#include <math.h>
#include <vector>
#include "DirectX11Renderer.h"
#include <d3dx9math.h>
#include <fstream>

#define SAFE_RELEASE(x) {if(x) x->Release(); x=0; }
#define RED   0x1
#define GREEN 0x2
#define BLUE  0x4
#define ALPHA 0x8
#define ALL (RED | GREEN | BLUE | ALPHA)

struct FontData
{
	IFW1FontWrapper*	pFont;
	int					iSpaceWidth;
};

namespace Gwen
{
	namespace Renderer
	{
		DirectX11::DirectX11( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext )
		{
			m_pDevice = pDevice;
			m_pDeviceContext = pDeviceContext;
			m_iVertNum = 0;
			m_pInputLayout=0;
			m_pUIShader=0;
			m_pUITechniqueCol=m_pUITechniqueTex=ActiveTechnique=0;
			m_pUITexture=0;
			m_pUIVertexBuffer=0;
			m_pUIBlendState=0;

			m_pUIRasterizerStateNormal=m_pUIRasterizerStateScissor=0;

			ID3D10Blob* effectBlob = NULL;
			ID3D10Blob* errorBlob = NULL;
			HR(D3DX11CompileFromMemory(GwenUIShader.c_str(), GwenUIShader.size(), NULL, NULL, NULL, NULL, "fx_5_0", NULL, NULL, NULL, &effectBlob, &errorBlob, NULL));
			HR(D3DX11CreateEffectFromMemory(effectBlob->GetBufferPointer(), effectBlob->GetBufferSize(), 0, m_pDevice, &m_pUIShader));
			effectBlob->Release();

			m_pUITechniqueCol = m_pUIShader->GetTechniqueByName("UITechniqueCol");
			m_pUITechniqueTex = m_pUIShader->GetTechniqueByName("UITechniqueTex");
			ActiveTechnique = m_pUITechniqueTex;
			m_pUITexture  = m_pUIShader->GetVariableByName("UITexture")->AsShaderResource();

			const D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0,		0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0,		8, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,       12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			int numElements = sizeof( layout ) / sizeof( layout[0] );
			D3DX11_PASS_DESC PassDesc;
			m_pUITechniqueCol->GetPassByIndex( 0 )->GetDesc( &PassDesc );
			HR( m_pDevice->CreateInputLayout( layout, numElements, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &m_pInputLayout ) );

			
			//create an empty, dynamic vertex buffer
			D3D11_BUFFER_DESC vbdesc;
			vbdesc.ByteWidth = MaxVerts * sizeof( VERTEXFORMAT2D );
			vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vbdesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			vbdesc.Usage = D3D11_USAGE_DYNAMIC;
			vbdesc.MiscFlags =0;
			HR(m_pDevice->CreateBuffer( &vbdesc, 0, &m_pUIVertexBuffer));
		

			// create a rast state to cull none and fill solid. Also create the scissor rast state
			D3D11_RASTERIZER_DESC descras;
			descras.CullMode = (D3D11_CULL_MODE) D3D11_CULL_NONE;
			descras.FillMode = (D3D11_FILL_MODE) D3D11_FILL_SOLID;
			descras.FrontCounterClockwise = FALSE;
			descras.DepthBias = 0;
			descras.DepthBiasClamp = 0.0f;
			descras.SlopeScaledDepthBias = 0.0f;
			descras.AntialiasedLineEnable = FALSE;
			descras.DepthClipEnable = FALSE;
			descras.MultisampleEnable = (BOOL) false;
			descras.ScissorEnable = (BOOL) false;
			HR(m_pDevice->CreateRasterizerState(&descras, &m_pUIRasterizerStateNormal));
			descras.ScissorEnable = true;
			HR(m_pDevice->CreateRasterizerState(&descras, &m_pUIRasterizerStateScissor));

			
			
			D3D11_BLEND_DESC blenddesc;
			blenddesc.AlphaToCoverageEnable = (BOOL) false;
			blenddesc.IndependentBlendEnable = (false);
			blenddesc.RenderTarget[0].BlendOp = (D3D11_BLEND_OP) D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[0].SrcBlend = (D3D11_BLEND) D3D11_BLEND_SRC_ALPHA;
			blenddesc.RenderTarget[0].DestBlend = (D3D11_BLEND) D3D11_BLEND_INV_SRC_ALPHA;
			blenddesc.RenderTarget[0].BlendOpAlpha = (D3D11_BLEND_OP) D3D11_BLEND_OP_ADD;
			blenddesc.RenderTarget[0].SrcBlendAlpha = (D3D11_BLEND) D3D11_BLEND_SRC_ALPHA;
			blenddesc.RenderTarget[0].DestBlendAlpha = (D3D11_BLEND) D3D11_BLEND_INV_SRC_ALPHA;
			blenddesc.RenderTarget[0].BlendEnable = true;
			blenddesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL ;
			HR(m_pDevice->CreateBlendState(&blenddesc, &m_pUIBlendState));

			D3D11_DEPTH_STENCIL_DESC depthdesc;
			memset(&depthdesc, 0, sizeof(depthdesc));
			depthdesc.DepthEnable = (BOOL) false;
			depthdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			depthdesc.DepthFunc = D3D11_COMPARISON_NEVER;
			depthdesc.StencilEnable = (BOOL) false;
			
			HR(m_pDevice->CreateDepthStencilState(&depthdesc, &m_pUIDepthState));

			FW1CreateFactory(FW1_VERSION, &m_pFW1Factory);
		}

		DirectX11::~DirectX11()
		{
			SAFE_RELEASE(m_pInputLayout);
			SAFE_RELEASE(m_pUIShader);
			SAFE_RELEASE(m_pUIVertexBuffer);
			SAFE_RELEASE(m_pUIBlendState);
			SAFE_RELEASE(m_pUIDepthState);
			SAFE_RELEASE(m_pUIRasterizerStateScissor);
			SAFE_RELEASE(m_pUIRasterizerStateNormal);
			//SAFE_RELEASE(m_StagingTexture);
		}

		void DirectX11::Begin()// get the states that this will change to set at the end call
		{
			ActiveTechnique=0;// set this to zero for this pass
			m_pDeviceContext->OMGetBlendState(&m_pUILastBlendState, m_LastBlendFactor, &m_LastBlendMask);
			m_pDeviceContext->RSGetState(&m_pUILastRasterizerState);
			m_pDeviceContext->OMGetDepthStencilState(&m_LastDepthState, &m_LastStencilRef);
			m_pDeviceContext->IAGetInputLayout(&m_LastInputLayout);
			m_pDeviceContext->IAGetPrimitiveTopology(&m_LastTopology);
			m_pDeviceContext->IAGetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);

			m_pDeviceContext->PSGetShader(&m_LastPSShader, 0, 0);
			m_pDeviceContext->GSGetShader(&m_LastGSShader, 0, 0);
			m_pDeviceContext->VSGetShader(&m_LastVSShader, 0, 0);

			float factor[4] = {0, 0, 0, 0};
			m_pDeviceContext->OMSetBlendState(m_pUIBlendState, factor, ~0);
			m_pDeviceContext->OMSetDepthStencilState(m_pUIDepthState, 0);
			m_pDeviceContext->RSSetState( m_pUIRasterizerStateNormal);
		}

		void DirectX11::End()// reset the device to its original state
		{
			Flush();
			m_pDeviceContext->OMSetBlendState(m_pUILastBlendState, m_LastBlendFactor, m_LastBlendMask);
			m_pDeviceContext->RSSetState(m_pUILastRasterizerState);
			m_pDeviceContext->OMSetDepthStencilState(m_LastDepthState, m_LastStencilRef);
			m_pDeviceContext->IASetInputLayout(m_LastInputLayout);
			m_pDeviceContext->IASetPrimitiveTopology(m_LastTopology);
			m_pDeviceContext->IASetVertexBuffers(0, 8, m_LastBuffers, m_LastStrides, m_LastOffsets);
			m_pDeviceContext->PSSetShader(m_LastPSShader, 0, 0);
			m_pDeviceContext->GSSetShader(m_LastGSShader, 0, 0);
			m_pDeviceContext->VSSetShader(m_LastVSShader, 0, 0);
		}

		void DirectX11::DrawLine( int x, int y, int a, int b )
		{
			Translate( x, y );
			Translate( a, b );	
		}

		void DirectX11::Flush()
		{
			if ( m_iVertNum > 0 )
			{
	
				//update the vertex buffer
				void* t = NULL;
				D3D11_MAPPED_SUBRESOURCE resource;
				HR( m_pDeviceContext->Map(m_pUIVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource));
				memcpy(resource.pData, m_pVerts , m_iVertNum * sizeof( VERTEXFORMAT2D ) );
				m_pDeviceContext->Unmap(m_pUIVertexBuffer, 0);
	
				// set topology to triangle strip
				m_pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
				m_pDeviceContext->IASetInputLayout(m_pInputLayout);
				UINT stride [] = { sizeof(VERTEXFORMAT2D) };
				UINT offset [] = { 0 };
				// set the vertex buffer
				m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pUIVertexBuffer, stride, offset);
				// apply the correct technique 
				ActiveTechnique->GetPassByIndex( 0 )->Apply(0, m_pDeviceContext);
				//draw
				m_pDeviceContext->Draw(m_iVertNum, 0);
				m_iVertNum = 0;
				
			}
		}

		void DirectX11::AddVert( int x, int y )
		{
			if ( m_iVertNum >= MaxVerts-1 )
			{
				Flush();
			}

			m_pVerts[ m_iVertNum ].x = (2.0f*(float)x/m_Wndx) -1.0f;
			m_pVerts[ m_iVertNum ].y = (-2.0f*(float)y/m_Wndy) +1.0f;
			m_pVerts[ m_iVertNum ].color = m_Color;

			m_iVertNum++;
		}

		void DirectX11::AddVert( int x, int y, float u, float v )
		{
			if ( m_iVertNum >= MaxVerts-1 )
			{
				Flush();
			}
			
			m_pVerts[ m_iVertNum ].x = (2.0f*(float)x/m_Wndx) -1.0f;
			m_pVerts[ m_iVertNum ].y = (-2.0f*(float)y/m_Wndy) +1.0f;
			m_pVerts[ m_iVertNum ].u = u;
			m_pVerts[ m_iVertNum ].v = v;
			
			m_pVerts[ m_iVertNum ].color = m_Color;

			m_iVertNum++;
		}

		void DirectX11::DrawFilledRect( Gwen::Rect rect )
		{
			if ( ActiveTechnique != m_pUITechniqueCol ){
				Flush();
				ActiveTechnique = m_pUITechniqueCol;

			}	
			
			Translate( rect );

			AddVert( rect.x, rect.y );
			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x, rect.y + rect.h );

			AddVert( rect.x+rect.w, rect.y );
			AddVert( rect.x+rect.w, rect.y+rect.h );
			AddVert( rect.x, rect.y + rect.h );
		}

		void DirectX11::SetDrawColor(Gwen::Color color)
		{
			m_Color = D3DCOLOR_ARGB( color.a, color.r, color.g, color.b );
		}

		void DirectX11::LoadFont( Gwen::Font* font )
		{
			m_FontList.push_back( font );
			// Scale the font according to canvas
			font->realsize = font->size * Scale();
			
			IFW1FontWrapper* fontHndl;
			HR(m_pFW1Factory->CreateFontWrapper(m_pDevice, font->facename.c_str(), &fontHndl));
	

			FontData*	pFontData = new FontData();
			memset(pFontData, 0, sizeof(FontData));
			pFontData->pFont = fontHndl;
			font->data = pFontData;
		}

		void DirectX11::FreeFont( Gwen::Font* pFont )
		{
			m_FontList.remove( pFont );

			if ( !pFont->data ) return;

			FontData* pFontData = (FontData*) pFont->data;

			if ( pFontData->pFont )
			{
				SAFE_RELEASE(pFontData->pFont);
			}

			delete pFontData;
			pFont->data = NULL;

		}

		void DirectX11::RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text )
		{
			Flush();

			// If the font doesn't exist, or the font size should be changed
			if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
			{
				FreeFont( pFont );
				LoadFont( pFont );
			}

			FontData* pFontData = (FontData*) pFont->data;
			
			Translate( pos.x, pos.y );

			RECT ClipRect = { pos.x, pos.y, 0, 0 };
			pFontData->pFont->DrawString(m_pDeviceContext, text.c_str(), pFont->size, pos.x, pos.y, m_Color, 0);
		}

		Gwen::Point DirectX11::MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text )
		{
			//// If the font doesn't exist, or the font size should be changed
			if ( !pFont->data || fabs( pFont->realsize - pFont->size * Scale() ) > 2 )
			{
				FreeFont( pFont );
				LoadFont( pFont );
			}

			FontData* pFontData = (FontData*) pFont->data;

			FW1_RECTF inputRect = {0, 0, 0, 0};
			FW1_RECTF dimensions = pFontData->pFont->MeasureString(text.c_str(), pFont->facename.c_str(), pFont->size, &inputRect, FW1_NOWORDWRAP);

			return Gwen::Point( (dimensions.Right - dimensions.Left) / Scale(), (dimensions.Bottom - dimensions.Top) / Scale() );
		}

		void DirectX11::StartClip()
		{
			Flush();

			m_pDeviceContext->RSSetState( m_pUIRasterizerStateScissor );

			const Gwen::Rect& rect = ClipRegion();

			D3D10_RECT r;

			r.left = ceil( ((float)rect.x) * Scale() );
			r.right = ceil(((float)(rect.x + rect.w)) * Scale()) + 1;
			r.top = ceil( (float)rect.y * Scale() );
			r.bottom = ceil( ((float)(rect.y + rect.h)) * Scale() ) + 1;
		
			m_pDeviceContext->RSSetScissorRects(1, &r );
		}

		void DirectX11::EndClip()
		{
			Flush();
			m_pDeviceContext->RSSetState(m_pUIRasterizerStateNormal);
		}

		void DirectX11::DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect rect, float u1, float v1, float u2, float v2 )
		{
			ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*) pTexture->data;

			// Missing image, not loaded properly?
			if ( !pImage )
			{
				return DrawMissingImage( rect );
			}

			Translate( rect );

			if ( ActiveTechnique != m_pUITechniqueTex )
			{
				Flush();
				ActiveTechnique = m_pUITechniqueTex;
			}	
			if( m_pCurrentTexture != pImage)
			{
				m_pUITexture->SetResource(pImage);
				m_pCurrentTexture =pImage;
			}
			AddVert( rect.x, rect.y,			u1, v1 );
			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x, rect.y + rect.h,	u1, v2 );

			AddVert( rect.x+rect.w, rect.y,		u2, v1 );
			AddVert( rect.x+rect.w, rect.y+rect.h, u2, v2 );
			AddVert( rect.x, rect.y + rect.h, u1, v2 );
			
		}


		Gwen::Color DirectX11::PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default )
		{
#pragma region Ugly color hack
			if(y == 508)
			{
				if(x == 4)
					return Gwen::Color(255, 255, 255, 255);
				else if(x == 12)
					return Gwen::Color(131, 131, 131, 255);
				else if(x == 20)
					return Gwen::Color(82, 82, 82, 255);
				else if(x == 28)
					return Gwen::Color(46, 114, 178, 255);
				else if(x == 36)
					return Gwen::Color(17, 102, 182, 255);
				else if(x == 44)
					return Gwen::Color(51, 148, 240, 255);
				else if(x == 52)
					return Gwen::Color(114, 114, 114, 255);
				else if(x == 60)
					return Gwen::Color(249, 249, 249, 255);
				else if(x == 68)
					return Gwen::Color(73, 73, 73, 255);
				else if(x == 76)
					return Gwen::Color(255, 0, 0, 255);
				else if(x == 84)
					return Gwen::Color(202, 209, 215, 255);
				else if(x == 92)
					return Gwen::Color(94, 102, 110, 255);
				else if(x == 100 || x == 108)
					return Gwen::Color(156, 156, 156, 255);
				else if(x == 116)
					return Gwen::Color(94, 102, 110, 0);
				else if(x == 124)
					return Gwen::Color(118, 199, 255, 255);
				else if(x == 132)
					return Gwen::Color(115, 115, 115, 255);
				else if(x == 140)
					return Gwen::Color(255, 255, 255, 255);
				else if(x == 148)
					return Gwen::Color(255, 255, 255, 51);
				else if(x == 156)
					return Gwen::Color(124, 124, 124, 255);
				else if(x == 164)
					return Gwen::Color(108, 108, 108, 255);
				else if(x == 172)
					return Gwen::Color(124, 124, 124, 255);
				else if(x == 180)
					return Gwen::Color(118, 199, 255, 26);
				else if(x == 188)
					return Gwen::Color(248, 220, 62, 219);
				else if(x == 196)
					return Gwen::Color(255, 255, 255, 255);
				else if(x == 204)
					return Gwen::Color(255, 255, 255, 3);
			}
			else if(y == 500)
			{
				if(x == 4)
					return Gwen::Color(255, 255, 255, 0);
				else if(x == 12)
					return Gwen::Color(131, 131, 131, 0);
				else if(x == 20)
					return Gwen::Color(255, 255, 255, 255);
				else if(x == 28)
					return Gwen::Color(82, 82, 82, 179);
				else if(x == 36)
					return Gwen::Color(87, 173, 255, 255);
				else if(x == 44)
					return Gwen::Color(82, 82, 82, 204);
				else if(x == 52)
					return Gwen::Color(87, 173, 255, 255);
				else if(x == 60)
					return Gwen::Color(82, 82, 82, 204);
				else if(x == 68)
					return Gwen::Color(0, 0, 0, 255);
				else if(x == 76)
					return Gwen::Color(255, 0, 0, 255);
				else if(x == 84)
					return Gwen::Color(51, 148, 240, 255);
				else if(x == 92)
					return Gwen::Color(255, 255, 255, 255);
				else if(x == 100)
					return Gwen::Color(156, 156, 156, 255);
				else if(x == 108)
					return Gwen::Color(67, 67, 67, 255); ///
				else if(x == 116)
					return Gwen::Color(118, 199, 255, 59);
				else if(x == 124)
					return Gwen::Color(210, 211, 212, 255);
				else if(x == 132)
					return Gwen::Color(168, 168, 168, 255);
				else if(x == 140)
					return Gwen::Color(255, 255, 255, 0);
				else if(x == 148)
					return Gwen::Color(255, 255, 255, 145);
				else if(x == 156)
					return Gwen::Color(124, 124, 124, 71); //
				else if(x == 164)
					return Gwen::Color(255, 255, 255, 255);
				else if(x == 172)
					return Gwen::Color(124, 124, 124, 0);
				else if(x == 180)
					return Gwen::Color(124, 124, 124, 255);
				else if(x == 188)
					return Gwen::Color(108, 108, 108, 255);
				else if(x == 196)
					return Gwen::Color(118, 199, 255, 26);
				else if(x == 204)
					return Gwen::Color(248, 220, 62, 219);
			}

#pragma endregion

			ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*) pTexture->data;
			if ( !pImage ) return col_default;

			ID3D11Texture2D *t;
			pImage->GetResource(reinterpret_cast<ID3D11Resource**>(&t));
			D3D11_TEXTURE2D_DESC desc;
			t->GetDesc(&desc);
			// this is how it must be done on DX10 and DX11. Textures cannot be used as a shader resource and have CPU read access.
			// the reason is to reduce dependencies for optimization reasons.
			//m_pDeviceContext->CopyResource(m_StagingTexture, t);

			// [NOTE] Doesn't get all mip map levels
			D3D11_MAPPED_SUBRESOURCE te;
			HR( m_pDeviceContext->Map(t, 0, D3D11_MAP_READ_WRITE, 0, &te));
			
			DWORD* pixels = (DWORD*)te.pData;
			D3DXCOLOR color = pixels[te.RowPitch / sizeof(DWORD) * y + x];
			m_pDeviceContext->Unmap(t, 0);
			t->Release();
		
			return Gwen::Color( color.r*255, color.g*255, color.b*255, color.a*255 );
		}
		void DirectX11::LoadTexture( Gwen::Texture* pTexture )
		{	
			std::ifstream file(pTexture->name.Get().c_str());
			if(!file) return;// missingfile, dont attempt to load
			file.close();
			// get the info from the file 
			D3DX11_IMAGE_INFO finfo;
			memset(&finfo, 0, sizeof(D3DX11_IMAGE_INFO));
			HR(D3DX11GetImageInfoFromFile(pTexture->name.Get().c_str(), 0, &finfo, 0));
		
			ID3D11ShaderResourceView *resource(0);
			HR(D3DX11CreateShaderResourceViewFromFile( m_pDevice, pTexture->name.Get().c_str(), 0, 0, &resource, 0));
			pTexture->data = resource;// store this instead of the ID3D10Texture2D because a ID3D10Texture2D object can be retrived from ID3D10ShaderResourceView by calling GetResource
			pTexture->width = finfo.Width;
			pTexture->height = finfo.Height;
		}

		void DirectX11::FreeTexture( Gwen::Texture* pTexture )
		{

			ID3D11ShaderResourceView* pImage = (ID3D11ShaderResourceView*) pTexture->data;
			if ( !pImage ) return;

			pImage->Release();
			pTexture->data = NULL;
			return;
		}

		void DirectX11::Release()
		{
			Font::List::iterator it = m_FontList.begin();

			while ( it != m_FontList.end() )
			{
				FreeFont( *it );
				it = m_FontList.begin();
			}
		}
	}
}