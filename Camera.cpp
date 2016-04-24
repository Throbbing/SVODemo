#include"Camera.h"

Camera::Camera() :mPos(0.0f,0.0f,0.0f),
	mUp(0.0f,1.0f,0.0f),
	mRight(1.0f,0.0f,0.0f),
	mLook(0.0f,0.0f,1.0f)
{
	setLens(3.141592657f / 2, 1.0f, 1.0f, 1000.0f);
}

Camera::~Camera()
{
}

void Camera::setLens(float fovY, float aspect, float nearZ, float farZ)
{
	mFrustum.FovY = fovY;
	mFrustum.Aspect = aspect;
	mFrustum.NearZ = nearZ;
	mFrustum.FarZ = farZ;

	mFrustum.NearWindowHeight = 2 * nearZ*tanf(fovY*0.5);
	mFrustum.FarWindowHeight = 2 * farZ*tanf(fovY*0.5);

	mFrustum.NearWindowWidth = aspect*mFrustum.NearWindowHeight;
	mFrustum.FarWindowWidth = aspect*mFrustum.FarWindowHeight;

	XMMATRIX P = XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
	XMStoreFloat4x4(&mProj, P);
}

void Camera::updateMatrix() 
{
	XMVECTOR	pos = XMLoadFloat3(&mPos);
	XMVECTOR	look = XMLoadFloat3(&mLook);
	XMVECTOR	up = XMLoadFloat3(&mUp);
	XMVECTOR	right = XMLoadFloat3(&mRight);

	look = XMVector3Normalize(look);

	up=XMVector3Normalize(XMVector3Cross(look, right));

	right = XMVector3Normalize(XMVector3Cross(up, look));

	float x = -XMVectorGetX(XMVector3Dot(pos, right));
	float y = -XMVectorGetX(XMVector3Dot(pos, up));
	float z = -XMVectorGetX(XMVector3Dot(pos, look));

	XMStoreFloat3(&mPos, pos);
	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mUp, up);
	XMStoreFloat3(&mLook, look);

	mView(0, 0) = mRight.x;
	mView(1, 0) = mRight.y;
	mView(2, 0) = mRight.z;
	mView(3, 0) = x;

	mView(0, 1) = mUp.x;
	mView(1, 1) = mUp.y;
	mView(2, 1) = mUp.z;
	mView(3, 1) = y;

	mView(0, 2) = mLook.x;
	mView(1, 2) = mLook.y;
	mView(2, 2) = mLook.z;
	mView(3, 2) = z;

	mView(0, 3) = 0;
	mView(1, 3) = 0;
	mView(2, 3) = 0;
	mView(3, 3) = 1.0f;


}

void Camera::walk(float dt)
{

	XMVECTOR dis = XMVectorReplicate(dt);
	XMVECTOR pos = XMLoadFloat3(&mPos);

	XMVECTOR look = XMLoadFloat3(&mLook);

	XMStoreFloat3(&mPos, XMVectorMultiplyAdd(dis, look, pos));
}

void Camera::strafe(float dt)
{
	XMVECTOR dis = XMVectorReplicate(dt);
	XMVECTOR pos = XMLoadFloat3(&mPos);

	XMVECTOR right = XMLoadFloat3(&mRight);

	XMStoreFloat3(&mPos, XMVectorMultiplyAdd(dis, right, pos));
}

void Camera::pitch(float angle)
{
	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat3(&mRight), angle);

	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), r));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), r));

}

void Camera::yaw(float angle)
{
	XMMATRIX r = XMMatrixRotationAxis(XMLoadFloat3(&mUp), angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), r));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), r));
}

void Camera::rotateY(float angle)
{
	XMMATRIX r = XMMatrixRotationY(angle);

	XMStoreFloat3(&mRight, XMVector3TransformNormal(XMLoadFloat3(&mRight), r));
	XMStoreFloat3(&mLook, XMVector3TransformNormal(XMLoadFloat3(&mLook), r));
	XMStoreFloat3(&mUp, XMVector3TransformNormal(XMLoadFloat3(&mUp), r));
}

void Camera::setTarget(const XMFLOAT3& pos, const XMFLOAT3& target, const XMFLOAT3& up)
{
	mPos = pos;
	XMVECTOR p = XMLoadFloat3(&pos);
	XMVECTOR u = XMLoadFloat3(&up);
	XMVECTOR t = XMLoadFloat3(&target);
	XMVECTOR l = XMVector3Normalize(t - p);

	XMVECTOR r = XMVector3Cross(u, l);

	XMStoreFloat3(&mLook, XMVector3Normalize(l));
	XMStoreFloat3(&mUp, XMVector3Normalize(u)); 
	XMStoreFloat3(&mRight, XMVector3Normalize(r));

	
	
}