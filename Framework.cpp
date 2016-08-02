#include "Framework.h"
#include "Definicoes.h"
#include <assert.h>

Framework::Framework(void)
	:
	mEnable4xMsaa(true),
	mAppPaused(false),
	mResizing(false),
	m4xMsaaQualidade(0),
	md3dDriverType(D3D_DRIVER_TYPE_HARDWARE),
	md3dDevice(0),
	md3dImmediateContext(0),
	mSwapChain(0),
	mDepthStencilBuffer(0),
	mRenderTargetView(0),
	mDepthStencilView(0),
	angulo(0.0f),
	currRasterIndex(0)
{

	ZeroMemory(&mScreenViewport, sizeof(D3D11_VIEWPORT));

	

	appHandle = this;


	//
	//	Teste com modelos e shaders
	//
	
	input = new Entrada();
	mouse = new Mouse();
	camera = new Camera(mouse);
	shaderModelo = new Shader();
	modelo = new Model();
	modelo2 = new Model();

	
	XMMATRIX I = XMMatrixIdentity();
	modelo->SetWorldMatrix(I);
	XMStoreFloat4x4(&mWorld, I);

	modelo2->SetWorldMatrix(XMMatrixTranslation(0.0f, 2.0f, 0.0f));
	
	XMStoreFloat4x4(&mProj, XMMatrixPerspectiveFovLH(XM_PIDIV4, (float)JANELA_WIDTH / (float)JANELA_HEIGHT, 0.1f, 1000.0f));

	cgWorldViewProj = new constantBufferShader();
}


Framework::~Framework(void)
{
	//if (USE_FULLSCREEN)
	//	ChangeDisplaySettings(NULL, 0);
	


	ReleaseCOM(mSwapChain);
	ReleaseCOM(mDepthStencilBuffer);
	ReleaseCOM(pDepthStencilState);
	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);

	if (md3dImmediateContext)
		md3dImmediateContext->ClearState();

	ReleaseCOM(md3dImmediateContext);
	ReleaseCOM(md3dDevice);
	
	ReleaseCOM(currRaster);
	ReleaseCOM(solidRasterizer);
	ReleaseCOM(wireframeRasterizer);

	SafeDelete(shaderModelo);
	SafeDelete(modelo);
	SafeDelete(camera);
	SafeDelete(mouse);

	UnregisterClass(nomeAplicacao, instanciaJanela);
	instanciaJanela = NULL;
	janelaMain = NULL;
	appHandle = NULL;
}





bool Framework::Iniciar() {
	
	if(!CriarJanela(JANELA_NOME, JANELA_X, JANELA_Y, JANELA_WIDTH, JANELA_HEIGHT))
		return false;

	if (!IniciarDirectX())
		return false;

	if (!CriarRasterizers())
		return false;


	//
	//	Criando as luzes
	//
	// Directional light.
	dirLight.Ambiente = XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	dirLight.Difuso = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Especular = XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
	dirLight.Direcao = XMFLOAT3(0.57735f, -0.57735f, 0.57735f);
	// Point light--position is changed every frame to animate
	// in UpdateScene function.
	pointLight.Ambiente = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	pointLight.Difuso = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	pointLight.Especular = XMFLOAT4(0.7f, 0.7f, 0.7f, 1.0f);
	pointLight.Att = XMFLOAT3(0.0f, 0.1f, 0.0f);
	pointLight.Alcance = 25.0f;
	pointLight.Posicao.y = 5.0f;
	// Spot light--position and direction changed every frame to
	// animate in UpdateScene function.
	spotLight.Ambiente = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	spotLight.Difuso = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f);
	spotLight.Especular = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	spotLight.Att = XMFLOAT3(1.0f, 0.0f, 0.0f);
	spotLight.Spot = 96.0f;
	spotLight.Alcance = 10000.0f;

	Material mLandMat;
	mLandMat.Ambiente = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.Difuso = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
	mLandMat.Especular = XMFLOAT4(0.2f, 0.2f, 0.2f, 16.0f);
	modelo->SetMaterial(mLandMat);

	Material mSkullMat;
	mSkullMat.Ambiente = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mSkullMat.Difuso = XMFLOAT4(0.8f, 0.8f, 0.8f, 1.0f);
	mSkullMat.Especular = XMFLOAT4(0.8f, 0.8f, 0.8f, 16.0f);
	modelo2->SetMaterial(mSkullMat);

	Geometrias geoGen;
	geoGen.Height(320.0f, 320.0f, 100, 100, *modelo);
	modelo->Build(md3dDevice);
	modelo2->OpenBookTXT("car.txt", md3dDevice);

	currRaster = solidRasterizer;

	if (!shaderModelo->Iniciar(L"Vertex.hlsl", md3dDevice))
		return false;

	

	return true;
}





