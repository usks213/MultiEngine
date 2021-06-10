/*****************************************************************//**
 * \file   ExplosionScript.cpp
 * \brief  課題爆発エフェクト
 *
 * \author USAMI KOSHI
 * \date   2021/05/14　課題爆発エフェクトクラス作成
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"

//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class ExplosionScript : public Script
	{
	private:
		// ----- メンバ -----
		std::weak_ptr<AssimpRenderer> m_renderer;
		std::vector<std::weak_ptr<GameObject>> m_aExplosionEffect;
		float m_destroyTime;
		float m_time;

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
