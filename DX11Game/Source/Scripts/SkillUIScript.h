/*****************************************************************//**
 * \file   SkillUIScript.h
 * \brief  �X�L��UI
 * 
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"
#include "PlayerScript.h"
#include "NumberScript.h"
#include <array>


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class SkillUIScript : public Script
	{
	private:
		// ----- �����o -----

		// �v���C���[
		std::weak_ptr<PlayerScript> m_player;
		
		// UI�f�[�^
		struct SkillUIData
		{
			std::weak_ptr<SpriteRenderer>	skillImage;
			std::weak_ptr<SpriteRenderer>	keyImage;
			std::weak_ptr<SpriteRenderer>	recastFade;
			std::weak_ptr<NumberScript>		recastTime;
		};
		std::array<SkillUIData, static_cast<int>(PlayerSkill::Max)> m_aUIData;
		

	public:
		// �v���C���[�̎擾
		void SetPlayer(std::weak_ptr<PlayerScript> player) { m_player = player; }

	protected:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;
		// ���t���[���Ă΂�܂�
		void Update() override;
		// ���t���[���X�V��ɌĂ΂�܂�
		void LateUpdate() override;
		// �I�����ɌĂ΂�܂�
		void End() override;
	};
}
