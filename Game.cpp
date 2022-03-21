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
	camera = std::make_shared<Camera>((float)this->width / this->height, DirectX::XMFLOAT3(0, 0, -1));
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

	lights = std::vector<Light>();
	Light dl1 = { 
		LIGHT_TYPE_DIRECTIONAL,				//Type
		DirectX::XMFLOAT3(1.0, 0, 0),		//Direction
		0,									//Range
		DirectX::XMFLOAT3(0, 0, 0),			//Position
		1,									//Intensity
		DirectX::XMFLOAT3(1.0, 0, 0),		//Color
		0,									//SpotFalloff
		DirectX::XMFLOAT3(0, 0, 0),			//Padding
	};
	Light dl2 = {
		LIGHT_TYPE_DIRECTIONAL,				//Type
		DirectX::XMFLOAT3(0, -1.0, 0),		//Direction
		0,									//Range
		DirectX::XMFLOAT3(0, 0, 0),			//Position
		1,									//Intensity
		DirectX::XMFLOAT3(0, 1.0, 0),		//Color
		0,									//SpotFalloff
		DirectX::XMFLOAT3(0, 0, 0),			//Padding
	};
	Light dl3 = {
		LIGHT_TYPE_DIRECTIONAL,				//Type
		DirectX::XMFLOAT3(-1.0, 0, 0),		//Direction
		0,									//Range
		DirectX::XMFLOAT3(0, 0, 0),			//Position
		1,									//Intensity
		DirectX::XMFLOAT3(0, 0, 1.0),		//Color
		0,									//SpotFalloff
		DirectX::XMFLOAT3(0, 0, 0),			//Padding
	};
	Light pl1 = {
		LIGHT_TYPE_POINT,					//Type
		DirectX::XMFLOAT3(0, 0, 0),			//Direction
		5.0,								//Range
		DirectX::XMFLOAT3(5.5, 0, -2),		//Position
		1,									//Intensity
		DirectX::XMFLOAT3(0.5, 0, 0.5),		//Color
		0,									//SpotFalloff
		DirectX::XMFLOAT3(0, 0, 0),			//Padding
	};
	Light pl2 = {
		LIGHT_TYPE_POINT,					//Type
		DirectX::XMFLOAT3(0, 0, 0),			//Direction
		5.0,								//Range
		DirectX::XMFLOAT3(-5.5, 0, -2),		//Position
		1,									//Intensity
		DirectX::XMFLOAT3(0.5, 0, 0.5),		//Color
		0,									//SpotFalloff
		DirectX::XMFLOAT3(0, 0, 0),			//Padding
	};
	lights.push_back(dl1);
	lights.push_back(dl2);
	lights.push_back(dl3);
	lights.push_back(pl1);
	lights.push_back(pl2);

	matWhite = std::make_shared<Material>(DirectX::XMFLOAT4(1, 1, 1, 1), 0.5, vertexShader, pixelShader);
	matRed = std::make_shared<Material>(DirectX::XMFLOAT4(1, 0, 0, 1), 1, vertexShader, pixelShader);
	matGreen = std::make_shared<Material>(DirectX::XMFLOAT4(0, 1, 0, 1), 0.1, vertexShader, pixelShader);
	matBlue = std::make_shared<Material>(DirectX::XMFLOAT4(0, 0, 1, 1), 0.75, vertexShader, pixelShader);

	gameEntities = std::vector<std::shared_ptr<GameEntity>>();
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(cube.get(), matWhite, DirectX::XMFLOAT3(7.5f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(cylinder.get(), matWhite, DirectX::XMFLOAT3(5.0f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(helix.get(), matWhite, DirectX::XMFLOAT3(2.5f, -1.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(quad.get(), matWhite, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(quadDoubleSided.get(), matWhite, DirectX::XMFLOAT3(-2.5f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(sphere.get(), matWhite, DirectX::XMFLOAT3(-5.0f, 0.0f, 0.0f))));
	gameEntities.push_back(std::make_shared<GameEntity>(GameEntity(torus.get(), matWhite, DirectX::XMFLOAT3(-7.5f, 0.0f, 0.0f))));
	
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
	ambientColor = DirectX::XMFLOAT3(0.1f, 0.3f, 0.45f);
	vertexShader = std::make_shared<SimpleVertexShader>(device, context, GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"PixelShader.cso").c_str());
	customPixelShader = std::make_shared<SimplePixelShader>(device, context, GetFullPathTo_Wide(L"CustomPixelShader.cso").c_str());
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
		ge->GetTransform()->Rotate(-0.25 * deltaTime, -0.25 * deltaTime, 0);
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

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(vsync ? 1 : 0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}