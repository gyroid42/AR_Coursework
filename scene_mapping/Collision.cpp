#include "Collision.h"

#include <graphics/mesh.h>
#include <graphics\mesh_instance.h>

Collision::Collision()
{
}


Collision::~Collision()
{
}



// Detects sphere to sphere collision
bool Collision::SphereToSphere(gef::MeshInstance* mesh1, gef::MeshInstance* mesh2)
{

	// get the sphere from each mesh
	gef::Sphere sphere1 = mesh1->mesh()->bounding_sphere().Transform(mesh1->transform());
	gef::Sphere sphere2 = mesh2->mesh()->bounding_sphere().Transform(mesh2->transform());

	// determine the distance between them and the sum of their radii
	float distance = (sphere1.position() - sphere2.position()).LengthSqr();
	float radiusCombined = sphere1.radius() + sphere2.radius();
	radiusCombined *= radiusCombined;


	// if distance between them is less than their combined radii then collision has occured
	if (distance < radiusCombined) {
		return true;
	}

	return false;
}


// detects box collision
bool Collision::AABBtoAABB(gef::MeshInstance* mesh1, gef::MeshInstance* mesh2) {

	gef::Aabb aabb1 = mesh1->mesh()->aabb().Transform(mesh1->transform());
	gef::Aabb aabb2 = mesh2->mesh()->aabb().Transform(mesh2->transform());
	

	if (aabb1.max_vtx().x() < aabb2.min_vtx().x() ||
		aabb1.max_vtx().y() < aabb2.min_vtx().y() ||
		aabb1.max_vtx().z() < aabb2.min_vtx().z() ||
		aabb2.max_vtx().x() < aabb1.min_vtx().x() ||
		aabb2.max_vtx().y() < aabb1.min_vtx().y() ||
		aabb2.max_vtx().z() < aabb1.min_vtx().z())
	{
		return false;
	}

	return true;
}

