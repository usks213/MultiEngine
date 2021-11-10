/*****************************************************************//**
 * \file   OBBTestScript.h
 * \brief  OBB�e�X�g�v���O����
 * 
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class OBBTestScript : public Script
	{
	private:
		// ----- �����o -----

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
