//==================================================================
//								PlayerScript.cpp
//	プレイヤースクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2020/12/27	プレイヤースクリプトクラス作成(Test)
//
//===================================================================


//====== インクルード部 ======

// 自身
#include "PlayerScript.h"

// システム
#include "../Engine/System/input.h"
#include "../Engine/System/Sound.h"
#include "../Engine/System/debugproc.h"

// マネージャー
#include "../Engine/ECS/Entity/EntityManager.h"

// エンティティ
#include "../Engine/ECSEntity/GameObject.h"

// コンポーネント
#include "../Engine/ECSCompoent/Transform.h"
#include "../Engine/ECSCompoent/MeshRenderer.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"
#include "../Engine/ECSCompoent/Rigidbody.h"
#include "../Engine/ECSCompoent/BoxCollider.h"
#include "../Engine/ECSCompoent/SphereCollider.h"
#include "../Engine/ECSCompoent/DeltaCollider.h"
#include "../Engine/ECSCompoent/InstancingMeshRenderer.h"

// レンダラー
#include "../Engine/ECSCompoent/Camera.h"
#include "../Engine/Renderer/Light.h"
#include "../Engine/Renderer/PostProcessing.h"

#include "../Engine/ECSSystem/DeltaCollisionSystem.h"

// ワールド
#include "../Engine/ECS/World/WorldManager.h"
#include "../Engine/ECSWorld/TitleWorld.h"

// スクリプト
#include "BulletScript.h"
#include "EnemyBaseScript.h"
#include "GameOverScript.h"
#include "BombCrystalScript.h"
#include "BombEffectScript.h"
#include "SkillUIScript.h"
#include "PlayerHPUIScript.h"


#include <iostream>


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====
#define ANIM_IDLE			(0)
#define ANIM_JUMP			(1)
#define ANIM_KICK			(2)
#define ANIM_RUN			(3)
#define ANIM_SLASH			(4)
#define ANIM_WALK			(5)


//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void PlayerScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Player");
	gameObject().lock()->SetTag("Player");

	transform().lock()->m_pos = Vector3(0, 0, 0);
	transform().lock()->m_scale = Vector3(2, 2, 2);
	transform().lock()->m_rot = Quaternion();

	// コンポーネントの追加

	// リジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);
	//rb->SetUseGravity(false);

	// レンダラー
	const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
	renderer->ModelLoad("data/model/Sword And Shield.fbx");
	renderer->SetAnimIndex(ANIM_IDLE);
	renderer->SetAnimSpeed(1.0f);
	m_assimp = renderer;

	// コライダー
	//const auto& collider = gameObject().lock()->AddComponent<SphereCollider>();
	//collider->SetRadius(50);
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();
	collider->SetRadius(50);

	// UI
	const auto& ui = gameObject().lock()->AddComponent<SkillUIScript>();
	ui->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());
	const auto& hp = gameObject().lock()->AddComponent<PlayerHPUIScript>();
	hp->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());

	// アクティブ
	m_bActive = true;
	// ジャンプ
	m_nJump = 0;
	// ショット
	m_bShot = false;
	// ステータス
	m_HP = getMaxHP();

	m_rotSpeed = 0.1f;
}

