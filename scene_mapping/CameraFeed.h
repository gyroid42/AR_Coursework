#pragma once

#ifndef _CAMERA_FEED_H
#define _CAMERA_FEED_H


#include <utility>
#include <maths\matrix44.h>
#include "Text.h"

// forward declarations
class MarkerManager;

namespace gef {
	class Sprite;
	class SpriteRenderer;
	class TextureVita;
}



// Camera Feed Class
// Used to control vita camera
// Creates output to render camera
// Contains Marker Manager
class CameraFeed
{
public:
	CameraFeed();
	~CameraFeed();


	// Initialise and CleanUp methods
	bool Init();
	bool CleanUp();


	// Starts camera feed
	bool StartCamera();

	// Updates camera feed
	bool UpdateCamera();


	// Renders camera feed to whole screen
	bool Render(gef::SpriteRenderer* sprite_renderer);

	bool GUI(gef::SpriteRenderer* sprite_renderer);

	// Getters
	inline float ImageScalingFactor() { return image_scaling_factor_; }
	inline gef::TextureVita* FeedTexture() { return camera_texture_; }
	inline bool IsOn() { return camera_on_; }

	// returns marker 'i'
	std::pair<bool, gef::Matrix44>& GetMarker(int i);


	// static reference to camera feed
	inline static CameraFeed* GetCameraFeed() { return camera_feed_; }

private:

	// Used to access camera feed in rest of application
	static CameraFeed* camera_feed_;

	// Marker manager to detect and store location of markers
	MarkerManager* marker_manager_;

	// Sprite and texture used to render camera feed
	gef::Sprite* camera_sprite_;
	gef::TextureVita* camera_texture_;

	// Scaling factor to fix camera feed for displaying onto Vita screen
	float image_scaling_factor_;

	// bool to tell whether camera is turned on
	bool camera_on_;


	Text marker_detected_text_;
};

#endif