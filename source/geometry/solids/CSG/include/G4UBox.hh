//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
// 
// G4UBox
//
// Class description:
//
// Wrapper class for G4Box to make use of VecGeom Box.

// 13.09.13 G.Cosmo, CERN
// --------------------------------------------------------------------
#ifndef G4UBOX_HH
#define G4UBOX_HH

#include "G4UAdapter.hh"

#if ( defined(G4GEOM_USE_USOLIDS) || defined(G4GEOM_USE_PARTIAL_USOLIDS) )

#include <VecGeom/volumes/UnplacedBox.h>

#include "G4Polyhedron.hh"

class G4UBox : public G4UAdapter<vecgeom::UnplacedBox>
{
  using Shape_t = vecgeom::UnplacedBox;
  using Base_t = G4UAdapter<vecgeom::UnplacedBox>;

  public:

    G4UBox(const G4String& pName, G4double pX, G4double pY, G4double pZ);
      // Constructs a box with name, and half lengths pX,pY,pZ

   ~G4UBox() override;

    void ComputeDimensions(      G4VPVParameterisation* p,
                           const G4int n,
                           const G4VPhysicalVolume* pRep) override;

    G4VSolid* Clone() const override;

    G4double GetXHalfLength() const;
    G4double GetYHalfLength() const;
    G4double GetZHalfLength() const;

    void SetXHalfLength(G4double dx);
    void SetYHalfLength(G4double dy);
    void SetZHalfLength(G4double dz);

    inline G4GeometryType GetEntityType() const override;

    inline G4bool IsFaceted() const override;

    void BoundingLimits(G4ThreeVector& pMin, G4ThreeVector& pMax) const override;

    G4bool CalculateExtent(const EAxis pAxis,
                           const G4VoxelLimits& pVoxelLimit,
                           const G4AffineTransform& pTransform,
                           G4double& pMin, G4double& pMax) const override;

    G4Polyhedron* CreatePolyhedron() const override;

    G4UBox(const G4UBox& rhs);
    G4UBox& operator=(const G4UBox& rhs); 
      // Copy constructor and assignment operator.
};

// --------------------------------------------------------------------
// Inline methods
// --------------------------------------------------------------------

inline G4GeometryType G4UBox::GetEntityType() const
{
  return "G4Box";
}

inline G4bool G4UBox::IsFaceted() const
{
  return true;
}

#endif  // G4GEOM_USE_USOLIDS

#endif
