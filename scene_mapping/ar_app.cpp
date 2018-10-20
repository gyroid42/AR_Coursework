#include "ar_app.h"

#include "ar_app_headers.h"



ARApp::ARApp(gef::Platform& platform) :
	Application(platform),
	sprite_renderer_(nullptr),
	renderer_3d_(nullptr),
	primitive_builder_(nullptr),
	primitive_renderer_(nullptr),
	input_(nullptr),
	font_manager_(nullptr),
	gameobject_manager_(nullptr),
	mesh_manager_(nullptr),
	level_manager_(nullptr),
	camera_(nullptr)
{
}

void ARApp::Init()
{
	//input_manager_ = gef::InputManager::Create(platform_);
	sprite_renderer_ = gef::SpriteRenderer::Create(platform_);
	renderer_3d_ = gef::Renderer3D::Create(platform_);
	primitive_builder_ = new PrimitiveBuilder(platform_);
	primitive_renderer_ = new PrimitiveRenderer(platform_);


	input_ = new Input();
	input_->Init(platform_);

	font_manager_ = new FontManager();
	font_manager_->Init();
	//InitFont();

	gameobject_manager_ = new GameObjectManager();
	gameobject_manager_->Init();
	mesh_manager_ = new MeshManager();
	mesh_manager_->Init(platform_);
	
	level_manager_ = new LevelManager(platform_);
	level_manager_->Init();
	level_manager_->ChangeLevel(TEST_LEVEL);

	camera_ = new Camera(platform_);
	camera_->Init();
	camera_->UpdateView();

	//camera_feed_ = new CameraFeed();
	//camera_feed_->Init();


	//camera_->InitCamera3DProjection();

	SetupLights();


	////// REMOVE THIS STUFF AT SOME POINT ////////////
	/*
	mesh_manager_->LoadScene(platform_, BALL1_SCENE, "ball1.scn");
	mesh_manager_->LoadMesh(platform_, BALL1, BALL1_SCENE);

	mesh_manager_->AddMesh(primitive_builder_->CreateSphereMesh(0.1f, 10, 10), BALL2);

	for (int i = 0; i < 6; i++) {

		GameObject* ball = new GameObject();
		ball->Init();// camera_feed_, i);


		ball->SetPosition(gef::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		gef::Quaternion rot;
		rot.Identity();
		ball->SetRotation(rot);
		ball->SetScale(gef::Vector4((float)(i + 1)*0.2f, (float)(i + 1)*0.2f, (float)(i + 1)*0.2f));
		//ball->SetScale(gef::Vector4((float)(0 + 1)*0.0002f, (float)(0 + 1)*0.0002f, (float)(0 + 1)*0.0002f, 1.0f));

		ball->set_mesh(mesh_manager_->GetMesh(BALL2));

		FollowMarkerBehaviour* followMarkerBehaviour = new FollowMarkerBehaviour();

		ball->AddBehaviour(followMarkerBehaviour);
		followMarkerBehaviour->SetMarker(i);


		gameobject_manager_->AddGameObject(ball);
	}


	GameObject* ball = new GameObject();
	ball->Init();// camera_feed_, i);


	ball->SetPosition(gef::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	gef::Quaternion rot;
	rot.Identity();
	ball->SetRotation(rot);
	ball->SetScale(gef::Vector4(1.0f, 1.0f, 1.0f));
	//ball->SetScale(gef::Vector4((float)(0 + 1)*0.0002f, (float)(0 + 1)*0.0002f, (float)(0 + 1)*0.0002f, 1.0f));

	//ball->set_mesh(mesh_manager_->GetMesh(BALL2));

	GameControllerBehaviour* newBehaviour = new GameControllerBehaviour();

	ball->AddBehaviour(newBehaviour);

	gameobject_manager_->AddGameObject(ball);
	*/
	/////////////////////////////////////////////////////////////////////////////

	// initialise sony framework
	//sampleInitialize();

}

