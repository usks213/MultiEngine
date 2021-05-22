#include "Light.h"
#include "../System/input.h"
#include "Camera.h"

using namespace DirectX;

// �}�N����`
#define LIGHT0_DIRECTION	XMFLOAT3(0.0f, -1.0f, 1.0f)
#define LIGHT0_DIFFUSE		XMFLOAT4(0.9f, 0.9f, 0.9f, 1.0f)
#define LIGHT0_AMBIENT		XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f)
#define LIGHT0_SPECULAR		XMFLOAT4(0.4f, 0.4f, 0.4f, 1.0f)

#define	LIT_POS_P_X			(0.0f)					// �J�����̎��_�����ʒu(X���W)
#define	LIT_POS_P_Y			(500.0f)				// �J�����̎��_�����ʒu(Y���W)
#define	LIT_POS_P_Z			(0.0f)				// �J�����̎��_�����ʒu(Z���W)
#define	LIT_POS_R_X			(0.0f)					// �J�����̒����_�����ʒu(X���W)
#define	LIT_POS_R_Y			(0.0f)					// �J�����̒����_�����ʒu(Y���W)
#define	LIT_POS_R_Z			(0.0f)					// �J�����̒����_�����ʒu(Z���W)


CLight* CLight::m_mainLight = nullptr;


// �R���X�g���N�^
CLight::CLight()
{
	Init();
}

// ������
void CLight::Init()
{
	XMFLOAT3 vDir = LIGHT0_DIRECTION;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));
	m_diffuse = LIGHT0_DIFFUSE;
	m_ambient = LIGHT0_AMBIENT;
	m_specular = LIGHT0_SPECULAR;

	// �}�g���b�N�X�쐬
	m_vPos = XMFLOAT3(LIT_POS_P_X, LIT_POS_P_Y, LIT_POS_P_Z);		// ���_
	m_vTarget = XMFLOAT3(LIT_POS_R_X, LIT_POS_R_Y, LIT_POS_R_Z);	// �����_
	m_vUp = XMFLOAT3(0.0f, 1.0f, 0.0f);								// ����x�N�g��

	XMMATRIX m = XMMatrixLookAtLH(XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp));

	//XMMATRIX m2 = XMMATRIX(
	//	0.5f, 0.0f, 0.0f, 0.0f,
	//	0.0f, -0.5f, 0.0f, 0.0f,
	//	0.0f, 0.0f, 1.0f, 0.0f,
	//	0.5f, 0.5f, 0.0f, 1.0f);
	//m = XMMatrixMultiply(m, m2);

	XMStoreFloat4x4(&m_mtxView, m);


	// �v���W�F�N�V�����}�g���b�N�X
	XMStoreFloat4x4(&m_mtxProj, XMMatrixOrthographicOffCenterLH(-SCREEN_CENTER_X, SCREEN_CENTER_X, -SCREEN_CENTER_Y, SCREEN_CENTER_Y, 
		VIEW_NEAR_Z, VIEW_FAR_Z));


	vDir = m_vTarget;
	vDir.x -= m_vPos.x;
	vDir.y -= m_vPos.y;
	vDir.z -= m_vPos.z;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));

}


void CLight::Update()
{
	// ���C�g�̌�����ύX

	//if (GetKeyPress(VK_L))
	//{
	//	m_vPos.x += 10.f;
	//}
	//if (GetKeyPress(VK_J))
	//{
	//	m_vPos.x -= 10.f;
	//}
	//if (GetKeyPress(VK_I))
	//{
	//	m_vPos.z += 10.f;
	//}
	//if (GetKeyPress(VK_K))
	//{
	//	m_vPos.z -= 10.f;
	//}
	const auto& trans = m_targetTrans.lock();
	if (trans)
	{
		Vector3 pos = trans->m_pos;

		m_vTarget.x = pos->x;
		m_vTarget.y = pos->y;
		m_vTarget.z = pos->z;

		m_vPos.x = m_vTarget.x + LIT_POS_P_X;
		m_vPos.y = m_vTarget.y + LIT_POS_P_Y;
		m_vPos.z = m_vTarget.z + LIT_POS_P_Z;
	}

	// ���C�g�}�g���b�N�X�̍X�V
	XMMATRIX m = XMMatrixLookAtLH(XMLoadFloat3(&m_vPos), XMLoadFloat3(&m_vTarget), XMLoadFloat3(&m_vUp));
	XMStoreFloat4x4(&m_mtxView, m);

	// ���C�g�x�N�g���̍X�V
	XMFLOAT3 vDir = m_vTarget;
	vDir.x -= m_vPos.x;
	vDir.y -= m_vPos.y;
	vDir.z -= m_vPos.z;
	XMStoreFloat3(&m_direction, XMVector3Normalize(XMLoadFloat3(&vDir)));
}