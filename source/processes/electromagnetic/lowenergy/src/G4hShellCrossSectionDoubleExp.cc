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
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:  G4hShellCrossSectionDoubleExp.cc   
//
// Author:     Simona Saliceti (simona.saliceti@ge.infn.it)
// 
// History:
// -----------
// From 23 Oct 2001 A. Mantero G4hShellCrossSection
// 30/03/2004 Simona Saliceti 1st implementation
// -------------------------------------------------------------------
// Class Description: 
// Empiric Model for shell cross sections in proton ionisation
// -------------------------------------------------------------------
// $Id: G4hShellCrossSectionDoubleExp.cc,v 1.1 2004-09-29 08:34:37 saliceti Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#include "globals.hh"
#include <vector>
#include "G4hShellCrossSectionDoubleExp.hh"
#include "G4AtomicTransitionManager.hh"
#include "G4Electron.hh"
#include "G4hShellCrossSectionDoubleExpData.hh"
#include "G4Proton.hh"
#include "G4ParticleDefinition.hh"

G4hShellCrossSectionDoubleExp::G4hShellCrossSectionDoubleExp()
{
  kShellData = new G4hShellCrossSectionDoubleExpData();
}

G4hShellCrossSectionDoubleExp::~G4hShellCrossSectionDoubleExp()
{ }

std::vector<G4double> G4hShellCrossSectionDoubleExp::GetCrossSection(G4int Z,
							             G4double incidentEnergy,
							             G4double mass,
							             G4double deltaEnergy,
							             G4bool testFlag) const
{
  mass = 0.0;
  deltaEnergy = 0.0;

  std::vector<G4double> aCrossSection; 
 
  // Fill the vector of cross sections with the value just calculated
  aCrossSection.push_back(GetCrossSectionDoubleExp(Z,incidentEnergy));
 
  if (testFlag) 
    {
      G4cout <<"Element: " <<Z<<" Particle Energy: "<<incidentEnergy/MeV<<" MeV" <<G4endl;
      G4cout <<"Cross Section: "<<aCrossSection[0]/barn<<" barns"<< G4endl;
    }
  return aCrossSection;
}

//This function calculated the cross section with the Empiric model
G4double G4hShellCrossSectionDoubleExp::GetCrossSectionDoubleExp(G4int Z, 
						     G4double incidentEnergy) const
{
  // Vector that stores the calculated cross-sections for each shell:
  G4double  crossSectionsInBarn = 0.0;
  G4double  crossSections = 0.0;

  std::vector<std::vector<G4double>*> parVec = kShellData->GetParam(Z);
  std::vector<G4double>*::energyVec = parVec[0];
  std::vector<G4double>*::par1Vec  = parVec[1];
  std::vector<G4double>*::par2Vec  = parVec[2];
  
  std::vector<G4double>::iterator i = (*par1Vec).begin();
  
  G4double a1 = *i;
  G4double b1 = *(i+1);
  G4double c1 = *(i+2);

  std::vector<G4double>::iterator j = (*par2Vec).begin();

  G4double a2 = *j;
  G4double b2 = *(j+1);
  G4double c2 = *(j+2);
  G4double d2 = *(j+3);
  G4double e2 = *(j+4);

  G4double incidentEnergyInMeV = incidentEnergy/MeV;
  
  // energy is the energy to split the file in low and high energy
  std::vector<G4double>::iterator l = (*energyVec).begin();
  G4double energy = *l;
  energy = energy/MeV;

 if(incidentEnergyInMeV <= energy)
    {
      if(Z<26)
	{
	 crossSectionsInBarn = (std::pow(incidentEnergyInMeV,(a1)))*exp((b1)-((c1)*(incidentEnergyInMeV))); 	 
	}          
      else if(Z>=26)
	{
	  crossSectionsInBarn = a1*(std::pow(b1,(1./incidentEnergyInMeV)))*(std::pow(incidentEnergyInMeV,c1));
	}  
    }   
 else if(incidentEnergyInMeV > energy)
   {
      if(Z<26 || (Z>=36 && Z<=65))
	{
	  crossSectionsInBarn = (a2)*(std::pow((b2),(1./incidentEnergyInMeV)))*(std::pow(incidentEnergyInMeV,(c2)));
	}
      else if(Z>=26 && Z<36)
	{
	  crossSectionsInBarn = a2+b2*(log(incidentEnergyInMeV))+c2*(std::pow(log(incidentEnergyInMeV),2))+d2*(std::pow(log(incidentEnergyInMeV),3));
 	} 
      else if(Z>65 && Z<=92)
	{
	  crossSectionsInBarn = a2+b2*(log(incidentEnergyInMeV))+c2*(std::pow(log(incidentEnergyInMeV),2))+d2*(std::pow(log(incidentEnergyInMeV),3))+e2*(std::pow(log(incidentEnergyInMeV),4));
 	}
    }

//   if(Z<26 && incidentEnergyInMeV <= energy)
//     {
// 	 crossSectionsInBarn = (std::pow(incidentEnergyInMeV,(a1)))*exp((b1)-((c1)*incidentEnergyInMeV)); 	 
//     }          
//   else if(Z>=26 && incidentEnergyInMeV <= energy)
//     {
//       crossSectionsInBarn = a1*(std::pow(b1,(1./incidentEnergyInMeV)))*(std::pow(incidentEnergyInMeV,c1));
//     }     
//   else if((Z<26 || (36<=Z && Z<=65)) && incidentEnergyInMeV > energy)
//     {
//       crossSectionsInBarn = (a2)*(std::pow((b2),(1./incidentEnergyInMeV)))*(std::pow(incidentEnergyInMeV,(c2)));
//     }
//   else if(Z>=26 && Z<=35 && incidentEnergyInMeV > energy)
//     {
//       crossSectionsInBarn = a2+b2*(log(incidentEnergyInMeV))+c2*(std::pow(log(incidentEnergyInMeV),2))+d2*(std::pow(log(incidentEnergyInMeV),3));
//     } 
//   else if(Z>=67 && Z<=92 && incidentEnergyInMeV > energy)
//     {
//       crossSectionsInBarn = a2+b2*(log(incidentEnergyInMeV))+c2*(std::pow(log(incidentEnergyInMeV),2))+d2*(std::pow(log(incidentEnergyInMeV),3))+e2*(std::pow(log(incidentEnergyInMeV),4));
//     }

  crossSections = crossSectionsInBarn*barn;
  return crossSections;
}
  
// This function gives the atomic cross section of k shell only
void G4hShellCrossSectionDoubleExp::SetTotalCS(G4double value)
{
  atomTotalCrossSection = value;
}

//A new implementation of Probability to calculate the cross section probability for k shell only
 std::vector<G4double> G4hShellCrossSectionDoubleExp::Probabilities(
							      G4int Z, 
							      G4double incidentEnergy, 
							      G4double hMass, 
							      G4double deltaEnergy
							      ) const
{  
  hMass = 0.0;
  deltaEnergy = 0.0;
  
  std::vector<G4double> kProbability;
  kProbability.push_back(GetCrossSectionDoubleExp(Z,incidentEnergy)/atomTotalCrossSection);
  kProbability.push_back(1 - kProbability[1]);

  return kProbability;
}
  
  

