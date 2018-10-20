#pragma once

#ifndef _MARKER_MANAGER_H
#define _MARKER_MANAGER_H

#include <vector>


// Forward declarations
namespace gef {
	class Matrix44;
}


// Marker Manager Class
// Tracks Markers and stores their data
class MarkerManager
{
public:
	MarkerManager();
	~MarkerManager();


	// Initialise and CleanUp methods
	bool Init();
	bool CleanUp();


	// Returns data on marker 'i'
	std::pair<bool, gef::Matrix44>& GetMarker(int id);


	// Used to update marker data from camera feed
	bool UpdateMarkers();

private:

	// Vector of marker data
	std::vector<std::pair<bool, gef::Matrix44>> markers_;
};

#endif