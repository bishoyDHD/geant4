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
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:    G4GammaNuclearXS
//
// Authors  V.Ivantchenko, Geant4, 20 October 2020
//          B.Kutsenko, BINP/NSU, 10 August 2021
//
// Modifications:
//

#include "G4GammaNuclearXS.hh"
#include "G4DynamicParticle.hh"
#include "G4ThreeVector.hh"
#include "G4ElementTable.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4ElementData.hh"
#include "G4PhysicsVector.hh"
#include "G4PhysicsLinearVector.hh"
#include "G4PhysicsFreeVector.hh"
#include "G4CrossSectionDataSetRegistry.hh"
#include "G4PhotoNuclearCrossSection.hh"
#include "G4HadronicParameters.hh"
#include "Randomize.hh"
#include "G4SystemOfUnits.hh"
#include "G4Gamma.hh"
#include "G4IsotopeList.hh"
#include "G4AutoLock.hh"

#include <fstream>
#include <sstream>
#include <vector>

G4ElementData* G4GammaNuclearXS::data = nullptr;

G4double G4GammaNuclearXS::coeff[3][3];
G4double G4GammaNuclearXS::xs150[] = {0.0};
const G4int G4GammaNuclearXS::freeVectorException[] = {
4, 6, 7, 8, 27, 39, 45, 65, 67, 69, 73};
G4String G4GammaNuclearXS::gDataDirectory = "";

namespace
{
  // Upper limit of the linear transition between IAEA database and CHIPS model
  const G4double eTransitionBound = 150.*CLHEP::MeV;
  // A limit energy to correct CHIPS parameterisation for light isotopes 
  const G4double ehigh = 10*CLHEP::GeV;
}

G4GammaNuclearXS::G4GammaNuclearXS() 
  : G4VCrossSectionDataSet(Default_Name()), gamma(G4Gamma::Gamma())
{
  verboseLevel = 0;
  if (verboseLevel > 0) {
    G4cout << "G4GammaNuclearXS::G4GammaNuclearXS Initialise for Z < " 
	   << MAXZGAMMAXS << G4endl;
  }
  ggXsection = dynamic_cast<G4PhotoNuclearCrossSection*>
    (G4CrossSectionDataSetRegistry::Instance()->GetCrossSectionDataSet("PhotoNuclearXS"));
  if (ggXsection == nullptr) {
    ggXsection = new G4PhotoNuclearCrossSection();
  }
  SetForAllAtomsAndEnergies(true);

  // full data set is uploaded once
  if (nullptr == data) { 
    data = new G4ElementData(MAXZGAMMAXS);
    data->SetName("gNuclear");
    for (G4int Z=1; Z<MAXZGAMMAXS; ++Z) {
      Initialise(Z);
    }
  }
}

void G4GammaNuclearXS::CrossSectionDescription(std::ostream& outFile) const
{
  outFile << "G4GammaNuclearXS calculates the gamma nuclear\n"
          << "cross-section for GDR energy region on nuclei using "
	  << "data from the high precision\n"
          << "IAEA photonuclear database (2019). Then liniear connection\n"
	  << "implemented with previous CHIPS photonuclear model." << G4endl;
}

G4bool 
G4GammaNuclearXS::IsElementApplicable(const G4DynamicParticle*, 
                                      G4int, const G4Material*)
{
  return true;
}

G4bool G4GammaNuclearXS::IsIsoApplicable(const G4DynamicParticle*,
                                         G4int, G4int,
                                         const G4Element*, const G4Material*)
{
  return true;
}

G4double 
G4GammaNuclearXS::GetElementCrossSection(const G4DynamicParticle* aParticle,
                                         G4int Z, const G4Material*)
{
  return ElementCrossSection(aParticle->GetKineticEnergy(), Z); 
}

