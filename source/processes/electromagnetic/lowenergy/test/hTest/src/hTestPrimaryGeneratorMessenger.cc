#define hTestPrimaryGeneratorMessenger_CPP 

//---------------------------------------------------------------------------
//
// ClassName:   hTestPrimaryGeneratorMessenger
//  
// Description: Definition of physics list parameters
//
// Author:      V.Ivanchenko 26/09/00
//
//----------------------------------------------------------------------------
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "hTestPrimaryGeneratorMessenger.hh"
#include "hTestPrimaryGeneratorAction.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

hTestPrimaryGeneratorMessenger::hTestPrimaryGeneratorMessenger(
                                hTestPrimaryGeneratorAction* gen):
  theGen(gen)
{
  G4cout << "hTestPrimaryGeneratorMessenger: Construct " << G4endl;

  beamXCmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/beamX",this);
  beamXCmd->SetGuidance("Set X position of the center of the beam.");
  beamXCmd->SetParameterName("range",true);
  beamXCmd->SetUnitCategory("Length");
  beamXCmd->AvailableForStates(Idle);

  beamYCmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/beamY",this);
  beamYCmd->SetGuidance("Set Y position of the center of the beam.");
  beamYCmd->SetParameterName("range",true);
  beamYCmd->SetUnitCategory("Length");
  beamYCmd->AvailableForStates(Idle);

  beamZCmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/beamZ",this);
  beamZCmd->SetGuidance("Set Z of the entry point of the beam.");
  beamZCmd->SetParameterName("range",true);
  beamZCmd->SetUnitCategory("Length");
  beamZCmd->AvailableForStates(Idle);

  sigmaXCmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/sigmaX",this);
  sigmaXCmd->SetGuidance("Set the beam Gussian width for X.");
  sigmaXCmd->SetParameterName("range",false);
  sigmaXCmd->SetRange("range>=0.");
  sigmaXCmd->SetUnitCategory("Length");
  sigmaXCmd->AvailableForStates(Idle);

  sigmaYCmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/sigmaY",this);
  sigmaYCmd->SetGuidance("Set the beam Gussian width for Y.");
  sigmaYCmd->SetParameterName("range",false);
  sigmaYCmd->SetRange("range>=0.");
  sigmaYCmd->SetUnitCategory("Length");
  sigmaYCmd->AvailableForStates(Idle);

  sigmaZCmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/sigmaZ",this);
  sigmaZCmd->SetGuidance("Set the beam Gussian width for Y.");
  sigmaZCmd->SetParameterName("range",false);
  sigmaZCmd->SetRange("range>=0.");
  sigmaZCmd->SetUnitCategory("Length");
  sigmaZCmd->AvailableForStates(Idle);

  sigmaECmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/sigmaE",this);
  sigmaECmd->SetGuidance("Set the beam Gussian width for Y.");
  sigmaECmd->SetParameterName("range",false);
  sigmaECmd->SetRange("range>=0.");
  sigmaECmd->SetUnitCategory("Length");
  sigmaECmd->AvailableForStates(Idle);

  maxThetaCmd = new G4UIcmdWithADoubleAndUnit("/hTest/generator/maxTheta",this);
  maxThetaCmd->SetGuidance("Set the beam maxTheta in degrees.");
  maxThetaCmd->SetParameterName("range",false);
  maxThetaCmd->SetRange("range<=90.0");
  maxThetaCmd->SetUnitCategory("Angle");
  maxThetaCmd->AvailableForStates(Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

hTestPrimaryGeneratorMessenger::~hTestPrimaryGeneratorMessenger()
{
  delete beamXCmd;
  delete beamYCmd;
  delete beamZCmd;
  delete sigmaXCmd;
  delete sigmaYCmd;
  delete sigmaZCmd;
  delete sigmaECmd;
  delete maxThetaCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
  
void hTestPrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command,
                                                 G4String newValue)
{
 
  if(1 < theGen->GetVerbose()) {
    G4cout << "hTestPrimaryGeneratorMessenger: Next command value " 
           << newValue << G4endl;
  }

  if(command == beamXCmd)
    {theGen->SetBeamX(beamXCmd->GetNewDoubleValue(newValue));}
  if(command == beamYCmd)
    {theGen->SetBeamY(beamYCmd->GetNewDoubleValue(newValue));}
  if(command == beamZCmd) {
     G4double z = beamZCmd->GetNewDoubleValue(newValue);
     theGen->SetBeamZ(z);
     theMCmanager->SetBeamTime(z/c_light);
    }
  if(command == sigmaXCmd)
    {theGen->SetBeamSigmaX(sigmaXCmd->GetNewDoubleValue(newValue));}
  if(command == sigmaYCmd)
    {theGen->SetBeamSigmaY(sigmaYCmd->GetNewDoubleValue(newValue));}
  if(command == sigmaZCmd)
    {theGen->SetBeamSigmaZ(sigmaZCmd->GetNewDoubleValue(newValue));}
  if(command == sigmaECmd)
    {theGen->SetBeamSigmaE(sigmaECmd->GetNewDoubleValue(newValue));}
  if(command == maxThetaCmd)
    {theGen->SetBeamMinCosTheta(
                   cos(maxThetaCmd->GetNewDoubleValue(newValue)));}

  if(1 < theGen->GetVerbose()) {
    G4cout << "hTestPrimaryGeneratorMessenger: O'K " << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

