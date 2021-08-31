//==================================================================
//								BombCollisionScript.h
//	�{�������蔻��X�N���v�g
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2021/01/11	�{���R���W�����X�N���v�g�N���X�쐬
//
//===================================================================

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class BombCollisionScript : public Script
	{
	private:
		// ----- �����o -----
		int m_nExitTime;	// ��������
		float m_damage;		// �_���[�W
	public:
		void SetDamage(float damage) { m_damage = damage; }
		float GetDamage() { return m_damage; }
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
