/*****************************************************************//**
 * \file   SkillUIScript.h
 * \brief  スキルUI
 * 
 * \author USAMI KOSHI
 * \date   2021/08/29
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
	class SkillUIScript : public Script
	{
	private:
		// ----- メンバ -----

		// プレイヤー
		std::weak_ptr<PlayerScript> m_player;
		
		// UIデータ
		struct SkillUIData
		{
			std::weak_ptr<SpriteRenderer>	skillImage;
			std::weak_ptr<SpriteRenderer>	keyImage;
			std::weak_ptr<SpriteRenderer>	recastFade;
			std::weak_ptr<NumberScript>		recastTime;
		};
		std::array<SkillUIData, static_cast<int>(PlayerSkill::Max)> m_aUIData;
		

	public:
		// プレイヤーの取得
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
