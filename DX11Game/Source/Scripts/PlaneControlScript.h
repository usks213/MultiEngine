/*****************************************************************//**
 * \file   PlaneControlScript.cpp
 * \brief  課題02 飛行機コントロール
 *
 * \author USAMI KOSHI
 * \date   2021/06/10　飛行機コントロールクラス作成
 *********************************************************************/
#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class PlaneControlScript : public Script
	{
	private:
		// ----- メンバ -----
		std::weak_ptr<Rigidbody> m_rb;
		float m_speed;
		float m_rotSpeed;
		int m_nShotCnt;
		float m_rad;
		enum ECameraMode
		{
			eConst,
			eOne,
			eTherd,
			eRotate,

			eMaxMode,
		};
		ECameraMode m_eMode;

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
