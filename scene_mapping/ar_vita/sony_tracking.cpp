#include <maths/matrix44.h>
#include <maths/quaternion.h>

#include "common/myvector.h"
#include "sony_tracking.h"

#ifdef __cplusplus
extern "C"
{
#endif

void sampleGetTargetRotPos(int idx, MyQuaternion* rot, MyVector* pos);
void sampleGetTargetTransform(int idx, MyMatrix* matrix);
void sampleGetViewRotPos(MyQuaternion* rot, MyVector* pos);


#ifdef __cplusplus
} // extern "C"
#endif


void sampleGetViewMatrix(gef::Matrix44* mat)
{
	if (mat)
	{
		gef::Matrix44 m0, m1, m2;
		gef::Vector4 v0;
		gef::Quaternion q0;

		MyQuaternion rot;
		MyVector pos;

		sampleGetViewRotPos( &rot, &pos);


		q0 = gef::Quaternion(myVectorGetX(&rot.v), myVectorGetY(&rot.v), myVectorGetZ(&rot.v), myVectorGetW(&rot.v));
		v0 = gef::Vector4(myVectorGetX(&pos), myVectorGetY(&pos), myVectorGetZ(&pos));

		m1.Rotation(q0);
		m2.SetIdentity();
		m2.SetTranslation(v0);
		*mat = m2 * m1;
	}
}

void sampleGetTargetTransform(
	int idx,
	gef::Matrix44* matrix)
{
	if (matrix)
	{
		gef::Matrix44 m0, m1, m2;
		gef::Vector4 v0;
		gef::Quaternion q0;

		MyQuaternion rot;
		MyVector pos;

		sampleGetTargetRotPos(idx, &rot, &pos);

		MyMatrix mt;
		sampleGetTargetTransform(idx, &mt);

		for (int i = 0; i < 4; i++)
		{
			matrix->set_m(i, 0, myVectorGetX(&mt.v[i]));
			matrix->set_m(i, 1, myVectorGetY(&mt.v[i]));
			matrix->set_m(i, 2, myVectorGetZ(&mt.v[i]));
			matrix->set_m(i, 3, myVectorGetW(&mt.v[i]));
		}

		q0.Conjugate(gef::Quaternion(myVectorGetX(&rot.v), myVectorGetY(&rot.v), myVectorGetZ(&rot.v), myVectorGetW(&rot.v)));
		v0 = gef::Vector4(myVectorGetX(&pos), myVectorGetY(&pos), myVectorGetZ(&pos)) * -1.0f;

		m1.Rotation(q0);
		m2.SetIdentity();
		m2.SetTranslation(v0);
		*matrix = m2 * m1;

		gef::Matrix44 view;
		sampleGetViewMatrix(&view);

		gef::Matrix44 inv_view;
		inv_view.Inverse(view);

		*matrix = *matrix * inv_view;
	}
}
