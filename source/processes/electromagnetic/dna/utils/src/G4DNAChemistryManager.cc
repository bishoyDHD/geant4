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
// Author: Mathieu Karamitros (kara@cenbg.in2p3.fr)
//
// WARNING : This class is released as a prototype.
// It might strongly evolve or even disapear in the next releases.
//
// History:
// -----------
// 10 Oct 2011 M.Karamitros created
//
// -------------------------------------------------------------------

#include "G4DNAChemistryManager.hh"
#include "G4Molecule.hh"
#include "G4ITStepManager.hh"
#include "G4H2O.hh"
#include "G4DNAMolecularReactionTable.hh"

using namespace std;

G4DNAChemistryManager* G4DNAChemistryManager::fInstance(0);

G4DNAChemistryManager::G4DNAChemistryManager() : fActiveChemistry(false)
{
    ;
}

G4DNAChemistryManager* G4DNAChemistryManager::Instance()
{
    if(!fInstance) fInstance = new G4DNAChemistryManager();
    return fInstance;
}

G4DNAChemistryManager::~G4DNAChemistryManager()
{
    G4DNAMolecularReactionTable::DeleteInstance();
}

void G4DNAChemistryManager::DeleteInstance()
{
    if(fInstance)  delete fInstance;
}

void G4DNAChemistryManager::CreateWaterMolecule(ElectronicModification modification,
                                           G4int electronicLevel,
                                           const G4Track* theIncomingTrack)
{
    G4Molecule * H2O = new G4Molecule (G4H2O::Definition());

    switch (modification)
    {
        case fExcitedMolecule :
            H2O -> ExciteMolecule(electronicLevel);
            break;
        case fIonizedMolecule :
            H2O -> IonizeMolecule(electronicLevel);
            break;
    }

    G4Track * H2OTrack = H2O->BuildTrack(1*picosecond,
                                         theIncomingTrack->GetPosition());

    H2OTrack -> SetParentID(theIncomingTrack->GetTrackID());
    H2OTrack -> SetTrackStatus(fStopButAlive);
    H2OTrack -> SetKineticEnergy(0.);

    G4ITStepManager::Instance()->PushTrack(H2OTrack);
}