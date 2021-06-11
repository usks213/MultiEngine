/*****************************************************************//**
 * \file   PlaneControlScript.cpp
 * \brief  �ۑ�02 ��s�@�R���g���[��
 *
 * \author USAMI KOSHI
 * \date   2021/06/10�@��s�@�R���g���[���N���X�쐬
 *********************************************************************/
#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class PlaneControlScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<Rigidbody> m_rb;
		float m_speed;
		float m_rotSpeed;
		int m_nShotCnt;
		float m_rad;
		enum ECameraMode
		{
			eConst,
			eOne,
			eTherd,
			eRotate,

			eMaxMode,
		};
		ECameraMode m_eMode;

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
