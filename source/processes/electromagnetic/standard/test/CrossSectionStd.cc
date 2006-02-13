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
//
//
// $Id: CrossSectionStd.cc,v 1.3 2006-02-13 16:52:24 maire Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
// 
// ------------------------------------------------------------
//
//  To print cross sections per atom and mean free path for simple material
//
#include "G4Material.hh"

#include "G4PEEffectModel.hh"
#include "G4KleinNishinaCompton.hh"
#include "G4BetheHeitlerModel.hh"

#include "G4eeToTwoGammaModel.hh"

#include "G4MollerBhabhaModel.hh"
#include "G4eBremsstrahlungModel.hh"

#include "G4BetheBlochModel.hh"

#include "G4MuBetheBlochModel.hh"
#include "G4MuBremsstrahlungModel.hh"
#include "G4MuPairProductionModel.hh"

#include "globals.hh"
#include "G4UnitsTable.hh"

#include "G4Gamma.hh"
#include "G4Positron.hh"
#include "G4Electron.hh"
#include "G4Proton.hh"
#include "G4MuonPlus.hh"

int main() {

  G4UnitDefinition::BuildUnitsTable();

  // define materials
  //
  G4double Z, A;

  G4Material* material =
  new G4Material("Iodine", Z=53., A=126.90*g/mole, 4.93*g/cm3);

  G4cout << *(G4Material::GetMaterialTable()) << G4endl;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // initialise gamma processes (models)
  //
  G4ParticleDefinition* gamma = G4Gamma::Gamma();
  
  G4VEmModel* phot = new G4PEEffectModel();
  G4VEmModel* comp = new G4KleinNishinaCompton();
  G4VEmModel* conv = new G4BetheHeitlerModel(); 
  
  // compute CrossSection per atom and MeanFreePath
  //
  G4double Emin = 1.01*MeV, Emax = 2.01*MeV, dE = 100*keV;

  G4cout << "\n #### Gamma : CrossSectionPerAtom and MeanFreePath for " 
         << material->GetName() << G4endl;
  G4cout << "\n Energy \t PhotoElec \t Compton \t Conversion \t";
  G4cout <<           "\t PhotoElec \t Compton \t Conversion" << G4endl;
  
  for (G4double Energy = Emin; Energy <= Emax; Energy += dE) {
    G4cout << "\n " << G4BestUnit (Energy, "Energy")
     << "\t" 
     << G4BestUnit (phot->ComputeCrossSectionPerAtom(gamma,Energy,Z), "Surface")
     << "\t"	 
     << G4BestUnit (comp->ComputeCrossSectionPerAtom(gamma,Energy,Z), "Surface")
     << "\t"	 
     << G4BestUnit (conv->ComputeCrossSectionPerAtom(gamma,Energy,Z), "Surface")
     << "\t \t"	 
     << G4BestUnit (phot->ComputeMeanFreePath(gamma,Energy,material), "Length")
     << "\t"	 
     << G4BestUnit (comp->ComputeMeanFreePath(gamma,Energy,material), "Length")
     << "\t"	 
     << G4BestUnit (conv->ComputeMeanFreePath(gamma,Energy,material), "Length");
  }

  G4cout << G4endl;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // initialise positron annihilation (model)
  // 
  G4ParticleDefinition* posit = G4Positron::Positron();
   
  G4VEmModel* anni = new G4eeToTwoGammaModel();
  
  // compute CrossSection per atom and MeanFreePath
  //
  Emin = 1.01*MeV; Emax = 2.01*MeV; dE = 100*keV;

  G4cout << "\n #### e+ annihilation : CrossSectionPerAtom and MeanFreePath for " 
         << material->GetName() << G4endl;
  G4cout << "\n Energy \t e+ annihil \t";
  G4cout <<           "\t e+ annihil" << G4endl;
  
  for (G4double Energy = Emin; Energy <= Emax; Energy += dE) {
    G4cout << "\n " << G4BestUnit (Energy, "Energy")
     << "\t" 
     << G4BestUnit (anni->ComputeCrossSectionPerAtom(posit,Energy,Z), "Surface")
     << "\t \t"	 
     << G4BestUnit (anni->ComputeMeanFreePath(posit,Energy,material), "Length");
  }

  G4cout << G4endl;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

  // initialise electron processes (models)
  // 
  G4ParticleDefinition* elec = G4Electron::Electron();
   
  G4VEmModel* ioni = new G4MollerBhabhaModel();
  G4VEmModel* brem = new G4eBremsstrahlungModel();
  
  // compute CrossSection per atom and MeanFreePath
  //
  Emin = 1.01*MeV; Emax = 101.01*MeV; dE = 10*MeV;
  G4double Ecut = 100*keV;

  G4cout << "\n ####electron: CrossSection, MeanFreePath and StoppingPower for "
         << material->GetName() 
	 << ";\tEnergy cut = " << G4BestUnit (Ecut, "Energy") << G4endl;
	 
  G4cout << "\n Energy \t ionization \t bremsstra \t";
  G4cout <<           "\t ionization \t bremsstra \t";
  G4cout <<           "\t ionization \t bremsstra" << G4endl;
  
  for (G4double Energy = Emin; Energy <= Emax; Energy += dE) {
    G4cout << "\n " << G4BestUnit (Energy, "Energy")
     << "\t" 
     << G4BestUnit (ioni->ComputeCrossSectionPerAtom(elec,Energy,Z,A,Ecut),
                   "Surface")
     << "\t" 
     << G4BestUnit (brem->ComputeCrossSectionPerAtom(elec,Energy,Z,A,Ecut),
                   "Surface")		   
     << "\t \t"	 
     << G4BestUnit (ioni->ComputeMeanFreePath(elec,Energy,material,Ecut),
                   "Length")
     << "\t"	 
     << G4BestUnit (brem->ComputeMeanFreePath(elec,Energy,material,Ecut),
                   "Length")		   
     << "\t \t"	 
     << G4BestUnit (ioni->ComputeDEDXPerVolume(material,elec,Energy,Ecut),
                   "Energy/Length")
     << "\t"	 
     << G4BestUnit (brem->ComputeDEDXPerVolume(material,elec,Energy,Ecut),
                   "Energy/Length");		   		   
  }
  
  G4cout << G4endl;
  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


  // initialise proton processes (models)
  // 
  G4ParticleDefinition* prot = G4Proton::Proton();
   
  ioni = new G4BetheBlochModel(prot);
  
  // compute CrossSection per atom and MeanFreePath
  //
  Emin = 1.01*MeV; Emax = 102.01*MeV; dE = 10*MeV;
  Ecut = 100*keV;

  G4cout << "\n #### proton : CrossSection, MeanFreePath and StoppingPower for "
         << material->GetName() 
	 << ";\tEnergy cut = " << G4BestUnit (Ecut, "Energy") << G4endl;
	 
  G4cout << "\n Energy \t ionization \t";
  G4cout <<           "\t ionization \t";
  G4cout <<           "\t ionization" << G4endl;
  
  for (G4double Energy = Emin; Energy <= Emax; Energy += dE) {
    G4cout << "\n " << G4BestUnit (Energy, "Energy")
     << "\t" 
     << G4BestUnit (ioni->ComputeCrossSectionPerAtom(prot,Energy,Z,A,Ecut),
                   "Surface")
     << "\t \t"	 
     << G4BestUnit (ioni->ComputeMeanFreePath(prot,Energy,material,Ecut),
                   "Length")	   
     << "\t \t"	 
     << G4BestUnit (ioni->ComputeDEDXPerVolume(material,prot,Energy,Ecut),
                   "Energy/Length");
  }
  
  G4cout << G4endl;
  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
  // initialise muon processes (models)
  // 
  G4ParticleDefinition* muon = G4MuonPlus::MuonPlus();
   
              ioni = new G4MuBetheBlochModel(muon);
              brem = new G4MuBremsstrahlungModel(muon);
  G4VEmModel* pair = new G4MuPairProductionModel(muon);
   
  // compute CrossSection per atom and MeanFreePath
  //
  Emin = 1.01*GeV; Emax = 101.01*GeV; dE = 10*GeV;
  Ecut = 10*MeV;

  G4cout << "\n ####muon: CrossSection and MeanFreePath for "
         << material->GetName() 
	 << ";\tEnergy cut = " << G4BestUnit (Ecut, "Energy") << G4endl;
	 
  G4cout << "\n Energy \t ionization \t bremsstra \t pair_prod \t";
  G4cout <<           "\t ionization \t bremsstra \t pair_prod" << G4endl;
  
  for (G4double Energy = Emin; Energy <= Emax; Energy += dE) {
    G4cout << "\n " << G4BestUnit (Energy, "Energy")
     << "\t" 
     << G4BestUnit (ioni->ComputeCrossSectionPerAtom(muon,Energy,Z,A,Ecut),
                   "Surface")
     << "\t" 
     << G4BestUnit (brem->ComputeCrossSectionPerAtom(muon,Energy,Z,A,Ecut),
                   "Surface")
     << "\t" 		   
     << G4BestUnit (pair->ComputeCrossSectionPerAtom(muon,Energy,Z,A,Ecut),
                   "Surface")		   		   
     << "\t \t"	 
     << G4BestUnit (ioni->ComputeMeanFreePath(muon,Energy,material,Ecut),
                   "Length")
     << "\t"	 
     << G4BestUnit (brem->ComputeMeanFreePath(muon,Energy,material,Ecut),
                   "Length")
     << "\t"	 
     << G4BestUnit (pair->ComputeMeanFreePath(muon,Energy,material,Ecut),
                   "Length");		   		      		   		   
  }
  
  G4cout << G4endl;
  
  G4cout << "\n ####muon: StoppingPower for "
         << material->GetName() 
	 << ";\tEnergy cut = " << G4BestUnit (Ecut, "Energy") << G4endl;
	 
  G4cout << "\n Energy \t ionization \t bremsstra \t pair_prod \t" << G4endl;
  
  for (G4double Energy = Emin; Energy <= Emax; Energy += dE) {
    G4cout << "\n " << G4BestUnit (Energy, "Energy")
     << "\t"	 
     << G4BestUnit (ioni->ComputeDEDXPerVolume(material,muon,Energy,Ecut),
                   "Energy/Length")
     << "\t"	 
     << G4BestUnit (brem->ComputeDEDXPerVolume(material,muon,Energy,Ecut),
                   "Energy/Length")
     << "\t"	 
     << G4BestUnit (pair->ComputeDEDXPerVolume(material,muon,Energy,Ecut),
                   "Energy/Length");		   		   		   
  }
  
  G4cout << G4endl;
    
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
                                 
return EXIT_SUCCESS;
}