void ARApp::CleanUp()
{
	if (primitive_renderer_) {
		delete primitive_renderer_;
		primitive_renderer_ = nullptr;
	}
	if (primitive_builder_) {
		delete primitive_builder_;
		primitive_builder_ = nullptr;
	}

	if (sprite_renderer_) {
		delete sprite_renderer_;
		sprite_renderer_ = nullptr;
	}

	if (renderer_3d_) {
		delete renderer_3d_;
		renderer_3d_ = nullptr;
	}

	if (input_) {
		input_->CleanUp();
		delete input_;
		input_ = nullptr;
	}

	if (font_manager_) {
		font_manager_->CleanUp();
		delete font_manager_;
		font_manager_ = nullptr;
	}

	if (gameobject_manager_) {
		gameobject_manager_->CleanUp();
		delete gameobject_manager_;
		gameobject_manager_ = nullptr;
	}

	if (mesh_manager_) {
		mesh_manager_->CleanUp();
		delete mesh_manager_;
		mesh_manager_ = nullptr;
	}

	if (level_manager_) {
		level_manager_->CleanUp();
		delete level_manager_;
		level_manager_ = nullptr;
	}
}

bool ARApp::Update(float frame_time)
{
	//fps_ = 1.0f / frame_time;

	level_manager_->Update();

	input_->Update();

	//input_manager_->Update();


	// Get this outta here //
	/*
	camera_feed_->UpdateCamera();

	const gef::SonyController* controller = input_->GetController();
	if (controller)
	{
		if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS) {
			camera_feed_->StartCamera();
		}
		if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE)
			sampleSceneMappingReset();
		if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE)
			sampleSceneMappingChangeMode();
	}*/

	////////////////////////////

	//sampleUpdate();


	gameobject_manager_->CheckCollisions();
	gameobject_manager_->EarlyUpdate(frame_time);
	gameobject_manager_->Update(frame_time);
	gameobject_manager_->UpdateMeshes();
	gameobject_manager_->DeleteObjects();


	return true;
}

void ARApp::Render()
{
	

	//
	// Render the camera feed
	//

	// REMEMBER AND SET THE PROJECTION MATRIX HERE

	camera_->SetOrtho(sprite_renderer_);
	camera_->SetView(renderer_3d_);
	camera_->SetProjection(renderer_3d_);
	camera_->UpdateView();

	sprite_renderer_->Begin(true);

	// DRAW CAMERA FEED SPRITE HERE
	//camera_feed_->Render(sprite_renderer_);
	gameobject_manager_->PreRenderGUI(sprite_renderer_);


	sprite_renderer_->End();

	//
	// Render 3D scene
	//

	// SET VIEW AND PROJECTION MATRIX HERE


	// Begin rendering 3D meshes, don't clear the frame buffer
	renderer_3d_->Begin(false);

	DrawSceneMappingDebugLines();

	// DRAW 3D MESHES HERE
	gameobject_manager_->Render(renderer_3d_);

	renderer_3d_->End();

	RenderOverlay();

}


void ARApp::DrawSceneMappingDebugLines()
{
	primitive_renderer_->Reset();

	// draw origin axes
	gef::Matrix44 origin_transform;
	origin_transform.SetIdentity();
	DrawAxes(primitive_renderer_, origin_transform, 0.059f);

	// draw landmarks
	int num_landmarks = sampleGetNumLandmarks();
	for (int landmark_num = 0; landmark_num < num_landmarks; ++landmark_num)
		DrawLandmark(primitive_renderer_, landmark_num);

	// marker cards
	const int marker_num = 0;
	if (sampleTargetDetected(marker_num))
	{
		gef::Matrix44 marker_transform;
		sampleGetTargetTransform(marker_num, &marker_transform);
		DrawAxes(primitive_renderer_, marker_transform, 0.059f*0.5f);
	}

	primitive_renderer_->Render(*renderer_3d_);
}

void ARApp::RenderOverlay()
{
	//
	// render 2d hud on top
	//
	gef::Matrix44 proj_matrix2d;

	proj_matrix2d = platform_.OrthographicFrustum(0.0f, platform_.width(), 0.0f, platform_.height(), -1.0f, 1.0f);
	sprite_renderer_->set_projection_matrix(proj_matrix2d);
	sprite_renderer_->Begin(false);
	DrawHUD();
	sprite_renderer_->End();
}

