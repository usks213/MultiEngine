//==================================================================
//								EnemyBaseScript.h
//	エネミーベーススクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/09	エネミーベースクラス作成
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "EnemyBaseScript.h"

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

// スクリプト
#include "StatusItemScript.h"
#include "EnemyEffectScript.h"
#include "DamageNumberScript.h"
#include "BulletScript.h"
#include "BombCollisionScript.h"

// ネームスペース
using namespace ECS;


//===== マクロ定義 =====
#define ADD_SCALE (200.0f / 10.0f)


//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void EnemyBaseScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Enemy");
	gameObject().lock()->SetTag("Enemy");

	// 大きさ
	m_fMaxScale = 300;
	m_fScale = 1.0f;
	transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);


	//--- コンポーネンの追加

	// ECSリジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	//rb->SetDrag({ 0,0,0 });
	rb->SetStaticFriction(0);
	rb->SetDynamicFriction(0);
	rb->SetMass(1);
	rb->SetTorqueDrag(0);
	// 回転
	Vector3 v = { rand() % 100 / 100.0f, rand() % 100 / 100.0f, rand() % 100 / 100.0f };
	Quaternion q = Quaternion::CreateFromAxisAngle(Mathf::Normalize(v),
		XMConvertToRadians(v.Length() * 5));
	rb->AddTorque(q);
	// リジッドボディ保存
	m_rb = rb;

	// ECSコライダー
	gameObject().lock()->AddComponent<DeltaCollider>()->SetMain(false);

	// HPUI
	const auto& hpUI = Instantiate<GameObject>(Vector3(), Vector3(), Vector3(m_fMaxScale, m_fMaxScale * 0.2f, 1));
	const auto& renderer = hpUI->AddComponent<InstancingMeshRenderer>();
	renderer->MakeSprite("EnemyHP");
	renderer->SetBillboard(true);
	renderer->SetDiffuseColor(Vector4(0, 1, 0.2f, 1));
	m_HPUI = hpUI;


	// ステータス
	SetUpStatus(Type::Balance, 1);

	// 生成イベント追加
	m_eventList.push_front(std::bind(&EnemyBaseScript::CreateUpdate, this));
}

//========================================
//
//	更新時
//
//========================================
void EnemyBaseScript::Update()
{
	// イベント更新
	if (m_eventList.size() > 0)
	{
		m_eventList.front()();

		// 次のイベントへ
		if (m_isNext)
		{
			m_eventList.pop_front();
			m_isNext = false;
		}
	}

	const auto& player = m_player.lock();
	if (!player) return;
	Vector3 vec = player->transform().lock()->m_pos - transform().lock()->m_pos;

	// プレイヤーをロックオン
	if (m_isLockon)
	{
		// 攻撃カウント
		m_nAttackCount++;

		Vector3 dir;
		vec.Normalize(dir);
		Matrix rotMat;
		rotMat.Up(Vector3(0,1,0));
		rotMat.Forward(dir);
		//transform().lock()->m_rot = Quaternion::CreateFromRotationMatrix(rotMat);
 	}
	else
	{
		// 自動徘徊
		m_nMoveCnt++;
		if (m_nMoveCnt > 300)
		{
			m_nMoveCnt = 0;
			m_dir = Vector3(rand() % 100, 0.0f, rand() % 100);
			m_dir.Normalize();
		}
		m_rb.lock()->SetForce(m_dir * 3);
	}

	// 一定距離近づいたらロックオン
	if (vec.Length() < 2000.0f)
	{
		m_isLockon = true;
	}

}

//========================================
//
//	後更新時
//
//========================================
void EnemyBaseScript::LateUpdate()
{
	// HPUIの同期
	m_HPUI.lock()->transform().lock()->m_pos = 
		transform().lock()->m_pos + Vector3(0, m_fMaxScale, 0);
	m_HPUI.lock()->transform().lock()->m_scale.x = m_fMaxScale * (m_status.HP / m_status.MaxHP);
}

//========================================
//
//	終了時
//
//========================================
void EnemyBaseScript::End()
{
	if (m_HPUI.lock())
	{
		Destroy(m_HPUI.lock());
	}
	if (m_maker.lock())
	{
		m_maker.lock()->m_nEnemyCount--;
	}
}


//******************** コールバック関数 ********************

