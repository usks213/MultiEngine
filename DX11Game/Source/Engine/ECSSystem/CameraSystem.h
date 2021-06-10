/*****************************************************************//**
 * \file   CameraSystem.h
 * \brief  カメラシステム
 * 
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../ECS/System/System.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	// 前定義
	class Camera;

	class CameraSystem final : public System<Camera>
	{
	public:
		// コンストラクタ
		explicit CameraSystem(World* pWorld);
		// デストラクタ
		virtual ~CameraSystem();

		// 生成時
		void OnCreate() override;
		// 更新
		void OnLateUpdate() override;
		// 削除時
		void OnDestroy() override;
	};
}
