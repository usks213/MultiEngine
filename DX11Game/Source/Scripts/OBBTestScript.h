/*****************************************************************//**
 * \file   OBBTestScript.h
 * \brief  OBBテストプログラム
 * 
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class OBBTestScript : public Script
	{
	private:
		// ----- メンバ -----

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


		// ----- コールバック関数 -----

		// 当たった時
		void OnCollisionEnter(Collider* collider) override;
		// 当たっている間
		void OnCollisionStay (Collider* collider) override;
		// 離れた時
		void OnCollisionExit (Collider* collider) override;
	};
}
