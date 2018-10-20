#include "Input.h"

#include <input/touch_input_manager.h>
#include <input/input_manager.h>
#include <input\sony_controller_input_manager.h>


Input::Input() :
	input_manager_(nullptr)
{
}


Input::~Input()
{
}



Input* Input::input_ = nullptr;


bool Input::Init(gef::Platform& platform) {


	// Create singleton
	if (!input_) {

		input_ = this;
	}
	else if (input_ != this) {

		return false;
	}

	input_manager_ = gef::InputManager::Create(platform);

	return true;
}

bool Input::CleanUp() {

	if (input_manager_) {

		delete input_manager_;
		input_manager_ = nullptr;
	}

	return true;
}


bool Input::Update() {

	input_manager_->Update();

	return true;
}


const gef::SonyController* Input::GetController() {

	return input_manager_->controller_input()->GetController(0);
}
