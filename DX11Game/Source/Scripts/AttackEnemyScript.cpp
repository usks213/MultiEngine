/*****************************************************************//**
 * \file   AttackEnemyScript.h
 * \brief  �A�^�b�N�G�l�~�[
 *
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "AttackEnemyScript.h"

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
void AttackEnemyScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("AttackEnemy");

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeS06("AttackEnemy");
	render->SetDiffuseColor({ 1,100 / 255.0f,0,1 });

	// �G�l�~�[
	m_enemy = gameObject().lock()->GetComponent<EnemyBaseScript>();
	m_enemy.lock()->SetMaxScale(400);

}

//========================================
//
//	�X�V��
//
//========================================
void AttackEnemyScript::Update()
{
	const auto& enemy = m_enemy.lock();
	if (!enemy) return;
	if (!enemy->m_isLockon) return;

	// �ړ�
	const auto& player = enemy->m_player.lock();
	if (!player) return;

	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;
	Vector3 dir; vec.Normalize(dir);
	float moveSpeed = enemy->m_status.MoveSpeed * 0.05f;

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

		// �^�b�N��
		m_nUp = rand() % 2 == 0 ? -1 : 1;
		enemy->GetRb().lock()->SetForce(dir * moveSpeed * 100);

		// �T�E���h
		CSound::PlaySE("Shot.wav", 0.12f);
	}

}

//========================================
//
//	��X�V��
//
//========================================
void AttackEnemyScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void AttackEnemyScript::End()
{
}

