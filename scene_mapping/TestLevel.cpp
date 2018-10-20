

#include "TestLevel.h"
#include "GameObjectManager.h"
#include "MeshManager.h"
#include "FontManager.h"
#include "BehaviourHeaders.h"
#include "primitive_builder.h"
#include "CameraFeed.h"
#include "Camera.h"
#include "GameObject.h"
#include "Player.h"


TestLevel::TestLevel(gef::Platform& platform) :
	Level(platform)
{
}

TestLevel::~TestLevel() {

}


bool TestLevel::Init() {

	return true;
}


bool TestLevel::CleanUp() {

	return true;
}


bool TestLevel::Load() {


	PrimitiveBuilder* primitive_builder = PrimitiveBuilder::GetPrimitiveBuilder();
	FontManager* font_manager = FontManager::GetFontManager();
	MeshManager* mesh_manager = MeshManager::GetMeshManager();
	GameObjectManager* gameobject_manager = GameObjectManager::GetGameObjectManager();

	mesh_manager->LoadScene(platform_, BALL1_SCENE, "ball1.scn");
	mesh_manager->LoadMesh(platform_, BALL1, BALL1_SCENE);

	mesh_manager->AddMesh(primitive_builder->CreateSphereMesh(0.1f, 10, 10), BALL2);
	
	font_manager->LoadFont(platform_, COMIC_SANS, "comic_sans");

	gef::Quaternion rotMeh;
	rotMeh.Identity();

	GameObject* camera_feed = new GameObject();
	camera_feed->Init();

	camera_feed->SetPosition(gef::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	camera_feed->SetRotation(rotMeh);
	camera_feed->SetScale(gef::Vector4(1.0f, 1.0f, 1.0f));

	CameraFeedBehaviour* camera_feed_behaviour = new CameraFeedBehaviour();
	camera_feed->AddBehaviour(camera_feed_behaviour);

	camera_feed->SetName("CameraFeed");

	gameobject_manager->AddGameObject(camera_feed);


	for (int i = 0; i < 6; i++) {

		GameObject* ball = new GameObject();
		ball->Init();

		ball->SetPosition(gef::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
		gef::Quaternion rot;
		rot.Identity();
		ball->SetRotation(rot);
		ball->SetScale(gef::Vector4((float)(i + 1)*0.2f, (float)(i + 1)*0.2f, (float)(i + 1)*0.2f));

		ball->set_mesh(mesh_manager->GetMesh(BALL2));

		FollowMarkerBehaviour* follow_marker_behaviour = new FollowMarkerBehaviour();

		ball->AddBehaviour(follow_marker_behaviour);
		follow_marker_behaviour->SetMarker(i);

		ball->SetName("Ball : " + i);

		gameobject_manager->AddGameObject(ball);
	}

	gef::Quaternion rotBall;
	rotBall.Identity();

	GameObject* ball = new GameObject();
	ball->Init();

	ball->SetPosition(gef::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
	ball->SetRotation(rotBall);
	ball->SetScale(gef::Vector4(1.0f, 1.0f, 1.0f));

	//ball->set_mesh(mesh_manager->GetMesh(BALL2));

	ball->SetName("GameController");

	GameControllerBehaviour* game_controller_behaviour = new GameControllerBehaviour();
	ball->AddBehaviour(game_controller_behaviour);
	gameobject_manager->AddGameObject(ball);

	gameobject_manager->AddGameObject(new Player());

	/*

	gef::Quaternion playerRot;
	playerRot.Identity();

	GameObject* player = new GameObject();
	player->Init();

	player->SetScale(gef::Vector4(0.2f, 0.2f, 0.2f));

	player->set_mesh(mesh_manager->GetMesh(BALL2));
	player->SetName("Player");

	PlayerMovementBehaviour* player_movement_behaviour = new PlayerMovementBehaviour();
	player->AddBehaviour(player_movement_behaviour);
	gameobject_manager->AddGameObject(player);


	*/


	


	return true;
}

bool TestLevel::Unload() {

	FontManager* font_manager = FontManager::GetFontManager();
	MeshManager* mesh_manager = MeshManager::GetMeshManager();
	GameObjectManager* gameobject_manager = GameObjectManager::GetGameObjectManager();

	font_manager->RemoveFont(COMIC_SANS);
	mesh_manager->RemoveMesh(BALL1);
	mesh_manager->RemoveScene(BALL1_SCENE);

	gameobject_manager->ClearObjects();


	return true;
}

