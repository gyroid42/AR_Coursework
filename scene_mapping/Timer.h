#pragma once

#ifndef _TIMER_H
#define _TIMER_H


class Timer 
{
public:
	Timer();
	~Timer();


	bool Update(float dt);


	bool IsFinished();
	inline float GetTime() { return timer_; }

	void SetTimer(float new_time, bool new_active = true);
	void SetActive(bool new_active);



private:


	float timer_;

	bool active_;

};


#endif