//========================================
//
// 当たった時
//
//========================================
void EnemyBaseScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	if (collider->gameObject().lock()->tag() == "Bullet" || 
		collider->gameObject().lock()->tag() == "Bomb")
	{
		if (!m_bActive) return;

		// ダメージを受けたらロックオン
		m_isLockon = true;

		// エフェクトの生成
		const auto& effect = Instantiate<GameObject>(collider->transform().lock()->m_pos);
		effect->AddComponent<EnemyEffectScript>();

		// 画面揺れ
		const auto& camera = Camera::main();
		camera->SetShakeOffset(Vector2(5,5));
		camera->SetShake(4);
		Vector3 cameraBack = Matrix::CreateFromQuaternion(camera->transform().lock()->m_rot).Backward();

		// ノックバック
		Vector3 forcedir = transform().lock()->m_pos - collider->transform().lock()->m_pos;
		forcedir.Normalize();
		m_rb.lock()->AddForce(forcedir * 2);

		// ダメージ
		float damage = 0.0f;
		if(collider->gameObject().lock()->tag() == "Bullet")
			damage = collider->gameObject().lock()->GetComponent<BulletScript>()->m_damage;
		else if(collider->gameObject().lock()->tag() == "Bomb")
			damage = collider->gameObject().lock()->GetComponent<BombCollisionScript>()->GetDamage();

		// ダメージ表記
		const auto& damageUI = Instantiate<GameObject>(collider->transform().lock()->m_pos + cameraBack * m_fMaxScale / 2);
		const auto& number = damageUI->AddComponent<DamageNumberScript>();
		number->SetNumber(damage);
		const auto& rb = damageUI->GetComponent<Rigidbody>();
		Vector3 dir((rand() % 200 - 100) * 0.01f, 1.0f, (rand() % 200 - 100) * 0.01f);
		dir.Normalize();
		rb->AddForce(dir * 10);

		// HP
		m_status.HP -= damage;
		if (m_status.HP >= 0) return;

		// ドロップアイテムの生成
		for (int i = 0; i < m_status.Level / 2 + 1; ++i)
		{
			const auto& obj = Instantiate<GameObject>(transform().lock()->m_pos);
			const auto& item = obj->AddComponent<StatusItemScript>();
			item->SetPlayer(m_player.lock());
			switch (m_type)
			{
			case ECS::EnemyBaseScript::Type::Attack:
				item->SetItemType(ItemType::Attack);
				break;
			case ECS::EnemyBaseScript::Type::Speed:
				item->SetItemType(ItemType::Speed);
				break;
			case ECS::EnemyBaseScript::Type::Skiil:
				item->SetItemType(ItemType::Skiil);
				break;
			case ECS::EnemyBaseScript::Type::Physical:
				item->SetItemType(ItemType::Physical);
				break;
			case ECS::EnemyBaseScript::Type::Balance:
				item->SetItemType((ItemType)(rand() % static_cast<int>(ItemType::Max)));
				break;
			}
		}

		// 削除イベント追加
		m_eventList.push_front(std::bind(&EnemyBaseScript::DestroyUpdate, this));
	}
}

//========================================
//
// 当たっている間
//
//========================================
void EnemyBaseScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	OnDeltaCollisionEnter(collider);
}

//========================================
//
// 離れた時
//
//========================================
void EnemyBaseScript::OnDeltaCollisionExit(DeltaCollider* collider)
{
}

// レベルからステータスを取得(エネミータイプで違いがある)
void EnemyBaseScript::SetUpStatus(Type type, float level)
{
	const int baseParam = 10;
	const int randParam = 5;
	m_type = type;
	m_status.Level = level;

	switch (type)
	{
	case ECS::EnemyBaseScript::Type::Attack:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam - 5) * level;
		m_status.Damage					= (rand() % randParam + baseParam + 10) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 5) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam - 5) * level;
		break;
	case ECS::EnemyBaseScript::Type::Speed:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam - 5) * level;
		m_status.Damage					= (rand() % randParam + baseParam - 5) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 5) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam + 10) * level;
		break;
	case ECS::EnemyBaseScript::Type::Skiil:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam + 0) * level;
		m_status.Damage					= (rand() % randParam + baseParam - 5) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 15) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam - 5) * level;
		break;
	case ECS::EnemyBaseScript::Type::Physical:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam + 15) * level;
		m_status.Damage					= (rand() % randParam + baseParam + 0) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam - 5) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam - 5) * level;
		break;
	case ECS::EnemyBaseScript::Type::Balance:
		m_status.HP = m_status.MaxHP	= (rand() % randParam + baseParam + 3) * level;
		m_status.Damage					= (rand() % randParam + baseParam + 3) * level;
		m_status.AttackSpeed			= (rand() % randParam + baseParam + 3) * level;
		m_status.MoveSpeed				= (rand() % randParam + baseParam + 3) * level;
		break;
	case ECS::EnemyBaseScript::Type::Max:
		break;
	default:
		break;
	}
}

// 生成時更新
void EnemyBaseScript::CreateUpdate()
{
	// 大きさを徐々に大きくする
	if (m_fScale < m_fMaxScale)
	{
		m_fScale += ADD_SCALE;
		if (m_fScale > m_fMaxScale)
		{
			m_fScale = m_fMaxScale;
			m_isNext = true;
		}

		// 大きさを反映
		transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	}
}
// 削除時更新
void EnemyBaseScript::DestroyUpdate()
{
	m_bActive = false;

	// 大きさを徐々に小さくする
	if (m_fScale > 0)
	{
		m_fScale -= ADD_SCALE;
		if (m_fScale <= 0)
		{
			m_fScale = 0;
			// 削除
			Destroy(gameObject().lock());
			Destroy(m_HPUI.lock());
			m_isNext = true;
		}

		// 大きさを反映
		transform().lock()->m_scale = Vector3(m_fScale, m_fScale, m_fScale);
	}
}
