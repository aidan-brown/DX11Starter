#include "Game.h"
#include "Vertex.h"
#include "Input.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true),			   // Show extra stats (fps) in title bar?
	vsync(false)
{
	camera = std::make_shared<Camera>((float)this->width / this->height, XMFLOAT3(0, 0, -1));
#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Note: Since we're using smart pointers (ComPtr),
	// we don't need to explicitly clean up those DirectX objects
	// - If we weren't using smart pointers, we'd need
	//   to call Release() on each DirectX object created in Game
	
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	LoadMeshes();

	// Sampler
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState;
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	device->CreateSamplerState(&samplerDesc, samplerState.GetAddressOf());

	// Skybox
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> skyboxSRV = CreateCubemap(
		GetFullPathTo_Wide(L"../../assets/textures/skybox/right.png").c_str(),
		GetFullPathTo_Wide(L"../../assets/textures/skybox/left.png").c_str(),
		GetFullPathTo_Wide(L"../../assets/textures/skybox/up.png").c_str(),
		GetFullPathTo_Wide(L"../../assets/textures/skybox/down.png").c_str(),
		GetFullPathTo_Wide(L"../../assets/textures/skybox/front.png").c_str(),
		GetFullPathTo_Wide(L"../../assets/textures/skybox/back.png").c_str()
	);
	sky = std::make_shared<Sky>(cube, skyboxSRV, skyVertexShader, skyPixelShader, samplerState, device);

	ambientColor = XMFLOAT3(0.0f, 0.0f, 0.0f);

	// Lights
	lights = std::vector<Light>();
	lights.push_back({
		LIGHT_TYPE_DIRECTIONAL,				//Type
		XMFLOAT3(0, 0, -1.0),		//Direction
		0,									//Range
		XMFLOAT3(0, 0, 0),			//Position
		1,									//Intensity
		XMFLOAT3(1.0, 1.0, 1.0),		//Color
		0,									//SpotFalloff
		XMFLOAT3(0, 0, 0),			//Padding
		});

	// Textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> starshipAlbedoSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/starship_albedo.png").c_str(), nullptr, starshipAlbedoSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> starshipEmissiveSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/starship_emissive.png").c_str(), nullptr, starshipEmissiveSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> starshipRoughSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/starship_roughness.png").c_str(), nullptr, starshipRoughSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> starshipMetalSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/starship_metallic.png").c_str(), nullptr, starshipMetalSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> starshipNormalSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/starship_normal.png").c_str(), nullptr, starshipNormalSRV.GetAddressOf());

	// Materials
	matStarship = std::make_shared<Material>(vertexShader, pixelShader, XMFLOAT4(1, 1, 1, 1), DirectX::XMFLOAT2(1, 1), DirectX::XMFLOAT2(0, 0));
	matStarship->AddTextureSRV(std::string("AlbedoMap"), starshipAlbedoSRV);
	matStarship->AddTextureSRV(std::string("EmissiveMap"), starshipEmissiveSRV);
	matStarship->AddTextureSRV(std::string("RoughMap"), starshipRoughSRV);
	matStarship->AddTextureSRV(std::string("MetalMap"), starshipMetalSRV);
	matStarship->AddTextureSRV(std::string("NormalMap"), starshipNormalSRV);
	matStarship->AddSampler(std::string("BasicSampler"), samplerState);

	// Game Entities
	gameEntities = std::vector<std::shared_ptr<GameEntity>>();
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(shuttle.get(), matStarship, XMFLOAT3(0.0f, 0.0f, 0.0f))));

	ResizeAllPostProcessResources();
	
	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	skyVertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyVertexShader.cso").c_str());
	skyPixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyPixelShader.cso").c_str());
	fullscreenVS = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"FullscreenVS.cso").c_str());
	gaussianBlurPS = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"GaussianBlurPS.cso").c_str());
	bloomExtractPS = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"BloomExtractPS.cso").c_str());
	bloomCombinePS = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"BloomCombinePS.cso").c_str());
}


