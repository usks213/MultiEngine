/*****************************************************************//**
 * \file   AnimPlayerScript.h
 * \brief  アシンプアニメーションプレイヤー
 * 
 * \author USAMI KOSHI
 * \date   2021/11/10
 *********************************************************************/
#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Scripts/StatusItemScript.h"
#include "../Scripts/PlayerHPUIScript.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class AnimPlayerScript : public Script
	{
	private:
		// ----- メンバ -----
		std::weak_ptr<Rigidbody> m_rb;
		std::weak_ptr<AssimpRenderer> m_assimp;
		float m_speed;
		float m_rotSpeed;
		int m_jumpCount = 0;
		int m_attackCount = 0;

		// HP
		float m_HP = 100;
		// ステータスアイテム
		int m_aItemCount[static_cast<std::size_t>(ItemType::Max)];
		// スキル
		int m_aSkillRecastCnt[static_cast<std::size_t>(PlayerSkill::Max)];

		//--- ステータス ---

		float getMaxHP() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Physical)];
			return 100 + cnt * 10;
		}
		float getHeelTime() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Physical)];
			float heel = 300 - cnt * 0.1f;
			if (heel < 0) heel = 0;
			return heel + 60;
		}

		float getDamage() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Attack)];
			return 5 + cnt * 0.5f;
		}
		float getAttackSpeed() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Attack)];
			float speed = 0.8f + cnt * 0.1f;
			if (speed < 0) speed = 0;
			return speed;
		}

		float getMoveSpeed() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 0.5f + cnt * 0.02f;
		}
		float getJumpForce() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 10 + cnt * 0.01f;
		}
		float getStepTime() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 10 + cnt * 0.05f;
		}

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
