/*****************************************************************//**
 * \file   AnimPlayerScript.h
 * \brief  アシンプアニメーションプレイヤー
 *
 * \author USAMI KOSHI
 * \date   2021/11/10
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "AnimPlayerScript.h"

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
#include "BulletScript.h"
#include "BombCrystalScript.h"


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
void AnimPlayerScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Player");
	gameObject().lock()->SetTag("Player");

	transform().lock()->m_pos = Vector3(0, 300, 0);
	transform().lock()->m_scale = Vector3(1, 1, 1);
	transform().lock()->m_rot =
		Quaternion::CreateFromYawPitchRoll(0, XMConvertToRadians(5), 0);

	// コンポーネントの追加

	// レンダラー
	const auto& renderer = gameObject().lock()->AddComponent<AssimpRenderer>();
	renderer->ModelLoad("data/model/Sword And Shield.fbx");
	renderer->SetAnimIndex(ANIM_IDLE);
	renderer->SetAnimSpeed(1.0f);
	m_assimp = renderer;

	// コライダー
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();

	// リジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetUseGravity(true);
	m_rb = rb;
	rb->SetDrag(Vector3{ 0.05f, 0.05f, 0.05f });

	// UI
	const auto& hp = gameObject().lock()->AddComponent<PlayerHPUIScript>();
	hp->SetPlayer(gameObject().lock()->GetComponent<PlayerScript>());

	// カメラ
	//Camera::main()->transform().lock()->m_pos = transform().lock()->m_pos + Vector3(0, 500, 100);

	// 初期化
	m_speed = 1.0f;
	m_rotSpeed = 0.1f;

}

//========================================
//
//	更新時
//
//========================================
void AnimPlayerScript::Update()
{
	const auto& rb = m_rb.lock();
	Quaternion rot = transform().lock()->m_rot;
	Vector3 forward = Matrix::CreateFromQuaternion(rot).Forward();
	Vector3 right = Matrix::CreateFromQuaternion(rot).Right();
	Vector3 up = Vector3(0,1,0);

	const auto& camera = Camera::main();
	auto& cameraTrans = camera->transform();
	Vector3 inputDir;
	if (m_attackCount < 0)
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
		if (m_jumpCount < 0)
		{
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

		// ジャンプ
		m_jumpCount--;
		if (GetKeyTrigger(VK_C) &&
			m_jumpCount < 0 &&
			m_assimp.lock()->GetCurrentAnimIndex() != ANIM_JUMP)
		{
			m_jumpCount = 50;
			m_assimp.lock()->SetAnimIndex(ANIM_JUMP);
			m_assimp.lock()->SetAnimSpeed(1.0f);
		}
	}

	// 攻撃
	m_attackCount--;
	if (GetKeyTrigger(VK_Z) &&
		m_attackCount < 0 && 
		m_assimp.lock()->GetCurrentAnimIndex() != ANIM_SLASH)
	{
		m_attackCount = 90 * getAttackSpeed();
		m_assimp.lock()->SetAnimIndex(ANIM_SLASH);
		m_assimp.lock()->SetAnimSpeed(getAttackSpeed());

	}
	if (m_attackCount == 90 * getAttackSpeed() / 3)
	{
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>()->m_damage = getDamage();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir =
			Mathf::Normalize(transform().lock()->forward());

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));
	}
}

//========================================
//
//	後更新時
//
//========================================
void AnimPlayerScript::LateUpdate()
{
	const auto& camera = Camera::main();
	auto cameraTrans = camera->transform().lock();

	cameraTrans->m_pos = transform().lock()->m_pos + Vector3(0, 10, -5) * 75;
	Vector3 dir = transform().lock()->m_pos - cameraTrans->m_pos;
	dir.Normalize();
	cameraTrans->forward(dir);
}

//========================================
//
//	終了時
//
//========================================
void AnimPlayerScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void AnimPlayerScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void AnimPlayerScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void AnimPlayerScript::OnCollisionExit(Collider* collider)
{

}

