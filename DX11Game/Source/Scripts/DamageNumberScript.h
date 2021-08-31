/*****************************************************************//**
 * \file   DamageNumberScript.h
 * \brief  �_���[�W�\�L
 * 
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/BillboardRenderer.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class DamageNumberScript : public Script
	{
	private:
		// ----- �����o -----

		// ���W�b�h�{�f�B
		std::weak_ptr<Rigidbody> m_rb;
		// ��������
		int m_nExistTime = 0;

		// ����
		struct NumberData
		{
			std::weak_ptr<BillboardRenderer> m_sprite;
			int m_nAnimNo;
		};
		std::vector<NumberData> m_data;

		// ��
		int m_nNumber = 0;
		// ����
		int m_nDigits = 0;
		// �����Ԋu
		int m_nNumSpace = 0;

	public:
		// �����w��
		void SetNumber(int num) { m_nNumber = num; }
		// �����Ԋu�w��
		void SetNumScape(int space) { m_nNumSpace = space; }

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

		// UV�X�V
		void UpdateTexCoord();
	};
}
