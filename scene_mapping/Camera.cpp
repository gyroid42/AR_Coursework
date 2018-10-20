#include "Camera.h"

#include <system\platform.h>
#include <graphics\sprite_renderer.h>



#include <maths\vector4.h>
#include <maths\matrix44.h>
#include <maths\math_utils.h>
#include <graphics\renderer_3d.h>
#include <graphics\sprite.h>
#include <platform\vita\graphics\texture_vita.h>
#include "CameraFeed.h"

//#include <sony_sample_framework.h>
#include <sony_tracking.h>

Camera::Camera(gef::Platform& platform) :
	platform_(platform),
	position_(gef::Vector4(0.0f, 0.0f, 0.0f))
{
}


Camera::~Camera()
{
}

Camera* Camera::camera_ = nullptr;


// Initialise Camera
bool Camera::Init() {


	// Create camera singleton
	if (!camera_) {

		camera_ = this;
	}
	else if (camera_ != this) {

		return false;
	}


	InitCamera3DProjection();

	ortho_matrix_.OrthographicFrustumGL(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);


	return true;
}

bool Camera::CleanUp() {


	camera_ = nullptr;
	return true;
}



// Creates 3D projection matrix for rendering objects ontop of camera feed
bool Camera::InitCameraFeedProjection() {

	if (!CameraFeed::GetCameraFeed()) {

		return false;
	}
	gef::Matrix44 perspective;
	perspective.PerspectiveFovGL((float)SCE_SMART_IMAGE_FOV, (float)SCE_SMART_IMAGE_WIDTH / (float)SCE_SMART_IMAGE_HEIGHT, 0.1f, 1000.0f);
	gef::Matrix44 scaleFactor;
	scaleFactor.SetIdentity();
	scaleFactor.SetRow(1, gef::Vector4(0.0f, (960.0f / 544.0f) / (640.0f / 480.0f), 0.0f, 0.0f));// CameraFeed::GetCameraFeed()->ImageScalingFactor(), 0.0f, 0.0f));
	projection_matrix_ = perspective * scaleFactor;
	
	mode_ = CAMERA_MODE_AR;

	return true;
}

bool Camera::InitCamera3DProjection() {

	// Setup vectors for projection matrix
	eye_ = gef::Vector4(300.0f, 300.0f, 300.0f);
	lookat_ = gef::Vector4(0.0f, 0.0f, 0.0f);
	up_ = gef::Vector4(0.0f, 1.0f, 0.0f);
	fov_ = gef::DegToRad(45.0f);
	near_plane_ = 0.01f;
	far_plane_ = 1000.0f;

	projection_matrix_ = platform_.PerspectiveProjectionFov(fov_, (float)platform_.width() / (float)platform_.height(), near_plane_, far_plane_);
	mode_ = CAMERA_MODE_NORMAL;

	return true;
}


bool Camera::UpdateProjectionMatrix(gef::Platform& platform) {


	// Create projection matrix
	return true;
}





// Called each frame
bool Camera::UpdateView() {


	// update view matrix look at
	switch (mode_) {
	case CAMERA_MODE_NORMAL:
		view_matrix_.LookAt(eye_, lookat_, up_);
		break;
	case CAMERA_MODE_AR:
		sampleGetViewMatrix(&view_matrix_);
		break;
	default:
		view_matrix_.SetIdentity();
		break;
	}
	//view_matrix_.LookAt(eye_, lookat_, up_);
	//view_matrix_.SetIdentity();
	//sampleGetViewMatrix(&view_matrix_);


	gef::Matrix44 temp;
	temp.AffineInverse(view_matrix_);
	position_ = temp.GetTranslation();
	lookat_ = temp.GetRow(2);


	UpdateGameInputDirections();

	return true;
}



bool Camera::UpdateGameInputDirections() {

	game_forward_input_ = lookat_ - position_;
	game_forward_input_.set_z(0.0f);
	game_forward_input_.Normalise();

	game_right_input_ = gef::Vector4(0.0f, 0.0f, 1.0f).CrossProduct(game_forward_input_);
	game_right_input_.Normalise();

	return true;
}


// Methods to set matrices in renderer
bool Camera::SetProjection(gef::Renderer3D* renderer_3d) {


	renderer_3d->set_projection_matrix(projection_matrix_);
	return true;
}
bool Camera::SetView(gef::Renderer3D* renderer_3d) {

	renderer_3d->set_view_matrix(view_matrix_);
	return true;
}
bool Camera::SetOrtho(gef::SpriteRenderer* sprite_renderer) {

	sprite_renderer->set_projection_matrix(ortho_matrix_);
	return true;
}
