/*****************************************************************//**
 * \file   CameraControlScript.h
 * \brief  
 * 
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "StatusItemScript.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class CameraControlScript : public Script
	{
	private:
		// ----- メンバ -----
		std::weak_ptr<Rigidbody> m_rb;

	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override;
		// 毎フレーム呼ばれます
		void Update() override;
		// 毎フレーム更新後に呼ばれます
		void LateUpdate() override;
		// 終了時に呼ばれます
		void End() override;
	};
}
