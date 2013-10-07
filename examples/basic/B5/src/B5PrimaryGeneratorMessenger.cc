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
// $Id$
//
/// \file B5PrimaryGeneratorMessenger.cc
/// \brief Implementation of the B5PrimaryGeneratorMessenger class

#include "B5PrimaryGeneratorMessenger.hh"
#include "B5PrimaryGeneratorAction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5PrimaryGeneratorMessenger::B5PrimaryGeneratorMessenger(
                                 B5PrimaryGeneratorAction* generator)
: G4UImessenger(), fGenerator(generator),
  fMomentumCmd(0), fSigmaMomCmd(0), fSigmaAngCmd(0), fRandomCmd(0)
{
    fMomentumCmd = new G4UIcmdWithADoubleAndUnit("/mydet/momentum",this);
    fMomentumCmd->SetGuidance("Mean momentum of primaries");
    fMomentumCmd->SetParameterName("p",true);
    fMomentumCmd->SetRange("p>=0.");
    fMomentumCmd->SetDefaultValue(1.);
    fMomentumCmd->SetDefaultUnit("GeV");
    
    fSigmaMomCmd = new G4UIcmdWithADoubleAndUnit("/mydet/sigmaMomentum",this);
    fSigmaMomCmd->SetGuidance("Sigma momentum of primaries");
    fSigmaMomCmd->SetParameterName("p",true);
    fSigmaMomCmd->SetRange("p>=0.");
    fSigmaMomCmd->SetDefaultValue(1.);
    fSigmaMomCmd->SetDefaultUnit("GeV");
    
    fSigmaAngCmd = new G4UIcmdWithADoubleAndUnit("/mydet/sigmaAngle",this);
    fSigmaAngCmd->SetGuidance("sigma angle divergence of primaries");
    fSigmaAngCmd->SetParameterName("t",true);
    fSigmaAngCmd->SetRange("t>=0.");
    fSigmaAngCmd->SetDefaultValue(1.);
    fSigmaAngCmd->SetDefaultUnit("deg");
    
    fRandomCmd = new G4UIcmdWithABool("/mydet/randomizePrimary",this);
    fRandomCmd->SetGuidance(
      "Boolean flag for randomizing primary particle types.");
    fRandomCmd->SetGuidance(
      "In case this flag is false, you can select the primary particle");
    fRandomCmd->SetGuidance(
      "  with /gun/particle command.");
    fRandomCmd->SetParameterName("flg",true);
    fRandomCmd->SetDefaultValue(true);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B5PrimaryGeneratorMessenger::~B5PrimaryGeneratorMessenger()
{
    delete fMomentumCmd;
    delete fSigmaMomCmd;
    delete fSigmaAngCmd;
    delete fRandomCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B5PrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command,
                                               G4String newValue)
{
    if( command==fMomentumCmd )
    { fGenerator->SetMomentum(fMomentumCmd->GetNewDoubleValue(newValue)); }
    if( command==fSigmaMomCmd )
    { fGenerator->SetSigmaMomentum(fSigmaMomCmd->GetNewDoubleValue(newValue)); }
    if( command==fSigmaAngCmd )
    { fGenerator->SetSigmaAngle(fSigmaAngCmd->GetNewDoubleValue(newValue)); }
    if( command==fRandomCmd )
    { fGenerator->SetRandomize(fRandomCmd->GetNewBoolValue(newValue)); }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4String B5PrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand * command)
{
    G4String cv;
    if( command==fMomentumCmd )
    { cv = fMomentumCmd->ConvertToString(fGenerator->GetMomentum(),"GeV"); }
    if( command==fSigmaMomCmd )
    { cv = fSigmaMomCmd->ConvertToString(fGenerator->GetSigmaMomentum(),"GeV"); }
    if( command==fSigmaAngCmd )
    { cv = fSigmaAngCmd->ConvertToString(fGenerator->GetSigmaAngle(),"deg"); }
    if( command==fRandomCmd )
    { cv = fRandomCmd->ConvertToString(fGenerator->GetRandomize()); }
    
    return cv;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
