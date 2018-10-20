#include "CameraFeed.h"
//#include <sony_sample_framework.h>
#include <sony_tracking.h>


#include <graphics\sprite_renderer.h>
#include <platform\vita\graphics\texture_vita.h>
#include <graphics\sprite.h>
#include "MarkerManager.h"
//#include "ar_vita\common\sample_main.h"
#include <string>
#include "FontManager.h"

CameraFeed::CameraFeed() :
	marker_manager_(nullptr),
	camera_sprite_(nullptr),
	camera_texture_(nullptr)
{
}
CameraFeed::~CameraFeed()
{
}


// Set singleton pointer to null
CameraFeed* CameraFeed::camera_feed_ = nullptr;


// Initialise method
bool CameraFeed::Init() {


	// Create singleton of camera feed
	if (!camera_feed_) {
		
		camera_feed_ = this;
	}
	else if (camera_feed_ != this) {

		return false;
	}


	sampleInitialize();


	// set camera to off
	camera_on_ = false;

	// create camera texture
	camera_texture_ = new gef::TextureVita();

	// create image scale factor
	image_scaling_factor_ = (960.0f / 544.0f) / (640.0f / 480.0f);


	// create image sprite
	camera_sprite_ = new gef::Sprite();
	camera_sprite_->set_position(gef::Vector4(0.0f, 0.0f, -1.0f));
	camera_sprite_->set_width(2.0f);
	camera_sprite_->set_height(2.0f * image_scaling_factor_);
	camera_sprite_->set_texture((gef::Texture*)camera_texture_);


	// create marker manager
	marker_manager_ = new MarkerManager();
	marker_manager_->Init();


	gef::Font* text_font = FontManager::GetFontManager()->GetFont(COMIC_SANS);
	marker_detected_text_.Init(gef::Vector4(10.0f, 10.0f, -0.9f), 1.0f, 0xff00ff00, gef::TJ_LEFT, text_font, "MARKERS DETECTED | 1:%s | 2 : %s | 3 : %s | 4 : %s | 5 : %s | 6 : %s");

	return true;
}


bool CameraFeed::CleanUp() {

	// Delete resources used
	if (camera_texture_) {
		delete camera_texture_;
		camera_texture_ = nullptr;
	}

	if (camera_sprite_) {
		delete camera_sprite_;
		camera_sprite_ = nullptr;
	}

	if (marker_manager_) {

		marker_manager_->CleanUp();
		delete marker_manager_;
		marker_manager_ = nullptr;
	}

	if (camera_on_) {
		// TURN OFF CAMERA HERE
		sampleRelease();
	}

	// Set singleton to null
	camera_feed_ = nullptr;

	return true;
}


// Method to start up camera
bool CameraFeed::StartCamera() {

	if (!camera_on_) {

		//smartInitialize();
		//smartInitialize();

		sampleUpdate();
		

		//AppData* dat = sampleUpdateBegin();
		//smartTrackingReset();
		//sampleUpdateEnd(dat);

		camera_on_ = true;
	}

	return true;
}



// Updates camera feed
bool CameraFeed::UpdateCamera() {

	// Only update feed if camera is on
	if (camera_on_) {

		sampleUpdate();
		
		//smartUpdate(1, )

		//AppData* dat = sampleUpdateBegin();

		//smartUpdate(dat->currentImage);

		// check to see if a particular marker can be found
		marker_manager_->UpdateMarkers();

		//sampleUpdateEnd(dat);
	}

	return true;
}


// Renders camera feed
bool CameraFeed::Render(gef::SpriteRenderer* sprite_renderer) {


	// Only render when camera is on
	if (camera_on_) {

		sampleRenderBegin();

		//AppData* dat = sampleRenderBegin();
		//SceGxmTexture texture = sampleGetCameraImageTexture();

		if (sampleIsCameraImageReady()) {

			camera_texture_->set_texture(sampleGetCameraImageTexture());
			sprite_renderer->DrawSprite(*camera_sprite_);
		}
		sampleRenderEnd();

		//sampleRenderEnd();
	}

	return true;
}



// Returns marker 'i'
std::pair<bool, gef::Matrix44>& CameraFeed::GetMarker(int i) {

	return marker_manager_->GetMarker(i);
}




bool CameraFeed::GUI(gef::SpriteRenderer* sprite_renderer) {

	bool markersDetected[6];

	for (int i = 0; i < 6; i++) {

		if (marker_manager_->GetMarker(i).first) {

			markersDetected[i] = true;
			
		}
		else {
			markersDetected[i] = false;
		}
	}

	marker_detected_text_.Draw(sprite_renderer, (markersDetected[0])?"TRUE":"FALSE", (markersDetected[1]) ? "TRUE" : "FALSE", (markersDetected[2]) ? "TRUE" : "FALSE", (markersDetected[3]) ? "TRUE" : "FALSE", (markersDetected[4]) ? "TRUE" : "FALSE", (markersDetected[5]) ? "TRUE" : "FALSE");

	return true;
}

