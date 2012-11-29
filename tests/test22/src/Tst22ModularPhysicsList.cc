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
// $Id$
//
// 

#include "Tst22ModularPhysicsList.hh"
#include "G4SystemOfUnits.hh"

Tst22ModularPhysicsList::Tst22ModularPhysicsList():  G4VModularPhysicsList()
{
  // default cut value  (1.0mm) 
  defaultCutValue = 1.0*mm;
  SetVerboseLevel(1);

  // General Physics
  RegisterPhysics( new Tst22GeneralPhysics("general") );

  // EM Physics
  this->RegisterPhysics( new Tst22EMPhysics("standard EM with bias for e/g Nuclear."));

  // Muon Physics
  RegisterPhysics(  new Tst22MuonPhysics("muon"));

  // Hadron Physics
  RegisterPhysics(  new Tst22HadronPhysics("hadron"));

  // Ion Physics
  RegisterPhysics( new Tst22IonPhysics("ion"));

}

Tst22ModularPhysicsList::~Tst22ModularPhysicsList() {}

// void Tst22ModularPhysicsList::SetCuts() {SetCutsWithDefault();}

