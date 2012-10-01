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
// $Id: G4ElectronCapture.cc,v 1.1 2010-08-31 11:23:58 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4ElectronCapture
//
// Description: The process to kill particles to save CPU
//
// Author:      V.Ivanchenko 31 August 2010
//
//----------------------------------------------------------------------------
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4ElectronCapture.hh"
#include "G4ParticleDefinition.hh"
#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Region.hh"
#include "G4RegionStore.hh"
#include "G4Electron.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ElectronCapture::G4ElectronCapture(const G4String& regName, G4double ekinlim)
  : G4VDiscreteProcess("eCapture", fElectromagnetic), kinEnergyThreshold(ekinlim),
    regionName(regName), region(0)
{
  if(regName == "" || regName == "world") { 
    regionName = "DefaultRegionForTheWorld";
  }
  pParticleChange = &fParticleChange;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ElectronCapture::~G4ElectronCapture() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ElectronCapture::SetKinEnergyLimit(G4double val)
{
  kinEnergyThreshold = val;
  if(verboseLevel > 0) {
    G4cout << "### G4ElectronCapture: Tracking cut E(MeV) = " 
	   << kinEnergyThreshold/MeV << G4endl;
  }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4ElectronCapture::BuildPhysicsTable(const G4ParticleDefinition&)
{
  region = (G4RegionStore::GetInstance())->GetRegion(regionName);
  if(region && verboseLevel > 0) {
    G4cout << "### G4ElectronCapture: Tracking cut E(MeV) = " 
	   << kinEnergyThreshold/MeV << " is assigned to " << regionName 
	   << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool G4ElectronCapture::IsApplicable(const G4ParticleDefinition&)
{
  return true;
}

G4double 
G4ElectronCapture::PostStepGetPhysicalInteractionLength(const G4Track& aTrack,
							G4double, 
							G4ForceCondition* condition)
{
  // condition is set to "Not Forced"
  *condition = NotForced;
  
  G4double limit = DBL_MAX; 
  if(region) {
    if(aTrack.GetVolume()->GetLogicalVolume()->GetRegion() == region && 
	 aTrack.GetKineticEnergy() < kinEnergyThreshold) { limit = 0.0; }
  }
  return limit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VParticleChange* G4ElectronCapture::PostStepDoIt(const G4Track& aTrack, 
						   const G4Step&)
{
  pParticleChange->Initialize(aTrack);
  pParticleChange->ProposeTrackStatus(fStopAndKill);
  pParticleChange->ProposeLocalEnergyDeposit(aTrack.GetKineticEnergy());
  fParticleChange.SetProposedKineticEnergy(0.0);
  return pParticleChange;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4ElectronCapture::GetMeanFreePath(const G4Track&,G4double,
					    G4ForceCondition*)
{
  return DBL_MAX;
}    

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


