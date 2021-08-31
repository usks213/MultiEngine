/*****************************************************************//**
 * \file   SkiilEnemyScript.h
 * \brief  �f�B�t�F���X�G�l�~�[
 *
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "SkiilEnemyScript.h"

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
void SkiilEnemyScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("SkiilEnemy");

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeS06("SkiilEnemy");
	render->SetDiffuseColor({ 1,1,0,1 });

	// �G�l�~�[
	m_enemy = gameObject().lock()->GetComponent<EnemyBaseScript>();
	m_enemy.lock()->GetRb().lock()->SetUseGravity(false);
	m_enemy.lock()->GetRb().lock()->SetGravityForce(Vector3());
	m_enemy.lock()->SetMaxScale(200);
}

//========================================
//
//	�X�V��
//
//========================================
void SkiilEnemyScript::Update()
{
	const auto& enemy = m_enemy.lock();
	if (!enemy) return;
	if (!enemy->m_isLockon) return;

	// �ړ�
	const auto& player = enemy->m_player.lock();
	if (!player) return;

	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;
	Vector3 dir; vec.Normalize(dir);
	float moveSpeed = enemy->m_status.MoveSpeed * 0.01f;

	if (vec.Length() > rand() % 300 + 100)
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
	if (enemy->m_nAttackCount > attack + 30 && enemy->m_nAttackCount % 10 == 0)
	{
		// �U��
		if(enemy->m_nAttackCount > attack + 30 + 50)
			enemy->m_nAttackCount = 0;

		// �X�e�b�v
		m_nUp = rand() % 2 == 0 ? -1 : 1;

		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = enemy->m_status.Damage;
		test->transform().lock()->m_scale = Vector3(100, 100, 100);
		test->SetTag("EnemyBullet");
		const auto& rb = test->GetComponent<Rigidbody>();
		const auto& col = test->GetComponent<DeltaCollider>();
		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 100;
		rb->AddForce(dir * 50);
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
		col->SetMain(false);

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.12f);
	}

}

//========================================
//
//	��X�V��
//
//========================================
void SkiilEnemyScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void SkiilEnemyScript::End()
{
}