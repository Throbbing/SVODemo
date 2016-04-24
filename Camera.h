#pragma once
#ifndef Camera_H_
#define Camara_H_

#include<d3d11.h>
#include<xnamath.h>



class Camera
{
private:
	
	struct Frustum
	{
		float NearZ;
		float FarZ;
		float Aspect;
		float FovY;
		float NearWindowHeight;
		float NearWindowWidth;
		float FarWindowHeight;
		float FarWindowWidth;
	};
	XMFLOAT3	mPos;
	XMFLOAT3	mLook;
	XMFLOAT3	mUp;
	XMFLOAT3	mRight;

	Frustum		mFrustum;

	XMFLOAT4X4	mView;
	XMFLOAT4X4	mProj;
public:
	

	XMFLOAT3	getPos() const{ return mPos; }
	XMVECTOR	getPosXM() const { return XMLoadFloat3(&mPos); }

	XMFLOAT3	getLook() const { return mLook; }
	XMVECTOR	getLookXM() const { return XMLoadFloat3(&mLook); }

	XMFLOAT3	getUp() const { return mUp; }
	XMVECTOR	getUpXM() const { return XMLoadFloat3(&mUp); }

	XMFLOAT3	getRight() const { return mRight; }
	XMVECTOR	getRightXM() const { return XMLoadFloat3(&mRight); }

	Frustum		getFrustum() const { return mFrustum; }

	float		getNearZ() const { return mFrustum.NearZ; }
	float		getFarZ() const { return mFrustum.FarZ; }
	float		getFovY() const { return mFrustum.FovY; }
	float		getAspect() const { return mFrustum.Aspect; }

	float		getNearWindowHeight() const { return mFrustum.NearWindowHeight; }
	float		getNearWindowWidth()  const	{ return mFrustum.NearWindowWidth; }
	float		getFarWindowHeight() const { return mFrustum.FarWindowHeight; }
	float		getFarWindowWidth()	 const { return mFrustum.FarWindowWidth; }
	XMMATRIX	getView() const { return XMLoadFloat4x4(&mView); }
	XMMATRIX	getProj() const { return XMLoadFloat4x4(&mProj); }
	XMMATRIX	getViewProj() const { return getView()*getProj(); }

	void		setPos(float x, float y, float z){ mPos = XMFLOAT3(x, y, z); }
	void		setPos(const XMFLOAT3& pos)	{ mPos = pos; }

	void		setTarget(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up);

	//666666666666666666666666666666666666666666666666666
	Camera();
	~Camera();
	void		setLens(float fovY, float aspect, float nearZ, float farZ);
	

	void		updateMatrix();

	//look 方向平移
	void		walk(float dt);
	//right方向平移
	void		strafe(float dt);
	
	//绕right轴
	void		pitch(float angle);

	//绕up轴
	void		yaw(float angle);

	//绕y轴
	void		rotateY(float angle);
	


	
};



#endif // !Camera_H_
