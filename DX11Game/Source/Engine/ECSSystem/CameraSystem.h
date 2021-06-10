/*****************************************************************//**
 * \file   CameraSystem.h
 * \brief  �J�����V�X�e��
 * 
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../ECS/System/System.h"


//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class Camera;

	class CameraSystem final : public System<Camera>
	{
	public:
		// �R���X�g���N�^
		explicit CameraSystem(World* pWorld);
		// �f�X�g���N�^
		virtual ~CameraSystem();

		// ������
		void OnCreate() override;
		// �X�V
		void OnLateUpdate() override;
		// �폜��
		void OnDestroy() override;
	};
}
