/*****************************************************************//**
 * \file   PlayerHPUIScript.h
 * \brief  �v���C���[HPUI
 * 
 * \author USAMI KOSHI
 * \date   2021/08/30
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
	class PlayerHPUIScript : public Script
	{
	private:
		// ----- �����o -----

		// �v���C���[
		std::weak_ptr<PlayerScript> m_player;

		// UI�f�[�^
		std::weak_ptr<SpriteRenderer>	m_bgImage;
		std::weak_ptr<SpriteRenderer>	m_gaugeImage;
		std::weak_ptr<SpriteRenderer>	m_slashImage;

		std::weak_ptr<NumberScript>		m_hpNum;
		std::weak_ptr<NumberScript>		m_maxHpNum;


	public:
		// �v���C���[�̎w��
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
