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

	// Lights
	lights = std::vector<Light>();
	Light dl1 = { 
		LIGHT_TYPE_DIRECTIONAL,				//Type
		XMFLOAT3(0, 0, -1.0),		//Direction
		0,									//Range
		XMFLOAT3(0, 0, 0),			//Position
		1,									//Intensity
		XMFLOAT3(1.0, 1.0, 1.0),		//Color
		0,									//SpotFalloff
		XMFLOAT3(0, 0, 0),			//Padding
	};
	lights.push_back(dl1);

	// Textures
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneAlbedoSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/stone-block-wall_albedo.png").c_str(), nullptr, stoneAlbedoSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneRoughSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/stone-block-wall_roughness.png").c_str(), nullptr, stoneRoughSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneMetalSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/stone-block-wall_metallic.png").c_str(), nullptr, stoneMetalSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> stoneNormalSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/stone-block-wall_normal.png").c_str(), nullptr, stoneNormalSRV.GetAddressOf());

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalDiffuseSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/ornate-celtic-gold_albedo.png").c_str(), nullptr, metalDiffuseSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalSpecSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/ornate-celtic-gold_roughness.png").c_str(), nullptr, metalSpecSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalRoughSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/ornate-celtic-gold_metallic.png").c_str(), nullptr, metalRoughSRV.GetAddressOf());
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> metalNormalSRV;
	CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../assets/textures/ornate-celtic-gold_normal.png").c_str(), nullptr, metalNormalSRV.GetAddressOf());

	// Materials
	matStone = std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), 0.5, vertexShader, pixelShader);
	matStone->AddTextureSRV(std::string("AlbedoMap"), stoneAlbedoSRV);
	matStone->AddTextureSRV(std::string("RoughMap"), stoneRoughSRV);
	matStone->AddTextureSRV(std::string("MetalMap"), stoneMetalSRV);
	matStone->AddTextureSRV(std::string("NormalMap"), stoneNormalSRV);
	matStone->AddSampler(std::string("BasicSampler"), samplerState);
	matMetal = std::make_shared<Material>(XMFLOAT4(1, 1, 1, 1), 0.5, vertexShader, pixelShader);
	matMetal->AddTextureSRV(std::string("AlbedoMap"), metalDiffuseSRV);
	matMetal->AddTextureSRV(std::string("RoughMap"), metalRoughSRV);
	matMetal->AddTextureSRV(std::string("MetalMap"), metalSpecSRV);
	matMetal->AddTextureSRV(std::string("NormalMap"), metalNormalSRV);
	matMetal->AddSampler(std::string("BasicSampler"), samplerState);

	// Game Entities
	gameEntities = std::vector<std::shared_ptr<GameEntity>>();
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(cube.get(), matStone, XMFLOAT3(7.5f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(cylinder.get(), matStone, XMFLOAT3(5.0f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(helix.get(), matStone, XMFLOAT3(2.5f, -1.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(quad.get(), matStone, XMFLOAT3(0.0f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(quadDoubleSided.get(), matMetal, XMFLOAT3(-2.5f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(sphere.get(), matMetal, XMFLOAT3(-5.0f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(torus.get(), matMetal, XMFLOAT3(-7.5f, 0.0f, 0.0f))));
	
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
	ambientColor = XMFLOAT3(0.04f, 0.06f, 0.1f);
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	skyVertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"SkyVertexShader.cso").c_str());
	skyPixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"SkyPixelShader.cso").c_str());
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
		ge->GetTransform()->Rotate(0, -0.25 * deltaTime, 0);
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


	// Ensure the pipeline knows how to interpret the data (numbers)
	// from the vertex buffer.  
	// - If all of your 3D models use the exact same vertex layout,
	//    this could simply be done once in Init()
	// - However, this isn't always the case (but might be for this course)
	context->IASetInputLayout(inputLayout.Get());

	// Draw the entity
	for (std::shared_ptr<GameEntity> ge : gameEntities)
	{
		ge->GetMaterial()->GetPixelShader()->SetFloat3("ambient", ambientColor);
		ge->GetMaterial()->GetPixelShader()->SetData("lights", &lights[0], (sizeof(Light) * (int)lights.size()));
		ge->Draw(camera);
	}

	sky->Draw(context, camera);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}