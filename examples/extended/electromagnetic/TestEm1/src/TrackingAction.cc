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
// $Id: TrackingAction.cc,v 1.10 2006-06-29 16:37:36 gunter Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "TrackingAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "RunAction.hh"
#include "HistoManager.hh"

#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(PrimaryGeneratorAction* prim, RunAction* run)
:fPrimary(prim), fRunAction(run)
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PreUserTrackingAction(const G4Track*)
{
  //  G4cout << "ID= " << aTrack->GetTrackID() << "  e(MeV)= " 
  //         << aTrack->GetDynamicParticle()->GetKineticEnergy()/MeV << "  "
  //         << aTrack->GetDynamicParticle()->GetDefinition()->GetParticleName()
  //         << G4endl;    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* aTrack)
{  
  //increase nb of processed tracks 
  //count nb of steps of this track
  G4int   nbSteps = aTrack->GetCurrentStepNumber();
  G4double Trleng = aTrack->GetTrackLength();
    
  if (aTrack->GetDefinition()->GetPDGCharge() == 0.) {
    fRunAction->CountTraks0(1); 
    fRunAction->CountSteps0(nbSteps);
  
  } else {
    fRunAction->CountTraks1(1); 
    fRunAction->CountSteps1(nbSteps);
  }
  
  //true and projected ranges for primary particle
  if (aTrack->GetTrackID() == 1) {
    fRunAction->AddTrueRange(Trleng);
    G4ThreeVector vertex = fPrimary->GetParticleGun()->GetParticlePosition();    
    G4ThreeVector position = aTrack->GetPosition() - vertex;      
    fRunAction->AddProjRange(position.x());
    fRunAction->AddTransvDev(position.y());
    fRunAction->AddTransvDev(position.z());
    
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->FillH1(1,Trleng);
    analysisManager->FillH1(2,(float)nbSteps);        
  }        
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

