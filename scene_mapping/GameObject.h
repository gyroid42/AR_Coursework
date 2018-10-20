#pragma once

#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#include <vector>
#include <graphics\mesh_instance.h>
#include <maths\quaternion.h>
#include <string>


// Forward declarations
class Behaviour;
enum BEHAVIOUR_NAME;

namespace gef {
	class Mesh;
	class MeshInstance;
	class Quaternion;
	class Vector4;
	class Matrix44;
	class SpriteRenderer;
	class Renderer3D;
}



// Base Gameobject class
// Used for basic functionality of every object in the game
class GameObject : public gef::MeshInstance
{
public:
	GameObject();
	virtual ~GameObject();


	// potentially use multithread farm for each step as they should be independant of each other //
	// add functionality for scripts to get other scripts // possibly use a map to store something searchable //
	// I think i've done number 2 gotta test so much crap ugh
	// consider a seperate enabaled and disabled behaviour lists
	// make globally accessed stuff thread safe

	// potential new ones////
	//virtual bool OnEnable();
	//virtual bool OnDisable();
	//virtual bool OnApplicationQuit();
	//////////////////////////

	// Init called when gameobject is created
	virtual bool Init();

	/////////////////////////////

	// Update methods /////

	// called each frame

	// Update is the main method used each frame
	virtual bool Update(float dt);

	// Late Update always called before update
	virtual bool EarlyUpdate(float dt);

	// Called straight after mesh transform's creation
	virtual void UpdateMesh();

	// Render Methods /////

	// Called before 3D objects are rendered
	virtual bool PreRenderGUI(gef::SpriteRenderer* sprite_renderer);
	
	// Called when game object is being rendered
	virtual bool PreRender(gef::Renderer3D* renderer_3d);
	virtual bool OnRender(gef::Renderer3D* renderer_3d);
	virtual bool PostRender(gef::Renderer3D* renderer_3d);

	// Called when GUI is being drawn
	virtual bool OnGUI(gef::SpriteRenderer* sprite_renderer);

	////////////////////////////////////

	// Collision methods /////

	// methods used when dealing with collisions with other game objects
	// "other" references object being collided with

	// OnEnterCollision called on frame collision begins
	virtual bool OnEnterCollision(GameObject* other);

	// OnCollision called each frame collision is occuring
	virtual bool OnCollision(GameObject* other);

	// OnExitCollision called on frame collision ends
	virtual bool OnExitCollision(GameObject* other);

	/////////////////////////////////////////

	// called when object is destroyed
	virtual bool OnDestroy();


	//////////////////////////////////////////


	// Methods for adding behaviours to game object
	void AddBehaviour(Behaviour* new_behaviour);
	void AddBehaviour(std::vector<Behaviour*> new_behaviours);// { behaviours_.insert(behaviours_.begin(), new_behaviours.begin(), new_behaviours.end()); }
	
	// Getter for behaviours on game object
	Behaviour* GetBehaviour(BEHAVIOUR_NAME name);


	/////////////////////////

	// Setters  //////////
	
	inline void SetName(std::string newName) { name_ = newName; }
	inline void SetPosition(gef::Vector4 newPos) { position_ = newPos; }
	inline void SetScale(gef::Vector4 newScale) { scale_ = newScale; }
	inline void SetRotation(gef::Quaternion newRot) { rotation_ = newRot; }
	inline void SetVelocity(gef::Vector4 newVel) { velocity_ = newVel; }
	inline void SetVisible(bool newValue) { visible_ = newValue; }
	inline void SetLocalTransform(gef::Matrix44 new_transform) { local_transform_ = new_transform; }
	inline void MultiplyTransformToLocal(gef::Matrix44 new_transform) { local_transform_ = local_transform_ * new_transform; }


	// Getters //////////////

	inline std::string Name() { return name_; }
	inline gef::Vector4 Position() { return position_; }
	inline gef::Vector4 Scale() { return scale_; }
	inline gef::Quaternion Rotation() { return rotation_; }
	inline gef::Vector4 Velocity() { return velocity_; }
	inline bool Visible() { return visible_; }
	inline gef::Matrix44 LocalTransform() { return local_transform_; }

protected:


	// Vector of behaviours the game object contains
	std::vector<Behaviour*> behaviours_;


	// local transform of game object
	gef::Matrix44 local_transform_;


	// velocity of game object
	gef::Vector4 velocity_;


	// position, scale and rotation of game object
	gef::Vector4 position_, scale_;
	gef::Quaternion rotation_;


	// flags for game object being destroyed/visible
	bool visible_;
	bool destroyed_;


	std::string name_;
};

#endif