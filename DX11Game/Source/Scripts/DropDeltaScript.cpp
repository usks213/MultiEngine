//==================================================================
//								DropDeltaScript.cpp
//	敵からドロップするアイテム
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	ドロップデルタスクリプトクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "DropDeltaScript.h"

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


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void DropDeltaScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("DropDelta");
	gameObject().lock()->SetTag("DropDelta");

	// 大きさ
	m_fScale = 100.0f;
	transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	transform().lock()->m_rot = Quaternion::CreateFromYawPitchRoll(0, XMConvertToRadians(90), 0);

	//--- コンポーネンの追加

	// インスタンシングレンダラー
	const auto& render = gameObject().lock()->AddComponent<InstancingMeshRenderer>();
	render->MakeTetraheron("DropDelta");
	render->SetDiffuseColor({ 0,1,0,1 });

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag(0);
	// 回転
	Quaternion q = Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(rand() % 3 + 1), 0, 0);
	rb->AddTorque(q);
	m_rb = rb;

	// ECSコライダー
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);


	// 生存時間
	m_nExitTime = 600;
	// 追尾オフ
	m_IsTracking = false;
}

//========================================
//
//	更新時
//
//========================================
void DropDeltaScript::Update()
{
	// 生存
	m_nExitTime--;
	if (m_nExitTime < 0)
	{
		// 自身の削除
		GetEntityManager()->DestroyEntity(gameObject().lock());
	}
	else if (m_nExitTime < 120)
	{
		if (m_nExitTime % 16 < 8)
		{
			// 消える
			transform().lock()->m_scale = Vector3(0, 0, 0);
		}
		else
		{
			// 見える
			transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
		}
	}

	// 地面に着いてから追尾
	if (transform().lock()->m_pos.y <= transform().lock()->m_scale.y / 2)
	{
		m_IsTracking = true;
	}
	// debug if (!m_IsTracking) return;

	// 近いプレイヤーに追尾
	const auto& player = m_player.lock();
	if (!player) return;

	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;
	// 一定距離以下なら
	if (vec.Length() > 1000) return;

	// プレイヤーに近づく
	m_rb.lock()->AddForce(Mathf::Normalize(vec) * 3);

}

//========================================
//
//	後更新時
//
//========================================
void DropDeltaScript::LateUpdate()
{
}

//========================================
//
//	終了時
//
//========================================
void DropDeltaScript::End()
{
}


//******************** コールバック関数 ********************

//========================================
//
// 当たった時
//
//========================================
void DropDeltaScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// 自身の削除
		GetEntityManager()->DestroyEntity(gameObject().lock());
		// サウンド
		CSound::PlaySE("Delta.wav");
	}
}

//========================================
//
// 当たっている間
//
//========================================
void DropDeltaScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Player")
	{
		// 自身の削除
		GetEntityManager()->DestroyEntity(gameObject().lock());
		// サウンド
		CSound::PlaySE("Delta.wav");
	}
}

//========================================
//
// 離れた時
//
//========================================
void DropDeltaScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