void Framework::Executar()
{
	MSG msg = { 0 };
	ZeroMemory(&msg, sizeof(msg));

	gameTimer.Reset();

	///Loop
	while (msg.message != WM_QUIT) {

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else 
		{
			//Executar o loop da engine aqui (Funcoes para renderizar e atualizar cenas, timer, etc)
			if (input->TeclaPressionada(VK_ESCAPE))
				break;

			gameTimer.Tick();

			if (!mAppPaused) {
				CalcularFPS();
				Update(gameTimer.DeltaTime());
				Render();
			}
			else 
				Sleep(100);
			
			
		}
	}
}



//
//	Funcao de update para cada frame
//
void Framework::Update(float deltaTime)
{
	camera->Update(deltaTime);

	if (input->TeclaPressionada(VK_F1)) {
		if (currRasterIndex == 0) {
			currRasterIndex = 1;
			currRaster = wireframeRasterizer;
		}
		else {
			currRasterIndex = 0;
			currRaster = solidRasterizer;
		}
	}



	//
	//	Atualizando a posicao da "lanterna"
	//
	// Circle light over the land surface.
	pointLight.Posicao.x = 70.0f*cosf(0.2f*gameTimer.TotalTime());
	pointLight.Posicao.z = 70.0f*sinf(0.2f*gameTimer.TotalTime());

	dirLight.Direcao = camera->ReturnGEyeW();

	spotLight.Posicao = camera->ReturnPosicao();
	spotLight.Direcao = camera->ReturnGEyeW();
}


//
//	Funcao para desenhar toda a frame
//
void Framework::Render()
{
	//TODO - Chamar o metodo render para todos os objetos da cena
	ComecoCena();


	//
	//	Inicializando o buffer per frame
	//
	cbPerFrameConst.gLuzDir = dirLight;
	cbPerFrameConst.gLuzFoco = spotLight;
	cbPerFrameConst.gLuzPt = pointLight;
	cbPerFrameConst.gPosOlhoW = camera->ReturnPosicao();
	cbPerFrameConst.pad = 0;

	shaderModelo->AtivarPerFrame(md3dImmediateContext, &cbPerFrameConst);

	////MODELO 1
	XMMATRIX world = modelo->GetWorldMatrix();//XMLoadFloat4x4(&mWorld);
	XMMATRIX view = camera->GetCameraView();
	XMMATRIX proj = XMLoadFloat4x4(&mProj);
	XMMATRIX WorldViewProj = world * view * proj;
	XMFLOAT4X4 gWorldViewProj;
	XMStoreFloat4x4(&gWorldViewProj, WorldViewProj);
	
	cgWorldViewProj->worldViewProj = gWorldViewProj;
	cgWorldViewProj->gMaterial = modelo->GetMaterial();																///
	XMStoreFloat4x4(&cgWorldViewProj->gWorld,modelo->GetWorldMatrix());												///
	XMStoreFloat4x4(&cgWorldViewProj->gWorldInvTranspose, MathHead::InverseTranspose(modelo->GetWorldMatrix()));	///

	modelo->Render(md3dImmediateContext, /*solidRasterizer*/currRaster);
	shaderModelo->AtivarPerObject(md3dImmediateContext, cgWorldViewProj);
	shaderModelo->Render(md3dImmediateContext, modelo->nModelIndex);
	////MODELO 1

	////MODELO 2
	world = modelo2->GetWorldMatrix();//XMLoadFloat4x4(&mWorld);
	view = camera->GetCameraView();
	WorldViewProj = world * view * proj;
	XMStoreFloat4x4(&gWorldViewProj, WorldViewProj);

	cgWorldViewProj->worldViewProj = gWorldViewProj;
	cgWorldViewProj->gMaterial = modelo2->GetMaterial();															///
	XMStoreFloat4x4(&cgWorldViewProj->gWorld, modelo2->GetWorldMatrix());											///
	XMStoreFloat4x4(&cgWorldViewProj->gWorldInvTranspose, MathHead::InverseTranspose(modelo2->GetWorldMatrix()));	///

	modelo2->Render(md3dImmediateContext, /*solidRasterizer*/currRaster);
	shaderModelo->AtivarPerObject(md3dImmediateContext, cgWorldViewProj);
	shaderModelo->Render(md3dImmediateContext, modelo2->nModelIndex);
	////MODELO 2


	TrocarBuffer();
}





