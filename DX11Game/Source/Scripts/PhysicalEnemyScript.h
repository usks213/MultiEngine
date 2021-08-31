/*****************************************************************//**
 * \file   PhysicalEnemyScript.h
 * \brief  �t�B�W�J���G�l�~�[
 * 
 * \author USAMI KOSHI
 * \date   2021/08/28
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "EnemyBaseScript.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class PhysicalEnemyScript : public Script
	{
		// ----- �����o -----
		std::weak_ptr<EnemyBaseScript> m_enemy;
		int m_nUp = 1;

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

	};
}
