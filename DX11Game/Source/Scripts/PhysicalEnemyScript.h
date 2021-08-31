/*****************************************************************//**
 * \file   PhysicalEnemyScript.h
 * \brief  フィジカルエネミー
 * 
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "EnemyBaseScript.h"


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class PhysicalEnemyScript : public Script
	{
		// ----- メンバ -----
		std::weak_ptr<EnemyBaseScript> m_enemy;
		int m_nUp = 1;

	protected:
		// ----- メソッド -----

		// 開始時に呼ばれます
		void Start() override final;
		// 毎フレーム呼ばれます
		void Update() override final;
		// 毎フレーム更新後に呼ばれます
		void LateUpdate() override final;
		// 終了時に呼ばれます
		void End() override final;

	};
}