LRESULT Framework::EventosEngine(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
	{
		input->SetTeclaPressionada((unsigned int)wParam);
	}
		break;

	case WM_KEYUP:
	{
		input->SetTeclaSolta((unsigned int)wParam);
	}
		break;


	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_MBUTTONDOWN:
		OnMouseDown(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MBUTTONUP:
		OnMouseUp(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_MOUSEMOVE:
		OnMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

	case WM_SIZE:
	{
		JANELA_WIDTH = LOWORD(lParam);
		JANELA_HEIGHT = HIWORD(lParam);
		if (md3dDevice)
			if (wParam == SIZE_RESTORED)
				if (!mResizing)
					OnResize();	
	}

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
		break;
	}
	return 0;
}





bool Framework::IniciarDirectX() {

	UINT deviceFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)  
	deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = D3D11CreateDevice(
		0,                 // default adapter
		md3dDriverType,
		0,                 // no software device
		deviceFlags,
		0, 0,              // default feature level array
		D3D11_SDK_VERSION,
		&md3dDevice,
		&featureLevel,
		&md3dImmediateContext);

	if (FAILED(hr)) {
		MessageBox(0, L"Falha ao criar o dispositivo.", 0, 0);
		return false;
	}

	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	hr = md3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQualidade);
	if (FAILED(hr))
		return false;
	
	assert(m4xMsaaQualidade > 0);

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = JANELA_WIDTH;
	sd.BufferDesc.Height = JANELA_HEIGHT;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// Use 4X MSAA? 
	if (mEnable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = m4xMsaaQualidade - 1;
	}
	// No MSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = janelaMain;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;


	IDXGIDevice* dxgiDevice = 0;
	hr = md3dDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
	if (FAILED(hr))
		return false;

	IDXGIAdapter* dxgiAdapter = 0;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgiAdapter);
	if (FAILED(hr))
		return false;

	IDXGIFactory* dxgiFactory = 0;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgiFactory);
	if (FAILED(hr))
		return false;

	hr = dxgiFactory->CreateSwapChain(md3dDevice, &sd, &mSwapChain);
	if (FAILED(hr))
		return false;

	IDXGIAdapter *enumeratedAdapter;
	DXGI_ADAPTER_DESC descricao;
	for (UINT i = 0; dxgiFactory->EnumAdapters(i, &enumeratedAdapter) != DXGI_ERROR_NOT_FOUND; ++i) {
		enumeratedAdapter->GetDesc(&descricao);
		///MessageBox(0, descricao.Description, L"Novo Adaptador Encontrado!", 0);
	}

	ReleaseCOM(enumeratedAdapter);
	ReleaseCOM(dxgiDevice);
	ReleaseCOM(dxgiAdapter);
	ReleaseCOM(dxgiFactory);



	if (!OnResize())
		return false;

	return true;
}




