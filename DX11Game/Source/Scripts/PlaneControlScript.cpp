/*****************************************************************//**
 * \file   PlaneControlScript.cpp
 * \brief  �ۑ�02 ��s�@�R���g���[��
 *
 * \author USAMI KOSHI
 * \date   2021/06/10�@��s�@�R���g���[���N���X�쐬
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "PlaneControlScript.h"

// �V�X�e��
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

// �}�l�[�W���[
#include "../Engine/ECS/Entity/EntityManager.h"

// �G���e�B�e�B
#include "../Engine/ECSEntity/GameObject.h"

// �R���|�[�l���g
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECS�R���|�[�l���g
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECS�V�X�e��
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// �X�N���v�g
#include "BulletScript.h"


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void PlaneControlScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("Player");
	gameObject().lock()->SetTag("Player");

	transform().lock()->m_pos = Vector3(0, 300, 0);
	transform().lock()->m_scale = Vector3(1, 1, 1);
	transform().lock()->m_rot =
		Quaternion::CreateFromYawPitchRoll(0, XMConvertToRadians(5), 0);

	// �R���|�[�l���g�̒ǉ�

	// �����_���[
	const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
	renderer->ModelLoad("data/model/JAS39.fbx");

	// �R���C�_�[
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetUseGravity(true);
	m_rb = rb;
	rb->SetDrag(Vector3{ 0.05f, 0.05f, 0.05f });


	// �J����
	Camera::main()->transform().lock()->m_pos = transform().lock()->m_pos + Vector3(0, 500, 100);

	// ������
	m_speed = 2.0f;
	m_rotSpeed = 1.5f;
	m_rad = 0;
	m_eMode = ECameraMode::eConst;

}

//========================================
//
//	�X�V��
//
//========================================
void PlaneControlScript::Update()
{
	const auto& rb = m_rb.lock();
	Quaternion rot = transform().lock()->m_rot;
	Vector3 forward = Matrix::CreateFromQuaternion(rot).Forward();
	Vector3 right = Matrix::CreateFromQuaternion(rot).Right();
	Vector3 up = Matrix::CreateFromQuaternion(rot).Up();

	// �O�i
	//if (GetKeyPress(VK_W))
	{
		rb->AddForce(forward * m_speed);
	}

	// ��������]
	if (GetKeyPress(VK_UP) || GetKeyPress(VK_W))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-m_rotSpeed));
		rot *= q;
	}
	if (GetKeyPress(VK_DOWN) || GetKeyPress(VK_S))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(m_rotSpeed));
		rot *= q;
	}

	// ���s����]
	if (GetKeyPress(VK_RIGHT) || GetKeyPress(VK_D))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(-m_rotSpeed));
		rot *= q;
	}
	if (GetKeyPress(VK_LEFT) || GetKeyPress(VK_A))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(m_rotSpeed));
		rot *= q;
	}
	transform().lock()->m_rot = rot;

	// �V���b�g
	m_nShotCnt--;
	if (GetKeyPress(VK_SPACE) && m_nShotCnt < 0)
	{
		//const auto& test = GetEntityManager()->CreateEntity<GameObject>();
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir = Mathf::Normalize(forward);

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 500;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.3f);
		m_nShotCnt = 6;
	}
}

//========================================
//
//	��X�V��
//
//========================================
void PlaneControlScript::LateUpdate()
{
	// �J������ǔ�
	Quaternion rot = transform().lock()->m_rot;
	const auto& camera = Camera::main();

	if (GetKeyPress(VK_1)) m_eMode = ECameraMode::eConst;
	if (GetKeyPress(VK_2)) m_eMode = ECameraMode::eOne;
	if (GetKeyPress(VK_3)) m_eMode = ECameraMode::eTherd;
	if (GetKeyPress(VK_4)) m_eMode = ECameraMode::eRotate;


	switch (m_eMode)
	{
	case ECS::PlaneControlScript::eConst:
	{
		//--- �Œ�
		Vector3 vec = transform().lock()->m_pos - camera->transform().lock()->m_pos;
		camera->transform().lock()->forward(Mathf::Normalize(vec));

		if (Mathf::Length(vec) > camera->GetFarZ() * 0.5f)
		{
			camera->transform().lock()->m_pos +=
				Mathf::Normalize(vec) * (Mathf::Length(vec) - camera->GetFarZ() * 0.5f);
		}
		break;
	}
	case ECS::PlaneControlScript::eOne:
	{
		//--- ��l��
		camera->transform().lock()->m_rot = rot;
		camera->transform().lock()->m_pos =
			transform().lock()->m_pos + Vector3::Transform(Vector3(0, 40, -150), rot);
		break;
	}
	case ECS::PlaneControlScript::eTherd:
	{
		//--- �O�l��
		camera->transform().lock()->m_rot = rot;
		camera->transform().lock()->m_pos =
			transform().lock()->m_pos + Vector3::Transform(Vector3(0, 500, 1000), rot) * 0.75f;
		break;
	}
	case ECS::PlaneControlScript::eRotate:
	{
		//--- ��]
		m_rad += 0.02f;
		Quaternion q = Quaternion::CreateFromYawPitchRoll(m_rad, 0, 0);
		camera->transform().lock()->m_pos =
			transform().lock()->m_pos + Vector3::Transform(Vector3(0, 0, 1000), q);

		Vector3 vec = transform().lock()->m_pos - camera->transform().lock()->m_pos;
		camera->transform().lock()->forward(Mathf::Normalize(vec));
		break;
	}
	}
}

//========================================
//
//	�I����
//
//========================================
void PlaneControlScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void PlaneControlScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void PlaneControlScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void PlaneControlScript::OnCollisionExit(Collider* collider)
{

}

