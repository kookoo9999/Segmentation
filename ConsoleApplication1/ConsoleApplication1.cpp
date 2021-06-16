// Test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "DataModel.h"
#include "InteractorCB_DIcomView.h"

int main(int argc, char* argv[])
{
	if (argc < 2) return -1;

	vtkObject::GlobalWarningDisplayOff();

	VTK_MODULE_INIT(vtkRenderingOpenGL2);
	VTK_MODULE_INIT(vtkInteractionStyle);
	VTK_MODULE_INIT(vtkRenderingFreeType);
	VTK_MODULE_INIT(vtkRenderingVolumeOpenGL2);
	VTK_MODULE_INIT(vtkRenderingContextOpenGL2);

	// Step 1. Configuration Rendering Pipeline
	// Create a renderer and render window
	C_VTK(vtkRenderer, pRenderer_A);
	pRenderer_A->SetBackground(0, 0, 0);
	C_VTK(vtkRenderer, pRenderer_S);
	pRenderer_S->SetBackground(0, 0, 0);
	C_VTK(vtkRenderer, pRenderer_C);
	pRenderer_C->SetBackground(0, 0, 0);
	C_VTK(vtkRenderer, pRenderer_3D);
	pRenderer_3D->SetBackground(0, 0, 0);

	C_VTK(vtkRenderWindow, renWin);
	renWin->SetSize(800, 800);

	renWin->AddRenderer(pRenderer_A); // Axial
	renWin->AddRenderer(pRenderer_S); // Sagittal
	renWin->AddRenderer(pRenderer_C); // Coronal
	renWin->AddRenderer(pRenderer_3D); // 3D

	double viewport_position[4][4] = {
		{ 0.0, 0.5, 0.5, 1.0 }, // 좌상
		{ 0.5, 0.5, 1.0, 1.0 }, // 우상
		{ 0.0, 0.0, 0.5, 0.5 }, // 좌하
		{ 0.5, 0.0, 1.0, 0.5 }, // 우하
	};

	pRenderer_A->SetViewport(viewport_position[0]); // 좌상
	pRenderer_3D->SetViewport(viewport_position[1]); // 우상
	pRenderer_S->SetViewport(viewport_position[2]); // 좌하
	pRenderer_C->SetViewport(viewport_position[3]); // 우하

	C_VTK(vtkRenderWindowInteractor, rwInteractor); // Create an interactor
	rwInteractor->SetRenderWindow(renWin);

	CDataModel data_model;
	data_model.AddRenderer(pRenderer_A, ED_AXIAL); // 추가 순서 중요.
	data_model.AddRenderer(pRenderer_S, ED_SAGITTAL);
	data_model.AddRenderer(pRenderer_C, ED_CORONAL);
	data_model.AddRenderer(pRenderer_3D, ED_3D);

	C_VTK(CInteractorCB_DicomView, pInteractorCB);
	pInteractorCB->SetInteractor(rwInteractor);

	// Step 2. Register Rendering Data	
	{
		C_VTK(vtkDICOMImageReader, reader);
		reader->SetDirectoryName(argv[1]);
		reader->Update();
		if (reader->GetErrorCode() == vtkErrorCode::NoError)
		{
			data_model.SetImage(reader->GetOutput());

			auto orient = reader->GetImageOrientationPatient();

			auto pOrigin = data_model.GetImage()->GetOrigin();

			double vector1[3] = { orient[0], orient[1], orient[2] };
			double vector2[3] = { orient[3], orient[4], orient[5] };
			double vector3[3] = { 0, };

			vtkMath::Cross(vector1, vector2, vector3);

			// 데이터 가시화 시 방향 정렬을 위한 Matrix 설정
			data_model.m_alignMatrix = {
				vector1[0], vector1[1], vector1[2], pOrigin[0],
				vector2[0], vector2[1], vector2[2], pOrigin[1],
				vector3[0], vector3[1], vector3[2], pOrigin[2],
				0, 0, 0, 1
			};
		}
		else
		{
			printf("DICOM Reading Error.\n");
		}
	}

	rwInteractor->Initialize();

	// 데이터 모델을 Interactor 에 설정함과 함께 Interactor 의 초기화 수행
	bool bInit = pInteractorCB->InitDataModel(&data_model);
	if (!bInit)
	{
		printf("Initiailization Error.\n");
	}

	// 3. Run Rendering Pipeline				
	renWin->Render(); // Render the scene (lights and cameras are created automatically)

	rwInteractor->Start();

	return 0;
}
