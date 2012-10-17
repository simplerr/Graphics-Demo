/*
	GWEN
	Copyright (c) 2010 Facepunch Studios
	See license in Gwen.h
*/

#ifndef GWEN_RENDERERS_DIRECTX10_H
#define GWEN_RENDERERS_DIRECTX10_H
#include "Gwen/Gwen.h"
#include "Gwen/BaseRender.h"
#include "d3dx11effect.h"
#include <d3d11.h>
#include <d3dx11.h>
#include "FW1FontWrapper.h"

#if defined(_DEBUG) | defined (DEBUG)
	#include <dxerr.h>
	#include <iostream>
	#pragma comment(lib, "dxerr") // needed for dxtrace
	#ifndef HR
	#define HR(x){																\
		HRESULT hr = (x);														\
		if(FAILED(hr)){															\
			std::cout<<"An error occured on line"<<(DWORD)__LINE__<<" in the file "<<__FILE__<<std::endl; \
			std::cout<<DXGetErrorStringA(hr)<<std::endl<<DXGetErrorDescriptionA(hr)<<std::endl;   \
		}																		\
	}
	#endif
	#ifndef OUTPUT_DEBUG_MSG // nothing special about this, I use the console for standard debug info
	#define OUTPUT_DEBUG_MSG(x) {												\
		std::cout<<x<<std::endl;													     	\
	}
	#endif
#else
	#ifndef HR
	#define HR(x) (x)
	#endif
	#ifndef OUTPUT_DEBUG_MSG
	#define OUTPUT_DEBUG_MSG(x) {}
	#endif
#endif 

struct VERTEXFORMAT2D
{
	FLOAT x, y;
	DWORD color;
	float u, v;
};


const std::string GwenUIShader = std::string(

"SamplerState LinearFilter {"
"	AddressU = CLAMP;"
"	AddressV = CLAMP;"
"	AddressW = CLAMP;"
"	Filter = MIN_MAG_MIP_LINEAR;"
"};"
"struct PSstruct {"
"	float4 position : SV_Position;"
"	float4 color    : COLOR;    "
"	float2 texcoord : TexCoord;"
"};"
"Texture2D UITexture;"
"PSstruct VS(float2 position : POSITION, float4 color: COLOR, float2 texcoord: TEXCOORD) { "
	"PSstruct Out;" 
	"Out.position = float4(position.xy, 1, 1);"
	"Out.color = color;"
    "Out.texcoord = texcoord;" 
    "return Out;" 
"}"
"float4 PSTex( PSstruct In ) : SV_Target0 {"
"    return UITexture.Sample( LinearFilter, In.texcoord );" 
"}" 
"float4 PSCol( PSstruct In ) : SV_Target0 {"
"    return In.color;" 
"}" 
" " 
"technique10 UITechniqueCol {" 
"    pass P0 {" 
"       SetVertexShader( CompileShader( vs_5_0, VS() ) );" 
"        SetGeometryShader( NULL );"
"        SetPixelShader( CompileShader( ps_5_0, PSCol() ) );" 
"    } "
"}"
"technique10 UITechniqueTex{" 
"    pass P0 {" 
"       SetVertexShader( CompileShader( vs_5_0, VS() ) );" 
"        SetGeometryShader( NULL );"
"        SetPixelShader( CompileShader( ps_5_0, PSTex() ) );" 
"    } "
"}"

);

namespace Gwen 
{
	namespace Renderer 
	{

		class GWEN_EXPORT DirectX11 : public Gwen::Renderer::Base
		{
			public:

				DirectX11( ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext );
				~DirectX11();

				virtual void Begin();
				virtual void End();
				virtual void Release();

				virtual void SetDrawColor(Gwen::Color color);

				virtual void DrawLine( int x, int y, int a, int b );
				virtual void DrawFilledRect( Gwen::Rect rect );

				virtual void LoadFont( Gwen::Font* pFont );
				virtual void FreeFont( Gwen::Font* pFont );
				virtual void RenderText( Gwen::Font* pFont, Gwen::Point pos, const Gwen::UnicodeString& text );
				virtual Gwen::Point MeasureText( Gwen::Font* pFont, const Gwen::UnicodeString& text );
				virtual Gwen::Color PixelColour( Gwen::Texture* pTexture, unsigned int x, unsigned int y, const Gwen::Color& col_default = Gwen::Color( 255, 255, 255, 255 ) );

				void StartClip();
				void EndClip();

				void DrawTexturedRect( Gwen::Texture* pTexture, Gwen::Rect pTargetRect, float u1=0.0f, float v1=0.0f, float u2=1.0f, float v2=1.0f );
				void LoadTexture( Gwen::Texture* pTexture );
				void FreeTexture( Gwen::Texture* pTexture );
				// set screensize should be called each time the screen size changes. This will allow the text to adjust itself to a change in the resolution.
				//Otherwise, the text will grow with the size of the window, and visa versa
				void SetScreenSize(float x, float y) { m_Wndx =x; m_Wndy =y; }
			protected:

				void*				m_pCurrentTexture;
				ID3D11Device*		m_pDevice;
				ID3D11DeviceContext* m_pDeviceContext;
				DWORD				m_Color;
				float				m_Wndx, m_Wndy;// screen size in pixels
				void Flush();
				void AddVert( int x, int y );
				void AddVert( int x, int y, float u, float v );

				static const int		MaxVerts = 1024;
				VERTEXFORMAT2D			m_pVerts[MaxVerts];
				int						m_iVertNum;
			
				ID3D11InputLayout*		m_pInputLayout;
				ID3DX11Effect*			m_pUIShader;
				ID3DX11EffectTechnique*	m_pUITechniqueCol, *m_pUITechniqueTex, *ActiveTechnique;
				ID3DX11EffectShaderResourceVariable* m_pUITexture;
				ID3D11Buffer*			m_pUIVertexBuffer;
				ID3D11BlendState*		m_pUILastBlendState, *m_pUIBlendState;
				ID3D11DepthStencilState* m_pUIDepthState;
				float					m_LastBlendFactor[4];
				UINT					m_LastBlendMask, m_LastStencilRef;
				ID3D11InputLayout*		m_LastInputLayout;
				D3D11_PRIMITIVE_TOPOLOGY m_LastTopology;
				ID3D11Buffer*			m_LastBuffers[8];
				UINT					m_LastStrides[8], m_LastOffsets[8];
				ID3D11PixelShader*		m_LastPSShader;
				ID3D11VertexShader*		m_LastVSShader;
				ID3D11GeometryShader*	m_LastGSShader;
				ID3D11DepthStencilState* m_LastDepthState;
				ID3D11RasterizerState*	m_pUILastRasterizerState, *m_pUIRasterizerStateScissor, *m_pUIRasterizerStateNormal;
				ID3D11Texture2D*		m_StagingTexture;
				Gwen::Font::List		m_FontList;

				// FW1FontWrapper
				IFW1Factory* m_pFW1Factory;
		};
	}
}
#endif
