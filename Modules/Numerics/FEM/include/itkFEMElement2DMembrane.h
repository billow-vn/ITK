/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#ifndef itkFEMElement2DMembrane_h
#define itkFEMElement2DMembrane_h

#include "itkFEMElementBase.h"
#include "itkFEMMaterialLinearElasticity.h"

namespace itk
{
namespace fem
{
/**
 * \class Element2DMembrane
 * \brief Class that is used to define a membrane energy problem in 2D space.
 *
 * This class only defines the physics of the problem. Use his class together
 * with element classes that specify the geometry to fully define the element.
 *
 * A membrane element a two dimensional flat extensional element.
 * There are two in plane displacement DOF's at each node of the element.
 * The elements can be used to model two dimensional elasticity problems, plane
 * strain and plane stress. The membrane element has no rotational stiffness or
 * stiffness normal to the plane of the element. It can be situated arbitrarily
 * in space but the resultant forces must lie in the plane of the element.
 *
 *
 * You can specify one template parameter:
 *
 *   TBaseClass - Class from which Element2DMembrane is derived. TBaseClass must
 *                be derived from the Element base class. This enables you
 *                to use this class at any level of element definition.
 *                If not specified, it defaults to the Element base class.
 * \ingroup ITKFEM
 */
template <typename TBaseClass = Element>
class ITK_TEMPLATE_EXPORT Element2DMembrane : public TBaseClass
{
public:
  /** Standard class type aliases. */
  using Self = Element2DMembrane;
  using Superclass = TBaseClass;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Run-time type information (and related methods). */
  itkOverrideGetNameOfClassMacro(Element2DMembrane);

  // Repeat the required type alias and enums from parent class
  using typename Superclass::Float;
  using typename Superclass::MatrixType;
  using typename Superclass::VectorType;

  /**
   * Default constructor only clears the internal storage
   */
  Element2DMembrane();

  // ////////////////////////////////////////////////////////////////////////
  /**
   * Methods related to the physics of the problem.
   */

  /**
   * Compute the B matrix.
   */
  void
  GetStrainDisplacementMatrix(MatrixType & B, const MatrixType & shapeDgl) const override;

  /**
   * Compute the D matrix.
   */
  void
  GetMaterialMatrix(MatrixType & D) const override;

  /**
   * Compute the mass matrix specific for 2D stress problems.
   */
  void
  GetMassMatrix(MatrixType & Me) const override;

  /**
   * 2D stress elements have 2 DOFs per node.
   */
  unsigned int
  GetNumberOfDegreesOfFreedomPerNode() const override
  {
    return 2;
  }

  /**
   * Get/Set the material properties for the element
   */
  Material::ConstPointer
  GetMaterial() const override
  {
    return m_mat;
  }

  void
  SetMaterial(Material::ConstPointer mat_) override
  {
    m_mat = dynamic_cast<const MaterialLinearElasticity *>(mat_.GetPointer());
  }

protected:
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /**
   * Pointer to material properties for the element
   */
  const MaterialLinearElasticity * m_mat{ nullptr };

}; // class Element2DMembrane
} // end namespace fem
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkFEMElement2DMembrane.hxx"
#endif

#endif // itkFEMElement2DMembrane_h
