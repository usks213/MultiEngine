//==================================================================
//												RendererSystem.h
//	�����_���[�Ǘ��V�X�e��
//
//==================================================================
//	author :	AT12A 05 �F�����W�V
//==================================================================
//	�J������
//
//	2020/12/23	�����_���[�V�X�e���쐬
//
//===================================================================


//===== �C���N���[�h�� =====
#include "RendererSystem.h"
#include "../ECS/Entity/EntityManager.h"
#include "../ECSCompoent/Renderer.h"
#include <algorithm>
#include <execution>

#include "../main.h"


using namespace ECS;


//===== �}�N����` =====


//===== �v���g�^�C�v�錾 =====


//===== �O���[�o���ϐ� =====



//===================================
//
//	�R���X�g���N�^
//
//===================================
RendererSystem::RendererSystem(World* pWorld)
	: System<Renderer>(pWorld)
{
	// �X�V��
	SetUpdateOrder(UpdateOrder::eRenderer);
}


//===================================
//
//	�f�X�g���N�^
//
//===================================
RendererSystem::~RendererSystem()
{
}


//===================================
//
//	�������R�[���o�b�N
//
//===================================
void RendererSystem::OnCreate()
{
}

//===================================
//
//	�`�掞�R�[���o�b�N
//
//===================================
void RendererSystem::OnDraw()
{
	// �f�o�C�X�R���e�L�X�g
	ID3D11DeviceContext* pDC = GetDeviceContext();
	std::vector<Renderer*> opequeList;
	std::vector<Renderer*> transparentList;
	opequeList.reserve(m_ComponentList.size());
	transparentList.reserve(m_ComponentList.size() / 2);

	// ���C���[�X�V
	std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
		[&opequeList, &transparentList](Renderer* trans)
		{
			trans->LayerUpdate();
			if (trans->GetTransparent())
			{
				transparentList.push_back(trans);
			}
			else
			{
				opequeList.push_back(trans);
			}
		});

	// �\�[�g
	std::sort(transparentList.begin(), transparentList.end(), Renderer::swapR);
	std::sort(opequeList.begin(), opequeList.end(), Renderer::swapL);
	//m_ComponentList.sort(Renderer::swapR);

	// �O�`��
	//std::for_each(m_ComponentList.begin(), m_ComponentList.end(),
	//	[&pDC](Renderer* trans)
	//	{
	//		trans->EarlyDraw(pDC);
	//	});

	// �s����
	std::for_each(opequeList.begin(), opequeList.end(),
		[&pDC](Renderer* trans)
		{
			trans->LateDraw(pDC);
		});
	// ������
	std::for_each(transparentList.begin(), transparentList.end(),
		[&pDC](Renderer* trans)
		{
			trans->LateDraw(pDC);
		});
}

//===================================
//
//	�폜���R�[���o�b�N
//
//===================================
void RendererSystem::OnDestroy()
{
}

//===================================
//
//	�\�[�g�p
//
//===================================
bool RendererSystem::swapR(Renderer* lhs, Renderer* rhs) {
	return lhs->GetLayer() > rhs->GetLayer(); // �~���ɂ��Ă݂�
}