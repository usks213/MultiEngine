#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>


#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

using namespace DirectX::SimpleMath;
using namespace DirectX;


//using Vector2 = DirectX::SimpleMath::Vector2;    // 2�������������_�l�x�N�g��
//using Vector3 = DirectX::SimpleMath::Vector3;    // 3�������������_�l�x�N�g��
//using Vector4 = DirectX::SimpleMath::Vector4;    // 4�������������_�l�x�N�g��
//
//using Quaternion = DirectX::SimpleMath::Quaternion; // �N�H�[�^�j�I��
//using Matrix = DirectX::SimpleMath::Matrix;			// �s��
//using Color = DirectX::SimpleMath::Color;			// �F
//
//using Rect = DirectX::SimpleMath::Rectangle;		// ��`
//using Plane = DirectX::SimpleMath::Plane;			// ����
//using Ray = DirectX::SimpleMath::Ray;				// ���C
//using Viewport = DirectX::SimpleMath::Viewport;		// �r���[�|�[�g
//using Resolution = Vector2Int;                    // �𑜓x

using Int = int;								// 32�r�b�g�����t�������l
using Uint8 = unsigned char;					//  8�r�b�g�����Ȃ������l
using Uint16 = unsigned short;                  // 16�r�b�g�����Ȃ������l
using Uint32 = uint32_t;                        // 32�r�b�g�����Ȃ������l
using Int32 = int32_t;							// 32�r�b�g�����t�������l


class AABB {
public:
	Vector3 min;
	Vector3 max;

	AABB() :
		min(-0.5f, -0.5f, -0.5f),
		max(0.5f, 0.5f, 0.5f)
	{
	}

public:
	Vector3 center() const { return (max + min) * 0.5f; };
	Vector3 size() const { return max - min; };

	static void transformAABB(const Matrix& mWorld, const AABB& sourceAABB, AABB& outAABB) {
		Vector3 pos[8] = {
			// ���
			{ sourceAABB.max.x, sourceAABB.max.y, sourceAABB.max.z },
			{ sourceAABB.min.x, sourceAABB.max.y, sourceAABB.max.z },
			{ sourceAABB.max.x, sourceAABB.max.y, sourceAABB.min.z },
			{ sourceAABB.min.x, sourceAABB.max.y, sourceAABB.min.z },
			// ����
			{ sourceAABB.max.x, sourceAABB.min.y, sourceAABB.max.z },
			{ sourceAABB.min.x, sourceAABB.min.y, sourceAABB.max.z },
			{ sourceAABB.max.x, sourceAABB.min.y, sourceAABB.min.z },
			{ sourceAABB.min.x, sourceAABB.min.y, sourceAABB.min.z },
		};

		outAABB.max = Vector3(-FLT_MAX, -FLT_MAX, -FLT_MAX);
		outAABB.min = Vector3(FLT_MAX, FLT_MAX, FLT_MAX);

		for (auto& p : pos) {
			Vector3 wPos = p.Transform(p, mWorld);
			outAABB.max = outAABB.max.Max(outAABB.max, wPos);
			outAABB.min = outAABB.min.Min(outAABB.min, wPos);
		}
	}
};

class Frustum
{
public:
	/// @brief �R���X�g���N�^
	/// @param worldMatrix ���[���h�}�g���b�N�X
	/// @param fov FOV
	/// @param aspect �A�X�y�N�g��
	/// @param nearZ �j�A�N���b�v
	/// @param farZ �t�@�[�N���b�v
	explicit Frustum(const Matrix& worldMatrix, float fov, float aspect, float nearZ, float farZ)
	{
		CreateViewFrustum(fov, aspect, nearZ, farZ);
		UpdateViewFrustum(worldMatrix);
	}

