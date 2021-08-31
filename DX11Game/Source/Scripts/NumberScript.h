/*****************************************************************//**
 * \file   NumberScript.h
 * \brief  �����X�N���v�g
 * 
 * \author USAMI KOSHI
 * \date   2021/08/29
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/SpriteRenderer.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class NumberScript : public Script
	{
	private:
		// ----- �����o -----

		// ����
		struct NumberData
		{
			std::weak_ptr<SpriteRenderer> m_sprite;
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
