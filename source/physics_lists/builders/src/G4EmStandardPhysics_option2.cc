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
// $Id: G4EmStandardPhysics_option2.cc,v 1.29 2010-11-21 15:47:03 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//---------------------------------------------------------------------------
//
// ClassName:   G4EmStandardPhysics_option2
//
// Author:      V.Ivanchenko 09.11.2005
//
// Modified:
// 19.12.2005 V.Ivanchenko rename 71 -> 72
// 15.06.2006 V.Ivanchenko use this class as a constructor of fast EM physics
// 13.11.2006 V.Ivanchenko use G4hMultipleScattering
// 14.11.2006 V.Ivanchenko use sub-cutoff option for all particles
// 13.02.2007 V.Ivanchenko use default msc 
// 15.05.2007 V.Ivanchenko rename to _option2 
// 13.03.2008 V.Ivanchenko use G4eMultipleScattering
// 21.04.2008 V.Ivanchenko add long-lived D and B mesons; use spline
// 28.05.2008 V.Ivanchenko linLossLimit=0.01; added hBrem and hPairProd processes
//
//----------------------------------------------------------------------------
//

#include "G4EmStandardPhysics_option2.hh"

#include "G4ParticleDefinition.hh"
#include "G4LossTableManager.hh"
#include "G4EmProcessOptions.hh"

#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4PEEffectFluoModel.hh"
#include "G4KleinNishinaModel.hh"

#include "G4eMultipleScattering.hh"
#include "G4hMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4CoulombScattering.hh"
#include "G4UrbanMscModel95.hh"
#include "G4WentzelVIModel.hh"
//#include "G4GoudsmitSaundersonMscModel.hh"

#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eBremsstrahlungModel.hh"
#include "G4eplusAnnihilation.hh"
#include "G4Generator2BS.hh"
#include "G4UAtomicDeexcitation.hh"

#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#include "G4hBremsstrahlung.hh"
#include "G4hPairProduction.hh"

#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"
#include "G4alphaIonisation.hh"
//#include "G4IonParametrisedLossModel.hh"

#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4PionPlus.hh"
#include "G4PionMinus.hh"
#include "G4KaonPlus.hh"
#include "G4KaonMinus.hh"
#include "G4Proton.hh"
#include "G4AntiProton.hh"
#include "G4Deuteron.hh"
#include "G4Triton.hh"
#include "G4He3.hh"
#include "G4Alpha.hh"
#include "G4GenericIon.hh"

