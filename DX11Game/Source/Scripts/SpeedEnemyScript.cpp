/*****************************************************************//**
 * \file   SpeedEnemyScript.h
 * \brief  �X�s�[�h����
 *
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/


//====== �C���N���[�h�� ======

// ���g
#include "SpeedEnemyScript.h"

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
void SpeedEnemyScript::Start()
{
	// ���O�E�^�O
	gameObject().lock()->SetName("SpeedEnemy");

	//--- �R���|�[�l���̒ǉ�

	// �C���X�^���V���O�����_���[
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeS06("SpeedEnemy");
	render->SetDiffuseColor({ 0,1,1,1 });

	// �G�l�~�[
	m_enemy = gameObject().lock()->GetComponent<EnemyBaseScript>();
	m_enemy.lock()->GetRb().lock()->SetDrag(Vector3(0, 0, 0));
	m_enemy.lock()->GetRb().lock()->SetUseGravity(false);
	m_enemy.lock()->GetRb().lock()->SetGravityForce(Vector3());
	m_enemy.lock()->SetMaxScale(200);
}

//========================================
//
//	�X�V��
//
//========================================
void SpeedEnemyScript::Update()
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
	if (enemy->m_nAttackCount > attack + 30)
	{
		// �U��
		enemy->m_nAttackCount = 0;

		// �X�e�b�v
		m_nUp = rand() % 2 == 0 ? -1 : 1;
		enemy->GetRb().lock()->SetForce(dir.Cross(Vector3(0, m_nUp, 0)) * moveSpeed * 200);

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
void SpeedEnemyScript::LateUpdate()
{
}

//========================================
//
//	�I����
//
//========================================
void SpeedEnemyScript::End()
{
}

//////========================================
//////
////// ��{�s��
//////
//////========================================
////void SpeedEnemyScript::MoveUpdate()
////{
////	// ��������
////	m_nExitTime--;
////	if (m_nExitTime < 0)
////	{
////		// ���g�̍폜
////		GetEntityManager()->DestroyEntity(gameObject().lock());
////	}
////
////	// �^�[�Q�b�g�����݂��Ȃ�
////	const auto& player = m_player.lock();
////	if (!player) return;
////	const auto& target = player->transform().lock();
////
////	//--- �ǔ�����
////	Vector3 dir = target->m_pos - transform().lock()->m_pos;
////	if (dir.Length() < 500) return;
////	dir = Mathf::Normalize(dir);
////
////	// �����Ă����
////	Vector3 cameraDir = Camera::main()->transform().lock()->forward();
////	if (Mathf::Dot(cameraDir, dir) >= 0) return;
////
////	// �͂�������
////	m_rb.lock()->SetForce(dir * m_status.MoveSpeed);
////
////	// �q�ɂ�������
////	for (const auto& child_w : m_childList)
////	{
////		const auto& child_s = child_w.lock();
////		if (!child_s) continue;
////
////		// �͂�������
////		child_s->GetRb().lock()->SetForce(dir * m_status.MoveSpeed);
////	}
////}
//
////========================================
////
////	�q�̐���
////
////========================================
//void SpeedEnemyScript::CreateChild(int nNum)
//{
//	//x = Mathf.Cos(phi) * Mathf.Cos(theta);
//	//y = Mathf.Cos(phi) * Mathf.Sin(theta);
//	//z = Mathf.Sin(phi);
//	//for (int n = 0; n < nNum; ++n)
//	//{
//	//	for (int i = 0; i < n; ++i)
//	//	{
//	//		float phi = 360.0f / n * i * (XM_PI / 180.0f);
//
//	//		for (int j = 0; j < n; ++j)
//	//		{
//	//			float theta = 360.0f / n * j * (XM_PI / 180.0f);
//	//			// ���W
//	//			Vector3 pos;
//	//			pos.x = cosf(phi) * cosf(theta);
//	//			pos.y = cosf(phi) * sinf(theta);
//	//			pos.z = sinf(phi);
//	//			pos *= 100 * n;
//
//	//			// �G�l�~�[����
//	//			const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
//	//			// �R���|�[�l���g�̒ǉ�
//	//			const auto& tracking = obj->AddComponent<SkiilEnemyScript>();
//	//			// ���X�g�֊i�[
//	//			m_childList.push_back(tracking);
//	//		}
//	//	}
//	//}
//
//	int j = 0;
//	int k = 0;
//	for (int n = 0; n < nNum; n++)
//	{
//		float phi = rand() % 1000 / 100.0f;
//
//		float theta = rand() % 1000 / 100.0f;
//		// ���W
//		Vector3 pos;
//		pos.x = cosf(phi) * cosf(theta);
//		pos.y = sinf(phi);
//		pos.z = cosf(phi) * sinf(theta);
//
//		if (n > j * 4)
//		{
//			k++;
//			j += k;
//		}
//		pos.x *= (150 + k * 150);
//		pos.z *= (150 + k * 150);
//		pos.y *= (150 + k * 2 * 150);
//
//		// �G�l�~�[����
//		const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
//		// �R���|�[�l���g�̒ǉ�
//		const auto& tracking = obj->AddComponent<SkiilEnemyScript>();
//		//tracking->SetPlayer(m_player.lock());
//		// ���X�g�֊i�[
//		m_childList.push_back(tracking);
//	}
//}
