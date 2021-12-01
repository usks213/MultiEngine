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

	m_rotSpeed = 0.2f;
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
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] <= 0 && 
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] <= 0 && 
		m_jumpCount < 0)
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
		if (len > 0.3f)
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

	// ジャンプ
	m_jumpCount--;
	if (GetKeyTrigger(VK_SPACE) &&
		m_jumpCount < 0 &&
		m_bGround == true &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_JUMP)
	{
		m_jumpCount = 50;
		m_assimp.lock()->SetAnimIndex(ANIM_JUMP);

		// ジャンプ力から逆算して、アニメーションのスピードを
		m_assimp.lock()->SetAnimSpeed(1.0f);
		rb->AddForceY(getJumpForce());

		// サウンド
		CSound::PlaySE("Jump.wav", 0.7f);
		// 画面揺れ
		Camera::main()->SetShakeOffset(Vector2(0, 20));
		Camera::main()->SetShake(8);
		m_bGround = false;
	}

	// 攻撃
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] = 0;
	if (GetKeyTrigger(MK_LBUTTON) &&
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] <= 0 &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_SLASH)
	{
		m_assimp.lock()->SetAnimIndex(ANIM_SLASH);
		m_assimp.lock()->SetAnimSpeed(90 / getSkiilRecast(static_cast<int>(PlayerSkill::Shot)));

		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] = 
			getSkiilRecast(static_cast<int>(PlayerSkill::Shot));
	}
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Shot)] ==
		(int)(getSkiilRecast(static_cast<int>(PlayerSkill::Shot)) / 2))
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir =
			Mathf::Normalize(transform().lock()->forward());

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
		test->transform().lock()->m_scale = Vector3(300, 300, 300);
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));

		// サウンド
		CSound::PlaySE("Shot.wav", 0.2f);
		// 画面揺れ
		Camera::main()->SetShakeOffset(Vector2(40, 20));
		Camera::main()->SetShake(4);
	}

	//// キック
	//m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)]--;
	//if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] < 0)
	//	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] = 0;
	//if (GetKeyTrigger(VK_E) &&
	//	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] <= 0 &&
	//	m_assimp.lock()->GetCurrentAnimIndex() != ANIM_KICK)
	//{
	//	m_assimp.lock()->SetAnimIndex(ANIM_KICK);
	//	m_assimp.lock()->SetAnimSpeed(75 / getSkiilRecast(static_cast<int>(PlayerSkill::Burst)));

	//	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] =
	//		getSkiilRecast(static_cast<int>(PlayerSkill::Burst));
	//}
	//if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] == 
	//	(int)(getSkiilRecast(static_cast<int>(PlayerSkill::Burst)) / 2))
	//{
	//	const auto& test = Instantiate<GameObject>();
	//	test->AddComponent<BulletScript>()->m_damage = getDamage();
	//	const auto& rb = test->GetComponent<Rigidbody>();

	//	Vector3 dir =
	//		Mathf::Normalize(transform().lock()->forward());

	//	test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
	//	test->transform().lock()->m_scale = Vector3(500, 500, 500);
	//	rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
	//	rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
	//}

	// キック
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] = 0;
	if (GetKeyTrigger(VK_E) &&
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] <= 0 &&
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_KICK)
	{
		m_assimp.lock()->SetAnimIndex(ANIM_KICK);
		m_assimp.lock()->SetAnimSpeed(75 / getSkiilRecast(static_cast<int>(PlayerSkill::Burst)));
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] =
					getSkiilRecast(static_cast<int>(PlayerSkill::Burst));
	}
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Burst)] == 
		(int)(getSkiilRecast(static_cast<int>(PlayerSkill::Burst)) / 3 * 2))
	{
		// バースト
		m_nBurstCount = 20;
		// 画面揺れ
		Camera::main()->SetShakeOffset(Vector2(10, 40));
		Camera::main()->SetShake(m_nBurstCount);
	}
	// バーストカウンタ
	m_nBurstCount--;
	if (m_nBurstCount < 0) m_nBurstCount = 0;
	// バースト弾生成
	if (m_nBurstCount > 0 && m_nBurstCount % 4 == 0)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();
		const auto& camera = Camera::main();
		Vector3 dir = Mathf::Normalize(transform().lock()->forward());
		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 300;
		test->transform().lock()->m_scale = Vector3(300, 300, 300);
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
		// サウンド
		CSound::PlaySE("Shot.wav", 0.2f);
	}

	// ステップ
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] = 0;
	Vector3 force = m_rb.lock()->GetForce();
	if (GetKeyTrigger(VK_RBUTTON) && m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] <= 0 &&
		force.Length() > 1.0f)
	{
		// ステップリキャスト
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Step)] =
			getSkiilRecast(static_cast<int>(PlayerSkill::Step));
		// 無敵時間
		m_nStepInvCount = getStepTime();
		// 画面揺れ
		Camera::main()->SetShakeOffset(Vector2(7, 5));
		Camera::main()->SetShake(m_nStepInvCount);
	}

	// 無敵時間
	m_nStepInvCount--;
	if (m_nStepInvCount < 0) m_nStepInvCount = 0;
	// ステップエフェクト生成
	if (m_nStepInvCount > 0 && m_nStepInvCount % 4 == 0)
	{
		force.y = 0;
		force.Normalize();
		m_rb.lock()->SetForce(force * 100);

		const int n = 10;
		//const float f = rand() % 314 / 100.0f;
		for (int i = 0; i < n; ++i)
		{
			float phi = 360.0f / n * i * (XM_PI / 180.0f);

			for (int j = 0; j < n; ++j)
			{
				float theta = 360.0f / n * j * (XM_PI / 180.0f);
				// 座標
				Vector3 pos;
				pos.x = cosf(phi) * cosf(theta);
				pos.y = cosf(phi) * sinf(theta);
				pos.z = sinf(phi);
				pos *= 50;

				// エフェクト生成
				const auto& obj = Instantiate<GameObject>(pos + transform().lock()->m_pos);
				// コンポーネントの追加
				const auto& effect = obj->AddComponent<BombEffectScript>();
				// リジッドボディ
				const auto& rb = obj->GetComponent<Rigidbody>();
				rb->AddForce(Mathf::Normalize(pos) * 5);
				// レンダラ
				const auto& render = obj->GetComponent<InstancingMeshRenderer>();
				render->SetDiffuseColor(Vector4(0, 1, 1, 1));
			}
		}
	}

	// ボム
	m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)]--;
	if (m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] < 0)
		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] = 0;
	if (GetKeyPress(VK_Q) && m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] <= 0)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BombCrystalScript>()->SetDamage(getDamage());
		const auto& rb = test->AddComponent<Rigidbody>();
		rb->SetUseGravity(false);
		rb->SetDrag(Vector3(0, 0, 0));
		rb->SetTorqueDrag(0);
		const auto& col = test->GetComponent<DeltaCollider>();
		col->SetMain(true);

		Vector3 dir = Mathf::Normalize(forward);
		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 500;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));

		m_aSkillRecastCnt[static_cast<int>(PlayerSkill::Bom)] =
			getSkiilRecast(static_cast<int>(PlayerSkill::Bom));

		// サウンド
		CSound::PlaySE("Shot.wav", 0.2f);
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