G4double
G4GammaNuclearXS::ElementCrossSection(const G4double ekin, const G4int ZZ)
{
  // check cache
  const G4int Z = (ZZ < MAXZGAMMAXS) ? ZZ : MAXZGAMMAXS - 1;
  if(Z == fZ && ekin == fEkin) { return fXS; }
  fZ = Z;
  fEkin = ekin;

  auto pv = data->GetElementData(Z);
  const G4double limCHIPS1 = 25*CLHEP::MeV;
  const G4double limCHIPS2 = 16*CLHEP::MeV;
  if (pv == nullptr || 1 == Z || Z == 40 || Z == 74 ||
     (Z == 24 && ekin >= limCHIPS1) ||
     (Z == 39 && ekin >= limCHIPS1) ||
     (Z == 50 && ekin >= limCHIPS2) ||
     (Z == 64 && ekin >= limCHIPS2)
     ) {
    fXS = ggXsection->ComputeElementXSection(ekin, Z);
    return fXS;
  }
  const G4double emax = pv->GetMaxEnergy();

  // low energy based on data
  if(ekin <= emax) {
    fXS = pv->Value(ekin);
    // high energy CHIPS parameterisation
  } else if(ekin >= eTransitionBound) {
    fXS = ggXsection->ComputeElementXSection(ekin, Z);
    // linear interpolation
  } else {
    const G4double rxs = xs150[Z];
    const G4double lxs = pv->Value(emax);
    fXS = lxs + (ekin - emax)*(rxs - lxs)/(eTransitionBound - emax);
  }

#ifdef G4VERBOSE
  if(verboseLevel > 1) {
    G4cout  << "Z= " << Z << " Ekin(MeV)= " << ekin/CLHEP::MeV 
	    << ",  nElmXS(b)= " << fXS/CLHEP::barn 
	    << G4endl;
  }
#endif
  return fXS;
}

G4double G4GammaNuclearXS::LowEnergyCrossSection(G4double ekin, G4int ZZ)
{    
  const G4int Z = (ZZ < MAXZGAMMAXS) ? ZZ : MAXZGAMMAXS - 1;
  auto pv = data->GetElementData(Z);
  return pv->Value(ekin);
}

G4double G4GammaNuclearXS::GetIsoCrossSection(
         const G4DynamicParticle* aParticle,
	 G4int Z, G4int A,
	 const G4Isotope*, const G4Element*, const G4Material*)
{
  return IsoCrossSection(aParticle->GetKineticEnergy(), Z, A);
}

G4double 
G4GammaNuclearXS::IsoCrossSection(const G4double ekin, const G4int ZZ, const G4int A)
{
  const G4int Z = (ZZ < MAXZGAMMAXS) ? ZZ : MAXZGAMMAXS - 1;
  // cross section per element
  G4double xs = ElementCrossSection(ekin, Z);

  if (Z > 2) {
    xs *= A/aeff[Z];
  } else {
    G4int AA = A - amin[Z];
    if(ekin >= ehigh && AA >=0 && AA <=2) { 
      xs *= coeff[Z][AA];
    } else {
      xs = ggXsection->ComputeIsoXSection(ekin, Z, A);
    }
  }

#ifdef G4VERBOSE
  if(verboseLevel > 1) {
    G4cout  << "G4GammaNuclearXS::IsoXS: Z= " << Z << " A= " << A 
	    << " Ekin(MeV)= " << ekin/CLHEP::MeV 
	    << ", ElmXS(b)= " << xs/CLHEP::barn << G4endl;
  }
#endif
  return xs;
}

const G4Isotope* G4GammaNuclearXS::SelectIsotope(
       const G4Element* anElement, G4double kinEnergy, G4double)
{
  std::size_t nIso = anElement->GetNumberOfIsotopes();
  const G4Isotope* iso = anElement->GetIsotope(0);

  if(1 == nIso) { return iso; }

  const G4double* abundVector = anElement->GetRelativeAbundanceVector();
  G4double sum = 0.0;
  G4int Z = anElement->GetZasInt();

  // use isotope cross sections
  std::size_t nn = temp.size();
  if(nn < nIso) { temp.resize(nIso, 0.); }
  
  for (std::size_t j=0; j<nIso; ++j) {
    //G4cout << j << "-th isotope " << (*isoVector)[j]->GetN() 
    //       <<  " abund= " << abundVector[j] << G4endl;
    sum += abundVector[j]*
      IsoCrossSection(kinEnergy, Z, anElement->GetIsotope((G4int)j)->GetN());
    temp[j] = sum;
  }
  sum *= G4UniformRand();
  for (std::size_t j = 0; j<nIso; ++j) {
    if(temp[j] >= sum) {
      iso = anElement->GetIsotope((G4int)j);
      break;
    }
  }
  return iso;
}