	/// @brief �R���X�g���N�^
	/// @param screenDepth �ő�̐[��
	/// @param viewMatrix �r���[�}�g���b�N�X
	/// @param projectionMatrix �v���W�F�N�V�����}�g���b�N�X
	explicit Frustum(float screenDepth, Matrix& viewMatrix, Matrix& projectionMatrix)
	{
		Matrix projMatrix = projectionMatrix;
		float zMinimum = -projMatrix._43 / projMatrix._33;
		float r = screenDepth / (screenDepth - zMinimum);
		projMatrix._33 = r;
		projMatrix._43 = -r * zMinimum;

		Matrix matrix = XMMatrixMultiply(viewMatrix, projMatrix);
		float a, b, c, d;

		// near
		a = matrix._14 + matrix._13;
		b = matrix._24 + matrix._23;
		c = matrix._34 + matrix._33;
		d = matrix._44 + matrix._43;
		m_planes[0] = XMVectorSet(a, b, c, d);
		m_planes[0] = XMPlaneNormalize(m_planes[0]);

		// far
		a = matrix._14 - matrix._13;
		b = matrix._24 - matrix._23;
		c = matrix._34 - matrix._33;
		d = matrix._44 - matrix._43;
		m_planes[1] = XMVectorSet(a, b, c, d);
		m_planes[1] = XMPlaneNormalize(m_planes[1]);

		// left
		a = matrix._14 + matrix._11;
		b = matrix._24 + matrix._21;
		c = matrix._34 + matrix._31;
		d = matrix._44 + matrix._41;
		m_planes[2] = XMVectorSet(a, b, c, d);
		m_planes[2] = XMPlaneNormalize(m_planes[2]);

		// right
		a = matrix._14 - matrix._11;
		b = matrix._24 - matrix._21;
		c = matrix._34 - matrix._31;
		d = matrix._44 - matrix._41;
		m_planes[3] = XMVectorSet(a, b, c, d);
		m_planes[3] = XMPlaneNormalize(m_planes[3]);

		// top
		a = matrix._14 - matrix._12;
		b = matrix._24 - matrix._22;
		c = matrix._34 - matrix._32;
		d = matrix._44 - matrix._42;
		m_planes[4] = XMVectorSet(a, b, c, d);
		m_planes[4] = XMPlaneNormalize(m_planes[4]);

		// bottom
		a = matrix._14 + matrix._12;
		b = matrix._24 + matrix._22;
		c = matrix._34 + matrix._32;
		d = matrix._44 + matrix._42;
		m_planes[5] = XMVectorSet(a, b, c, d);
		m_planes[5] = XMPlaneNormalize(m_planes[5]);
	}

	~Frustum() = default;

public:
	bool CheckPoint(float x, float y, float z)
	{
		for (int i = 0; i < 6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(x, y, z, 1.0f)));
			if (ret < 0.0f)
				return false;
		}

		return true;
	}

	bool CheckCube(const Vector3& center, float size)
	{
		for (int i = 0; i < 6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y - size), (center.z - size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y - size), (center.z - size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y + size), (center.z - size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y + size), (center.z - size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y - size), (center.z + size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y - size), (center.z + size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size), (center.y + size), (center.z + size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size), (center.y + size), (center.z + size), 1.0f)));
			if (ret >= 0.0f)
				continue;

			return false;
		}

		return true;
	}

	bool CheckSphere(const Vector3& center, float radius)
	{
		for (int i = 0; i < 6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet(center.x, center.y, center.z, 1.0f)));
			if (ret < -radius)
				return false;
		}

		return true;
	}

	bool CheckRectangle(const Vector3& center, const Vector3& size)
	{
		for (int i = 0; i < 6; i++)
		{
			float ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y - size.y), (center.z - size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y - size.y), (center.z - size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y + size.y), (center.z - size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y - size.y), (center.z + size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y + size.y), (center.z - size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y - size.y), (center.z + size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x - size.x), (center.y + size.y), (center.z + size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			ret = XMVectorGetX(XMPlaneDotCoord(m_planes[i], XMVectorSet((center.x + size.x), (center.y + size.y), (center.z + size.z), 1.0f)));
			if (ret >= 0.0f)
				continue;

			return false;
		}

		return true;
	}

	bool CheckAABB(const AABB& aabb)
	{
		return CheckRectangle(aabb.center(), aabb.size());
	}

