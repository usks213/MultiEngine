/*****************************************************************//**
 * \file   PlayerHPUIScript.h
 * \brief  プレイヤーHPUI
 * 
 * \author USAMI KOSHI
 * \date   2021/08/30
 *********************************************************************/

#pragma once

//====== インクルード部 ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "PlayerScript.h"
#include "NumberScript.h"
#include <array>


//===== マクロ定義 =====


//===== クラス定義 =====
namespace ECS
{
	class PlayerHPUIScript : public Script
	{
	private:
		// ----- メンバ -----

		// プレイヤー
		std::weak_ptr<PlayerScript> m_player;

		// UIデータ
		std::weak_ptr<SpriteRenderer>	m_bgImage;
		std::weak_ptr<SpriteRenderer>	m_gaugeImage;
		std::weak_ptr<SpriteRenderer>	m_slashImage;

		std::weak_ptr<NumberScript>		m_hpNum;
		std::weak_ptr<NumberScript>		m_maxHpNum;


	public:
		// プレイヤーの指定
		void SetPlayer(std::weak_ptr<PlayerScript> player) { m_player = player; }

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
