/*****************************************************************//**
 * \file   TestMoveScript.h
 * \brief
 *
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "TestMoveScript.h"

// �V�X�e��
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"
#include "../Engine/System/debugproc.h"

// �}�l�[�W���[
#include "../Engine/ECS/Entity/EntityManager.h"

// �G���e�B�e�B
#include "../Engine/ECSEntity/GameObject.h"

// �R���|�[�l���g
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"
#include "../Engine/ECSCompoent/DeltaCollider.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"

// �����_���[
#include "../Engine/ECSCompoent/Camera.h"
#include "../Engine/Renderer/Light.h"
#include "../Engine/Renderer/PostProcessing.h"

#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// ���[���h
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/TitleWorld.h"

// �X�N���v�g
#include "BulletScript.h"
#include "EnemyBaseScript.h"
#include "GameOverScript.h"
#include "BombCrystalScript.h"
#include "BombEffectScript.h"
#include "SkillUIScript.h"
#include "PlayerHPUIScript.h"


#include <iostream>


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void TestMoveScript::Start()
{
	// ���O�E�^�O

	// �R���|�[�l���g�̒ǉ�

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);
	rb->SetUseGravity(false);
	rb->SetDrag(Vector3(0.05f, 0.05f, 0.05f));
}

//========================================
//
//	�X�V��
//
//========================================
void TestMoveScript::Update()
{

	const float speed = 1.0f;
	const float rotSpeed = 3.141592f / 60.0f;

	// ����
	const auto& trans = transform().lock();
	Matrix matR = Matrix::CreateFromQuaternion(trans->m_rot);
	Quaternion rot = transform().lock()->m_rot;
	Vector3 vPos = matR.Translation();
	Vector3 right = matR.Right();
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = matR.Forward();
	Vector3 vLook = vPos + forward;
	float focus = 0.0f;

	if (!GetKeyPress(VK_RBUTTON))
	{
		// �ړ�
		if (GetKeyPress(VK_W))
		{
			m_rb.lock()->AddForce(Vector3(0, 1, 0));
		}
		if (GetKeyPress(VK_S))
		{
			m_rb.lock()->AddForce(-Vector3(0, 1, 0));
		}
		if (GetKeyPress(VK_D))
		{
			m_rb.lock()->AddForce(-Vector3(1, 0, 0));
		}
		if (GetKeyPress(VK_A))
		{
			m_rb.lock()->AddForce(Vector3(1, 0, 0));
		}

		// ��]
		// ��������]
		if (GetKeyPress(VK_UP))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-1));
			rot *= q;
		}
		if (GetKeyPress(VK_DOWN))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(1));
			rot *= q;
		}
		// ���s����]
		if (GetKeyPress(VK_RIGHT))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(-1));
			rot *= q;
		}
		if (GetKeyPress(VK_LEFT))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(1));
			rot *= q;
		}
		transform().lock()->m_rot = rot;
	}
}

//========================================
//
//	��X�V��
//
//========================================
void TestMoveScript::LateUpdate()
{

}

//========================================
//
//	�I����
//
//========================================
void TestMoveScript::End()
{

}

