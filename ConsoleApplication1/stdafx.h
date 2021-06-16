// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


//#include <stdio.h>
//#include <tchar.h>
#include <iostream>


// TODO: reference additional headers your program requires here

#pragma comment( lib, "vtkalglib-8.0.lib")
#pragma comment( lib, "vtkChartsCore-8.0.lib")
#pragma comment( lib, "vtkCommonColor-8.0.lib")
#pragma comment( lib, "vtkCommonComputationalGeometry-8.0.lib")
#pragma comment( lib, "vtkCommonCore-8.0.lib")
#pragma comment( lib, "vtkCommonDataModel-8.0.lib")
#pragma comment( lib, "vtkCommonExecutionModel-8.0.lib")
#pragma comment( lib, "vtkCommonMath-8.0.lib")
#pragma comment( lib, "vtkCommonMisc-8.0.lib")
#pragma comment( lib, "vtkCommonSystem-8.0.lib")
#pragma comment( lib, "vtkCommonTransforms-8.0.lib")
#pragma comment( lib, "vtkDICOMParser-8.0.lib")
#pragma comment( lib, "vtkDomainsChemistry-8.0.lib")
#pragma comment( lib, "vtkDomainsChemistryOpenGL2-8.0.lib")
#pragma comment( lib, "vtkexoIIc-8.0.lib")
#pragma comment( lib, "vtkexpat-8.0.lib")
#pragma comment( lib, "vtkFiltersAMR-8.0.lib")
#pragma comment( lib, "vtkFiltersCore-8.0.lib")
#pragma comment( lib, "vtkFiltersExtraction-8.0.lib")
#pragma comment( lib, "vtkFiltersFlowPaths-8.0.lib")
#pragma comment( lib, "vtkFiltersGeneral-8.0.lib")
#pragma comment( lib, "vtkFiltersGeneric-8.0.lib")
#pragma comment( lib, "vtkFiltersGeometry-8.0.lib")
#pragma comment( lib, "vtkFiltersHybrid-8.0.lib")
#pragma comment( lib, "vtkFiltersHyperTree-8.0.lib")
#pragma comment( lib, "vtkFiltersImaging-8.0.lib")
#pragma comment( lib, "vtkFiltersModeling-8.0.lib")
#pragma comment( lib, "vtkFiltersParallel-8.0.lib")
#pragma comment( lib, "vtkFiltersParallelImaging-8.0.lib")
#pragma comment( lib, "vtkFiltersPoints-8.0.lib")
#pragma comment( lib, "vtkFiltersProgrammable-8.0.lib")
#pragma comment( lib, "vtkFiltersSelection-8.0.lib")
#pragma comment( lib, "vtkFiltersSMP-8.0.lib")
#pragma comment( lib, "vtkFiltersSources-8.0.lib")
#pragma comment( lib, "vtkFiltersStatistics-8.0.lib")
#pragma comment( lib, "vtkFiltersTexture-8.0.lib")
#pragma comment( lib, "vtkFiltersTopology-8.0.lib")
#pragma comment( lib, "vtkFiltersVerdict-8.0.lib")
#pragma comment( lib, "vtkfreetype-8.0.lib")
#pragma comment( lib, "vtkGeovisCore-8.0.lib")
#pragma comment( lib, "vtkgl2ps-8.0.lib")
#pragma comment( lib, "vtkglew-8.0.lib")
#pragma comment( lib, "vtkGUISupportQt-8.0.lib")
#pragma comment( lib, "vtkGUISupportQtOpenGL-8.0.lib")
#pragma comment( lib, "vtkGUISupportQtSQL-8.0.lib")
#pragma comment( lib, "vtkhdf5_hl-8.0.lib")
#pragma comment( lib, "vtkhdf5-8.0.lib")
#pragma comment( lib, "vtkImagingColor-8.0.lib")
#pragma comment( lib, "vtkImagingCore-8.0.lib")
#pragma comment( lib, "vtkImagingFourier-8.0.lib")
#pragma comment( lib, "vtkImagingGeneral-8.0.lib")
#pragma comment( lib, "vtkImagingHybrid-8.0.lib")
#pragma comment( lib, "vtkImagingMath-8.0.lib")
#pragma comment( lib, "vtkImagingMorphological-8.0.lib")
#pragma comment( lib, "vtkImagingSources-8.0.lib")
#pragma comment( lib, "vtkImagingStatistics-8.0.lib")
#pragma comment( lib, "vtkImagingStencil-8.0.lib")
#pragma comment( lib, "vtkInfovisCore-8.0.lib")
#pragma comment( lib, "vtkInfovisLayout-8.0.lib")
#pragma comment( lib, "vtkInteractionImage-8.0.lib")
#pragma comment( lib, "vtkInteractionStyle-8.0.lib")
#pragma comment( lib, "vtkInteractionWidgets-8.0.lib")
#pragma comment( lib, "vtkIOAMR-8.0.lib")
#pragma comment( lib, "vtkIOCore-8.0.lib")
#pragma comment( lib, "vtkIOEnSight-8.0.lib")
#pragma comment( lib, "vtkIOExodus-8.0.lib")
#pragma comment( lib, "vtkIOExport-8.0.lib")
#pragma comment( lib, "vtkIOExportOpenGL2-8.0.lib")
#pragma comment( lib, "vtkIOGeometry-8.0.lib")
#pragma comment( lib, "vtkIOImage-8.0.lib")
#pragma comment( lib, "vtkIOImport-8.0.lib")
#pragma comment( lib, "vtkIOInfovis-8.0.lib")
#pragma comment( lib, "vtkIOLegacy-8.0.lib")
#pragma comment( lib, "vtkIOLSDyna-8.0.lib")
#pragma comment( lib, "vtkIOMINC-8.0.lib")
#pragma comment( lib, "vtkIOMovie-8.0.lib")
#pragma comment( lib, "vtkIONetCDF-8.0.lib")
#pragma comment( lib, "vtkIOParallel-8.0.lib")
#pragma comment( lib, "vtkIOParallelXML-8.0.lib")
#pragma comment( lib, "vtkIOPLY-8.0.lib")
#pragma comment( lib, "vtkIOSQL-8.0.lib")
#pragma comment( lib, "vtkIOTecplotTable-8.0.lib")
#pragma comment( lib, "vtkIOVideo-8.0.lib")
#pragma comment( lib, "vtkIOXML-8.0.lib")
#pragma comment( lib, "vtkIOXMLParser-8.0.lib")
#pragma comment( lib, "vtkjpeg-8.0.lib")
#pragma comment( lib, "vtkjsoncpp-8.0.lib")
#pragma comment( lib, "vtklibharu-8.0.lib")
#pragma comment( lib, "vtklibxml2-8.0.lib")
#pragma comment( lib, "vtklz4-8.0.lib")
#pragma comment( lib, "vtkmetaio-8.0.lib")
#pragma comment( lib, "vtknetcdf_c++.lib")
#pragma comment( lib, "vtkNetCDF-8.0.lib")
#pragma comment( lib, "vtkoggtheora-8.0.lib")
#pragma comment( lib, "vtkParallelCore-8.0.lib")
#pragma comment( lib, "vtkpng-8.0.lib")
#pragma comment( lib, "vtkproj4-8.0.lib")
#pragma comment( lib, "vtkRenderingAnnotation-8.0.lib")
#pragma comment( lib, "vtkRenderingContext2D-8.0.lib")
#pragma comment( lib, "vtkRenderingContextOpenGL2-8.0.lib")
#pragma comment( lib, "vtkRenderingCore-8.0.lib")
#pragma comment( lib, "vtkRenderingFreeType-8.0.lib")
#pragma comment( lib, "vtkRenderingGL2PSOpenGL2-8.0.lib")
#pragma comment( lib, "vtkRenderingImage-8.0.lib")
#pragma comment( lib, "vtkRenderingLabel-8.0.lib")
#pragma comment( lib, "vtkRenderingLOD-8.0.lib")
#pragma comment( lib, "vtkRenderingOpenGL2-8.0.lib")
#pragma comment( lib, "vtkRenderingQt-8.0.lib")
#pragma comment( lib, "vtkRenderingVolume-8.0.lib")
#pragma comment( lib, "vtkRenderingVolumeOpenGL2-8.0.lib")
#pragma comment( lib, "vtksqlite-8.0.lib")
#pragma comment( lib, "vtksys-8.0.lib")
#pragma comment( lib, "vtktiff-8.0.lib")
#pragma comment( lib, "vtkverdict-8.0.lib")
#pragma comment( lib, "vtkViewsContext2D-8.0.lib")
#pragma comment( lib, "vtkViewsCore-8.0.lib")
#pragma comment( lib, "vtkViewsInfovis-8.0.lib")
#pragma comment( lib, "vtkViewsQt-8.0.lib")
#pragma comment( lib, "vtkzlib-8.0.lib")
#pragma comment(lib, "OpenGL32.lib")

#include <vtkSmartPointer.h>

// �̰ɷ� vtk ��ü�� ����� ���� ������ ���� �ʾƵ� �ȴ�.
#ifndef C_VTK
#define C_VTK(type, name) \
	vtkSmartPointer<type> name = vtkSmartPointer<type>::New()
#endif

#ifndef vtkSP
#define vtkSP vtkSmartPointer
#endif

// �������� ���� VTK include
#include <vtkActor.h>
#include <vtkAutoInit.h>
#include <vtkAxesActor.h>
#include <vtkCamera.h>
#include <vtkCellPicker.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkCoordinate.h>
#include <vtkDataArray.h>
#include <vtkDICOMImageReader.h>
#include <vtkErrorCode.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkImageMapToColors.h>
#include <vtkInteractorStyle.h>
#include <vtkInteractorStyleImage.h>
#include <vtkLookupTable.h>
#include <vtkMarchingCubes.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPiecewiseFunction.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkVolumeProperty.h>