private:
	// �v���W�F�N�V�����ϊ��p�����[�^���王���䐶��
	void CreateViewFrustum(float fov, float aspect, float nearZ, float farZ)
	{
		// 0:�O 1:��� 2:�� 3:�E 4:�� 5:��
		float fTan =
			tanf(XMConvertToRadians(fov) * 0.5f);
		float fTan2 = fTan * aspect;
		m_planes[0] = XMFLOAT4(0.0f, 0.0f, 1.0f, -nearZ);
		m_planes[1] = XMFLOAT4(0.0f, 0.0f, -1.0f, farZ);
		m_planes[2] = XMFLOAT4(1.0f, 0.0f, fTan2, 0.0f);
		m_planes[3] = XMFLOAT4(-1.0f, 0.0f, fTan2, 0.0f);
		m_planes[4] = XMFLOAT4(0.0f, -1.0f, fTan, 0.0f);
		m_planes[5] = XMFLOAT4(0.0f, 1.0f, fTan, 0.0f);
		// 0�`3�𐳋K��
		for (int i = 0; i < 6; ++i) {
			m_planes[i].Normalize();
		}
	}

	// �J�����̃��[���h�}�g���b�N�X���王������ړ�
	void UpdateViewFrustum(const Matrix& worldMatrix)
	{
		// CalcWorldMatrix��m_mtxWorld���X�V�ς�O��
		XMMATRIX mW = XMLoadFloat4x4(&worldMatrix);
		mW = XMMatrixInverse(nullptr, mW);//�t�s��
		mW = XMMatrixTranspose(mW);//�]�u�s��
		// ���t�s��̓]�u�s������߂�̂�
		//   XMPlaneTransform�̎d�l!!
		for (int i = 0; i < 6; ++i) {
			m_planes[i] = XMPlaneTransform(m_planes[i], mW);
		}
	}

private:
	// 0:�O 1:��� 2:�� 3:�E 4:�� 5:��
	Plane m_planes[6];
};

namespace Mathf
{
	// ����
	inline float Dot(Vector3 vec1, Vector3 vec2)
	{
		float dot;

		dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;

		return dot;
	}

	// �O��
	inline Vector3 Cross(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec;
		vec.x = vec1.y * vec2.z - vec1.z * vec2.y;
		vec.y = vec1.z * vec2.x - vec1.x * vec2.z;
		vec.z = vec1.x * vec2.y - vec1.y * vec2.x;
		return vec;
	}

	// �O�_�O��
	inline Vector3 Cross(Vector3 center, Vector3 pos1, Vector3 pos2)
	{
		return Cross(pos1 - center, pos2 - center);
	}

	// �O��2D
	inline float Cross2D(Vector3 vec1, Vector3 vec2)
	{
		return vec1.x * vec2.y - vec1.y * vec2.x;
	}

	// �x�N�g���̒���
	inline float Length(Vector3 vec)
	{
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	// ��_�Ԃ̋���
	inline float Length(Vector3 vec1, Vector3 vec2)
	{
		Vector3 vec = vec1 - vec2;
		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
	}

	// �x�N�g���̐��K��
	inline Vector3 Normalize(Vector3 v)
	{
		Vector3 vec = v;
		float mag = Length(vec);
		if (!mag) return vec;
		vec.x /= mag;
		vec.y /= mag;
		vec.z /= mag;

		return vec;
	}

	// �ǂ���x�N�g��
	inline Vector3 WallScratchVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return (front - normal * Dot(front, normal));
	}

