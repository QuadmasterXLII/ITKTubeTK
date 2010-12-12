/*=========================================================================

Library:   TubeTK

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 ( the "License" );
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkSpatialObjectReader.h"
#include "itkGroupSpatialObject.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkMersenneTwisterRandomVariateGenerator.h"

#include "tubeMessage.h"
#include "tubeMatrixMath.h"
#include "itkTubeRadiusExtractor.h"

int itkTubeRadiusExtractorTest2( int argc, char * argv[] )
  {
  if( argc != 3 )
    {
    std::cout
      << "itkTubeRadiusExtractorTest2 <inputImage> <vessel.tre>"
      << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image<float, 3>   ImageType;

  typedef itk::ImageFileReader< ImageType > ImageReaderType;
  ImageReaderType::Pointer imReader = ImageReaderType::New();
  imReader->SetFileName( argv[1] );
  imReader->Update();

  ImageType::Pointer im = imReader->GetOutput();

  typedef itk::tube::RadiusExtractor<ImageType> RadiusOpType;
  RadiusOpType::Pointer radiusOp = RadiusOpType::New();

  radiusOp->SetInputImage( im );

  bool returnStatus = EXIT_SUCCESS;

  radiusOp->SetThreshMedialness( 0.005 );
  if( radiusOp->GetThreshMedialness() != 0.005 )
    {
    tube::ErrorMessage( "ThreshMedialness != 0.005" );
    returnStatus = EXIT_FAILURE;
    }

  radiusOp->SetThreshMedialnessStart( 0.002 );
  if( radiusOp->GetThreshMedialnessStart() != 0.002 )
    {
    tube::ErrorMessage( "ThreshMedialnessStart != 0.002" );
    returnStatus = EXIT_FAILURE;
    }

  radiusOp->SetExtractRidge( true );
  if( radiusOp->GetExtractRidge() != true )
    {
    tube::ErrorMessage( "ExtractRidge != true" );
    returnStatus = EXIT_FAILURE;
    }

  typedef itk::SpatialObjectReader<>                   ReaderType;
  typedef itk::SpatialObject<>::ChildrenListType       ObjectListType;
  typedef itk::GroupSpatialObject<>                    GroupType;
  typedef itk::VesselTubeSpatialObject<>               TubeType;
  typedef TubeType::PointListType                      PointListType;
  typedef TubeType::PointType                          PointType;
  typedef TubeType::TubePointType                      TubePointType;

  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[2] );
  reader->Update();
  GroupType::Pointer group = reader->GetGroup();

  std::cout << "Number of children = " << group->GetNumberOfChildren()
    << std::endl;

  char tubeName[17];
  strcpy( tubeName, "Tube" );
  ObjectListType * tubeList = group->GetChildren( -1, tubeName );

  unsigned int numTubes = tubeList->size();
  std::cout << "Number of tubes = " << numTubes << std::endl;

  typedef itk::Statistics::MersenneTwisterRandomVariateGenerator
    RandGenType;
  RandGenType::Pointer rndGen = RandGenType::New();
  rndGen->Initialize(); // set seed here

  double avgFailures = 0;
  double avgAvgDiff = 0;
  double avgMaxDiff = 0;
  unsigned int numMCRuns = 20;
  for( unsigned int mcRun=0; mcRun<numMCRuns; mcRun++ )
    {
    std::cout << std::endl;
    std::cout << "*** RUN = " << mcRun << std::endl;
    unsigned int rndTubeNum = rndGen->GetUniformVariate( 0, 1 ) * numTubes;
    if( rndTubeNum >= numTubes )
      {
      rndTubeNum = numTubes-1;
      }
    ObjectListType::iterator tubeIter = tubeList->begin();
    for( unsigned int i=0; i<rndTubeNum; i++ )
      {
      ++tubeIter;
      }

    std::cout << "Test tube = " << rndTubeNum << std::endl;

    TubeType * tubep = static_cast< TubeType * >(
      tubeIter->GetPointer() );
    TubeType::Pointer tube = static_cast< TubeType * >(
      tubeIter->GetPointer() );

    tube::ComputeTubeTangentsAndNormals< TubeType >( tubep );

    unsigned int numPoints = tube->GetPoints().size();

    unsigned int startPoint = rndGen->GetUniformVariate( 0, 1 )
      * (numPoints-10) + 5;
    double radius0 = rndGen->GetUniformVariate( 0, 1 ) * 2 + 1;

    double failures = 0;
    std::vector< double > idealR;
    idealR.clear();
    PointListType::iterator pntIter = tube->GetPoints().begin();
    for( unsigned int i=0; i<numPoints; i++ )
      {
      if( i == startPoint )
        {
        pntIter->SetID( 0 );
        radius0 = (radius0 + pntIter->GetRadius() ) / 2;
        }
      else
        {
        pntIter->SetID( i+1 );
        }

      if( vnl_math_abs( pntIter->GetTangent().GetVnlVector().magnitude()-1 )
        > 0.01 )
        {
        std::cout << "Point: " << i << ": Tangent not of unit length."
          << std::endl;
        ++failures;
        }
      if( vnl_math_abs( pntIter->GetNormal1().GetVnlVector().magnitude()-1 )
        > 0.01 )
        {
        std::cout << "Point: " << i << ": Normal1 not of unit length."
          << std::endl;
        ++failures;
        }
      if( vnl_math_abs( pntIter->GetNormal2().GetVnlVector().magnitude()-1 )
        > 0.01 )
        {
        std::cout << "Point: " << i << ": Normal2 not of unit length."
          << std::endl;
        ++failures;
        }
      if( vnl_math_abs( dot_product( pntIter->GetTangent().GetVnlVector(),
        pntIter->GetNormal1().GetVnlVector() ) ) > 0.001 )
        {
        std::cout << "Point: " << i
          << ": dot_product( Tangent, Normal1 ) != 0." << std::endl;
        ++failures;
        }
      if( vnl_math_abs( dot_product( pntIter->GetTangent().GetVnlVector(),
        pntIter->GetNormal2().GetVnlVector() ) ) > 0.001 )
        {
        std::cout << "Point: " << i
          << ": dot_product( Tangent, Normal2 ) != 0." << std::endl;
        ++failures;
        }
      if( vnl_math_abs( dot_product( pntIter->GetNormal1().GetVnlVector(),
        pntIter->GetNormal2().GetVnlVector() ) ) > 0.001 )
        {
        std::cout << "Point: " << i
          << ": dot_product( Normal1, Normal2 ) != 0." << std::endl;
        ++failures;
        }

      idealR.push_back( pntIter->GetRadius() );
      if( pntIter->GetRadius() < 0 )
        {
        std::cout << "Point: " << i << ": radius < 0. ("
          << pntIter->GetRadius() << ")" << std::endl;
        ++failures;
        }

      ++pntIter;
      }

    radiusOp->SetRadius0( radius0 );

    //radiusOp->SetDebug( true );
    radiusOp->ComputeTubeRadii( *tubep );

    double diff;
    double avgDiff = 0;
    double maxDiff = 0;
    pntIter = tube->GetPoints().begin();
    for( unsigned int i=0; i<numPoints; i++ )
      {
      diff = vnl_math_abs( pntIter->GetRadius() - idealR[i] );
      avgDiff += diff;
      if( diff > maxDiff )
        {
        maxDiff = diff;
        }
      if( diff > 1 )
        {
        std::cout << "Point: " << i
          << ": estimatedR - idealR != 0." << std::endl;
        std::cout << "  idealR = " << idealR[i] << std::endl;
        std::cout << "  estimatedR = " << pntIter->GetRadius() << std::endl;
        ++failures;
        }
      else
        {
        std::cout << "Point: " << i << "  idealR = " << idealR[i]
          << "  estimatedR = " << pntIter->GetRadius() << std::endl;
        }
      // reset radius for re-testing
      pntIter->SetRadius( idealR[i] );
      ++pntIter;
      }
    avgDiff /= numPoints;
    failures = failures / numPoints;
    std::cout << "Failures = " << failures << std::endl;
    std::cout << "Averge diff = " << avgDiff << std::endl;
    std::cout << "Max diff = " << maxDiff << std::endl;
    avgAvgDiff += avgDiff;
    avgMaxDiff += maxDiff;
    avgFailures += failures;
    }

  avgFailures /= numMCRuns;
  avgAvgDiff /= numMCRuns;
  avgMaxDiff /= numMCRuns;
  std::cout << "Average Failures = " << avgFailures << std::endl;
  std::cout << "Average Averge diff = " << avgAvgDiff << std::endl;
  std::cout << "Average Max diff = " << avgMaxDiff << std::endl;

  if( avgFailures > 0.15 )
    {
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
  }