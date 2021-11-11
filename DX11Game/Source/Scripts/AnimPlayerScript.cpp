/*****************************************************************//**
 * \file   AnimPlayerScript.h
 * \brief  �A�V���v�A�j���[�V�����v���C���[
 *
 * \author USAMI KOSHI
 * \date   2021/11/10
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "AnimPlayerScript.h"

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
#include "BombCrystalScript.h"


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====
#define ANIM_IDLE			(0)
#define ANIM_JUMP			(1)
#define ANIM_KICK			(2)
#define ANIM_RUN			(3)
#define ANIM_SLASH			(4)
#define ANIM_WALK			(5)

//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void AnimPlayerScript::Start()
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
	renderer->ModelLoad("data/model/Sword And Shield.fbx");
	renderer->SetAnimIndex(ANIM_IDLE);
	renderer->SetAnimSpeed(1.0f);
	m_assimp = renderer;

	// �R���C�_�[
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();

	// ���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetUseGravity(true);
	m_rb = rb;
	rb->SetDrag(Vector3{ 0.05f, 0.05f, 0.05f });

	// UI
	const auto& hp = gameObject().lock()->AddComponent<PlayerHPUIScript>();
	hp->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());

	// �J����
	//Camera::main()->transform().lock()->m_pos = transform().lock()->m_pos + Vector3(0, 500, 100);

	// ������
	m_speed = 1.0f;
	m_rotSpeed = 0.1f;

}

//========================================
//
//	�X�V��
//
//========================================
void AnimPlayerScript::Update()
{
	const auto& rb = m_rb.lock();
	Quaternion rot = transform().lock()->m_rot;
	Vector3 forward = Matrix::CreateFromQuaternion(rot).Forward();
	Vector3 right = Matrix::CreateFromQuaternion(rot).Right();
	Vector3 up = Vector3(0,1,0);

	const auto& camera = Camera::main();
	auto& cameraTrans = camera->transform();
	Vector3 inputDir;
	if (m_attackCount < 0)
	{
		// �O��
		if (GetKeyPress(VK_UP) || GetKeyPress(VK_W))
		{
			inputDir.z -= 1.0f;
		}
		if (GetKeyPress(VK_DOWN) || GetKeyPress(VK_S))
		{
			inputDir.z += 1.0f;
		}
		// ���E
		if (GetKeyPress(VK_RIGHT) || GetKeyPress(VK_D))
		{
			inputDir.x += 1.0f;
		}
		if (GetKeyPress(VK_LEFT) || GetKeyPress(VK_A))
		{
			inputDir.x -= 1.0f;
		}

		// ��]�𔽉f
		if (inputDir.x != 0.0f || inputDir.z != 0.0f)
		{
			inputDir.Normalize();
			Vector3 moveForce = Vector3::Transform(inputDir, Matrix::CreateFromQuaternion(cameraTrans.lock()->m_rot));
			moveForce.y = 0;
			moveForce.Normalize();
			moveForce.x *= -1;
			Quaternion moveRot = Quaternion::CreateFromRotationMatrix(Matrix::CreateLookAt(Vector3(), moveForce, up));
			rot = Quaternion::Slerp(rot, moveRot, m_rotSpeed);
			rot.Normalize();
			transform().lock()->m_rot = rot;
			// �͂𔽉f
			moveForce.x *= -1;
			moveForce *= getMoveSpeed();
			rb->AddForce(moveForce);

		}

		// ���x�����ɃA�j���[�V����
		if (m_jumpCount < 0)
		{
			Vector3 velo = rb->GetVelocity();
			velo.y = 0;
			float len = velo.Length() * 0.1f;
			if (len > 0.1f)
			{
				// ���s�A�j���[�V����
				if (m_assimp.lock()->GetCurrentAnimIndex() != ANIM_RUN)
				{
					m_assimp.lock()->SetAnimIndex(ANIM_RUN);
				}
				len = len > 1.0f ? 1.0f : len;
				m_assimp.lock()->SetAnimSpeed(len);
			}
			else
			{
				// �ҋ@�A�j���[�V����
				if (m_assimp.lock()->GetCurrentAnimIndex() != ANIM_IDLE)
				{
					m_assimp.lock()->SetAnimIndex(ANIM_IDLE);
					m_assimp.lock()->SetAnimSpeed(1.0f);
				}
			}
		}

		// �W�����v
		m_jumpCount--;
		if (GetKeyTrigger(VK_C) &&
			m_jumpCount < 0 &&
			m_assimp.lock()->GetCurrentAnimIndex() != ANIM_JUMP)
		{
			m_jumpCount = 50;
			m_assimp.lock()->SetAnimIndex(ANIM_JUMP);
			m_assimp.lock()->SetAnimSpeed(1.0f);
		}
	}

	// �U��
	m_attackCount--;
	if (GetKeyTrigger(VK_Z) &&
		m_attackCount < 0 && 
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_SLASH)
	{
		m_attackCount = 90 * getAttackSpeed();
		m_assimp.lock()->SetAnimIndex(ANIM_SLASH);
		m_assimp.lock()->SetAnimSpeed(getAttackSpeed());

	}
	if (m_attackCount == 90 * getAttackSpeed() / 3)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir =
			Mathf::Normalize(transform().lock()->forward());

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
	}
}

//========================================
//
//	��X�V��
//
//========================================
void AnimPlayerScript::LateUpdate()
{
	const auto& camera = Camera::main();
	auto cameraTrans = camera->transform().lock();

	cameraTrans->m_pos = transform().lock()->m_pos + Vector3(0, 10, -5) * 75;
	Vector3 dir = transform().lock()->m_pos - cameraTrans->m_pos;
	dir.Normalize();
	cameraTrans->forward(dir);
}

//========================================
//
//	�I����
//
//========================================
void AnimPlayerScript::End()
{
}


//******************** �R�[���o�b�N�֐� ********************


//========================================
//
// ����������
//
//========================================
void AnimPlayerScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// �������Ă����
//
//========================================
void AnimPlayerScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// ���ꂽ��
//
//========================================
void AnimPlayerScript::OnCollisionExit(Collider* collider)
{

}

