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

	transform().lock()->m_pos = Vector3(0, 300, 1000);
	transform().lock()->m_scale = Vector3(1, 1, 1);
	transform().lock()->m_rot = Quaternion::CreateFromYawPitchRoll(XMConvertToRadians(180), 0, 0);

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


	// カメラ固定


	// 初期化
	m_speed = 2.0f;
	m_rotSpeed = 1.5f;
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
	if (GetKeyPress(VK_W))
	{
		rb->AddForce(forward * m_speed);
	}

	// 水平軸回転
	if (GetKeyPress(VK_UP))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-m_rotSpeed));
		rot *= q;
	}
	if (GetKeyPress(VK_DOWN))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(m_rotSpeed));
		rot *= q;
	}

	// 奥行軸回転
	if (GetKeyPress(VK_RIGHT))
	{
		Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(-m_rotSpeed));
		rot *= q;
	}
	if (GetKeyPress(VK_LEFT))
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
		CSound::PlaySE("Shot.wav", 0.12f);
		m_nShotCnt = 6;
	}

	// カメラを追尾
	const auto& camera = Camera::main();
	Vector3 vec = 
		transform().lock()->m_pos - camera->transform().lock()->m_pos;
	camera->transform().lock()->forward(Mathf::Normalize(vec));

	if (Mathf::Length(vec) > camera->GetFarZ() * 0.2f)
	{
		camera->transform().lock()->m_pos += 
			Mathf::Normalize(vec) * (Mathf::Length(vec) - camera->GetFarZ() * 0.2f);
	}
}

//========================================
//
//	後更新時
//
//========================================
void PlaneControlScript::LateUpdate()
{

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

