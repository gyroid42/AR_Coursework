#pragma once

#ifndef _COLLISION_H
#define _COLLISION_H


// forward declarations 
namespace gef {
	class Mesh;
	class Scene;
	class MeshInstance;
}



// Collision class
// Static class containing methods for checking collisions between objects
class Collision
{
public:
	Collision();
	~Collision();


	// Methods for detecting collisions
	static bool SphereToSphere(gef::MeshInstance* mesh1, gef::MeshInstance* mesh2);
	static bool AABBtoAABB(gef::MeshInstance* mesh1, gef::MeshInstance* mesh2);

private:


};

#endif