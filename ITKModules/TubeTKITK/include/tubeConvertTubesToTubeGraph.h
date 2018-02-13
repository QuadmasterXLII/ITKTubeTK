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
#ifndef __tubeConvertTubesToTubeGraph_h
#define __tubeConvertTubesToTubeGraph_h

// ITK includes
#include "itkMacro.h"

#include "itkProcessObject.h"

// TubeTK includes
#include "tubeWrappingMacros.h"

#include "itktubeTubeSpatialObjectToTubeGraphFilter.h"

namespace tube
{
/** \class ConvertTubesToTubeGraph
 *
 *  \ingroup TubeTKITK
 */

template< class TPixel, unsigned int Dimension >
class ConvertTubesToTubeGraph:
  public itk::ProcessObject
{
public:
  /** Standard class type alias. */
  using Self = ConvertTubesToTubeGraph;
  using Superclass = itk::ProcessObject;
  using Pointer = itk::SmartPointer< Self >;
  using ConstPointer = itk::SmartPointer< const Self >;

  using FilterType = itk::tube::TubeSpatialObjectToTubeGraphFilter
    < TPixel, Dimension >;

  using InputImageType = typename FilterType::InputImageType;
  using TubeGroupType = typename FilterType::TubeGroupType;

  /** Method for creation through the object factory. */
  itkNewMacro( Self );

  /** Run-time type information ( and related methods ). */
  itkTypeMacro( ConvertTubesToTubeGraph, ProcessObject );

  /** Set Number of Centroids */
  tubeWrapSetMacro( NumberOfCenteroids, int, Filter );
  tubeWrapGetMacro( NumberOfCenteroids, int, Filter );

  /* Set Central Voronoi Tesselation image */
  tubeWrapSetObjectMacro( CVTImage, InputImageType, Filter );
  tubeWrapGetObjectMacro( CVTImage, InputImageType, Filter );

  /* Set input tubes */
  tubeWrapSetObjectMacro( InputTubeGroup, TubeGroupType, Filter );
  tubeWrapGetObjectMacro( InputTubeGroup, TubeGroupType, Filter );

  /* Runs tubes to image conversion */
  tubeWrapUpdateMacro( Filter );

  /** Get Adjacency Matrix */
  vnl_matrix< double > GetAdjacencyMatrix( void );

  /** Get Root Nodes Vector */
  vnl_vector< int > GetRootNodes( void );

  /** Get Branch Nodes Vector */
  vnl_vector< double > GetBranchNodes( void );

protected:
  ConvertTubesToTubeGraph( void );
  ~ConvertTubesToTubeGraph() {}
  void PrintSelf( std::ostream & os, itk::Indent indent ) const;

private:
  /** itktubeTubeSpatialObjectToTubeGraphFilter parameters **/
  ConvertTubesToTubeGraph( const Self & );
  void operator=( const Self & );

  // To remove warning "was hidden [-Woverloaded-virtual]"
  void SetInput( const DataObjectIdentifierType &, itk::DataObject * ) {};

  typename FilterType::Pointer m_Filter;
};

} // End namespace tube

#ifndef ITK_MANUAL_INSTANTIATION
#include "tubeConvertTubesToTubeGraph.hxx"
#endif

#endif // End !defined( __tubeConvertTubesToTubeGraph_h )
