/*=========================================================================

  Library:   CTK

=========================================================================*/

// Qt includes
#include <QApplication>
#include <QCheckBox>
#include <QDebug>
#include <QTimer>
#include <QVBoxLayout>
#include <QHBoxLayout>


// CTK includes
#include "ctkCommandLineParser.h"
#include "ctkVTKObjectEventsObserver.h"
#include "ctkVTKRenderView.h"
#include "ctkVTKSliceView.h"

// VTK includes
#include <vtkCamera.h>
#include <vtkGlyphSource2D.h>
#include <vtkHandleWidget.h>
#include <vtkImageReader2Factory.h>
#include <vtkImageReader2.h>
#include <vtkImageData.h>
#include <vtkInteractorStyleImage.h>
#include <vtkNew.h>
#include <vtkOrientedPolygonalHandleRepresentation3D.h>
#include <vtkProperty.h>
#include <vtkRendererCollection.h>
#include <vtkRenderWindow.h>
#include <vtkSeedWidget.h>
#include <vtkSeedRepresentation.h>
#include <vtkSmartPointer.h>
#include <vtkRenderWindowInteractor.h>


// STD includes
#include <iostream>

// call back for the seed widgets
class ctkVTKSliceAndRenderViewWithSeedWidgetTestCallback : public vtkCommand
{
public:
  static ctkVTKSliceAndRenderViewWithSeedWidgetTestCallback *New()
  { return new ctkVTKSliceAndRenderViewWithSeedWidgetTestCallback; }

  ctkVTKSliceAndRenderViewWithSeedWidgetTestCallback() {}

  virtual void Execute (vtkObject *vtkNotUsed(caller), unsigned long event, void *callData)
  {
    if (!this->Widget)
      {
      return;
      }
    vtkSeedWidget *widget = vtkSeedWidget::SafeDownCast(this->Widget);
    if (!widget)
      {
      return;
      }

    vtkSeedWidget *widget2 = vtkSeedWidget::SafeDownCast(this->SecondWidget);
    if (!widget2)
      {
      return;
      }

    //unsigned int *seedNumber =  reinterpret_cast<unsigned int *>(callData);
    // we need the widgetRepresentation
    vtkSeedRepresentation * representation = vtkSeedRepresentation::SafeDownCast(this->Widget->GetRepresentation());
    if (!representation)
      {
      std::cerr << "Representation is null.\n";
      return;
      }
    double displayCoordinates1[4];
    double worldCoordinates1[3];
    unsigned int *seedIndex =  NULL;
    if (callData != NULL)
      {
      // have a single seed that moved
      seedIndex =  reinterpret_cast<unsigned int *>(callData);
      if (seedIndex ||
          (seedIndex == 0 && representation->GetNumberOfSeeds() == 1))
        {
        // first, we get the current displayCoordinates of the points
        representation->GetSeedDisplayPosition(*seedIndex, displayCoordinates1);
        representation->GetSeedWorldPosition(*seedIndex, worldCoordinates1);
        std::cout << "seedIndex = " << *seedIndex << ", disp coords = "
                  << displayCoordinates1[0] << ", "
                  << displayCoordinates1[1] << ", "
                  << displayCoordinates1[2] << "\n\t"
                  << "world coordinates = "
                  << worldCoordinates1[0] << ", "
                  << worldCoordinates1[1] << ", "
                  << worldCoordinates1[2]
                  << std::endl;
        }
      }

    if (event ==  vtkCommand::PlacePointEvent)
      {
      std::cout << "PlacePointEvent" << std::endl;
      if (widget2)
        {
        // add a point at this coordinate
        vtkHandleWidget* newhandle = widget2->CreateNewHandle();
        //newhandle->ManagesCursorOff();
        newhandle->EnabledOn();
        vtkSeedRepresentation * representation2 = vtkSeedRepresentation::SafeDownCast(this->SecondWidget->GetRepresentation());
        if (representation2)
          {
          int numberOfHandles = representation2->GetNumberOfSeeds();
          std::cout << "numberOfHandles = " << numberOfHandles << std::endl;
          representation2->SetSeedDisplayPosition(numberOfHandles-1, displayCoordinates1);
          representation2->NeedToRenderOn();
          this->SecondWidget->Modified();
          }
        }
      }
    else if (event == vtkCommand::EndInteractionEvent)
      {
      std::cout << "EndInteractionEvent" << std::endl;
      }
    else if (event == vtkCommand::InteractionEvent)
      {
      std::cout << "InteractionEvent" << std::endl;
      vtkSeedRepresentation * representation2 = vtkSeedRepresentation::SafeDownCast(this->SecondWidget->GetRepresentation());
      if (representation2)
        {
        std::cout << "Rep2 num seeds = "
                  << representation2->GetNumberOfSeeds()
                  << std::endl;
        if (seedIndex ||
            (seedIndex == 0 && representation2->GetNumberOfSeeds() == 1))
          {
          std::cout << "\n\tupdating second widget at index " << *seedIndex << std::endl;
          representation2->SetSeedDisplayPosition(*seedIndex, displayCoordinates1);
          vtkCamera *cam = NULL;
          vtkRendererCollection *rendererCollection = this->Widget->GetInteractor()->GetRenderWindow()->GetRenderers();
          for (unsigned int i = 0; i < rendererCollection->GetNumberOfItems(); ++i)
            {
            vtkRenderer *renderer = vtkRenderer::SafeDownCast(rendererCollection->GetItemAsObject(i));
            cam = renderer->GetActiveCamera();
            double cameraPos[3];
            cam->GetPosition(cameraPos);
            std::cout << "\n\tRenderer " << i  << " widget 2D camera position: " << cameraPos[0] << ", " << cameraPos[1] << ", " << cameraPos[2] << std::endl;
            vtkIndent indent;
            //cam->PrintSelf(std::cout, indent);
            }
          /* print the 2d seed
          std::cout << "\n\tHandle actor = " << std::endl;
          vtkPropCollection * propCol = vtkPropCollection::New();
          representation->GetHandleRepresentation(*seedIndex)->GetActors(propCol);
          vtkActor *actor = vtkActor::SafeDownCast(propCol->GetItemAsObject(0));
          if (!actor->IsA("vtkFollower"))
            {
            actor->PrintSelf(std::cout, indent);
            }
          else
            {
            actor = vtkActor::SafeDownCast(propCol->GetItemAsObject(1));
            actor->PrintSelf(std::cout, indent);
            }
          */
          // 3d camera
          vtkCamera *cam3d = NULL;
          vtkRenderer *renderer3d = vtkRenderer::SafeDownCast(this->SecondWidget->GetInteractor()->GetRenderWindow()->GetRenderers()->GetItemAsObject(0));
          cam3d = renderer3d->GetActiveCamera();
          double cameraPos3d[3];
          cam3d->GetPosition(cameraPos3d);
          std::cout << "\twidget 3D camera position: " << cameraPos3d[0] << ", " << cameraPos3d[1] << ", " << cameraPos3d[2] << std::endl;
          if (this->RenderView)
            {
            this->RenderView->scheduleRender();
            }
          }
        }
      }
  }
  void SetWidget(vtkAbstractWidget *w)
    {
    this->Widget = w;
    }
  void SetSecondWidget(vtkAbstractWidget *w)
    {
    this->SecondWidget = w;
    }
  void SetRenderView(ctkVTKRenderView *v)
    {
    this->RenderView = v;
    }