//========================================
//
//	更新時
//
//========================================
void PlayerScript::Update()
{
	// アクティブ
	if (!m_bActive) return;

	const auto& rb = m_rb.lock();
	Quaternion rot = transform().lock()->m_rot;
	Vector3 forward = Matrix::CreateFromQuaternion(rot).Forward();
	Vector3 right = Matrix::CreateFromQuaternion(rot).Right();
	Vector3 up = Vector3(0, 1, 0);

	const auto& camera = Camera::main();
	auto& cameraTrans = camera->transform();
	Vector3 inputDir;
	if (m_attackCount < 0 && m_kickCount < 0 && m_jumpCount < 0)
	{
		// 前後
		if (GetKeyPress(VK_UP) || GetKeyPress(VK_W))
		{
			inputDir.z -= 1.0f;
		}
		if (GetKeyPress(VK_DOWN) || GetKeyPress(VK_S))
		{
			inputDir.z += 1.0f;
		}
		// 左右
		if (GetKeyPress(VK_RIGHT) || GetKeyPress(VK_D))
		{
			inputDir.x += 1.0f;
		}
		if (GetKeyPress(VK_LEFT) || GetKeyPress(VK_A))
		{
			inputDir.x -= 1.0f;
		}

		// 回転を反映
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
			// 力を反映
			moveForce.x *= -1;
			moveForce *= getMoveSpeed();
			rb->AddForce(moveForce);

		}

		// 速度を元にアニメーション
		Vector3 velo = rb->GetVelocity();
		velo.y = 0;
		float len = velo.Length() * 0.1f;
		if (len > 0.1f)
		{
			// 歩行アニメーション
			if (m_assimp.lock()->GetCurrentAnimIndex() != ANIM_RUN)
			{
				m_assimp.lock()->SetAnimIndex(ANIM_RUN);
			}
			len = len > 1.0f ? 1.0f : len;
			m_assimp.lock()->SetAnimSpeed(len);
		}
		else
		{
			// 待機アニメーション
			if (m_assimp.lock()->GetCurrentAnimIndex() != ANIM_IDLE)
			{
				m_assimp.lock()->SetAnimIndex(ANIM_IDLE);
				m_assimp.lock()->SetAnimSpeed(1.0f);
			}
		}
	}

	// 攻撃
	if ((GetKeyTrigger(VK_C) || GetMouseButton(MK_LBUTTON)) &&
		m_attackCount < 0 &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_SLASH)
	{
		m_attackCount = 180 - 90 * getAttackSpeed();
		m_assimp.lock()->SetAnimIndex(ANIM_SLASH);
		m_assimp.lock()->SetAnimSpeed(getAttackSpeed());

	}
	m_attackCount--;
	if (m_attackCount == (int)(180 - 90 * getAttackSpeed()) / 2)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir =
			Mathf::Normalize(transform().lock()->forward());

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
		test->transform().lock()->m_scale = Vector3(500, 500, 500);
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
	}

	// キック
	if ((GetKeyTrigger(VK_X) || GetMouseButton(MK_RBUTTON)) &&
		m_kickCount < 0 &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_KICK)
	{
		m_kickCount = 180 - 100 * getAttackSpeed();
		m_assimp.lock()->SetAnimIndex(ANIM_KICK);
		m_assimp.lock()->SetAnimSpeed(getAttackSpeed());

	}
	m_kickCount--;
	if (m_kickCount == (int)(180 - 100 * getAttackSpeed()) / 2)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir =
			Mathf::Normalize(transform().lock()->forward());

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
		test->transform().lock()->m_scale = Vector3(500, 500, 500);
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
	}

	// ジャンプ
	m_jumpCount--;
	if ((GetKeyTrigger(VK_Z) || GetKeyTrigger(VK_SPACE)) &&
		m_jumpCount < 0 &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_JUMP)
	{
		m_jumpCount = 50;
		m_assimp.lock()->SetAnimIndex(ANIM_JUMP);
		m_assimp.lock()->SetAnimSpeed(1.0f);

		rb->AddForceY(getJumpForce());
	}
}