void G4GammaNuclearXS::BuildPhysicsTable(const G4ParticleDefinition& p)
{
  if(verboseLevel > 1) {
    G4cout << "G4GammaNuclearXS::BuildPhysicsTable for " 
	   << p.GetParticleName() << G4endl;
  }
  if(p.GetParticleName() != "gamma") { 
    G4ExceptionDescription ed;
    ed << p.GetParticleName() << " is a wrong particle type -"
       << " only gamma is allowed";
    G4Exception("G4GammaNuclearXS::BuildPhysicsTable(..)","had012",
		FatalException, ed, "");
    return; 
  }

  // prepare isotope selection
  const G4ElementTable* table = G4Element::GetElementTable();
  std::size_t nIso = temp.size();
  for (auto const & elm : *table ) {
    std::size_t n = elm->GetNumberOfIsotopes();
    if (n > nIso) { nIso = n; }
  }
  temp.resize(nIso, 0.0);   
}

const G4String& G4GammaNuclearXS::FindDirectoryPath()
{
  // build the complete string identifying the file with the data set
  if(gDataDirectory.empty()) {
    std::ostringstream ost;
    ost << G4HadronicParameters::Instance()->GetDirPARTICLEXS() << "/gamma/inel";
    gDataDirectory = ost.str();
  }
  return gDataDirectory;
}

void G4GammaNuclearXS::Initialise(G4int Z)
{
  // upload data from file
  std::ostringstream ost;
  ost << FindDirectoryPath() << Z ;
  G4PhysicsVector* v = RetrieveVector(ost, true, Z);
  
  data->InitialiseForElement(Z, v);
  /*
  G4cout << "G4GammaNuclearXS::Initialise for Z= " << Z 
	 << " A= " << Amean << "  Amin= " << amin[Z] 
	 << "  Amax= " << amax[Z] << G4endl;
  */
  xs150[Z] = ggXsection->ComputeElementXSection(eTransitionBound, Z);

  // compute corrections for low Z data
  if(Z <= 2){
    if(amax[Z] > amin[Z]) {
      for(G4int A=amin[Z]; A<=amax[Z]; ++A) {
        G4int AA = A - amin[Z];
	if(AA >= 0 && AA <= 2) {
	  G4double sig1 = ggXsection->ComputeIsoXSection(ehigh, Z, A);
	  G4double sig2 = ggXsection->ComputeElementXSection(ehigh, Z);
	  if(sig2 > 0.) { coeff[Z][AA] = (sig1/sig2); }
	  else { coeff[Z][AA] = 1.; }
	}
      }
    }
  }
}

G4PhysicsVector* 
G4GammaNuclearXS::RetrieveVector(std::ostringstream& ost, G4bool warn, G4int Z)
{
  G4PhysicsVector* v = nullptr;

  std::ifstream filein(ost.str().c_str());
  if (!filein.is_open()) {
    if(warn) {
      G4ExceptionDescription ed;
      ed << "Data file <" << ost.str().c_str()
	 << "> is not opened!";
      G4Exception("G4GammaNuclearXS::RetrieveVector(..)","had014",
		  FatalException, ed, "Check G4PARTICLEXSDATA");
    }
  } else {
    if(verboseLevel > 1) {
      G4cout << "File " << ost.str() 
	     << " is opened by G4GammaNuclearXS" << G4endl;
    }
    // retrieve data from DB
    if(std::find(std::begin(freeVectorException), std::end(freeVectorException), Z)
       == std::end(freeVectorException)) {
      v = new G4PhysicsLinearVector(false);
    } else {
      v = new G4PhysicsFreeVector(false);
    }
    if(!v->Retrieve(filein, true)) {
      G4ExceptionDescription ed;
      ed << "Data file <" << ost.str().c_str()
	 << "> is not retrieved!";
      G4Exception("G4GammaNuclearXS::RetrieveVector(..)","had015",
		  FatalException, ed, "Check G4PARTICLEXSDATA");
    }
  }
  return v;
}