void ARApp::DrawHUD()
{

	gameobject_manager_->OnGUI(sprite_renderer_);

	
	//camera_feed_->GUI(sprite_renderer_);
}



void ARApp::SetupLights()
{
	gef::PointLight default_point_light;
	default_point_light.set_colour(gef::Colour(0.7f, 0.7f, 1.0f, 1.0f));
	default_point_light.set_position(gef::Vector4(-300.0f, -500.0f, 100.0f));

	gef::Default3DShaderData& default_shader_data = renderer_3d_->default_shader_data();
	default_shader_data.set_ambient_light_colour(gef::Colour(0.5f, 0.5f, 0.5f, 1.0f));
	default_shader_data.AddPointLight(default_point_light);
}

void ARApp::DrawAxes(PrimitiveRenderer* primitive_renderer, const gef::Matrix44& transform, const float size)
{
	gef::Vector4 x_axis(1.0f, 0.0f, 0.0f);
	gef::Vector4 y_axis(0.0f, 1.0f, 0.0f);
	gef::Vector4 z_axis(0.0f, 0.0f, 1.0f);
	gef::Vector4 origin(0.0f, 0.0, 0.0f);

	x_axis = x_axis * size;
	y_axis = y_axis * size;
	z_axis = z_axis * size;

	x_axis = x_axis.Transform(transform);
	y_axis = y_axis.Transform(transform);
	z_axis = z_axis.Transform(transform);
	origin = origin.Transform(transform);

	primitive_renderer->AddLine(origin, x_axis, gef::Colour(1.0f, 0.0f, 0.0f));
	primitive_renderer->AddLine(origin, y_axis, gef::Colour(0.0f, 1.0f, 0.0f));
	primitive_renderer->AddLine(origin, z_axis, gef::Colour(0.0f, 0.0f, 1.0f));
}

void ARApp::DrawLandmark(PrimitiveRenderer* primitive_renderer, int landmark_num)
{
	SceSmartSceneMappingLandmarkInfo landmark_info = sampleGetLandmarkInfo(landmark_num);

	const float landmark_half_size = 0.005f;

	gef::Vector4 landmark_pos(landmark_info.pos.x, landmark_info.pos.y, landmark_info.pos.z);

	gef::Colour landmark_colour(0.0f, 0.0f, 0.0f);

	switch (landmark_info.state)
	{
	case SCE_SMART_SCENE_MAPPING_LANDMARK_TRACKED:
		landmark_colour = gef::Colour(0.0f, 1.0f, 0.0f);		// Green
		break;
	case SCE_SMART_SCENE_MAPPING_LANDMARK_LOST:
		landmark_colour = gef::Colour(1.0f, 0.0f, 0.0f);		// Red
		break;
	case SCE_SMART_SCENE_MAPPING_LANDMARK_SUSPENDED:
		landmark_colour = gef::Colour(1.0f, 1.0f, 0.0f);		// Yellow
		break;
	case SCE_SMART_SCENE_MAPPING_LANDMARK_MASKED:
		landmark_colour = gef::Colour(0.0f, 0.0f, 1.0f);		// Blue
		break;
	}


	primitive_renderer->AddLine(gef::Vector4(landmark_pos.x() - landmark_half_size, landmark_pos.y(), landmark_pos.z()), gef::Vector4(landmark_pos.x() + landmark_half_size, landmark_pos.y(), landmark_pos.z()), landmark_colour);
	primitive_renderer->AddLine(gef::Vector4(landmark_pos.x(), landmark_pos.y() - landmark_half_size, landmark_pos.z()), gef::Vector4(landmark_pos.x(), landmark_pos.y() + landmark_half_size, landmark_pos.z()), landmark_colour);
	primitive_renderer->AddLine(gef::Vector4(landmark_pos.x(), landmark_pos.y(), landmark_pos.z() - landmark_half_size), gef::Vector4(landmark_pos.x(), landmark_pos.y(), landmark_pos.z() + landmark_half_size), landmark_colour);
}