  vtkAbstractWidget * Widget;
  vtkAbstractWidget * SecondWidget;
  ctkVTKRenderView * RenderView;
};

//
// test seed widget in mulitple slice views, with and without picking manager
//-----------------------------------------------------------------------------
int ctkVTKSliceAndRenderViewWithSeedWidgetTest(int argc, char * argv [] )
{
  QApplication app(argc, argv);

  // Test arguments
  QString filename = "HeadMRVolume.mhd";

  // Command line parser
  ctkCommandLineParser parser;
  parser.addArgument("", "-I", QVariant::Bool);
  parser.addArgument("", "-D", QVariant::String);
  bool ok = false;
  QHash<QString, QVariant> parsedArgs = parser.parseArguments(app.arguments(), &ok);
  if (!ok)
    {
    std::cerr << qPrintable(parser.errorString()) << std::endl;
    return EXIT_FAILURE;
    }

  bool interactive = parsedArgs["-I"].toBool();
  QString data_directory = parsedArgs["-D"].toString();

  QString imageFilename = data_directory + "/" + filename;

  // Instanciate the reader factory
  vtkSmartPointer<vtkImageReader2Factory> imageFactory =
      vtkSmartPointer<vtkImageReader2Factory>::New();

  // Instanciate an image reader
  vtkSmartPointer<vtkImageReader2> imageReader;
  imageReader.TakeReference(imageFactory->CreateImageReader2(imageFilename.toLatin1()));
  if (!imageReader)
    {
    std::cerr << "Failed to instanciate image reader using: "
              << qPrintable(imageFilename) << std::endl;
    return EXIT_FAILURE;
    }

  // Read image
  imageReader->SetFileName(imageFilename.toLatin1());
#if (VTK_MAJOR_VERSION <= 5)
  imageReader->Update();
  vtkImageData* image = imageReader->GetOutput();
#else
  imageReader->Update(); // XXX This shouldn't be needed. See issue #467
  vtkAlgorithmOutput* imagePort = imageReader->GetOutputPort();
#endif

  // Top level widget
  QWidget widget;

  // .. and its associated layout
  QVBoxLayout * topLevelLayout = new QVBoxLayout(&widget);
  topLevelLayout->setContentsMargins(0, 0, 0, 0);

  ctkVTKSliceView * sliceView1 = new ctkVTKSliceView;
  sliceView1->setRenderEnabled(true);
  sliceView1->setMinimumSize(300, 300);
#if (VTK_MAJOR_VERSION <= 5)
  sliceView1->setImageData(image);
#else
  sliceView1->setImageDataConnection(imagePort);
#endif
  sliceView1->scheduleRender();
  topLevelLayout->addWidget(sliceView1);

  //vtkNew<vtkThreeDViewInteractorStyle> threeDInteractorStyle;

  ctkVTKRenderView * renderView = new ctkVTKRenderView;
  renderView->setRenderEnabled(true);
  renderView->setMinimumSize(300, 300);
  renderView->scheduleRender();
  topLevelLayout->addWidget(renderView);

  widget.show();

  // add a seed widget
  vtkNew<vtkOrientedPolygonalHandleRepresentation3D> handle;
  handle->GetProperty()->SetColor(1,0,0);
  handle->SetHandleSize(1);

  vtkNew<vtkGlyphSource2D> glyphSource;
  glyphSource->SetGlyphTypeToCircle();
  glyphSource->Update();
  glyphSource->SetScale(1.0);
  handle->SetHandle(glyphSource->GetOutput());
  //handle->SetUniformScale(0.001);

  vtkNew<vtkSeedRepresentation> rep1;
  rep1->SetHandleRepresentation(handle.GetPointer());

  vtkNew<vtkSeedWidget> seedWidget1;
  seedWidget1->CreateDefaultRepresentation();
  seedWidget1->SetRepresentation(rep1.GetPointer());

  // add the callback
  ctkVTKSliceAndRenderViewWithSeedWidgetTestCallback *myCallback = ctkVTKSliceAndRenderViewWithSeedWidgetTestCallback::New();
  myCallback->SetWidget(vtkAbstractWidget::SafeDownCast(seedWidget1.GetPointer()));
  seedWidget1->AddObserver(vtkCommand::PlacePointEvent,myCallback);
  seedWidget1->AddObserver(vtkCommand::EndInteractionEvent,myCallback);
  seedWidget1->AddObserver(vtkCommand::InteractionEvent,myCallback);

  // add a second widget to mirror the first
  vtkNew<vtkOrientedPolygonalHandleRepresentation3D> handle2;
  handle2->GetProperty()->SetColor(0,1,0);
  handle2->SetHandle(glyphSource->GetOutput());
  handle2->SetHandleSize(1);
  // handle2->SetUniformScale(0.001);

  vtkNew<vtkSeedRepresentation> rep2;
  rep2->SetHandleRepresentation(handle2.GetPointer());

  vtkNew<vtkSeedWidget> seedWidget2;
  seedWidget2->CreateDefaultRepresentation();
  seedWidget2->SetRepresentation(rep2.GetPointer());

  // set the second widget to have it map the first one
  myCallback->SetSecondWidget(vtkAbstractWidget::SafeDownCast(seedWidget2.GetPointer()));
  // need to set the second slice view to schedule renders
  myCallback->SetRenderView(renderView);

  myCallback->Delete();

  seedWidget1->SetInteractor(sliceView1->interactor());
  // Slicer adds seeds programatically
  //seedWidget1->CompleteInteraction();


  // second widget, put it in completed interaction state
  seedWidget2->SetInteractor(renderView->interactor());
  // seedWidget2->CompleteInteraction();


  // print out cameras to compare
  // three d
  vtkRendererCollection *rendererCollection = renderView->renderWindow()->GetRenderers();
  std::cout << "Number of 3d renderers = " << rendererCollection->GetNumberOfItems() << std::endl;
  vtkRenderer *renderer = vtkRenderer::SafeDownCast(rendererCollection->GetItemAsObject(0));
  vtkCamera *threeDCam = renderer->GetActiveCamera();
  //std::cout << "Three Dimensional Camera:\n";
  vtkIndent indent;
  //threeDCam->PrintSelf(std::cout, indent);
  //std::cout << "Resetting 3d camera position to 100\n";
  threeDCam->SetPosition(0,0,100);

  // two d
  vtkRendererCollection *rendererCollection2d = sliceView1->renderWindow()->GetRenderers();
  std::cout << "Number of 2d renderers = " << rendererCollection2d->GetNumberOfItems() << std::endl;
  for (int i = 0; i < rendererCollection2d->GetNumberOfItems(); ++i)
    {
    vtkRenderer *renderer2d =  vtkRenderer::SafeDownCast(rendererCollection2d->GetItemAsObject(i));
    vtkCamera *twoDCam = renderer2d->GetActiveCamera();
    std::cout << "Two Dimensional Camera i: " << i << "\n";
    twoDCam->SetPosition(0,0,100);
    twoDCam->PrintSelf(std::cout, indent);
    }
  if (!interactive)
    {
    QTimer::singleShot(1000, &app, SLOT(quit()));
    }
  return app.exec();
}
