//==================================================================
//								BombCollisionScript.h
//	ボム当たり判定スクリプト
//
//==================================================================
//	author :	AT12A 05 宇佐美晃之
//==================================================================
//	開発履歴
//
//	2021/01/11	ボムコリジョンスクリプトクラス作成
//
//===================================================================

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class BombCollisionScript : public Script
	{
	private:
		// ----- メンバ -----
		int m_nExitTime;	// 生存時間
		float m_damage;		// ダメージ
	public:
		void SetDamage(float damage) { m_damage = damage; }
		float GetDamage() { return m_damage; }
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
