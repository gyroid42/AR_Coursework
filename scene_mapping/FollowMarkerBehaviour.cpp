

#include "FollowMarkerBehaviour.h"
#include "Behaviour.h"
#include "GameObject.h"
#include "CameraFeed.h"


FollowMarkerBehaviour::FollowMarkerBehaviour() :
	Behaviour(),
	camera_feed_(nullptr)
{
}
FollowMarkerBehaviour::~FollowMarkerBehaviour() {
}



// Initialises behaviour
bool FollowMarkerBehaviour::Init(GameObject* newGameObject) {

	Behaviour::Init(newGameObject);

	behaviour_name_ = FOLLOW_MARKER_BEHAVIOUR;


	return true;
}


// Start method called when behaviour begins
bool FollowMarkerBehaviour::Start() {

	// Get camera feed and set default marker id
	camera_feed_ = CameraFeed::GetCameraFeed();
	marker_id_ = 0;

	return true;
}


// Update method called each frame
bool FollowMarkerBehaviour::Update(float dt) {

	// Update marker data from camera feed
	marker_ = camera_feed_->GetMarker(marker_id_);

	// Set whether object is visible
	if (marker_.first) {

		gameobject_->SetVisible(true);
	}
	else {

		gameobject_->SetVisible(false);
	}

	return true;
}


// Called when gameobject mesh is updated
bool FollowMarkerBehaviour::OnUpdateMesh() {

	// Apply marker transform to gameobject
	gameobject_->MultiplyTransformToLocal(marker_.second);
	return true;
}

