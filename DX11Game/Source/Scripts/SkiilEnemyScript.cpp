/*****************************************************************//**
 * \file   SkiilEnemyScript.h
 * \brief  ディフェンスエネミー
 *
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "SkiilEnemyScript.h"

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
void SkiilEnemyScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("SkiilEnemy");

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeS06("SkiilEnemy");
	render->SetDiffuseColor({ 1,1,0,1 });

	// エネミー
	m_enemy = gameObject().lock()->GetComponent<EnemyBaseScript>();
	m_enemy.lock()->GetRb().lock()->SetUseGravity(false);
	m_enemy.lock()->GetRb().lock()->SetGravityForce(Vector3());
	m_enemy.lock()->SetMaxScale(200);
}

//========================================
//
//	更新時
//
//========================================
void SkiilEnemyScript::Update()
{
	const auto& enemy = m_enemy.lock();
	if (!enemy) return;
	if (!enemy->m_isLockon) return;

	// 移動
	const auto& player = enemy->m_player.lock();
	if (!player) return;

	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;
	Vector3 dir; vec.Normalize(dir);
	float moveSpeed = enemy->m_status.MoveSpeed * 0.01f;

	if (vec.Length() > rand() % 300 + 100)
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
	if (enemy->m_nAttackCount > attack + 30 && enemy->m_nAttackCount % 10 == 0)
	{
		// 攻撃
		if(enemy->m_nAttackCount > attack + 30 + 50)
			enemy->m_nAttackCount = 0;

		// ステップ
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

		// サウンド
		CSound::PlaySE("Shot.wav", 0.12f);
	}

}

//========================================
//
//	後更新時
//
//========================================
void SkiilEnemyScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void SkiilEnemyScript::End()
{
}