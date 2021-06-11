//==================================================================
//								Transform.h
//	�g�����X�t�H�[��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/07/22	�g�����X�t�H�[���N���X�̍쐬
//	2020/09/23	�R���|�[�l���g��
//	2020/12/23	ECS�p�ɍĐ݌v
//	2021/05/25	��]���N�H�[�^�j�I����
//
//===================================================================

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

	class Transform final : public IComponent
	{
	public:
		// �R���X�g���N�^
		Transform();
		// �f�X�g���N�^
		~Transform();

		// �I�u�W�F�N�g������
		void OnCreate() override;
		// �I�u�W�F�N�g�j����
		void OnDestroy() override;

		// ���b�Z�[�W��M
		void SendComponentMessage(const std::string& message, void* value) override;

		// �}�g���b�N�X�̍X�V
		void UpdateMatrix();

		// �Q�b�g�֐�
		Matrix* GetWorldMatrix() { return &m_mtxWorld; }
		// �Z�b�g�֐�
		void SetWorldMatrix(const Matrix& world) { m_mtxWorld = world; }

		// �X�e�[�^�X
		Vector3 m_pos;				// ���S���W
		Vector3 m_scale;			// �c���T�C�Y
		//Vector3 m_rot;				// �I�u�W�F�N�g�̉�]�p�x
		Quaternion m_rot;

		void right(Vector3 right) {
			Matrix mtx = Matrix::CreateFromQuaternion(m_rot);
			mtx.Right(right);
			m_rot = Quaternion::CreateFromRotationMatrix(mtx);
		}
		Vector3 right() {
			return Matrix::CreateFromQuaternion(m_rot).Right();
		}

		void up(Vector3 up) {
			Matrix mtx = Matrix::CreateFromQuaternion(m_rot);
			mtx.Up(up);
			m_rot = Quaternion::CreateFromRotationMatrix(mtx);
		}
		Vector3 up() {
			return Matrix::CreateFromQuaternion(m_rot).Up();
		}

		void forward(Vector3 forward) {
			Matrix mtx = Matrix::CreateFromQuaternion(m_rot);
			mtx.Forward(forward);
			m_rot = Quaternion::CreateFromRotationMatrix(mtx);
		}
		Vector3 forward() {
			return Matrix::CreateFromQuaternion(m_rot).Forward();
		}

		// �Q�[���I�u�W�F�N�g�Z�b�g
		void SetGameObject(std::weak_ptr<GameObject>& gameObject) { m_gameObject = gameObject; }
		// �Q�[���I�u�W�F�N�g�̎擾
		const std::weak_ptr<GameObject>& gameObject()			  { return m_gameObject; }

	private:
		// �}�g���b�N�X
		Matrix m_mtxWorld;

		// �e�̃Q�[���I�u�W�F�N�g
		std::weak_ptr<GameObject> m_gameObject;
	};
}
