/*****************************************************************//**
 * \file   PlaneControlScript.cpp
 * \brief  課題02 飛行機コントロール
 *
 * \author USAMI KOSHI
 * \date   2021/06/10　飛行機コントロールクラス作成
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "PlaneControlScript.h"

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


// ネームスペース
using namespace ECS;


//===== マクロ定義 =====



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void PlaneControlScript::Start()
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
	renderer->ModelLoad("data/model/JAS39.fbx");

	// コライダー
	const auto& collider = gameObject().lock()->AddComponent<DeltaCollider>();

	// リジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	rb->SetUseGravity(true);
	m_rb = rb;
	rb->SetDrag(Vector3{ 0.05f, 0.05f, 0.05f });


	// カメラ
	Camera::main()->transform().lock()->m_pos = transform().lock()->m_pos + Vector3(0, 500, 100);

	// 初期化
	m_speed = 2.0f;
	m_rotSpeed = 1.5f;
	m_rad = 0;
	m_eMode = ECameraMode::eConst;

}

//========================================
//
//	更新時
//
//========================================
void PlaneControlScript::Update()
{
	const auto& rb = m_rb.lock();
	Quaternion rot = transform().lock()->m_rot;
	Vector3 forward = Matrix::CreateFromQuaternion(rot).Forward();
	Vector3 right = Matrix::CreateFromQuaternion(rot).Right();
	Vector3 up = Matrix::CreateFromQuaternion(rot).Up();

	// 前進
	//if (GetKeyPress(VK_W))
	{
		rb->AddForce(forward * m_speed);
	}

	// 水平軸回転
	if (GetKeyPress(VK_UP) || GetKeyPress(VK_W))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-m_rotSpeed));
		rot *= q;
	}
	if (GetKeyPress(VK_DOWN) || GetKeyPress(VK_S))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(m_rotSpeed));
		rot *= q;
	}

	// 奥行軸回転
	if (GetKeyPress(VK_RIGHT) || GetKeyPress(VK_D))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(-m_rotSpeed));
		rot *= q;
	}
	if (GetKeyPress(VK_LEFT) || GetKeyPress(VK_A))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(m_rotSpeed));
		rot *= q;
	}
	transform().lock()->m_rot = rot;

	// ショット
	m_nShotCnt--;
	if (GetKeyPress(VK_SPACE) && m_nShotCnt < 0)
	{
		//const auto& test = GetEntityManager()->CreateEntity<GameObject>();
		const auto& test = Instantiate<GameObject>();
		test->AddComponent<BulletScript>();
		const auto& rb = test->GetComponent<Rigidbody>();

		Vector3 dir = Mathf::Normalize(forward);

		test->transform().lock()->m_pos = transform().lock()->m_pos + dir * 500;
		rb->AddForce(dir * 100 + Mathf::WallVerticalVector(m_rb.lock()->GetForce(), dir));
		rb->AddTorque(Quaternion::CreateFromAxisAngle(dir, XMConvertToRadians(dir.Length() * 10)));

		// サウンド
		CSound::PlaySE("Shot.wav", 0.3f);
		m_nShotCnt = 6;
	}
}

//========================================
//
//	後更新時
//
//========================================
void PlaneControlScript::LateUpdate()
{
	// カメラを追尾
	Quaternion rot = transform().lock()->m_rot;
	const auto& camera = Camera::main();

	if (GetKeyPress(VK_1)) m_eMode = ECameraMode::eConst;
	if (GetKeyPress(VK_2)) m_eMode = ECameraMode::eOne;
	if (GetKeyPress(VK_3)) m_eMode = ECameraMode::eTherd;
	if (GetKeyPress(VK_4)) m_eMode = ECameraMode::eRotate;


	switch (m_eMode)
	{
	case ECS::PlaneControlScript::eConst:
	{
		//--- 固定
		Vector3 vec = transform().lock()->m_pos - camera->transform().lock()->m_pos;
		camera->transform().lock()->forward(Mathf::Normalize(vec));

		if (Mathf::Length(vec) > camera->GetFarZ() * 0.5f)
		{
			camera->transform().lock()->m_pos +=
				Mathf::Normalize(vec) * (Mathf::Length(vec) - camera->GetFarZ() * 0.5f);
		}
		break;
	}
	case ECS::PlaneControlScript::eOne:
	{
		//--- 一人称
		camera->transform().lock()->m_rot = rot;
		camera->transform().lock()->m_pos =
			transform().lock()->m_pos + Vector3::Transform(Vector3(0, 40, -150), rot);
		break;
	}
	case ECS::PlaneControlScript::eTherd:
	{
		//--- 三人称
		camera->transform().lock()->m_rot = rot;
		camera->transform().lock()->m_pos =
			transform().lock()->m_pos + Vector3::Transform(Vector3(0, 500, 1000), rot) * 0.75f;
		break;
	}
	case ECS::PlaneControlScript::eRotate:
	{
		//--- 回転
		m_rad += 0.02f;
		Quaternion q = Quaternion::CreateFromYawPitchRoll(m_rad, 0, 0);
		camera->transform().lock()->m_pos =
			transform().lock()->m_pos + Vector3::Transform(Vector3(0, 0, 1000), q);

		Vector3 vec = transform().lock()->m_pos - camera->transform().lock()->m_pos;
		camera->transform().lock()->forward(Mathf::Normalize(vec));
		break;
	}
	}
}

//========================================
//
//	終了時
//
//========================================
void PlaneControlScript::End()
{
}


//******************** コールバック関数 ********************


//========================================
//
// 当たった時
//
//========================================
void PlaneControlScript::OnCollisionEnter(Collider* collider)
{

}

//========================================
//
// 当たっている間
//
//========================================
void PlaneControlScript::OnCollisionStay(Collider* collider)
{

}

//========================================
//
// 離れた時
//
//========================================
void PlaneControlScript::OnCollisionExit(Collider* collider)
{

}

