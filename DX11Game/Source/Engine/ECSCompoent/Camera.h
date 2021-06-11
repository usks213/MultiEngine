/*****************************************************************//**
 * \file   Camera.h
 * \brief  �J����
 * 
 * \author USAMI KOSHI
 * \date   2021/06/10
 *********************************************************************/

#pragma once

//====== �C���N���[�h�� ======
#include "../System/Math.h"
#include "../ECS/Component/IComponent.h"

//===== �}�N����` =====


//===== �N���X��` =====
namespace ECS
{
	// �O��`
	class GameObject;
	class Transform;

	class Camera final : public IComponent
	{
	public:
		// �R���X�g���N�^
		Camera();
		// �f�X�g���N�^
		~Camera();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value) override;

		// �}�g���b�N�X�̍X�V
		inline void UpdateCameraMatrix();

		// �Q�[���I�u�W�F�N�g�Z�b�g
		void SetGameObject(std::weak_ptr<GameObject>& gameObject) { m_gameObject = gameObject; }
		// �Q�[���I�u�W�F�N�g�̎擾
		const std::weak_ptr<GameObject>& gameObject()			  { return m_gameObject; }

		// �g�����X�t�H�[���擾
		const std::weak_ptr<Transform>& transform() { return m_transform; }

		// �Q�b�g
		Matrix& GetViewMatrix() { return m_mtxView; }
		Matrix& GetProjMatrix() { return m_mtxProj; }
		float GetApsect() { return m_fAspectRatio; }
		float GetFOV() { return m_fFovY; }
		float GetNearZ() { return m_fNearZ; }
		float GetFarZ() { return m_fFarZ; }
		float GetFogNearZ() { return m_fFogNearZ; }
		float GetFogFarZ() { return m_fFogFarZ; }

	public:
		static void SetMainCamera(const std::shared_ptr<Camera>& camera) 
		{ m_mainCamera = camera; }
		static Camera* GetMainCamera() { 
			if (m_mainCamera.lock())
			{
				return m_mainCamera.lock().get();
			}
			return nullptr;
		}
		static std::shared_ptr<Camera> main() { return m_mainCamera.lock(); }

	private:
		// �e�̃Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
		// �g�����X�t�H�[��
		std::weak_ptr<Transform> m_transform;

		// �r���[�}�g���b�N�X
		Matrix m_mtxView;
		// �v���W�F�N�V�����}�g���b�N�X
		Matrix m_mtxProj;

		Vector3 m_up;					// UP�x�N�g��
		float m_fAspectRatio;			// �c����
		float m_fFovY;					// ����p(Degree)
		float m_fNearZ;					// �O���N���b�v����
		float m_fFarZ;					// ����N���b�v����
		float m_fFogNearZ;				// �t�H�O�̂�����n�߂鋗��
		float m_fFogFarZ;				// �t�H�O�̂�����I��鋗��


		static std::weak_ptr<Camera> m_mainCamera;
	};
}
