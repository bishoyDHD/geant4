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
// $Id: VoxelSD.cc,v 1.1 2007-11-16 14:29:33 kmura Exp $
// $Name: not supported by cvs2svn $
//
// ====================================================================
//   VoxelSD.cc
//
//                                         2007 Q
// ====================================================================
#include "VoxelSD.hh"
#include "G4SDManager.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4VTouchable.hh"
#include "G4TouchableHistory.hh"
#include "Randomize.hh"
#include "G4MPImanager.hh"
#include "Analysis.hh"

// ====================================================================
//
// class description
//
// ====================================================================

//////////////////////////////////////
VoxelSD::VoxelSD(const G4String& name)
  : G4VSensitiveDetector(name)
//////////////////////////////////////
{
  G4SDManager::GetSDMpointer()-> AddNewDetector(this);
}


///////////////////
VoxelSD::~VoxelSD()
///////////////////
{
}

///////////////////////////////////////////////////////////////
G4bool VoxelSD::ProcessHits(G4Step* astep, G4TouchableHistory*)
///////////////////////////////////////////////////////////////
{
  G4ThreeVector x0= astep-> GetPreStepPoint()-> GetPosition();
  G4ThreeVector x1= astep-> GetPostStepPoint()-> GetPosition();
  
  G4int tid= astep-> GetTrack()-> GetTrackID();
  G4int iz= astep-> GetPreStepPoint()-> GetTouchable()-> GetReplicaNumber(1);

  Analysis* myana= Analysis::GetAnalysis();

  // incident position
  if (tid==1 && iz==1000) { // scored @ first layer
    myana-> FillIncident(x0);
  }
  
  // energy deposit
  G4ThreeVector p= G4UniformRand()*(x1-x0) + x0;   // position sampling
  G4double edep= astep-> GetTotalEnergyDeposit();
  myana-> FillDose(p, edep);  

  return true;
}


////////////////////////////////////////////
void VoxelSD::Initialize(G4HCofThisEvent*)
////////////////////////////////////////////
{
}


////////////////////////////////////////////
void VoxelSD::EndOfEvent(G4HCofThisEvent*)
////////////////////////////////////////////
{
}

