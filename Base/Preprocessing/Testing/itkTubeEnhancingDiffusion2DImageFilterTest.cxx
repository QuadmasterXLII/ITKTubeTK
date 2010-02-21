/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkTopHatImageFilterTest.cxx,v $
  Language:  C++
  Date:      $Date: 2005-09-01 13:41:14 $
  Version:   $Revision: 1.1 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif


#include <itkImage.h>
#include "itkFilterWatcher.h"
#include <itkExceptionObject.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <itkTubeEnhancingDiffusion2DImageFilter.h>

int itkTubeEnhancingDiffusion2DImageFilterTest(int argc, char* argv [] ) 
{
  if( argc < 3 )
    {
    std::cerr << "Missing arguments." << std::endl;
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << "  inputImage outputImage [UseParameterSet2]" << std::endl;
    return EXIT_FAILURE;
    }
  
  // Define the dimension of the images
  const unsigned int Dimension = 2;

  // Define the pixel type
  typedef float PixelType;
  
  // Declare the types of the images
  typedef itk::Image<PixelType, Dimension>  ImageType;

  // Declare the reader and writer
  typedef itk::ImageFileReader< ImageType > ReaderType;
  typedef itk::ImageFileWriter< ImageType > WriterType;
  
 
  // Declare the type for the Filter
  typedef itk::TubeEnhancingDiffusion2DImageFilter<
                           PixelType, Dimension > FilterType;

  // Create the reader and writer
  ReaderType::Pointer reader = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();
  
  reader->SetFileName( argv[1] );
  writer->SetFileName( argv[2] );

  bool useParameterSet2 = false;
  if( argc == 4 )
    {
    useParameterSet2 = true;
    }
  
  FilterType::Pointer filter = FilterType::New();
  FilterWatcher watcher(filter, "filter");

  // Connect the pipeline
  filter->SetInput( reader->GetOutput() );
  filter->SetDefaultPars( ); // duplicates assignments given below
  filter->SetIterations( 50 ); // Default is 200
  filter->SetRecalculateTubeness( 11 ); // Default is 100
  std::vector< float > scales;
  if( useParameterSet2 )
    {
    filter->SetTimeStep( 0.02 ); // Default is 0.25
    scales.resize(3);
    scales[0] = 0.5;
    scales[1] = 1.5;
    scales[2] = 4.0;
    filter->SetScales( scales );
    filter->SetDarkObjectLightBackground( false );
    }
  else
    {
    filter->SetTimeStep( 0.25 ); // Default is 0.25
    scales.resize(3);
    scales[0] = 6;
    scales[1] = 12;
    scales[2] = 24;
    filter->SetScales( scales );
    filter->SetDarkObjectLightBackground( true );
    }
  filter->SetBeta( 0.5 );
  filter->SetGamma( 5.0 );
  filter->SetEpsilon( 0.01 );
  filter->SetOmega( 25.0 );
  filter->SetSensitivity( 20.0 );
  filter->SetVerbose( true );

  writer->SetInput( filter->GetOutput() );
  
  // Execute the filter
  try
    {
    writer->Update();
    }
  catch (itk::ExceptionObject& e)
    {
    std::cerr << "Exception caught during pipeline Update\n"  << e;
    return EXIT_FAILURE;
    }

  // All objects should be automatically destroyed at this point

  return EXIT_SUCCESS;

}




