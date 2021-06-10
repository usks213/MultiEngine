/*****************************************************************//**
 * \file   CameraSystem.h
 * \brief  カメラシステム
 *
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/


//===== インクルード部 =====
#include "CameraSystem.h"
#include "../ECSCompoent/Camera.h"
#include <algorithm>

using namespace ECS;


//===== マクロ定義 =====


//===== プロトタイプ宣言 =====


//===== グローバル変数 =====



//===================================
//
//	コンストラクタ
//
//===================================
CameraSystem::CameraSystem(World* pWorld)
	: System<Camera>(pWorld)
{
	// 更新順
	SetUpdateOrder(UpdateOrder::eCamera);
}


//===================================
//
//	デストラクタ
//
//===================================
CameraSystem::~CameraSystem()
{
}


//===================================
//
//	生成時コールバック
//
//===================================
void CameraSystem::OnCreate()
{
}

//===================================
//
//	更新時コールバック
//
//===================================
void CameraSystem::OnLateUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Camera* trans)
		{
			trans->UpdateCameraMatrix();
		});
}

//===================================
//
//	削除時コールバック
//
//===================================
void CameraSystem::OnDestroy()
{
}
