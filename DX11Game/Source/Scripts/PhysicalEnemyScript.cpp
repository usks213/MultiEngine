/*****************************************************************//**
 * \file   PhysicalEnemyScript.h
 * \brief  フィジカルエネミー
 *
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "PhysicalEnemyScript.h"

// システム
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"

// マネージャー
#include "../Engine/ECS/Entity/EntityManager.h"

// エンティティ
#include "../Engine/ECSEntity/GameObject.h"

// コンポーネント
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"

// ECSコンポーネント
#include "../Engine/ECSCompoent/DeltaCollider.h"

// ECSシステム
#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

#include "BulletScript.h"



// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void PhysicalEnemyScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("PhysicalEnemy");

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeS06("PhysicalEnemy");
	render->SetDiffuseColor({ 0, 1, 0, 1 });

	// エネミー
	m_enemy = gameObject().lock()->GetComponent<EnemyBaseScript>();

}

//========================================
//
//	更新時
//
//========================================
void PhysicalEnemyScript::Update()
{
	const auto& enemy = m_enemy.lock();
	if (!enemy) return;
	if (!enemy->m_isLockon) return;

	// 移動
	const auto& player = enemy->m_player.lock();
	if (!player) return;

	Vector3 playerPos = player->transform().lock()->m_pos;
	Vector3 vec = playerPos - transform().lock()->m_pos;
	Vector3 dir; vec.Normalize(dir);
	float moveSpeed = enemy->m_status.MoveSpeed * 0.02f;

	if (vec.Length() > rand() % 500 + 300)
	{
		// 近づく
		enemy->GetRb().lock()->AddForce(dir * moveSpeed);
	}
	else
	{
		// 旋回
		enemy->GetRb().lock()->AddForce(dir.Cross(Vector3(0, m_nUp, 0)) * moveSpeed);
	}

	int attack = 180 - enemy->m_status.AttackSpeed;
	if (attack < 0) attack = 0;
	if (enemy->m_nAttackCount > attack + 30)
	{
		// 攻撃
		enemy->m_nAttackCount = 0;
		m_nUp = rand() % 2 == 0 ? -1 : 1;

		// 生成数
		int SpawnNum = 6;
		// 生成座標
		Vector3 pos = dir * 1000;
		Vector3 backPos = pos;
		// 生成数の半分だけ角度をずらす
		float addAngle = 45.0f / SpawnNum;
		pos = Mathf::RotationY(pos, -addAngle * (SpawnNum / 2));
		Vector3 center = transform().lock()->m_pos;
		Vector3 temp = playerPos;
		for (int i = 0; i < SpawnNum; i++)
		{
			// エネミー生成
			const auto& obj = Instantiate<GameObject>(Mathf::RotationY(pos, addAngle * i) - backPos + transform().lock()->m_pos);
			obj->AddComponent<BulletScript>()->m_damage = enemy->m_status.Damage;
			obj->transform().lock()->m_scale = Vector3(100, 100, 100);
			obj->SetTag("EnemyBullet");
			const auto& col = obj->GetComponent<DeltaCollider>();
			col->SetMain(false);
			// 進む向き
			const auto& rb = obj->GetComponent<Rigidbody>();
			Vector3 dir = Mathf::Normalize(temp - center);
			rb->AddForce(dir * 30);
			rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
		}

		// サウンド
		CSound::PlaySE("Shot.wav", 0.12f);
	}

}

//========================================
//
//	後更新時
//
//========================================
void PhysicalEnemyScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void PhysicalEnemyScript::End()
{
}
