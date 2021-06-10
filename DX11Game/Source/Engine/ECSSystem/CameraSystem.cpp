/*****************************************************************//**
 * \file   CameraSystem.h
 * \brief  �J�����V�X�e��
 *
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/


//===== �C���N���[�h�� =====
#include "CameraSystem.h"
#include "../ECSCompoent/Camera.h"
#include <algorithm>

using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====



//===================================
//
//	�R���X�g���N�^
//
//===================================
CameraSystem::CameraSystem(World* pWorld)
	: System<Camera>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eCamera);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
CameraSystem::~CameraSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void CameraSystem::OnCreate()
{
}

//===================================
//
//	�X�V���R�[���o�b�N
//
//===================================
void CameraSystem::OnLateUpdate()
{
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[](Camera* trans)
		{
			trans->UpdateCameraMatrix();
		});
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void CameraSystem::OnDestroy()
{
}
