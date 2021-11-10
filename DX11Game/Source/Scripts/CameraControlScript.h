/*****************************************************************//**
 * \file   CameraControlScript.h
 * \brief  
 * 
 * \author USAMI KOSHI
 * \date   2021/09/09
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../Engine/ECSCompoent/Script.h"
#include "StatusItemScript.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	class CameraControlScript : public Script
	{
	private:
		// ----- �����o -----
		std::weak_ptr<Rigidbody> m_rb;

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
