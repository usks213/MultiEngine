/*****************************************************************//**
 * \file   PhysicalEnemyScript.h
 * \brief  �t�B�W�J���G�l�~�[
 *
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "PhysicalEnemyScript.h"

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
void PhysicalEnemyScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("PhysicalEnemy");

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeS06("PhysicalEnemy");
	render->SetDiffuseColor({ 0, 1, 0, 1 });

	// �G�l�~�[
	m_enemy = gameObject().lock()->GetComponent<EnemyBaseScript>();

}

//========================================
//
//	�X�V��
//
//========================================
void PhysicalEnemyScript::Update()
{
	const auto& enemy = m_enemy.lock();
	if (!enemy) return;
	if (!enemy->m_isLockon) return;

	// �ړ�
	const auto& player = enemy->m_player.lock();
	if (!player) return;

	Vector3 playerPos = player->transform().lock()->m_pos;
	Vector3 vec = playerPos - transform().lock()->m_pos;
	Vector3 dir; vec.Normalize(dir);
	float moveSpeed = enemy->m_status.MoveSpeed * 0.02f;

	if (vec.Length() > rand() % 500 + 300)
	{
		// �߂Â�
		enemy->GetRb().lock()->AddForce(dir * moveSpeed);
	}
	else
	{
		// ����
		enemy->GetRb().lock()->AddForce(dir.Cross(Vector3(0, m_nUp, 0)) * moveSpeed);
	}

	int attack = 180 - enemy->m_status.AttackSpeed;
	if (attack < 0) attack = 0;
	if (enemy->m_nAttackCount > attack + 30)
	{
		// �U��
		enemy->m_nAttackCount = 0;
		m_nUp = rand() % 2 == 0 ? -1 : 1;

		// ������
		int SpawnNum = 6;
		// �������W
		Vector3 pos = dir * 1000;
		Vector3 backPos = pos;
		// �������̔��������p�x�����炷
		float addAngle = 45.0f / SpawnNum;
		pos = Mathf::RotationY(pos, -addAngle * (SpawnNum / 2));
		Vector3 center = transform().lock()->m_pos;
		Vector3 temp = playerPos;
		for (int i = 0; i < SpawnNum; i++)
		{
			// �G�l�~�[����
			const auto& obj = Instantiate<GameObject>(Mathf::RotationY(pos, addAngle * i) - backPos + transform().lock()->m_pos);
			obj->AddComponent<BulletScript>()->m_damage = enemy->m_status.Damage;
			obj->transform().lock()->m_scale = Vector3(100, 100, 100);
			obj->SetTag("EnemyBullet");
			const auto& col = obj->GetComponent<DeltaCollider>();
			col->SetMain(false);
			// �i�ތ���
			const auto& rb = obj->GetComponent<Rigidbody>();
			Vector3 dir = Mathf::Normalize(temp - center);
			rb->AddForce(dir * 30);
			rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
		}

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.12f);
	}

}

//========================================
//
//	��X�V��
//
//========================================
void PhysicalEnemyScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void PhysicalEnemyScript::End()
{
}
