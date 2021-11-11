//==================================================================
//								PlayerScript.h
//	�v���C���[�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/27	�v���C���[�X�N���v�g�N���X�쐬(Test)
//	2021/01/09	�f���^�J�E���^�[�̒ǉ�
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "StatusItemScript.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �v���C���[�X�L��
	enum class PlayerSkill {
		Shot,	// �ʏ�
		Step,	// ���
		Burst,	// �A��
		Bom,	// ����
		Max,
	};

	class PlayerScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<Rigidbody> m_rb;
		std::weak_ptr<AssimpRenderer> m_assimp;
		float m_speed;
		float m_rotSpeed;
		int m_jumpCount = 0;
		int m_attackCount = 0;
		int m_kickCount = 0;

		// �A�N�e�B�u
		bool m_bActive = true;
		// �W�����v�t���O
		int m_nJump = 0;
		// �V���b�g
		bool m_bShot = false;
		// �n��
		bool m_bGround = false;

		// �J�E���^
		int m_nStepInvCount = 0;
		int m_nDamageInvCount = 0;
		int m_nBurstCount = 0;
		int m_nHeelCount = 0;

	public:
		// HP
		float m_HP = 100;
		// �X�e�[�^�X�A�C�e��
		int m_aItemCount[static_cast<std::size_t>(ItemType::Max)];
		// �X�L��
		int m_aSkillRecastCnt[static_cast<std::size_t>(PlayerSkill::Max)];

		//--- �X�e�[�^�X ---

		float getMaxHP() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Physical)];
			return 200 + cnt * 10;
		}
		float getHeelTime() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Physical)];
			float heel = 300 - cnt * 0.1f;
			if (heel < 0) heel = 0;
			return heel + 60;
		}

		float getDamage() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Attack)];
			return 10 + cnt * 5.0f;
		}
		float getAttackSpeed() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Attack)];
			float speed = 1.2f + cnt * 0.1f;
			if (speed < 0) speed = 0;
			return speed;
		}

		float getMoveSpeed() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 1.2f + cnt * 0.2f;
		}
		float getJumpForce() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 15 + cnt * 0.1f;
		}
		float getStepTime() {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Speed)];
			return 10 + cnt * 0.05f;
		}

		float getSkiilRecast(int skillIndex) {
			int cnt = m_aItemCount[static_cast<std::size_t>(ItemType::Skiil)];
			float recast = 180 - cnt;
			if (recast < 0) recast = 0;

			int skillTime = 0;
			switch (skillIndex)
			{
			case (int)ECS::PlayerSkill::Shot:
				return getAttackSpeed();
				break;
			case (int)ECS::PlayerSkill::Step:
				skillTime = 120;
				break;
			case (int)ECS::PlayerSkill::Burst:
				skillTime = 60;
				break;
			case (int)ECS::PlayerSkill::Bom:
				skillTime = 240;
				break;
			}
			return recast + skillTime;
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
		void OnDeltaCollisionEnter(DeltaCollider* collider) override;
		// �������Ă����
		void OnDeltaCollisionStay(DeltaCollider* collider) override;
		// ���ꂽ��
		void OnDeltaCollisionExit(DeltaCollider* collider) override;
	};
}
