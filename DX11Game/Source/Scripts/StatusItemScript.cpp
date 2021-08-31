/*****************************************************************//**
 * \file   StatusItemScript.h
 * \brief  �X�e�[�^�X�A�C�e��
 *
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "StatusItemScript.h"

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


// �l�[���X�y�[�X
using namespace ECS;


//===== �}�N����` =====



//******************** �X�N���v�g���\�b�h ********************


//========================================
//
//	�J�n��
//
//========================================
void StatusItemScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("StatusItem");
	gameObject().lock()->SetTag("StatusItem");

	// �傫��
	m_fScale = 100.0f;
	transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	transform().lock()->m_rot = Quaternion::CreateFromYawPitchRoll(0, XMConvertToRadians(90), 0);

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeTetraheron("StatusItem");
	render->SetDiffuseColor({ 1,1,1,1 });

	// ECS���W�b�h�{�f�B
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag(0);
	// ��]
	Quaternion q = Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(rand() % 3 + 1), 0, 0);
	rb->AddTorque(q);
	m_rb = rb;

	// ECS�R���C�_�[
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);


	// ��������
	m_nExitTime = 600;
	// �ǔ��I�t
	m_IsTracking = false;
}

//========================================
//
//	�X�V��
//
//========================================
void StatusItemScript::Update()
{
	// ����
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
	else if (m_nExitTime < 120)
	{
		if (m_nExitTime % 16 < 8)
		{
			// ������
			transform().lock()->m_scale = Vector3(0, 0, 0);
		}
		else
		{
			// ������
			transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
		}
	}

	// �n�ʂɒ����Ă���ǔ�
	if (transform().lock()->m_pos.y <= transform().lock()->m_scale.y / 2)
	{
		m_IsTracking = true;
	}
	// debug if (!m_IsTracking) return;

	// �߂��v���C���[�ɒǔ�
	const auto& player = m_player.lock();
	if (!player) return;

	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;
	// ��苗���ȉ��Ȃ�
	if (vec.Length() > 1000) return;

	// �v���C���[�ɋ߂Â�
	m_rb.lock()->AddForce(Mathf::Normalize(vec) * 3);

}

//========================================
//
//	��X�V��
//
//========================================
void StatusItemScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void StatusItemScript::End()
{
}

// �A�C�e���^�C�v�w��
void StatusItemScript::SetItemType(ItemType type)
{
	m_eType = type;

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->GetComponent<InstancingMeshRenderer>();
	switch (m_eType)
	{
	case ECS::ItemType::Physical:
		render->SetDiffuseColor({ 0,1,0,1 });
		break;
	case ECS::ItemType::Attack:
		render->SetDiffuseColor({ 1,0,0,1 });
		break;
	case ECS::ItemType::Speed:
		render->SetDiffuseColor({ 0,1,1,1 });
		break;
	case ECS::ItemType::Skiil:
		render->SetDiffuseColor({ 1,1,0,1 });
		break;
	case ECS::ItemType::Max:
		break;
	default:
		break;
	}
}

//******************** �R�[���o�b�N�֐� ********************

//========================================
//
// ����������
//
//========================================
void StatusItemScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// ���g�̍폜
		GetEntityManager()->DestroyEntity(gameObject().lock());
		// �T�E���h
		CSound::PlaySE("Delta.wav");
	}
}

//========================================
//
// �������Ă����
//
//========================================
void StatusItemScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	OnDeltaCollisionEnter(collider);
}

//========================================
//
// ���ꂽ��
//
//========================================
void StatusItemScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