bool Framework::OnResize()
{

	XMMATRIX pProj = XMMatrixPerspectiveFovLH(XM_PIDIV4, JANELA_WIDTH / JANELA_HEIGHT, CLIP_PERTO, CLIP_LONGE);
	XMStoreFloat4x4(&mProj, pProj);


	assert(md3dImmediateContext);
	assert(md3dDevice);
	assert(mSwapChain);

	// Release the old views, as they hold references to the buffers we
	// will be destroying.  Also release the old depth/stencil buffer.

	ReleaseCOM(mRenderTargetView);
	ReleaseCOM(mDepthStencilView);
	ReleaseCOM(mDepthStencilBuffer);


	// Resize the swap chain and recreate the render target view.

	HRESULT hr = mSwapChain->ResizeBuffers(1, JANELA_WIDTH, JANELA_HEIGHT, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if (FAILED(hr)) {
		MessageBox(0, L"ResizeBuffers erro.", 0, 0);
		return false;
	}
	
	ID3D11Texture2D* backBuffer;
	hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	if (FAILED(hr)) {
		MessageBox(0, L"GetBuffer erro.", 0, 0);
		return false;
	}
	
	hr = md3dDevice->CreateRenderTargetView(backBuffer, 0, &mRenderTargetView);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateRenderTargetView erro.", 0, 0);
		return false;
	}
	ReleaseCOM(backBuffer);






	//Criando o depth stencil state
	D3D11_DEPTH_STENCIL_DESC dsDesc;
	ZeroMemory(&dsDesc, sizeof(dsDesc));
	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;


	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = md3dDevice->CreateDepthStencilState(&dsDesc, &pDepthStencilState);
	if (FAILED(hr)) {
		MessageBox(0, L"PORASPRASR", 0, 0);
		return false;
	}
	//--------------------

	///md3dImmediateContext->OMSetDepthStencilState(pDepthStencilState, 1);






	// Create the depth/stencil buffer and view.

	D3D11_TEXTURE2D_DESC depthStencilDesc;

	depthStencilDesc.Width = JANELA_WIDTH;
	depthStencilDesc.Height = JANELA_HEIGHT;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	// Use 4X MSAA? --must match swap chain MSAA values.
	if (mEnable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQualidade - 1;
	}
	// No MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = md3dDevice->CreateTexture2D(&depthStencilDesc, 0, &mDepthStencilBuffer);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateTexture2D erro.", 0, 0);
		return false;
	}
	
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = md3dDevice->CreateDepthStencilView(mDepthStencilBuffer, /*&depthStencilViewDesc*/0, &mDepthStencilView);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateDepthStencilView erro.", 0, 0);
		return false;
	}

	// Bind the render target view and depth/stencil view to the pipeline.

	md3dImmediateContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);


	// Set the viewport transform.

	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(JANELA_WIDTH);
	mScreenViewport.Height = static_cast<float>(JANELA_HEIGHT);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	md3dImmediateContext->RSSetViewports(1, &mScreenViewport);

	return true;
}



//
// Criando todos os rasterizers aqui
//
bool Framework::CriarRasterizers()
{

	//
	//	Criando o rasterizer para solidos
	//

	D3D11_RASTERIZER_DESC rdSolid;
	ZeroMemory(&rdSolid, sizeof(rdSolid));
	rdSolid.FillMode = D3D11_FILL_SOLID;
	rdSolid.CullMode = D3D11_CULL_BACK;
	rdSolid.FrontCounterClockwise = false;
	rdSolid.DepthClipEnable = true;

	HRESULT hr = md3dDevice->CreateRasterizerState(&rdSolid, &solidRasterizer);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateRasterizerState falhou.", L"Erro Fatal", 0);
		return false;
	}




	//
	//	Criando o rasterizer para wireframe
	//

	D3D11_RASTERIZER_DESC rdWireframe;
	ZeroMemory(&rdWireframe, sizeof(rdWireframe));
	rdWireframe.FillMode = D3D11_FILL_WIREFRAME;
	rdWireframe.CullMode = D3D11_CULL_BACK;
	rdWireframe.FrontCounterClockwise = false;
	rdWireframe.DepthClipEnable = true;

	hr = md3dDevice->CreateRasterizerState(&rdWireframe, &wireframeRasterizer);
	if (FAILED(hr)) {
		MessageBox(0, L"CreateRasterizerState falhou.", L"Erro Fatal", 0);
		return false;
	}

	return true;
}



//
//	Comeco de cena
//
void Framework::ComecoCena() {

	float cor[4];
	cor[0] = 0.0f;
	cor[1] = 0.2f;
	cor[2] = 0.4f;
	cor[3] = 1.0f;

	md3dImmediateContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	md3dImmediateContext->ClearRenderTargetView(mRenderTargetView, cor);
}


//
//	Fim de cena
//
void Framework::TrocarBuffer()
{
	if (VSYNC_ENABLED)
		mSwapChain->Present(1, 0);
	else
		mSwapChain->Present(0, 0);
}



