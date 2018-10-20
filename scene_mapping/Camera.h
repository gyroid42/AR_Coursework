#pragma once

#ifndef _CAMERA_H
#define _CAMERA_H


#include <maths\vector4.h>
#include <maths\matrix44.h>


// forward declarations
class CameraFeed;

namespace gef {
	class Matrix44;
	class Vector4;
	class Platform;
	class Sprite;
	class TextureVita;
	class SpriteRenderer;
	class Renderer3D;
}


enum CAMERA_MODE {
	CAMERA_MODE_NORMAL,
	CAMERA_MODE_AR
};


// Camera class
// Used to create and store matrices for rendering
class Camera
{
public:
	Camera(gef::Platform& platform);
	~Camera();


	// Initialise/CleanUp methods
	bool Init();
	bool CleanUp();


	// Creates 3D projection matrix from camera feed
	bool InitCameraFeedProjection();
	bool InitCamera3DProjection();


	// Updates view matrix, if using camera_feed then view matrix is identity
	bool UpdateView();
	bool UpdateGameInputDirections();

	// Updates normal projection matrix i.e. not on using camera feed
	bool UpdateProjectionMatrix(gef::Platform& platform);


	// Sets matrices to renderers
	bool SetProjection(gef::Renderer3D* renderer_3d);
	bool SetView(gef::Renderer3D* renderer_3d);
	bool SetOrtho(gef::SpriteRenderer* sprite_renderer);


	// Getters for matrices
	inline gef::Matrix44 Projection_Matrix() { return projection_matrix_; }
	inline gef::Matrix44 View_Matrix() { return view_matrix_; }
	inline gef::Matrix44 Ortho_Matrix() { return ortho_matrix_; }



	inline gef::Vector4 Position() { return position_; }
	inline gef::Vector4 GetLookAt() { return lookat_; }
	inline gef::Vector4 GetForwardInput() { return game_forward_input_; }
	inline gef::Vector4 GetRightInput() { return game_right_input_; }

	// Static reference to camera
	inline static Camera* GetCamera() { return camera_; }

private:

	// Used to access camera in rest of application
	static Camera* camera_;


	gef::Platform& platform_;


	CAMERA_MODE mode_;

	// vectors to construct view matrix
	gef::Vector4 eye_, lookat_, up_;

	// floats to construct projection matrix
	float fov_, near_plane_, far_plane_;

	// matrices used for rendering
	gef::Matrix44 projection_matrix_, view_matrix_;
	gef::Matrix44 ortho_matrix_;

	gef::Vector4 position_;


	gef::Vector4 game_forward_input_;
	gef::Vector4 game_right_input_;

};

#endif