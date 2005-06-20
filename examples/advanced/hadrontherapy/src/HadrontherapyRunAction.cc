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
// $Id: HadrontherapyRunAction.cc,v 3.0, September 2004;
// ----------------------------------------------------------------------------
//                 GEANT 4 - Hadrontherapy example
// ----------------------------------------------------------------------------
// Code developed by:
//
// G.A.P. Cirrone(a)*, F. Di Rosa(a), S. Guatelli(b), G. Russo(a)
// 
// (a) Laboratori Nazionali del Sud 
//     of the INFN, Catania, Italy
// (b) INFN Section of Genova, Genova, Italy
// 
// * cirrone@lns.infn.it
// ----------------------------------------------------------------------------

#include "HadrontherapyRunAction.hh"
#include "HadrontherapyEventAction.hh"
#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4ios.hh"
#include "HadrontherapyDetectorConstruction.hh"
#include "G4SDManager.hh"
#include "G4Timer.hh"
#include "HadrontherapyRunAction.hh"

HadrontherapyRunAction::HadrontherapyRunAction()
{
}

HadrontherapyRunAction::~HadrontherapyRunAction()
{ 
}

void HadrontherapyRunAction::BeginOfRunAction(const G4Run* aRun)
{ 	
   G4RunManager::GetRunManager()->SetRandomNumberStore(true);
   G4cout << "Run " << aRun -> GetRunID() << " starts ..." << G4endl;

   electromagnetic = 0;
   hadronic = 0;
}

void HadrontherapyRunAction::EndOfRunAction(const G4Run* aRun)
{
  G4cout << " Summary of Run " << aRun -> GetRunID() <<" :"<< G4endl;
  G4cout << "Number of electromagnetic processes of primary particles in the phantom:"
	 << electromagnetic << G4endl;
  G4cout << "Number of hadronic processes of primary particles in the phantom:"
	 << hadronic << G4endl;
}
void HadrontherapyRunAction::AddEMProcess()
{
 electromagnetic += 1;
}
void HadrontherapyRunAction::AddHadronicProcess()
{
  hadronic += 1;
}



