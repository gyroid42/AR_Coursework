#include "CameraFeedBehaviour.h"

#include "CameraFeed.h"
#include "Camera.h"
#include "Input.h"
#include <input/sony_controller_input_manager.h>
#include <sony_tracking.h>



CameraFeedBehaviour::CameraFeedBehaviour() :
	Behaviour(),
	camera_feed_(nullptr),
	input_(nullptr)
{

}

CameraFeedBehaviour::~CameraFeedBehaviour() {

}


bool CameraFeedBehaviour::Init(GameObject* newGameObject) {

	Behaviour::Init(newGameObject);

	behaviour_name_ = CAMERA_FEED_BEHAVIOUR;

	return true;
}

bool CameraFeedBehaviour::CleanUp() {

	OnDestroy();

	return true;
}

bool CameraFeedBehaviour::Start() {


	camera_feed_ = new CameraFeed();
	camera_feed_->Init();

	Camera::GetCamera()->InitCameraFeedProjection();

	input_ = Input::GetInput();

	return true;
}



bool CameraFeedBehaviour::EarlyUpdate(float dt) {

	camera_feed_->UpdateCamera();

	const gef::SonyController* controller = input_->GetController();
	if (controller) {

		if (controller->buttons_pressed() & gef_SONY_CTRL_CROSS) {
			camera_feed_->StartCamera();
		}
		if (camera_feed_->IsOn()) {
			if (controller->buttons_pressed() & gef_SONY_CTRL_TRIANGLE) {
				sampleSceneMappingReset();
			}
			if (controller->buttons_pressed() & gef_SONY_CTRL_SQUARE) {
				sampleSceneMappingChangeMode();
			}
		}
	}

	return true;
}

bool CameraFeedBehaviour::PreRenderGUI(gef::SpriteRenderer* sprite_renderer) {

	camera_feed_->Render(sprite_renderer);

	return true;
}

bool CameraFeedBehaviour::OnGUI(gef::SpriteRenderer* sprite_renderer) {

	camera_feed_->GUI(sprite_renderer);
	return true;
}

bool CameraFeedBehaviour::OnDestroy() {

	if (camera_feed_) {
		camera_feed_->CleanUp();
		delete camera_feed_;
		camera_feed_ = nullptr;
	}

	Camera::GetCamera()->InitCamera3DProjection();

	return true;
}
