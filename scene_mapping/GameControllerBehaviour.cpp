#include "GameControllerBehaviour.h"
#include "GameObjectManager.h"
#include "CameraFeed.h"
#include "FontManager.h"


GameControllerBehaviour::GameControllerBehaviour() :
	Behaviour(),
	gameobject_manager_(nullptr),
	camera_feed_(nullptr)
{
}
GameControllerBehaviour::~GameControllerBehaviour()
{
}


bool GameControllerBehaviour::Init(GameObject* newGameObject) {

	Behaviour::Init(newGameObject);

	behaviour_name_ = GAME_CONTROLLER_BEHAVIOUR;

	return true;
}



bool GameControllerBehaviour::Start() {

	gameobject_manager_ = GameObjectManager::GetGameObjectManager();
	camera_feed_ = CameraFeed::GetCameraFeed();


	gef::Font* text_font = FontManager::GetFontManager()->GetFont(COMIC_SANS);

	fps_text_.Init(gef::Vector4(850.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, text_font, "FPS: %.1f");
	scene_map_mode_text_.Init(gef::Vector4(10.0f, 485.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, text_font, "SM INIT MODE: %s");
	scene_map_state_text_.Init(gef::Vector4(10.0f, 510.0f, -0.9f), 1.0f, 0xffffffff, gef::TJ_LEFT, text_font, "SM STATE: %s");


	// Create objects for game here


	return true;
}


bool GameControllerBehaviour::Update(float dt) {


	fps_ = 1.0f / dt;


	scene_mapping_state_ = sampleGetSceneMappingState();
	scene_mapping_mode_ = sampleGetSceneMappingInitMode();


	return true;
}


bool GameControllerBehaviour::LateUpdate(float dt) {

	return true;
}


bool GameControllerBehaviour::OnGUI(gef::SpriteRenderer* sprite_renderer) {

	const char* scene_mode = "";

	switch (scene_mapping_mode_) {
	case SCE_SMART_SCENE_MAPPING_INIT_LEARNED_IMAGE:
		scene_mode = "LEARNED_IMAGE";
		break;
	case SCE_SMART_SCENE_MAPPING_INIT_WAAR:
		scene_mode = "WAAR";
		break;
	case SCE_SMART_SCENE_MAPPING_INIT_HFG:
		scene_mode = "HFG";
		break;
	case SCE_SMART_SCENE_MAPPING_INIT_VFG:
		scene_mode = "VFG";
		break;
	case SCE_SMART_SCENE_MAPPING_INIT_SFM:
		scene_mode = "SFM";
		break;
	case SCE_SMART_SCENE_MAPPING_INIT_NULL:
		scene_mode = "NULL";
		break;
	case SCE_SMART_SCENE_MAPPING_INIT_DRYRUN:
		scene_mode = "DRYRUN";
		break;
	}
	scene_map_mode_text_.Draw(sprite_renderer, scene_mode);

	const char* scene_state = "";

	switch (scene_mapping_state_) {
	case SCE_SMART_SCENE_MAPPING_STATE_IDLE:
		scene_state = "IDLE";
		break;
	case SCE_SMART_SCENE_MAPPING_STATE_SEARCH:
		scene_state = "SEARCH";
		break;
	case SCE_SMART_SCENE_MAPPING_STATE_SLAM:
		scene_state = "SLAM";
		break;
	case SCE_SMART_SCENE_MAPPING_STATE_LOCALIZE:
		scene_state = "LOCALIZE";
		break;
	case SCE_SMART_SCENE_MAPPING_STATE_LOCALIZE_IMPOSSIBLE:
		scene_state = "IMPOSSIBLE";
		break;
	}

	scene_map_state_text_.Draw(sprite_renderer, scene_state);


	fps_text_.Draw(sprite_renderer, fps_);

	return true;
}


bool GameControllerBehaviour::OnDestroy() {

	return true;
}
