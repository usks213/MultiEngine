/*****************************************************************//**
 * \file   AnimPlayerScript.h
 * \brief  �A�V���v�A�j���[�V�����v���C���[
 * 
 * \author USAMI KOSHI
 * \date   2021/11/10
 *********************************************************************/
#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Scripts/StatusItemScript.h"
#include "../Scripts/PlayerHPUIScript.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class AnimPlayerScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<Rigidbody> m_rb;
		std::weak_ptr<AssimpRenderer> m_assimp;
		float m_speed;
		float m_rotSpeed;
		int m_jumpCount = 0;
		int m_attackCount = 0;

		// HP
		float m_HP = 100;
		// �X�e�[�^�X�A�C�e��
		int m_aItemCount[static_cast<std::size_t>(ItemType::Max)];
		// �X�L��
		int m_aSkillRecastCnt[static_cast<std::size_t>(PlayerSkill::Max)];

		//--- �X�e�[�^�X ---

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
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override;
		// ���t���[���Ă΂�܂�
		void Update() override;
		// ���t���[���X�V��ɌĂ΂�܂�
		void LateUpdate() override;
		// �I�����ɌĂ΂�܂�
		void End() override;


		// ----- �R�[���o�b�N�֐� -----

		// ����������
		void OnCollisionEnter(Collider* collider) override;
		// �������Ă����
		void OnCollisionStay (Collider* collider) override;
		// ���ꂽ��
		void OnCollisionExit (Collider* collider) override;
	};
}
