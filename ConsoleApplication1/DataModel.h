
#pragma once

#include <vector>

enum E_DIRECTION
{
	ED_AXIAL = 0,
	ED_SAGITTAL,
	ED_CORONAL,
	ED_3D,
};

class CMPROverlayGenerator;

struct stLabelItem
{
	stLabelItem(int pv, double pr, double pg, double pb, double pa)
		: value(pv)
	{
		rgba[0] = pr;
		rgba[1] = pg;
		rgba[2] = pb;
		rgba[3] = pa;
	}

	int value;
	double rgba[4];	
};

// VTK DICOM Rendering Interactor ���� ����� ������ ��
class CDataModel
{
public:

	CDataModel();
	virtual ~CDataModel();

	// 3D Image ������ ����, ��ȸ �Լ�
	void SetImage(vtkImageData* pImage);
	vtkImageData* GetImage();

	// Label Image �� �����ϱ� ���� �ڷᱸ��
	vtkImageData* GetLabelImage();

	// ���� ������ �߰�, ��ȸ, ���� ���� �Լ�	
	void AddRenderer(vtkRenderer* pRen, E_DIRECTION viewMode);
	vtkRenderer* GetRenderer(int idx);
	int GetRendererCount() const;

	// MPR Scene Generator �� ����
	CMPROverlayGenerator* GetMPRGenerator(int idx);

	// ��ũ�� ��ǥ�� ��ġ�� ��� View port �� �������� Ŭ���Ͽ����� �ε��� ����. ������ ��� -1 ����
	int CheckRendererIndex(vtkRenderWindow* pRenWin, int nScreenPos[2]);

	std::vector<stLabelItem>* GetLabelColorTable();

public:

	std::vector<double> m_alignMatrix; // Main �������� Align Matrix

protected:

	vtkSP<vtkImageData> m_pMainImage; // Main ������
	vtkSP<vtkImageData> m_pLabelImage; // Label ������

	std::vector<stLabelItem> m_arColorTable; // Color Table ������
	
	std::vector<vtkRenderer*> m_arRenderer; // 4���� ������ ����. �»�, ���, ����, ���� ����.
	std::vector<CMPROverlayGenerator*> m_arMPRGen; // MPR Image ���� Ŭ����. Axial, Sagittal, Coronal.

};