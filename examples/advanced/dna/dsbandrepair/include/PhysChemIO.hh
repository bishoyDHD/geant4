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
//
/// \file PhysChemIO.hh
/// \brief Definition of the PhysChemIO class

#ifndef PhysChemIO_h
#define PhysChemIO_h 1

#include "G4VPhysChemIO.hh"
class SteppingAction;
class PhysChemIO : public G4VPhysChemIO
{
public:
    PhysChemIO(SteppingAction* steppingAction);
    ~PhysChemIO() override = default;
    void CreateWaterMolecule(G4int /*electronicModif*/,
                            G4int /*electronicLevel*/,
                            G4double /*energy*/,
                            const G4Track* /*theIncomingTrack*/) override;
    void CreateSolvatedElectron(const G4Track* /*theIncomingTrack*/,
                                G4ThreeVector* finalPosition = 0) override;

    void InitializeFile() override {};
    void NewRun() override {};
    void NewEvent() override {};
    void WriteInto(const G4String&, std::ios_base::openmode) override {};
    void CloseFile() override {};
private:
    SteppingAction* fSteppingAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif