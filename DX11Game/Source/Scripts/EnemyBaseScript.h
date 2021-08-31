//==================================================================
//								EnemyBaseScript.h
//	�G�l�~�[�x�[�X�X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/09	�G�l�~�[�x�[�X�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "MakeEnemyScript.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class MakeEnemyScript;

	class EnemyBaseScript : public Script
	{
		friend MakeEnemyScript;
	protected:
		// ----- �����o -----
		// HPUI
		std::weak_ptr<GameObject> m_HPUI;
		// ���W�b�h�{�f�B
		std::weak_ptr<Rigidbody> m_rb;

		// �A�N�e�B�u
		bool m_bActive = true;

		// �X�P�[��
		float m_fScale;
		float m_fMaxScale;

		// �C�x���g���X�g
		std::list<std::function<void(void)>> m_eventList;
		// �C�x���g�t���O
		bool m_isNext = false;

		// �p�j�J�E���g
		int m_nMoveCnt = 240;
		Vector3 m_dir;
	public:
		// �^�C�v
		enum class Type
		{
			Attack,		// �U��
			Speed,		// �X�s�[�h
			Skiil,	// �h��
			Physical,	// �̗�
			Balance,	// �o�����X
			Max,
		};
		Type m_type;

		// �X�e�[�^�X
		struct Status
		{
			int   Level;
			float HP;
			float MaxHP;
			float Damage;
			float AttackSpeed;
			float MoveSpeed;
		};
		Status m_status;

		// ���C�J�[
		std::weak_ptr<MakeEnemyScript> m_maker;
		// �^�[�Q�b�g
		std::weak_ptr<GameObject> m_player;
		// ���b�N�I��
		bool m_isLockon = false;
		// �U���J�E���g
		int m_nAttackCount = 0;

	public:
		// ���W�b�h�{�f�B�擾
		std::weak_ptr<Rigidbody> GetRb() { return m_rb; }
		// �v���C���[�̃Z�b�g
		void SetPlayer(std::weak_ptr<GameObject> player) { m_player = player; }

		// �C�x���g�i�[�X�V
		void EventPushBack(const std::function<void(void)>& event) { m_eventList.push_back(event); }
		void EventPushFront(const std::function<void(void)>& event) { m_eventList.push_front(event); }
		// ���x������X�e�[�^�X���擾(�G�l�~�[�^�C�v�ňႢ������)
		void SetUpStatus(Type type, float level);
		// �T�C�Y
		void SetMaxScale(float scale) { m_fMaxScale = scale; }
	protected:
		// ----- ���\�b�h -----

		// �J�n���ɌĂ΂�܂�
		void Start() override final;
		// ���t���[���Ă΂�܂�
		void Update() override final;
		// ���t���[���X�V��ɌĂ΂�܂�
		void LateUpdate() override final;
		// �I�����ɌĂ΂�܂�
		void End() override final;

		// ----- �R�[���o�b�N�֐� -----

		// Delta
		// ����������
		virtual void OnDeltaCollisionEnter(DeltaCollider* collider) override final;
		// �������Ă����
		virtual void OnDeltaCollisionStay(DeltaCollider* collider) override final;
		// ���ꂽ��
		virtual void OnDeltaCollisionExit(DeltaCollider* collider) override final;

		
		// ----- �G�l�~�[ -----

		// �������X�V
		void CreateUpdate();
		// �폜���X�V
		void DestroyUpdate();
	};
}