#include "G4PhysicsListHelper.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics_option2::G4EmStandardPhysics_option2(G4int ver)
  : G4VPhysicsConstructor("G4EmStandard_opt2"), verbose(ver)
{
  G4LossTableManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics_option2::G4EmStandardPhysics_option2(G4int ver, const G4String&)
  : G4VPhysicsConstructor("G4EmStandard_opt2"), verbose(ver)
{
  G4LossTableManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4EmStandardPhysics_option2::~G4EmStandardPhysics_option2()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4EmStandardPhysics_option2::ConstructParticle()
{
// gamma
  G4Gamma::Gamma();

// leptons
  G4Electron::Electron();
  G4Positron::Positron();
  G4MuonPlus::MuonPlus();
  G4MuonMinus::MuonMinus();

// mesons
  G4PionPlus::PionPlusDefinition();
  G4PionMinus::PionMinusDefinition();
  G4KaonPlus::KaonPlusDefinition();
  G4KaonMinus::KaonMinusDefinition();

// barions
  G4Proton::Proton();
  G4AntiProton::AntiProton();

// ions
  G4Deuteron::Deuteron();
  G4Triton::Triton();
  G4He3::He3();
  G4Alpha::Alpha();
  G4GenericIon::GenericIonDefinition();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4EmStandardPhysics_option2::ConstructProcess()
{
  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();

  // Add standard EM Processes
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4String particleName = particle->GetParticleName();
    if(verbose > 1)
      G4cout << "### " << GetPhysicsName() << " instantiates for " 
	     << particleName << G4endl;

    if (particleName == "gamma") {

      G4PhotoElectricEffect* pe = new G4PhotoElectricEffect;
      pe->SetModel(new G4PEEffectFluoModel());
      G4ComptonScattering* cs   = new G4ComptonScattering;
      cs->SetModel(new G4KleinNishinaModel());

      ph->RegisterProcess(pe, particle);
      ph->RegisterProcess(cs, particle);
      ph->RegisterProcess(new G4GammaConversion(), particle);

    } else if (particleName == "e-") {

      G4eMultipleScattering* msc = new G4eMultipleScattering();
      //msc->AddEmModel(0, new G4WentzelVIModel());
      //msc->SetRangeFactor(0.04);
      msc->AddEmModel(0, new G4UrbanMscModel95());
      //      msc->AddEmModel(0, new G4GoudsmitSaundersonMscModel());
      G4eBremsstrahlung* brem = new G4eBremsstrahlung();
      G4eBremsstrahlungModel* br = new G4eBremsstrahlungModel();
      br->SetAngularDistribution(new G4Generator2BS());
      brem->SetEmModel(br);

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(new G4eIonisation(), particle);
      ph->RegisterProcess(brem, particle);

    } else if (particleName == "e+") {

      G4eMultipleScattering* msc = new G4eMultipleScattering();
      msc->AddEmModel(0, new G4UrbanMscModel95());
      //msc->AddEmModel(0, new G4WentzelVIModel());
      //msc->SetRangeFactor(0.04);
      // msc->AddEmModel(0, new G4GoudsmitSaundersonMscModel());
      G4eBremsstrahlung* brem = new G4eBremsstrahlung();
      G4eBremsstrahlungModel* br = new G4eBremsstrahlungModel();
      br->SetAngularDistribution(new G4Generator2BS());
      brem->SetEmModel(br);

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(new G4eIonisation(), particle);
      ph->RegisterProcess(brem, particle);
      ph->RegisterProcess(new G4eplusAnnihilation(), particle);

    } else if (particleName == "mu+" ||
               particleName == "mu-"    ) {

      G4MuMultipleScattering* msc = new G4MuMultipleScattering();
      msc->AddEmModel(0, new G4WentzelVIModel());

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(new G4MuIonisation(), particle);
      ph->RegisterProcess(new G4MuBremsstrahlung(), particle);
      ph->RegisterProcess(new G4MuPairProduction(), particle);
      ph->RegisterProcess(new G4CoulombScattering(), particle);

    } else if (particleName == "alpha" ||
               particleName == "He3") {

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4ionIonisation(), particle);

    } else if (particleName == "GenericIon") {

      G4ionIonisation* ionIoni = new G4ionIonisation();
      //ionIoni->SetEmModel(new G4IonParametrisedLossModel());
      //ionIoni->SetStepFunction(0.1, 20*um);

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(ionIoni, particle);

    } else if (particleName == "pi+" ||
               particleName == "pi-" ||
	       particleName == "kaon+" ||
               particleName == "kaon-" ||
               particleName == "proton" ) {

      G4hMultipleScattering* msc = new G4hMultipleScattering();
      msc->AddEmModel(0, new G4WentzelVIModel());

      ph->RegisterProcess(msc, particle);
      ph->RegisterProcess(new G4hIonisation(), particle);
      ph->RegisterProcess(new G4hBremsstrahlung(), particle);
      ph->RegisterProcess(new G4hPairProduction(), particle);

    } else if (particleName == "B+" ||
	       particleName == "B-" ||
	       particleName == "D+" ||
	       particleName == "D-" ||
	       particleName == "Ds+" ||
	       particleName == "Ds-" ||
               particleName == "anti_He3" ||
               particleName == "anti_alpha" ||
               particleName == "anti_deuteron" ||
               particleName == "anti_lambda_c+" ||
               particleName == "anti_omega-" ||
               particleName == "anti_proton" ||
               particleName == "anti_sigma_c+" ||
               particleName == "anti_sigma_c++" ||
               particleName == "anti_sigma+" ||
               particleName == "anti_sigma-" ||
               particleName == "anti_triton" ||
               particleName == "anti_xi_c+" ||
               particleName == "anti_xi-" ||
               particleName == "deuteron" ||
	       particleName == "lambda_c+" ||
               particleName == "omega-" ||
               particleName == "sigma_c+" ||
               particleName == "sigma_c++" ||
               particleName == "sigma+" ||
               particleName == "sigma-" ||
               particleName == "tau+" ||
               particleName == "tau-" ||
               particleName == "triton" ||
               particleName == "xi_c+" ||
               particleName == "xi-" ) {

      ph->RegisterProcess(new G4hMultipleScattering(), particle);
      ph->RegisterProcess(new G4hIonisation(), particle);
    }
  }
    
  // Em options
  //    
  G4EmProcessOptions opt;
  opt.SetVerbose(verbose);
  opt.SetApplyCuts(true);
  
  // Scattering options
  //
  opt.SetPolarAngleLimit(0.2);
    
  // Ionization
  //
  //opt.SetSubCutoff(true);  

  // Deexcitation
  //
  G4VAtomDeexcitation* de = new G4UAtomicDeexcitation();
  G4LossTableManager::Instance()->SetAtomDeexcitation(de);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
