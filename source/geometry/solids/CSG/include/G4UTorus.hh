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
// G4UTorus
//
// Class description:
//
// Wrapper class for G4Torus to make use of VecGeom Torus.

// 19.08.15 Guilherme Lima, FNAL
// --------------------------------------------------------------------
#ifndef G4UTORUS_HH
#define G4UTORUS_HH

#include "G4UAdapter.hh"

#if ( defined(G4GEOM_USE_USOLIDS) || defined(G4GEOM_USE_PARTIAL_USOLIDS) )

#include <VecGeom/volumes/UnplacedTorus2.h>

#include "G4Polyhedron.hh"

class G4UTorus : public G4UAdapter<vecgeom::UnplacedTorus2>
{
  using Shape_t = vecgeom::UnplacedTorus2;
  using Base_t  = G4UAdapter<vecgeom::UnplacedTorus2>;

  public:

    G4UTorus(const G4String& pName,
                   G4double rmin, G4double rmax, G4double rtor,
                   G4double sphi, G4double dphi);
      // Constructs a torus with name and geometrical parameters

   ~G4UTorus() override;

    void ComputeDimensions(G4VPVParameterisation* p,
                           const G4int n,
                           const G4VPhysicalVolume* pRep) override;

    G4VSolid* Clone() const override;

    G4double GetRmin() const;
    G4double GetRmax() const;
    G4double GetRtor() const;
    G4double GetSPhi() const;
    G4double GetDPhi() const;
    G4double GetSinStartPhi() const;
    G4double GetCosStartPhi() const;
    G4double GetSinEndPhi  () const;
    G4double GetCosEndPhi  () const;

    void SetRmin(G4double arg);
    void SetRmax(G4double arg);
    void SetRtor(G4double arg);
    void SetSPhi(G4double arg);
    void SetDPhi(G4double arg);

    void SetAllParameters(G4double arg1, G4double arg2,
                          G4double arg3, G4double arg4, G4double arg5);

    inline G4GeometryType GetEntityType() const override;

    void BoundingLimits(G4ThreeVector& pMin, G4ThreeVector& pMax) const override;

    G4bool CalculateExtent(const EAxis pAxis,
                           const G4VoxelLimits& pVoxelLimit,
                           const G4AffineTransform& pTransform,
                                 G4double& pmin, G4double& pmax) const override;

    G4Polyhedron* CreatePolyhedron() const override;

    G4UTorus(const G4UTorus& rhs);
    G4UTorus& operator=(const G4UTorus& rhs);
      // Copy constructor and assignment operator.
};

// --------------------------------------------------------------------
// Inline methods
// --------------------------------------------------------------------

inline G4GeometryType G4UTorus::GetEntityType() const
{
  return "G4Torus";
}

#endif  // G4GEOM_USE_USOLIDS

#endif
