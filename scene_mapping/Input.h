#pragma once


#ifndef _INPUT_H
#define _INPUT_H


#include <system\platform.h>

namespace gef {
	class SonyController;
	class InputManager;
}

class Input
{
public:
	Input();
	~Input();


	bool Init(gef::Platform& platform);
	bool CleanUp();
	bool Update();



	const gef::SonyController* GetController();

	inline static Input* GetInput() { return input_; }
	

private:


	static Input* input_;

	gef::InputManager* input_manager_;

};

#endif