
#pragma once

#include "MPRGenerator.h"

enum E_DIRECTION;
struct stLabelItem;

// MPR Overlay Render Scene �� �������ִ� Ŭ����
class CMPROverlayGenerator : public CMPRGenerator
{

public:

	CMPROverlayGenerator();

	bool SetLabelImage(vtkImageData *pImage);

	void SetLabelColorTable(std::vector<stLabelItem>* pTable);

	virtual void MoveSlice(int amount);
		
	void UpdateLabel();

protected:

	// MRI Overlay ����, �ڷᱸ���� �ܺο� �����ϸ� �����͸� ����.	
	vtkImageData* m_pImageLabel;

	vtkSP<vtkImageReslice> m_pResliceLabel;
	vtkSP<vtkLookupTable> m_pTableLabel;
	vtkSP<vtkImageMapToColors> m_pColorLabel;
	vtkSP<vtkImageActor> m_pActorLabel;

	std::vector<stLabelItem>* m_pLabelColorTable;

};
