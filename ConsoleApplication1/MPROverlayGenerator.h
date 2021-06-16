
#pragma once

#include "MPRGenerator.h"

enum E_DIRECTION;
struct stLabelItem;

// MPR Overlay Render Scene 을 생성해주는 클래스
class CMPROverlayGenerator : public CMPRGenerator
{

public:

	CMPROverlayGenerator();

	bool SetLabelImage(vtkImageData *pImage);

	void SetLabelColorTable(std::vector<stLabelItem>* pTable);

	virtual void MoveSlice(int amount);
		
	void UpdateLabel();

protected:

	// MRI Overlay 영상, 자료구조가 외부에 존재하며 포인터만 관리.	
	vtkImageData* m_pImageLabel;

	vtkSP<vtkImageReslice> m_pResliceLabel;
	vtkSP<vtkLookupTable> m_pTableLabel;
	vtkSP<vtkImageMapToColors> m_pColorLabel;
	vtkSP<vtkImageActor> m_pActorLabel;

	std::vector<stLabelItem>* m_pLabelColorTable;

};
