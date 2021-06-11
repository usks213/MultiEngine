/*****************************************************************//**
 * \file   Camera.cpp
 * \brief  �J�����R���|�[�l���g
 *
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/


//====== �C���N���[�h�� ======
#include "Camera.h"
#include "../main.h"
#include "../ECSSystem/CameraSystem.h"

#include "../ECS/Entity/EntityManager.h"
#include "../ECSCompoent/Transform.h"
#include "../ECSEntity/GameObject.h"

using namespace ECS;
using namespace DirectX;


//===== �}�N����` =====
#define	VIEW_ANGLE			(80.0f)								// �r���[���ʂ̎���p
#define	VIEW_ASPECT			((float)SCREEN_WIDTH/SCREEN_HEIGHT)	// �r���[���ʂ̃A�X�y�N�g��
#define	VIEW_NEAR_Z			(50.0f)								// �r���[���ʂ�NearZ�l
#define	VIEW_FAR_Z			(5000.0f * 1.5f)					// �r���[���ʂ�FarZ�l
#define FOG_NEAR_Z	(VIEW_FAR_Z * 0.3f)
#define FOG_FAR_Z	(VIEW_FAR_Z * 0.6f)


//===== �O���[�o���ϐ� =====
std::weak_ptr<Camera> Camera::m_mainCamera;


//========================================
//
//	�R���X�g���N�^
//
//========================================
Camera::Camera()
{
	m_fAspectRatio = VIEW_ASPECT;			// �c����
	m_fFovY = VIEW_ANGLE;					// ����p(Degree)
	m_fNearZ = VIEW_NEAR_Z;					// �O���N���b�v����
	m_fFarZ = VIEW_FAR_Z;					// ����N���b�v����
	m_fFogNearZ = FOG_NEAR_Z;
	m_fFogFarZ = FOG_FAR_Z;
	m_up = Vector3(0, 1, 0);
}

//========================================
//
//	�f�X�g���N�^
//
//========================================
Camera::~Camera()
{

}

//========================================
//
//	������
//
//========================================
void Camera::OnCreate()
{
	// �V�X�e���Ɋi�[
	CameraSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CameraSystem>();
	if (sys) sys->AddList(this);

	// �g�����X�t�H�[���擾
	m_transform = m_Parent.lock()->GetComponent<Transform>();
	// �Q�[���I�u�W�F�N�g�擾
	m_gameObject = m_transform.lock()->gameObject();
	// ���O�E�^�O
	m_gameObject.lock()->SetName("Camera");
	m_gameObject.lock()->SetTag("Camera");

	// �}�g���b�N�X�X�V
	UpdateCameraMatrix();
}

//========================================
//
//	�j����
//
//========================================
void Camera::OnDestroy()
{
	// �V�X�e�����珜�O
	CameraSystem* sys = GetEntityManager()->GetWorld()->GetSystem<CameraSystem>();
	if (sys) sys->RemoveList(this);
}

//========================================
//
//	���b�Z�[�W��M
//
//========================================
void Camera::SendComponentMessage(const std::string& message, void* value)
{
	// Update���b�Z�[�W�Ń}�g���b�N�X���X�V
	if ("UpdateCameraMatrix" == message)
	{
		UpdateCameraMatrix();
	}
}

//========================================
//
//	�X�V
//
//========================================
void Camera::UpdateCameraMatrix()
{
	const auto& trans = transform().lock();
	if (!trans) return;

	const Vector3& pos = trans->m_pos;
	const Vector3& sca = trans->m_scale;
	const Quaternion& rot = trans->m_rot;

	Vector3 target = pos + Matrix::CreateFromQuaternion(rot).Forward();

	Vector3 up = Mathf::Normalize(Matrix::CreateFromQuaternion(rot).Up());
	//Vector3 up = Mathf::Normalize(Mathf::Cross(
	//	Matrix::CreateFromQuaternion(rot).Right(),
	//	Matrix::CreateFromQuaternion(rot).Forward()));
	//Vector3 up = Vector3(0, 1, 0);

	XMStoreFloat4x4(&m_mtxView, XMMatrixLookAtLH(
		XMLoadFloat3(&pos), XMLoadFloat3(&target), XMLoadFloat3(&up)));

	XMStoreFloat4x4(&m_mtxProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(m_fFovY), m_fAspectRatio, m_fNearZ, m_fFarZ));
}