void Game::LoadMeshes()
{
	cube = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/cube.obj").c_str(), device, context);
	cylinder = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/cylinder.obj").c_str(), device, context);
	helix = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/helix.obj").c_str(), device, context);
	quad = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/quad.obj").c_str(), device, context);
	quadDoubleSided = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/quad_double_sided.obj").c_str(), device, context);
	sphere = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/sphere.obj").c_str(), device, context);
	torus = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/torus.obj").c_str(), device, context);
	shuttle = std::make_shared<Mesh>(GetFullPathTo("../../assets/meshes/starship.obj").c_str(), device, context);
}

void Game::ResizeAllPostProcessResources()
{
	ResizeOnePostProcessResource(ppRTV, ppSRV, 1.0f, DXGI_FORMAT_R16G16B16A16_FLOAT);
	ResizeOnePostProcessResource(bloomExtractRTV, bloomExtractSRV, 0.5f, DXGI_FORMAT_R16G16B16A16_FLOAT);

	float rtScale = 0.5f;
	for (int i = 0; i < MaxBloomLevels; i++)
	{
		ResizeOnePostProcessResource(blurHorizontalRTV[i], blurHorizontalSRV[i], rtScale, DXGI_FORMAT_R16G16B16A16_FLOAT);
		ResizeOnePostProcessResource(blurVerticalRTV[i], blurVerticalSRV[i], rtScale, DXGI_FORMAT_R16G16B16A16_FLOAT);

		rtScale *= 0.5f;
	}
}

void Game::ResizeOnePostProcessResource(
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& rtv,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& srv,
	float renderTargetScale,
	DXGI_FORMAT format)
{
	rtv.Reset();
	srv.Reset();

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (unsigned int)(width * renderTargetScale);
	textureDesc.Height = (unsigned int)(height * renderTargetScale);
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // Will render to it and sample from it!
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = format;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	device->CreateRenderTargetView(ppTexture.Get(), &rtvDesc, rtv.ReleaseAndGetAddressOf());

	device->CreateShaderResourceView(ppTexture.Get(), 0, srv.ReleaseAndGetAddressOf());
}

void Game::BloomExtract()
{
	D3D11_VIEWPORT vp = {};
	vp.Width = width * 0.5f;
	vp.Height = height * 0.5f;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	context->OMSetRenderTargets(1, bloomExtractRTV.GetAddressOf(), 0);

	bloomExtractPS->SetShader();
	bloomExtractPS->SetShaderResourceView("pixels", ppSRV.Get());
	bloomExtractPS->SetFloat("bloomThreshold", bloomThreshold);
	bloomExtractPS->CopyAllBufferData();

	context->Draw(3, 0);
}

void Game::SingleDirectionBlur(float renderTargetScale, DirectX::XMFLOAT2 blurDirection, Microsoft::WRL::ComPtr<ID3D11RenderTargetView> target, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> sourceTexture)
{
	D3D11_VIEWPORT vp = {};
	vp.Width = width * renderTargetScale;
	vp.Height = height * renderTargetScale;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	context->OMSetRenderTargets(1, target.GetAddressOf(), 0);

	gaussianBlurPS->SetShader();
	gaussianBlurPS->SetShaderResourceView("pixels", sourceTexture.Get());
	gaussianBlurPS->SetFloat2("pixelUVSize", XMFLOAT2(1.0f / (width * renderTargetScale), 1.0f / (height * renderTargetScale)));
	gaussianBlurPS->SetFloat2("blurDirection", blurDirection);
	gaussianBlurPS->CopyAllBufferData();

	context->Draw(3, 0);
}