	// ���˃x�N�g��
	inline Vector3 WallReflectVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return (front - normal * Dot(front, normal) * 2.0f);
	}

	// �����x�N�g��
	inline Vector3 WallVerticalVector(Vector3 front, Vector3 normal)
	{
		normal.Normalize();
		return normal * Dot(front, normal);
	}

	// X����]
	inline Vector3 RotationX(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.y = vec.y * cosf(rad) + vec.z * sinf(rad);
		v.z = -vec.y * sinf(rad) + vec.z * cosf(rad);

		return v;
	}

	// Y����]
	inline Vector3 RotationY(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.x = vec.x * cosf(rad) - vec.z * sinf(rad);
		v.z = vec.x * sinf(rad) + vec.z * cosf(rad);

		return v;
	}

	// Z����]
	inline Vector3 RotationZ(const Vector3& vec, float angle)
	{
		float rad = DirectX::XMConvertToRadians(angle);
		Vector3 v = vec;

		v.x = vec.x * cosf(rad) + vec.y * sinf(rad);
		v.y = -vec.x * sinf(rad) + vec.y * cosf(rad);

		return v;
	}
}


//class Vector3
//{
//public:
//
//	Vector3() {
//		m_vec = { 0.0f, 0.0f, 0.0f };
//	}
//	Vector3(const Vector3& vec) {
//		m_vec = vec.m_vec;
//	}
//	Vector3(XMFLOAT3 vec) {
//		m_vec = vec;
//	}
//	Vector3(float x, float y, float z) {
//		m_vec.x = x;
//		m_vec.y = y;
//		m_vec.z = z;
//	}
//	~Vector3() {}
//
//	void operator = (Vector3 vec)
//	{
//		m_vec = vec.m_vec;
//	}
//	void operator = (XMFLOAT3 vec)
//	{
//		m_vec = vec;
//	}
//
//	void operator += (Vector3 vec)
//	{
//		m_vec.x += vec.m_vec.x;
//		m_vec.y += vec.m_vec.y;
//		m_vec.z += vec.m_vec.z;
//	}
//	void operator -= (Vector3 vec)
//	{
//		m_vec.x -= vec.m_vec.x;
//		m_vec.y -= vec.m_vec.y;
//		m_vec.z -= vec.m_vec.z;
//	}
//	void operator *= (Vector3 vec)
//	{
//		m_vec.x *= vec.m_vec.x;
//		m_vec.y *= vec.m_vec.y;
//		m_vec.z *= vec.m_vec.z;
//	}
//	void operator /= (Vector3 vec)
//	{
//		m_vec.x /= vec.m_vec.x;
//		m_vec.y /= vec.m_vec.y;
//		m_vec.z /= vec.m_vec.z;
//	}
//
//	void operator *= (float vec)
//	{
//		m_vec.x *= vec;
//		m_vec.y *= vec;
//		m_vec.z *= vec;
//	}
//	void operator /= (float vec)
//	{
//		m_vec.x /= vec;
//		m_vec.y /= vec;
//		m_vec.z /= vec;
//	}
//
//	Vector3 operator + (Vector3 vec)
//	{
//		Vector3 v = *this;
//		v += vec;
//		return v;
//	}
//	Vector3 operator - (Vector3 vec)
//	{
//		Vector3 v = *this;
//		v -= vec;
//		return v;
//	}
//	Vector3 operator * (Vector3 vec)
//	{
//		Vector3 v = *this;
//		v *= vec;
//		return v;
//	}
//	Vector3 operator / (Vector3 vec)
//	{
//		Vector3 v = *this;
//		v /= vec;
//		return v;
//	}
//
//	Vector3 operator * (float vec)
//	{
//		Vector3 v = *this;
//		v *= vec;
//		return v;
//	}
//	Vector3 operator / (float vec)
//	{
//		Vector3 v = *this;
//		v /= vec;
//		return v;
//	}
//
//	XMFLOAT3* operator -> ()
//	{
//		return &m_vec;
//	}
//
//
//	// ����
//	static float Dot(Vector3 vec1, Vector3 vec2)
//	{
//		float dot;
//
//		dot = vec1.x * vec2.x + vec1.y * vec2.y + vec1.z * vec2.z;
//
//		return dot;
//	}
//
//	// �O��
//	static Vector3 Cross(Vector3 vec1, Vector3 vec2)
//	{
//		Vector3 vec;
//		vec.x = vec1.y*vec2.z - vec1.z*vec2.y;
//		vec.y = vec1.z*vec2.x - vec1.x*vec2.z;
//		vec.z = vec1.x*vec2.y - vec1.y*vec2.x;
//		return vec;
//	}
//
//	// �O�_�O��
//	static Vector3 Cross(Vector3 center, Vector3 pos1, Vector3 pos2)
//	{
//		return Cross(pos1 - center, pos2 - center);
//	}
//
//	// �O��2D
//	static float Cross2D(Vector3 vec1, Vector3 vec2)
//	{
//		return vec1.x * vec2.y - vec1.y * vec2.x;
//	}
//
//	// �x�N�g���̒���
//	static float Length(Vector3 vec)
//	{
//		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
//	}
//
//	// ��_�Ԃ̋���
//	static float Length(Vector3 vec1, Vector3 vec2)
//	{
//		Vector3 vec = vec1 - vec2;
//		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
//	}
//
//	// �x�N�g���̐��K��
//	static Vector3 Normalize(Vector3 vec)
//	{
//		float mag = Length(vec);
//		if (!mag) return vec;
//		vec.x /= mag;
//		vec.y /= mag;
//		vec.z /= mag;
//
//		return vec;
//	}
//
//	// �x�N�g���̑傫��
//	float Length()
//	{
//		Vector3 vec = *this;
//		return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
//	}
//	// �x�N�g���̑傫�� �������Ȃ�
//	float LengthSquared()
//	{
//		Vector3 vec = *this;
//		return vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;
//	}
//
//	// �x�N�g���̐��K��
//	Vector3 normalized()
//	{
//		Vector3 vec = *this;
//		float mag = Length(vec);
//		if (!mag) return vec;
//		vec.x /= mag;
//		vec.y /= mag;
//		vec.z /= mag;
//
//		return vec;
//	}
//
//	// �ǂ���x�N�g��
//	static Vector3 WallScratchVector(Vector3 front, Vector3 normal)
//	{
//		normal = normal.normalized();
//		return (front - normal * Dot(front, normal));
//	}
//
//	// ���˃x�N�g��
//	static Vector3 WallReflectVector(Vector3 front, Vector3 normal)
//	{
//		normal = normal.normalized();
//		return (front - normal * Dot(front, normal) * 2.0f);
//	}
//
//	// �����x�N�g��
//	static Vector3 WallVerticalVector(Vector3 front, Vector3 normal)
//	{
//		normal = normal.normalized();
//		return normal * Dot(front, normal);
//	}
//
//	// X����]
//	Vector3 RotationX(float angle)
//	{
//		float rad = XMConvertToRadians(angle);
//		XMFLOAT3 v = m_vec;
//
//		v.y = m_vec.y * cosf(rad) + m_vec.z * sinf(rad);
//		v.z= -m_vec.y * sinf(rad) + m_vec.z * cosf(rad);
//
//		return v;
//	}
//
//	// Y����]
//	Vector3 RotationY(float angle)
//	{
//		float rad = XMConvertToRadians(angle);
//		XMFLOAT3 v = m_vec;
//
//		v.x =  m_vec.x * cosf(rad) - m_vec.z * sinf(rad);
//		v.z =  m_vec.x * sinf(rad) + m_vec.z * cosf(rad);
//
//		return v;
//	}
//
//	// Z����]
//	Vector3 RotationZ(float angle)
//	{
//		float rad = XMConvertToRadians(angle);
//		XMFLOAT3 v = m_vec;
//
//		v.x =  m_vec.x * cosf(rad) + m_vec.y * sinf(rad);
//		v.y = -m_vec.x * sinf(rad) + m_vec.y * cosf(rad);
//
//		return v;
//	}
//
//
//	// �����擾
//	XMFLOAT3* GetFloat3() { return &m_vec; }
//
//private:
//	XMFLOAT3 m_vec;
//};
