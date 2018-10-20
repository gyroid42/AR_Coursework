#pragma once

#ifndef _FOLLOW_MARKER_BEHAVIOUR_H
#define _FOLLOW_MARKER_BEHAVIOUR_H

#include <utility>
#include "Behaviour.h"
#include <maths\matrix44.h>


// forward declarations
class CameraFeed;
class GameObject;




// Follow Marker Behaviour
// Child of Behaviour class
// Tracks a marker and updates object position to follow it
class FollowMarkerBehaviour : public Behaviour {

public:
	FollowMarkerBehaviour();
	~FollowMarkerBehaviour();

	// initialise gameobject reference and behaviour name
	bool Init(GameObject* newGameObject);

	// Called when behaviour starts
	bool Start();

	// Called Each frame behaviour is enabled
	bool Update(float dt);

	// called after local transform has been calculated
	bool OnUpdateMesh();


	// setters
	inline void SetMarker(int new_id) { marker_id_ = new_id; }

private:


	// reference to camera feed to get marker data
	CameraFeed* camera_feed_;

	// id of marker this behaviour tracks
	int marker_id_;

	// stores marker data
	std::pair<bool, gef::Matrix44> marker_;
};



#endif