void Game::BloomCombine()
{
	D3D11_VIEWPORT vp = {};
	vp.Width = (float)width;
	vp.Height = (float)height;
	vp.MaxDepth = 1.0f;
	context->RSSetViewports(1, &vp);

	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), 0);

	bloomCombinePS->SetShader();
	bloomCombinePS->SetShaderResourceView("originalPixels", ppSRV.Get());
	bloomCombinePS->SetShaderResourceView("bloomedPixels0", blurVerticalSRV[0].Get());
	bloomCombinePS->SetShaderResourceView("bloomedPixels1", blurVerticalSRV[1].Get());
	bloomCombinePS->SetShaderResourceView("bloomedPixels2", blurVerticalSRV[2].Get());
	bloomCombinePS->SetShaderResourceView("bloomedPixels3", blurVerticalSRV[3].Get());
	bloomCombinePS->SetShaderResourceView("bloomedPixels4", blurVerticalSRV[4].Get());

	bloomCombinePS->SetFloat("intensityLevel0", bloomLevelIntensities[0]);
	bloomCombinePS->SetFloat("intensityLevel1", bloomLevelIntensities[1]);
	bloomCombinePS->SetFloat("intensityLevel2", bloomLevelIntensities[2]);
	bloomCombinePS->SetFloat("intensityLevel3", bloomLevelIntensities[3]);
	bloomCombinePS->SetFloat("intensityLevel4", bloomLevelIntensities[4]);
	bloomCombinePS->CopyAllBufferData();

	context->Draw(3, 0);
}

// --------------------------------------------------------
// Loads six individual textures (the six faces of a cube map), then
// creates a blank cube map and copies each of the six textures to
// another face.  Afterwards, creates a shader resource view for
// the cube map and cleans up all of the temporary resources.
// --------------------------------------------------------
Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Game::CreateCubemap(
	const wchar_t* right,
	const wchar_t* left,
	const wchar_t* up,
	const wchar_t* down,
	const wchar_t* front,
	const wchar_t* back)
{
	// Load the 6 textures into an array.
	// - We need references to the TEXTURES, not the SHADER RESOURCE VIEWS!
	// - Specifically NOT generating mipmaps, as we usually don't need them for the sky!
	// - Order matters here!  +X, -X, +Y, -Y, +Z, -Z
	ID3D11Texture2D* textures[6] = {};
	CreateWICTextureFromFile(device.Get(), right, (ID3D11Resource**)&textures[0], 0);
	CreateWICTextureFromFile(device.Get(), left, (ID3D11Resource**)&textures[1], 0);
	CreateWICTextureFromFile(device.Get(), up, (ID3D11Resource**)&textures[2], 0);
	CreateWICTextureFromFile(device.Get(), down, (ID3D11Resource**)&textures[3], 0);
	CreateWICTextureFromFile(device.Get(), front, (ID3D11Resource**)&textures[4], 0);
	CreateWICTextureFromFile(device.Get(), back, (ID3D11Resource**)&textures[5], 0);

	// We'll assume all of the textures are the same color format and resolution,
	// so get the description of the first shader resource view
	D3D11_TEXTURE2D_DESC faceDesc = {};
	textures[0]->GetDesc(&faceDesc);

	// Describe the resource for the cube map, which is simply 
	// a "texture 2d array".  This is a special GPU resource format, 
	// NOT just a C++ array of textures!!!
	D3D11_TEXTURE2D_DESC cubeDesc = {};
	cubeDesc.ArraySize = 6; // Cube map!
	cubeDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE; // We'll be using as a texture in a shader
	cubeDesc.CPUAccessFlags = 0; // No read back
	cubeDesc.Format = faceDesc.Format; // Match the loaded texture's color format
	cubeDesc.Width = faceDesc.Width;  // Match the size
	cubeDesc.Height = faceDesc.Height; // Match the size
	cubeDesc.MipLevels = 1; // Only need 1
	cubeDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; // A CUBE MAP, not 6 separate textures
	cubeDesc.Usage = D3D11_USAGE_DEFAULT; // Standard usage
	cubeDesc.SampleDesc.Count = 1;
	cubeDesc.SampleDesc.Quality = 0;

	// Create the actual texture resource
	ID3D11Texture2D* cubeMapTexture = 0;
	device->CreateTexture2D(&cubeDesc, 0, &cubeMapTexture);

	// Loop through the individual face textures and copy them,
	// one at a time, to the cube map texure
	for (int i = 0; i < 6; i++)
	{
		// Calculate the subresource position to copy into
		unsigned int subresource = D3D11CalcSubresource(
			0,	// Which mip (zero, since there's only one)
			i,	// Which array element?
			1); 	// How many mip levels are in the texture?

		// Copy from one resource (texture) to another
		context->CopySubresourceRegion(
			cubeMapTexture, // Destination resource
			subresource,	// Dest subresource index (one of the array elements)
			0, 0, 0,		// XYZ location of copy
			textures[i],	// Source resource
			0,	// Source subresource index (we're assuming there's only one)
			0);	// Source subresource "box" of data to copy (zero means the whole thing)
	}

	// At this point, all of the faces have been copied into the 
	// cube map texture, so we can describe a shader resource view for it
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = cubeDesc.Format; 	// Same format as texture
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE; // Treat this as a cube!
	srvDesc.TextureCube.MipLevels = 1;	// Only need access to 1 mip
	srvDesc.TextureCube.MostDetailedMip = 0; // Index of the first mip we want to see

	// Make the SRV
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeSRV;
	device->CreateShaderResourceView(cubeMapTexture, &srvDesc, cubeSRV.GetAddressOf());

	// Now that we're done, clean up the stuff we don't need anymore
	cubeMapTexture->Release();  // Done with this particular reference (the SRV has another)
	for (int i = 0; i < 6; i++)
		textures[i]->Release();

	// Send back the SRV, which is what we need for our shaders
	return cubeSRV;
}

// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	camera->UpdateProjectionMatrix((float)this->width / this->height);
	// Handle base-level DX resize stuff
	DXCore::OnResize();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Example input checking: Quit if the escape key is pressed
	if (Input::GetInstance().KeyDown(VK_ESCAPE))
		Quit();

	camera->Update(deltaTime);

	for (std::shared_ptr<GameEntity> ge : gameEntities)
	{
		ge->GetTransform()->Rotate(-0.01 * deltaTime, 0, -0.005 * deltaTime);
	}
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	{
		context->ClearRenderTargetView(ppRTV.Get(), color);
		context->ClearRenderTargetView(bloomExtractRTV.Get(), color);

		for (int i = 0; i < MaxBloomLevels; i++) {
			context->ClearRenderTargetView(blurHorizontalRTV[i].Get(), color);
			context->ClearRenderTargetView(blurVerticalRTV[i].Get(), color);
		}

		context->OMSetRenderTargets(1, ppRTV.GetAddressOf(), depthStencilView.Get());
	}


	// Ensure the pipeline knows how to interpret the data (numbers)
	// from the vertex buffer.  
	// - If all of your 3D models use the exact same vertex layout,
	//    this could simply be done once in Init()
	// - However, this isn't always the case (but might be for this course)
	context->IASetInputLayout(inputLayout.Get());

	// Draw the entity
	for (std::shared_ptr<GameEntity> ge : gameEntities)
	{
		ge->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], (sizeof(Light) * (int)lights.size()));
		ge->GetMaterial()->GetPixelShader()->SetInt("lightCount", (int)lights.size());
		ge->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientColor);
		ge->Draw(camera);
	}

	sky->Draw(context, camera);

	{
		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		ID3D11Buffer* nothing = 0;
		context->IASetIndexBuffer(0, DXGI_FORMAT_R32_UINT, 0);
		context->IASetVertexBuffers(0, 1, &nothing, &stride, &offset);

		fullscreenVS->SetShader();
		context->PSSetSamplers(0, 1, ppSampler.GetAddressOf());

		BloomExtract();

		if (bloomLevels >= 1) {
			float levelScale = 0.5f;
			SingleDirectionBlur(levelScale, XMFLOAT2(1, 0), blurHorizontalRTV[0], bloomExtractSRV);
			SingleDirectionBlur(levelScale, XMFLOAT2(0, 1), blurVerticalRTV[0], blurHorizontalSRV[0]);

			for (int i = 1; i < bloomLevels; i++) {
				levelScale *= 0.5f;
				SingleDirectionBlur(levelScale, XMFLOAT2(1, 0), blurHorizontalRTV[i], blurVerticalSRV[i - 1]);
				SingleDirectionBlur(levelScale, XMFLOAT2(0, 1), blurVerticalRTV[i], blurHorizontalSRV[i]);
			}
		}

		BloomCombine();

		ID3D11ShaderResourceView* nullSRVs[16] = {};
		context->PSSetShaderResources(0, 16, nullSRVs);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}