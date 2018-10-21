

#include "Timer.h"


Timer::Timer()
{
	timer_ = 0.0f;
	active_ = false;
}

Timer::~Timer()
{

}



bool Timer::Update(float dt) {

	if (active_) {

		if (timer_ > 0.0f) {

			timer_ -= dt;
		}

		if (timer_ <= 0.0f) {

			active_ = false;
			return true;
		}
	}

	return false;
}



bool Timer::IsFinished() {

	if (timer_ <= 0.0f) {

		return true;
	}

	return false;
}


void Timer::SetTimer(float new_time, bool new_active) {

	timer_ = new_time;
	SetActive(new_active);

	return;
}

void Timer::SetActive(bool new_active) {

	active_ = new_active;
}
