/*****************************************************************//**
 * \file   TestMoveScript.h
 * \brief
 *
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "TestMoveScript.h"

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



//******************** スクリプトメソッド ********************


//========================================
//
//	開始時
//
//========================================
void TestMoveScript::Start()
{
	// 名前・タグ

	// コンポーネントの追加

	// リジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);
	rb->SetUseGravity(false);
	rb->SetDrag(Vector3(0.05f, 0.05f, 0.05f));
}

//========================================
//
//	更新時
//
//========================================
void TestMoveScript::Update()
{

	const float speed = 1.0f;
	const float rotSpeed = 3.141592f / 60.0f;

	// 向き
	const auto& trans = transform().lock();
	Matrix matR = Matrix::CreateFromQuaternion(trans->m_rot);
	Quaternion rot = transform().lock()->m_rot;
	Vector3 vPos = matR.Translation();
	Vector3 right = matR.Right();
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = matR.Forward();
	Vector3 vLook = vPos + forward;
	float focus = 0.0f;

	if (!GetKeyPress(VK_RBUTTON))
	{
		// 移動
		if (GetKeyPress(VK_W))
		{
			m_rb.lock()->AddForce(Vector3(0, 1, 0));
		}
		if (GetKeyPress(VK_S))
		{
			m_rb.lock()->AddForce(-Vector3(0, 1, 0));
		}
		if (GetKeyPress(VK_D))
		{
			m_rb.lock()->AddForce(-Vector3(1, 0, 0));
		}
		if (GetKeyPress(VK_A))
		{
			m_rb.lock()->AddForce(Vector3(1, 0, 0));
		}

		// 回転
		// 水平軸回転
		if (GetKeyPress(VK_UP))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-1));
			rot *= q;
		}
		if (GetKeyPress(VK_DOWN))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(1));
			rot *= q;
		}
		// 奥行軸回転
		if (GetKeyPress(VK_RIGHT))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(-1));
			rot *= q;
		}
		if (GetKeyPress(VK_LEFT))
		{
			Quaternion q = Quaternion::CreateFromAxisAngle(forward, XMConvertToRadians(1));
			rot *= q;
		}
		transform().lock()->m_rot = rot;
	}
}

//========================================
//
//	後更新時
//
//========================================
void TestMoveScript::LateUpdate()
{

}

//========================================
//
//	終了時
//
//========================================
void TestMoveScript::End()
{

}

