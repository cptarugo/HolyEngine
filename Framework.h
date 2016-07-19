#ifndef _FRAMEWORK_H
#define _FRAMEWORK_H


#include "FrameworkIncludes.h"


class Framework
{
public:
	Framework();
	~Framework();

	bool Iniciar();
	void Executar();

	void Update(float deltaTime);
	void Render();

	LRESULT CALLBACK EventosEngine(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

private:

	LPCWSTR nomeAplicacao;
	HINSTANCE instanciaJanela;
	HWND janelaMain;
	UINT m4xMsaaQualidade;
	Entrada* input;

	//
	//	Testes
	//
	Shader *shaderModelo;
	Model *modelo;
	XMFLOAT4X4 mView, mProj, mWorld;

	//
	//	Fundamentais para DirectX
	//
	ID3D11Device* md3dDevice;
	ID3D11DeviceContext* md3dImmediateContext;
	IDXGISwapChain* mSwapChain;
	ID3D11Texture2D* mDepthStencilBuffer;
	ID3D11RenderTargetView* mRenderTargetView;
	ID3D11DepthStencilView* mDepthStencilView;
	D3D11_VIEWPORT mScreenViewport;
	Timer gameTimer;

	//
	//	Rasterizers
	//
	ID3D11RasterizerState* solidRasterizer;
	ID3D11RasterizerState* wireframeRasterizer;
	



	D3D_DRIVER_TYPE md3dDriverType;
	bool mEnable4xMsaa;
	bool mAppPaused;
	bool mResizing;


	//
	//	Metodos
	//
	bool CriarJanela(LPCWSTR titulo, int x, int y, int width, int height);

	bool IniciarDirectX();

	bool OnResize();

	bool CriarRasterizers();


	void ComecoCena();
	void TrocarBuffer();
};



static LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
static Framework *appHandle = 0;

#endif