/*****************************************************************//**
 * \file   ExplosionScript.cpp
 * \brief  �ۑ蔚���G�t�F�N�g
 *
 * \author USAMI KOSHI
 * \date   2021/05/14�@�ۑ蔚���G�t�F�N�g�N���X�쐬
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "../Engine/ECSCompoent/AssimpRenderer.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class ExplosionScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<AssimpRenderer> m_renderer;
		std::vector<std::weak_ptr<GameObject>> m_aExplosionEffect;
		float m_destroyTime;
		float m_time;

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
