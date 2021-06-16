
#pragma once

#include "InteractorCB_MPR.h"

class CDataModel;

// VTK DICOM Rendering Interator �� Interactor Callback
class CInteractorCB_DicomView: public CInteractorCB_MPR
{
public:
	VTK_NEW_MACRO(CInteractorCB_DicomView);
	
	// ������ ���� Interactor �� �����԰� �Բ� Interactor �� �ʱ�ȭ ����
	bool InitDataModel(CDataModel* pData);

	// Ȱ��ȭ View Index �� ����� ������ �ڵ� ����Ǵ� �Լ�.
	virtual void OnChangeFocusedView(int prev, int cur);

	// ���콺 ���� ���۵� ������ �ڵ� ����Ǵ� �Լ�.
	virtual void OnMouseWheel(int delta);

	// Label �ڷᱸ���� ������ 3D View �� ����
	virtual void OnUpdateLabelTo3DView();

	// �׸��� �׼�
	virtual void OnDrawAction(int nScrPos[2]);
	virtual void OnOverDrawAction(int nScrPos[2]);

	static void PrintUsage();

public: // Utility Class

	// 3D Image �����͸� �������� ���� ������ �������������� �߰�.
	static void Add_Volume_Rendering_Data_To_Renderer(vtkImageData* pData, vtkMatrix4x4* pMat,
		vtkRenderer* pRenderer);

	// ���� �𵨸��ϴ� �Լ�
	static void Modeling_Sphere(vtkImageData* pLabelImage, float radius, int pt3d[3], unsigned char nLabel);

	// ť�긦 �𵨸��ϴ� �Լ�
	static void Modeling_Cube(vtkImageData* pLabelImage, int radius, int pt3d[3], unsigned char nLabel);

	// vtkImageData �ڷᱸ���� ���� ���� �Լ�
	static void SetLabelValue(vtkImageData* pLabelImage, int x, int y, int z,
		int anDim[3], unsigned char nLabel);
	
protected:

	// ���콺 �� Ű���� ���� �ڵ鷯 �Լ�
	virtual void Execute(vtkObject *Caller, unsigned long event, void *callData);

	CInteractorCB_DicomView(); // ������ �� �ʱ�ȭ ��ƾ
		
protected:

	CDataModel* m_pDataset;
	int m_nIdxFocusedView; // ���� Ȱ��ȭ �� View Index �� ����

	// 3D Orientation Marker ǥ�ø� ���� �ڷᱸ��
	vtkSP<vtkOrientationMarkerWidget> m_pOrientMarker;

	bool m_bLBtnDown;

	// Label ������ 3D View �� ǥ���� �� Label �� �� Object �ڷᱸ��
	std::vector<vtkSP<vtkActor>> m_pVisLabel3D;

	int m_nBrushValue;
	int m_nBrushSize;
	
};