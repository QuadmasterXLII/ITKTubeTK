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

#ifndef __itktubeNJetFeatureVectorGenerator_h
#define __itktubeNJetFeatureVectorGenerator_h

#include "itktubeFeatureVectorGenerator.h"

#include <itkImage.h>

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>

#include <vector>

namespace itk
{

namespace tube
{

template< class TImage >
class NJetFeatureVectorGenerator
  : public FeatureVectorGenerator< Image< typename TImage::PixelType,
                                          TImage::ImageDimension > >
{
public:

  using Self = NJetFeatureVectorGenerator;
  using Superclass = FeatureVectorGenerator< TImage >;
  using Pointer = SmartPointer< Self >;
  using ConstPointer = SmartPointer< const Self >;

  itkTypeMacro( NJetFeatureVectorGenerator, FeatureVectorGenerator );

  itkNewMacro( Self );

  static constexpr unsigned int ImageDimension = TImage::ImageDimension;

  using FeatureValueType = typename Superclass::FeatureValueType;

  using FeatureVectorType = typename Superclass::FeatureVectorType;

  using ImageType = typename Superclass::ImageType;

  using IndexType = typename Superclass::IndexType;

  using NJetScalesType = std::vector< double >;

  virtual unsigned int GetNumberOfFeatures( void ) const;

  void SetZeroScales( const NJetScalesType & scales );
  void SetFirstScales( const NJetScalesType & scales );
  void SetSecondScales( const NJetScalesType & scales );
  void SetRidgeScales( const NJetScalesType & scales );

  const NJetScalesType & GetZeroScales( void ) const;
  const NJetScalesType & GetFirstScales( void ) const;
  const NJetScalesType & GetSecondScales( void ) const;
  const NJetScalesType & GetRidgeScales( void ) const;

  virtual FeatureVectorType GetFeatureVector(
    const IndexType & indx ) const;

  virtual FeatureValueType  GetFeatureVectorValue( const IndexType & indx,
    unsigned int fNum ) const;

protected:

  NJetFeatureVectorGenerator( void );
  virtual ~NJetFeatureVectorGenerator( void );

  void PrintSelf( std::ostream & os, Indent indent ) const;

private:

  // Purposely not implemented
  NJetFeatureVectorGenerator( const Self & );
  void operator = ( const Self & );

  NJetScalesType m_ZeroScales;
  NJetScalesType m_FirstScales;
  NJetScalesType m_SecondScales;
  NJetScalesType m_RidgeScales;

}; // End class NJetFeatureVectorGenerator

}  // End namespace tube

}  // End namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itktubeNJetFeatureVectorGenerator.hxx"
#endif

#endif // End !defined( __itktubeNJetFeatureVectorGenerator_h )
