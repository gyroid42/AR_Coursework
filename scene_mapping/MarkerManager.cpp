#include "MarkerManager.h"

//#include <sony_sample_framework.h>
#include <sony_tracking.h>
#include <maths\matrix44.h>



MarkerManager::MarkerManager()
{
}
MarkerManager::~MarkerManager()
{
}



// Init and CleanUp /////////

bool MarkerManager::Init() {

	for (int i = 0; i < 6; i++) {

		gef::Matrix44 identity;
		identity.SetIdentity();
		markers_.push_back(std::make_pair(false, identity));
	}

	return true;
}


bool MarkerManager::CleanUp() {


	return true;
}




// Returns marker of 'id'
std::pair<bool, gef::Matrix44>& MarkerManager::GetMarker(int id) {

	return markers_.at(id);
}



// Updates marker data
bool MarkerManager::UpdateMarkers() {


	// Loops for each marker and updates it's transform matrix
	for (int i = 0; i < markers_.size(); i++) {

		markers_.at(i).first = false;
		if (sampleTargetDetected(i)) {
		//if (sampleIsMarkerFound(i)) {

			markers_.at(i).first = true;
			sampleGetTargetTransform(
				i,
				&(markers_.at(i).second)
			);
			//sampleGetTransform(
			//	i,
			//	&(markers_.at(i).second)
			//);
		}
	}


	return true;
}
