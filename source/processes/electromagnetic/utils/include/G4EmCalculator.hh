//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
// $Id: G4EmCalculator.hh,v 1.7 2004-11-16 12:05:51 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// -------------------------------------------------------------------
//
// GEANT4 Class header file
//
//
// File name:     G4EmCalculator
//
// Author:        Vladimir Ivanchenko
//
// Creation date: 27.06.2004
//
// Modifications:
//
//
// Class Description:
//
// Provide access to dE/dx and cross sections

// -------------------------------------------------------------------
//

#ifndef G4EmCalculator_h
#define G4EmCalculator_h 1

#include <vector>
#include "globals.hh"
#include "G4DataVector.hh"

class G4LossTableManager;
class G4Material;
class G4MaterialCutsCouple;
class G4ParticleDefinition;
class G4PhysicsTable;
class G4VEmModel;
class G4VEnergyLossProcess;
class G4ionEffectiveCharge;
class G4Region;

class G4EmCalculator
{

public:

  G4EmCalculator();

  ~G4EmCalculator();

  // Methods to access precalculated dE/dx and cross sections
  // Materials should exist in the list of the G4MaterialCutsCouple

  G4double GetDEDX(const G4ParticleDefinition*, const G4Material*, G4double kinEnergy,
                   const G4Region* r = 0);
  G4double GetDEDX(const G4String&, const G4String&, G4double kinEnergy,
                   const G4String& s = "world");

  G4double GetRange(const G4ParticleDefinition*, const G4Material*, G4double kinEnergy,
                   const G4Region* r = 0);
  G4double GetRange(const G4String&, const G4String&, G4double kinEnergy,
                  const G4String& s = "world");

  G4double GetKinEnergy(const G4ParticleDefinition*, const G4Material*, G4double range,
                        const G4Region* r = 0);
  G4double GetKinEnergy(const G4String&, const G4String&, G4double range,
                        const G4String& s = "world");

  G4double GetCrossSectionPerMass(const G4ParticleDefinition*, const G4Material*,
                                  const G4String& processName, G4double kinEnergy,
                                  const G4Region* r = 0);
  G4double GetCrossSectionPerMass(const G4String&, const G4String&,
                                  const G4String& processName, G4double kinEnergy,
                                  const G4String& s = "world");

  G4double GetMeanFreePath(const G4ParticleDefinition*, const G4Material*,
                           const G4String& processName, G4double kinEnergy,
                           const G4Region* r = 0);
  G4double GetMeanFreePath(const G4String&, const G4String&,
                           const G4String& processName, G4double kinEnergy,
                           const G4String& s = "world");

  void PrintDEDXTable(const G4ParticleDefinition*);

  void PrintRangeTable(const G4ParticleDefinition*);

  void PrintInverseRangeTable(const G4ParticleDefinition*);

  // Methods to calculate dE/dx and cross sections "on fly"
  // Existing tables and G4MaterialCutsCouples are not used

  G4double ComputeDEDX(const G4ParticleDefinition*, const G4Material*,
                       const G4String& processName, G4double kinEnergy,
                             G4double cut = DBL_MAX);
  G4double ComputeDEDX(const G4String&, const G4String&,
                       const G4String& processName, G4double kinEnergy,
                             G4double cut = DBL_MAX);

  G4double ComputeCrossSectionPerMass(const G4ParticleDefinition*, const G4Material*,
                                      const G4String& processName, G4double kinEnergy,
                                            G4double cut = 0.0);
  G4double ComputeCrossSectionPerMass(const G4String&, const G4String&,
                                      const G4String& processName, G4double kinEnergy,
                                            G4double cut = 0.0);
/*
  G4double ComputeCrossSectionPerAtom(const G4ParticleDefinition*, const G4Element*,
                                      const G4String& processName, G4double kinEnergy,
                                            G4double cut = 0.0);
  G4double ComputeCrossSectionPerAtom(const G4String&, const G4String&,
                                      const G4String& processName, G4double kinEnergy,
                                            G4double cut = 0.0);
*/
  G4double ComputeMeanFreePath(const G4ParticleDefinition*, const G4Material*,
                               const G4String& processName, G4double kinEnergy,
                                     G4double cut = 0.0);
  G4double ComputeMeanFreePath(const G4String&, const G4String&,
                               const G4String& processName, G4double kinEnergy,
                                     G4double cut = 0.0);

  const G4ParticleDefinition* FindParticle(const G4String&);

  const G4Material* FindMaterial(const G4String&);

//  const G4Material* FindMaterial(const G4Element*);

  const G4Region* FindRegion(const G4String&);

  const G4MaterialCutsCouple* FindCouple(const G4Material*, const G4Region* r = 0);

  void SetVerbose(G4int val);

private:

  G4bool UpdateParticle(const G4ParticleDefinition*, G4double kinEnergy);

  G4bool UpdateCouple(const G4Material*, G4double cut);

  void FindLambdaTable(const G4ParticleDefinition*, const G4String& processName);

  G4bool FindEmModel(const G4ParticleDefinition*, 
                     const G4String& processName,
                           G4double kinEnergy);

  const G4VEnergyLossProcess* FindEnergyLossProcess(const G4ParticleDefinition*);

  G4EmCalculator & operator=(const  G4EmCalculator &right);
  G4EmCalculator(const  G4EmCalculator&);

  std::vector<const G4Material*>            localMaterials;
  std::vector<const G4MaterialCutsCouple*>  localCouples;

  G4LossTableManager*          manager;
  G4DataVector                 localCuts;
  G4int                        nLocalMaterials;

  G4int                        verbose;

  // cash
  G4int                        currentCoupleIndex;
  const G4MaterialCutsCouple*  currentCouple;
  const G4Material*            currentMaterial;
  const G4ParticleDefinition*  currentParticle;
  const G4ParticleDefinition*  baseParticle;
  const G4PhysicsTable*        currentLambda;
        G4VEmModel*            currentModel;

  G4ionEffectiveCharge*        ionEffCharge;

  G4String                     currentName;
  G4double                     currentCut;
  G4double                     chargeSquare;
  G4double                     massRatio;
  G4bool                       isIon;
  G4bool                       isApplicable;

  G4String                     currentParticleName;
  G4String                     currentMaterialName;
};

//....oooOO0OOooo.......oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