//========================================
//
//	後更新時
//
//========================================
void PlayerScript::LateUpdate()
{
	// アクティブ
	if (!m_bActive) return;

	const auto& camera = Camera::main();
	auto cameraTrans = camera->transform().lock();

	cameraTrans->m_pos = transform().lock()->m_pos + Vector3(0, 10, -5) * 75 * 2;
	Vector3 dir = transform().lock()->m_pos - cameraTrans->m_pos;
	dir.Normalize();
	cameraTrans->forward(dir);

	if (!m_bGround && transform().lock()->m_pos.y <= transform().lock()->m_scale.y / 2)
	{
		m_bGround = true;
		// 画面揺れ
		Camera::main()->SetShakeOffset(Vector2(0, 20));
		Camera::main()->SetShake(6);
		// サウンド
		CSound::PlaySE("PlayerGround.wav", 1.0f);
	}


	// --- ステータス

	// 無敵時間
	m_nDamageInvCount--;
	if (m_nDamageInvCount < 0) m_nDamageInvCount = 0;

	// 回復
	m_nHeelCount++;
	if (m_nHeelCount > getHeelTime())
	{
		m_HP += getMaxHP() * 0.1f;
		m_nHeelCount = 0;
	}
	// 最大値
	if (m_HP > getMaxHP()) m_HP = getMaxHP();


	// HPを画面に反映
	if (m_HP < getMaxHP() / 2)
	{
		auto post = PostProcessing::GetMain();
		post->GetColor()->y = m_HP / (getMaxHP() / 2);
		post->GetColor()->z = m_HP / (getMaxHP() / 2);
	}
}

//========================================
//
//	終了時
//
//========================================
void PlayerScript::End()
{

}


//******************** コールバック関数 ********************

//========================================
//
// 当たった時
//
//========================================
void PlayerScript::OnDeltaCollisionEnter(DeltaCollider* collider)
{
	// アクティブ
	if (!m_bActive) return;

	if (collider->gameObject().lock()->tag() == "StatusItem")
	{
		// カウンター加算
		const auto& item = collider->gameObject().lock()->GetComponent<StatusItemScript>();
		m_aItemCount[static_cast<int>(item->getType())]++;
		if(item->getType() == ItemType::Physical)
			m_HP += getMaxHP() * 0.1f;
	}
	else if (collider->gameObject().lock()->tag() == "BombCrystal")
	{
		m_nJump = 15;
		// 画面揺れ
		//Camera::GetMainCamera()->SetShakeFrame(6);
		// 回復
	}
	else if (collider->gameObject().lock()->tag() == "StartCrystal")
	{
		m_bShot = true;
		// 画面揺れ
		//Camera::GetMainCamera()->SetShakeFrame(6);
		// BGM
		CSound::PlayBGM("GameBGM.mp3", 0.3f);

	}
	else if (collider->gameObject().lock()->tag() == "Enemy" || 
		collider->gameObject().lock()->tag() == "EnemyBullet")
	{
		// 無敵時間
		if (m_nStepInvCount > 0) return;
		if (m_nDamageInvCount > 0) return;
		m_nDamageInvCount = 60;

		if (collider->gameObject().lock()->tag() == "Enemy")
		{
			// 取得
			const auto& enemy = collider->gameObject().lock()->GetComponent<EnemyBaseScript>();
			// ダメージ
			float damage = enemy->m_status.Damage;
			// HP
			m_HP -= damage;
		}
		else if (collider->gameObject().lock()->tag() == "EnemyBullet")
		{
			// 取得
			const auto& enemy = collider->gameObject().lock()->GetComponent<BulletScript>();
			// ダメージ
			float damage = enemy->m_damage;
			// HP
			m_HP -= damage;
		}

		// ノックバック
		Vector3 dir = transform().lock()->m_pos - collider->transform().lock()->m_pos;
		dir.Normalize();
		m_rb.lock()->SetForce(dir * 20);

		// 画面揺れ
		Camera::main()->SetShakeOffset(Vector2(20, 20));
		Camera::main()->SetShake(16);

		// サウンド
		CSound::PlaySE("PlayerDamage.wav", 1.0f);

		if (m_HP > 0) return;
		// ゲームオーバー
		const auto& gameover = GetEntityManager()->CreateEntity<GameObject>();
		gameover->AddComponent<GameOverScript>();

		// アクティブ
		m_bActive = false;

		// 削除
		Destroy(gameObject().lock());
	}
}

//========================================
//
// 当たっている間
//
//========================================
void PlayerScript::OnDeltaCollisionStay(DeltaCollider* collider)
{
	// 同じ処理
	OnDeltaCollisionEnter(collider);
}

//========================================
//
// 離れた時
//
//========================================
void PlayerScript::OnDeltaCollisionExit(DeltaCollider* collider)
{

}

