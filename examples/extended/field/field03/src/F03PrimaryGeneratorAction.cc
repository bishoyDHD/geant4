// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: F03PrimaryGeneratorAction.cc,v 1.1 2001-06-08 11:55:58 grichine Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

#include "F03PrimaryGeneratorAction.hh"

#include "F03DetectorConstruction.hh"
#include "F03PrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "Randomize.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....
 
 G4String F03PrimaryGeneratorAction::thePrimaryParticleName="proton" ; 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

F03PrimaryGeneratorAction::F03PrimaryGeneratorAction(
                                            F03DetectorConstruction* F03DC)
:F03Detector(F03DC),rndmFlag("off"),xvertex(0.),yvertex(0.),zvertex(0.),
 vertexdefined(false)
{
  G4int n_particle = 1;
  particleGun  = new G4ParticleGun(n_particle);
  
  //create a messenger for this class
  gunMessenger = new F03PrimaryGeneratorMessenger(this);

  // default particle kinematic

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4String particleName;
  G4ParticleDefinition* particle
                    = particleTable->FindParticle(particleName="proton");
  particleGun->SetParticleDefinition(particle);
  
  thePrimaryParticleName = particle->GetParticleName() ;

  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,-1.));
  particleGun->SetParticleEnergy(1.*GeV);

  zvertex = 21990.5 ; // 21989.0 ; //  -0.5*(F03Detector->GetAbsorberThickness());
  particleGun->SetParticlePosition(G4ThreeVector(xvertex,yvertex,zvertex));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

F03PrimaryGeneratorAction::~F03PrimaryGeneratorAction()
{
  delete particleGun;
  delete gunMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

void F03PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  //this function is called at the begining of event
  // 
  thePrimaryParticleName = particleGun->GetParticleDefinition()->
                                                GetParticleName() ;
  G4double x0,y0,z0 ;
  if(vertexdefined)
  {
    x0 = xvertex ;
    y0 = yvertex ;
    z0 = zvertex ;
  }
  else
  {
    x0 = 0. ;
    y0 = 0. ;
    z0 = 21985.0 ; // -0.5*(F03Detector->GetWorldSizeZ()) ;
  }
  G4double r0,phi0 ;
  /* ****************************************************
  if (rndmFlag == "on")
  {
      r0 = (F03Detector->GetAbsorberRadius())*sqrt(G4UniformRand());
      phi0 = twopi*G4UniformRand();
      x0 = r0*cos(phi0);
      y0 = r0*sin(phi0);
  } 
  ************************************************************ */
  particleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));
  particleGun->GeneratePrimaryVertex(anEvent);
}

///////////////////////////////////////////////////////////////////////
//
//

G4String F03PrimaryGeneratorAction::GetPrimaryName()
{
   return thePrimaryParticleName ;
}

void F03PrimaryGeneratorAction::Setzvertex(G4double z)
{
  vertexdefined = true ;
  zvertex = z ;
  G4cout << " Z coordinate of the primary vertex = " << zvertex/mm <<
            " mm." << G4endl;
}
void F03PrimaryGeneratorAction::Setxvertex(G4double x)
{
  vertexdefined = true ;
  xvertex = x ;
  G4cout << " X coordinate of the primary vertex = " << xvertex/mm <<
            " mm." << G4endl;
}

void F03PrimaryGeneratorAction::Setyvertex(G4double y)
{
  vertexdefined = true ;
  yvertex = y ;
  G4cout << " Y coordinate of the primary vertex = " << yvertex/mm <<
            " mm." << G4endl;
}