bool Framework::CriarJanela(LPCWSTR titulo, int x, int y, int width, int height)
{
	WNDCLASSEX cj;
	
	nomeAplicacao = titulo;

	instanciaJanela = GetModuleHandle(NULL);

	//Criando a classe da janela
	cj.cbSize			= sizeof(WNDCLASSEX);
	cj.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	cj.lpfnWndProc		= MainWindowProc;
	cj.cbClsExtra		= 0;
	cj.cbWndExtra		= 0;
	cj.hInstance		= instanciaJanela;
	cj.hIcon			= LoadIcon(NULL, IDI_WINLOGO);
	cj.hIconSm			= cj.hIcon;
	cj.hCursor			= LoadCursor(NULL, IDC_ARROW);
	cj.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	cj.lpszMenuName		= NULL;
	cj.lpszClassName	= nomeAplicacao;

	//Registrando a classe
	if (!RegisterClassEx(&cj)) {
		MessageBox(0, L"Falha no registro da janela.", L"Erro", 0);
		return false;
	}

	int nStyle = WS_OVERLAPPEDWINDOW | WS_SYSMENU | WS_VISIBLE | WS_CAPTION | WS_MINIMIZEBOX;


	RECT R = { 0, 0, JANELA_WIDTH, JANELA_HEIGHT };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int widthT = R.right - R.left;
	int heightT = R.bottom - R.top;

	janelaMain = CreateWindowEx(WS_EX_APPWINDOW, nomeAplicacao, nomeAplicacao, nStyle, x, y, widthT, heightT, 0, 0, instanciaJanela, 0);

	if (!janelaMain) {
		MessageBox(0, L"CreatwWindowEx falhou.", L"Erro", 0);
		PostQuitMessage(0);
		return false;
	}

	ShowWindow(janelaMain, SW_SHOW);
	SetForegroundWindow(janelaMain);
	SetFocus(janelaMain);

	return true;
}







static LRESULT CALLBACK MainWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) {

	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);
		EndPaint(hwnd, &ps);
	}
	break;



	case WM_CLOSE:
	{
		PostQuitMessage(0);
		DestroyWindow(hwnd);
	}
	break;


	break;

	default:
	{
		return appHandle->EventosEngine(hwnd, message, wParam, lParam);
	}
	};

	return 0;
}



void Framework::CalcularFPS()
{
	// Code computes the average frames per second, and also the 
	// average time it takes to render one frame.  These stats 
	// are appended to the window caption bar.

	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((gameTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::wostringstream outs;
		outs.precision(6);
		outs << nomeAplicacao << L"    "
			<< L"FPS: " << fps << L"    "
			<< L"Frame Time: " << mspf << L" (ms)";
		SetWindowText(janelaMain, outs.str().c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}






//
//	Mouse functions
//

void Framework::OnMouseDown(WPARAM mouseButton, int x, int y) {
	
	mouse->lastMousePos.x = x;
	mouse->lastMousePos.y = y;

	SetCapture(janelaMain);
}

void Framework::OnMouseUp(WPARAM mouseButton, int x, int y) {
	ReleaseCapture();
}

void Framework::OnMouseMove(WPARAM mouseButton, int x, int y) {
	
	//
	//	Camera Rotate
	//
	if ((mouseButton & MK_LBUTTON) != 0) {
		mouse->dx = XMConvertToRadians(0.25f*static_cast<float>(x - mouse->lastMousePos.x));
		mouse->dy = XMConvertToRadians(0.25f*static_cast<float>(y - mouse->lastMousePos.y));

		camera->mTheta += mouse->dx;
		camera->mPhi += mouse->dy;

		camera->mPhi = Clamp(camera->mPhi, 0.1f, XM_PI - 0.1f);
	}

	//
	//	Camera Zoom
	//
	else if ((mouseButton & MK_RBUTTON) != 0)
	{
		// Make each pixel correspond to 0.005 unit in the scene.
		mouse->dx = 0.05f*static_cast<float>(x - mouse->lastMousePos.x);
		mouse->dy = 0.05f*static_cast<float>(y - mouse->lastMousePos.y);

		// Update the camera radius based on input.
		camera->raio += (mouse->dx - mouse->dy) * 2;

		// Restrict the radius.
		camera->raio = Clamp(camera->raio, 3.0f, 150.0f);
	}

	//
	//	Camera Pan
	//
	else if ((mouseButton & MK_MBUTTON) != 0) {	
		mouse->dx = 0.75f*static_cast<float>(mouse->lastMousePos.x - x);
		mouse->dy = 0.75f*static_cast<float>(y - mouse->lastMousePos.y);

		camera->IncPosition(mouse->dx, mouse->dy, 0);
	}

	mouse->lastMousePos.x = x;
	mouse->lastMousePos.y = y;
}