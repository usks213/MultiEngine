/*****************************************************************//**
 * \file   CameraControlScript.h
 * \brief
 *
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/


//====== インクルード部 ======

// 自身
#include "CameraControlScript.h"

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
void CameraControlScript::Start()
{
	// 名前・タグ
	gameObject().lock()->SetName("Camera");
	gameObject().lock()->SetTag("Camera");

	transform().lock()->m_scale = Vector3(100, 100, 100);
	transform().lock()->m_rot = Quaternion();

	// コンポーネントの追加

	// リジッドボディ
	const auto& rb = gameObject().lock()->AddComponent<Rigidbody>();
	m_rb = rb;
	rb->SetMass(2);
	rb->SetUseGravity(false);
	rb->SetDrag(Vector3(0.05f, 0.05f, 0.05f));

	// マウス
	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);

	Update();
	LateUpdate();
}

//========================================
//
//	更新時
//
//========================================
void CameraControlScript::Update()
{
	// 視点移動
	static POINT m_oldMousePos;
	POINT* mousePos = GetMousePosition();
	POINT mouseDist = {
		mousePos->x - m_oldMousePos.x,
		mousePos->y - m_oldMousePos.y,
	};
	m_oldMousePos = *mousePos;

	if (!GetKeyPress(VK_RBUTTON)) return;

	const float speed = 1.0f;
	const float rotSpeed = 3.141592f / 60.0f;

	// 向き
	const auto& trans = transform().lock();
	Matrix matR = Matrix::CreateFromQuaternion(trans->m_rot);
	Vector3 vPos = matR.Translation();
	Vector3 right = matR.Right();
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = matR.Forward();
	Vector3 vLook = vPos + forward;
	float focus = 0.0f;

	// 移動
	if (GetKeyPress(VK_W))
	{
		m_rb.lock()->AddForce(forward);
	}
	if (GetKeyPress(VK_S))
	{
		m_rb.lock()->AddForce(-forward);
	}
	if (GetKeyPress(VK_D))
	{
		m_rb.lock()->AddForce(-right);
	}
	if (GetKeyPress(VK_A))
	{
		m_rb.lock()->AddForce(right);
	}

	// 左ボタン(カメラ回り込み
	static bool isRot = true;
	if (GetKeyTrigger(VK_MBUTTON))
	{
		isRot ^= 1;
		//SetShowCursor(!isRot);
	}
	if(true)
	{
		// マウス固定
		//SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);
		//m_oldMousePos = POINT{ SCREEN_CENTER_X, SCREEN_CENTER_Y };
		//SetShowCursor(false);

		// 回転量
		float angleX = 360.f * mouseDist.x / SCREEN_WIDTH * 0.5f;	// 画面一周で360度回転(画面サイズの半分で180度回転)
		float angleY = 180.f * mouseDist.y / SCREEN_HEIGHT * 0.5f;	// 画面一周で180度回転(画面サイズの半分で90度回転)

		//Vector3::Transform(right, Matrix::CreateFromAxisAngle(up, XMConvertToRadians(angleX)));
		trans->m_rot *= Quaternion::CreateFromAxisAngle(right, XMConvertToRadians(-angleY));
		trans->m_rot *= Quaternion::CreateFromAxisAngle(up, XMConvertToRadians(angleX));
	}
	else
	{
		//SetShowCursor(true);
	}
}

//========================================
//
//	後更新時
//
//========================================
void CameraControlScript::LateUpdate()
{
	// カメラを追尾
	Quaternion rot = transform().lock()->m_rot;
	const auto& camera = Camera::main();

	//--- 一人称
	camera->transform().lock()->m_rot = rot;
	camera->transform().lock()->m_pos =
		transform().lock()->m_pos;
	//--- 三人称
	//camera->transform().lock()->m_rot = rot;
	//camera->transform().lock()->m_pos =
	//	transform().lock()->m_pos + Vector3::Transform(Vector3(0, 500, 1000), rot) * 0.75f;

}

//========================================
//
//	終了時
//
//========================================
void CameraControlScript::End()
{

}

