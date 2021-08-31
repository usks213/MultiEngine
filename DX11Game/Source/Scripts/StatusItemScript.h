/*****************************************************************//**
 * \file   StatusItemScript.h
 * \brief  �X�e�[�^�X�A�C�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	enum class ItemType
	{
		Physical,
		Attack,
		Speed,
		Skiil,
		Max,
	};

	class StatusItemScript : public Script
	{
	private:
		// ----- �����o -----
		int m_nExitTime;	// ��������
		std::weak_ptr<Rigidbody> m_rb;
		// �v���C���[
		std::weak_ptr<GameObject> m_player;
		// �ǔ��t���O
		bool m_IsTracking;
		// �X�P�[��
		float m_fScale;
		// �^�C�v
		ItemType m_eType;

	public:
		// �v���C���[�̃Z�b�g
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }
		// �^�C�v�̎w��
		void SetItemType(ItemType type);
		// �A�C�e���^�C�v
		ItemType getType() { return m_eType; }
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
