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
// V. Ivanchenko September 2023 
//               
// G4CrossSectionHP is a generic class implementing 
// cross sections for neutrons, protons and light ions
// It is an alternative to code developed by J.P. Wellisch & T.Koi
//

#include <fstream>
#include <sstream>
#include <thread>

#include "G4CrossSectionHP.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4ElementDataRegistry.hh"
#include "G4ElementTable.hh"
#include "G4IsotopeList.hh"
#include "G4HadronicParameters.hh"
#include "G4ParticleHPManager.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4Pow.hh"
#include "G4Log.hh"
#include "Randomize.hh"
#include "G4RandomDirection.hh"
#include "G4SystemOfUnits.hh"
#include "G4Neutron.hh"
#include "G4NucleiProperties.hh"
#include "G4AutoLock.hh"

namespace
{
  G4Mutex theHPXS = G4MUTEX_INITIALIZER;
}

G4CrossSectionHP::G4CrossSectionHP(const G4ParticleDefinition* p,
                                   const G4String& nameData,
                                   const G4String& nameDir, G4double emaxHP,
                                   G4int zmin, G4int zmax)
  : G4VCrossSectionDataSet(nameData),
    fParticle(p),
    fNeutron(G4Neutron::Neutron()),
    fManagerHP(G4ParticleHPManager::GetInstance()),
    emax(emaxHP),
    emaxT(fManagerHP->GetMaxEnergyDoppler()),
    elimit(1.0e-11*CLHEP::eV),
    logElimit(G4Log(elimit)),
    minZ(zmin),
    maxZ(zmax),
    fDataName(nameData),
    fDataDirectory(nameDir)
{
  //  verboseLevel = 1;
  if (verboseLevel > 1) {
    G4cout << "G4CrossSectionHP::G4CrossSectionHP: Initialise for "
	   << fDataName << "  " << minZ << " < Z < " << maxZ
	   << "  EmaxT(MeV)=" << emaxT << G4endl;
    G4cout << "Data directory: " << fDataDirectory << G4endl; 
  }
  auto ptr = G4ElementDataRegistry::Instance();
  auto data = ptr->GetElementDataByName(fDataName);
  if (nullptr == data) { 
    data = new G4ElementData(maxZ - minZ + 1);
    data->SetName(fDataName);
  }
  fData = data;
}

G4bool G4CrossSectionHP::IsIsoApplicable(const G4DynamicParticle* dp,
					 G4int, G4int,
                                         const G4Element*,
					 const G4Material*)
{
  return (dp->GetKineticEnergy() <= emax);
}

G4double G4CrossSectionHP::GetIsoCrossSection(const G4DynamicParticle* dp,
                                              G4int Z, G4int A,
                                              const G4Isotope*,
					      const G4Element*,
					      const G4Material* mat)
{
  G4double ekin = dp->GetKineticEnergy();
  G4double loge = dp->GetLogKineticEnergy();
  if (ekin < elimit) {
    ekin = elimit;
    loge = logElimit;
  }
  if (mat != fCurrentMat) { PrepareCache(mat); }

  return IsoCrossSection(ekin, loge, Z, A, mat->GetTemperature());
}

G4double
G4CrossSectionHP::ComputeIsoCrossSection(G4double e, G4double le,
                                         const G4ParticleDefinition*,
                                         G4int Z, G4int A,
					 const G4Isotope*,
					 const G4Element*,
					 const G4Material* mat)
{
  G4double ekin = e;
  G4double loge = le;
  if (ekin < elimit) {
    ekin = elimit;
    loge = logElimit;
  }
  if (mat != fCurrentMat) { PrepareCache(mat); }

  return IsoCrossSection(ekin, loge, Z, A, mat->GetTemperature()); 
}

G4double G4CrossSectionHP::IsoCrossSection(const G4double ekin,
                                           const G4double logek,
					   const G4int Z, const G4int A,
                                           const G4double T)
{
  // G4cout << "G4CrossSectionHP::IsoCrossSection Z=" << Z << " A=" << A
  // << " E(MeV)=" << ekin/MeV << " T=" << T << "  " << GetName() << G4endl;
  G4double xs = 0.0;
  if (ekin > emax || Z > maxZ || Z < minZ || ekin < elimit) { return xs; }

  auto pv0 = fData->GetElementData(Z - minZ);
  if (nullptr == pv0) {
    Initialise(Z);
    pv0 = fData->GetElementData(Z - minZ);
  }

  // if there is no element data then no iso data
  if (nullptr == pv0) { return xs; }

  const auto pv = fData->GetComponentDataByID(Z - minZ, A);
  if (nullptr == pv) { return xs; }

  // no Doppler broading
  // G4double factT = T/CLHEP::STP_Temperature;
  if (ekin >= emaxT*T/CLHEP::STP_Temperature || fManagerHP->GetNeglectDoppler()) {
    xs = pv->LogFreeVectorValue(ekin, logek);

  } else {

    // Doppler broading
    G4double e0 = CLHEP::k_Boltzmann*T;
    G4double mass = fParticle->GetPDGMass();
    G4double massTarget = G4NucleiProperties::GetNuclearMass(A, Z);
    G4double sig = std::sqrt(2.0*e0/(3.0*massTarget));

    // projectile
    G4LorentzVector lv(0., 0., std::sqrt(ekin*(ekin + 2*mass)), mass + ekin);

    // limits of integration
    const G4double lim = 1.01;
    const G4int nmin = 3;
    G4int ii = 0;
    const G4int nn = 20;
    G4double xs2 = 0.0;

    for (G4int i=0; i<nn; ++i) {
      G4double vx = G4RandGauss::shoot(0., sig);
      G4double vy = G4RandGauss::shoot(0., sig);
      G4double vz = G4RandGauss::shoot(0., sig);
      fLV.set(massTarget*vx, massTarget*vy, massTarget*vz, massTarget*(1.0 + 0.5*(vx*vx + vy*vy + vz*vz)));
      fBoost = fLV.boostVector();
      fLV = lv.boost(-fBoost);
      if (fLV.pz() <= 0.0) { continue; }
      ++ii;
      G4double e = fLV.e() - mass;
      G4double y = pv->Value(e, index);
      xs += y;
      xs2 += y*y;
      if (ii >= nmin && ii*xs2 <= lim*xs*xs) { break; } 
    }
    if (ii > 0) { xs /= (G4double)(ii); }
  }
#ifdef G4VERBOSE
  if (verboseLevel > 1) {
    G4cout << "G4CrossSectionHP::IsoXS " << fDataName 
	   << "  Z= " << Z << "  A= " << A 
	   << " Ekin(MeV)= " << ekin/MeV << "  xs(b)= " << xs/barn 
           << "  size=" << fZA.size() << G4endl;
  }
#endif

  // save cross section into struct
  for (std::size_t i=0; i<fZA.size(); ++i) {
    if (Z == fZA[i].first && A == fZA[i].second) {
      fIsoXS[i] = xs;
      break;
    }
  }
  return xs;
}

const G4Isotope* G4CrossSectionHP::SelectIsotope(const G4Element* elm,
                                                 G4double, G4double)
{
  std::size_t nIso = elm->GetNumberOfIsotopes();
  const G4Isotope* iso = elm->GetIsotope(0);

  //G4cout << "SelectIsotope NIso= " << nIso << G4endl;
  if(1 == nIso) { return iso; }

  // more than 1 isotope
  G4int Z = elm->GetZasInt();
  if (Z >= minZ && Z <= maxZ && nullptr == fData->GetElementData(Z - minZ)) {
    Initialise(Z);
  }
  
  const G4double* abundVector = elm->GetRelativeAbundanceVector();
  G4double q = G4UniformRand();
  G4double sum = 0.0;

  // is there cached isotope wise cross section?
  std::size_t j;
  if (Z < minZ || Z > maxZ || !CheckCache(Z) ||
      0 == fData->GetNumberOfComponents(Z - minZ)) {
    for (j = 0; j<nIso; ++j) {
      sum += abundVector[j];
      if(q <= sum) {
	iso = elm->GetIsotope((G4int)j);
	break;
      }
    }
    return iso;
  } 
  std::size_t nn = fTemp.size();
  if (nn < nIso) { fTemp.resize(nIso, 0.); }

  // reuse cache
  for (j=0; j<nIso; ++j) {
    sum += abundVector[j]*
      GetCrossSection(Z - minZ, elm->GetIsotope((G4int)j)->GetN());
    fTemp[j] = sum;
  }
  sum *= q;
  for (j = 0; j<nIso; ++j) {
    if (fTemp[j] >= sum) {
      iso = elm->GetIsotope((G4int)j);
      break;
    }
  }
  return iso;
}

void G4CrossSectionHP::BuildPhysicsTable(const G4ParticleDefinition& p)
{
  if (verboseLevel > 1) {
    G4cout << "G4CrossSectionHP::BuildPhysicsTable for "
	   << p.GetParticleName() << " and " << fDataName << G4endl;
  }

  // it is possible re-initialisation for the second run
  const G4ElementTable* table = G4Element::GetElementTable();

  // Access to elements
  for ( auto const & elm : *table ) {
    G4int Z = elm->GetZasInt();
    if (Z >= minZ && Z <= maxZ && nullptr == fData->GetElementData(Z - minZ)) { 
      Initialise(Z);
    }
  }

  // prepare isotope selection
  std::size_t nmax = 0;
  std::size_t imax = 0;
  for ( auto const & mat : *G4Material::GetMaterialTable() ) {
    std::size_t n = 0; 
    for ( auto const & elm : *mat->GetElementVector() ) {
      std::size_t niso = elm->GetNumberOfIsotopes();
      n += niso;
      if(niso > imax) { imax = niso; }
    }
    if (n > nmax) { nmax = n; }
  }
  fTemp.resize(imax, 0.0);
  fZA.clear();
  fZA.reserve(nmax);
  fIsoXS.resize(nmax, 0.0);
}

void G4CrossSectionHP::DumpPhysicsTable(const G4ParticleDefinition&)
{
  if (fManagerHP->GetVerboseLevel() == 0 || fPrinted)
    return;

  //
  // Dump element based cross section
  // range 10e-5 eV to 20 MeV
  // 10 point per decade
  // in barn
  //
  fPrinted = true;
  G4cout << G4endl;
  G4cout << "HP Cross Section " << fDataName << " for "
	 << fParticle->GetParticleName() << G4endl;
  G4cout << "(Pointwise cross-section at 0 Kelvin.)" << G4endl;
  G4cout << G4endl;
  G4cout << "Name of Element" << G4endl;
  G4cout << "Energy[eV]  XS[barn]" << G4endl;
  G4cout << G4endl;

  const G4ElementTable* table = G4Element::GetElementTable();
  for ( auto const & elm : *table ) {
    G4int Z = elm->GetZasInt();
    if (Z >= minZ && Z <= maxZ && nullptr != fData->GetElementData(Z - minZ)) {
      G4cout << "---------------------------------------------------" << G4endl;
      G4cout << elm->GetName() << G4endl;
      std::size_t n = fData->GetNumberOfComponents(Z);
      for (size_t i=0; i<n; ++i) {
	G4cout << "##  Z=" << Z << "  A=" << fData->GetComponentID(Z - minZ, i);
	G4cout << *(fData->GetComponentDataByIndex(Z - minZ, i)) << G4endl;
      }
    }
  }
}

void G4CrossSectionHP::PrepareCache(const G4Material* mat)
{
  fCurrentMat = mat;
  fZA.clear();
  for ( auto const & elm : *(mat->GetElementVector()) ) {
    G4int Z = elm->GetZasInt();
    for ( auto const & iso : *(elm->GetIsotopeVector()) ) {
      G4int A = iso->GetN();
      fZA.emplace_back(Z, A);
    }
  }
  fIsoXS.resize(fZA.size(), 0.0);
}

void G4CrossSectionHP::Initialise(const G4int Z)
{
  if (fManagerHP->GetVerboseLevel() > 1) {
    G4cout << " G4CrossSectionHP::Initialise: Z=" << Z 
           << " for " << fDataName
	   << " minZ=" << minZ << " maxZ=" << maxZ << G4endl;
  }
  if (Z < minZ || Z > maxZ || nullptr != fData->GetElementData(Z - minZ)) { 
    return;
  }
  G4AutoLock l(&theHPXS);
  if (nullptr != fData->GetElementData(Z - minZ)) { 
    l.unlock();
    return;
  }

  // add empty vector to avoid double initialisation
  fData->InitialiseForElement(Z - minZ, new G4PhysicsVector());

  G4String tnam = "temp";
  G4bool noComp = true;
  for (G4int A=amin[Z]; A<=amax[Z]; ++A) {
    std::ostringstream ost;
    ost << fDataDirectory;
    // first check special cases
    if (6 == Z && 12 == A && fParticle == fNeutron) {
      ost << Z << "_nat_" << elementName[Z];
    } else if (18 == Z && 40 != A) {
      continue;
    } else if (27 == Z && 62 == A) {
      ost << Z << "_62m1_" << elementName[Z];
    } else if (47 == Z && 106 == A) {
      ost << Z << "_106m1_" << elementName[Z];
    } else if (48 == Z && 115 == A) {
      ost << Z << "_115m1_" << elementName[Z];
    } else if (52 == Z && 127 == A) {
      ost << Z << "_127m1_" << elementName[Z];
    } else if (52 == Z && 129 == A) {
      ost << Z << "_129m1_" << elementName[Z];
    } else if (52 == Z && 131 == A) {
      ost << Z << "_131m1_" << elementName[Z];
    } else if (61 == Z && 145 == A) {
      ost << Z << "_147_" << elementName[Z];
    } else if (67 == Z && 166 == A) {
      ost << Z << "_166m1_" << elementName[Z];
    } else if (73 == Z && 180 == A) {
      ost << Z << "_180m1_" << elementName[Z];
    } else if ((Z == 85 && A == 210) || (Z == 86 && A == 222) || (Z == 87 && A == 223)) {
      ost << "84_209_" << elementName[84];
    } else {
      // the main file name
      ost << Z << "_" << A << "_" << elementName[Z];
    }
    std::istringstream theXSData(tnam, std::ios::in);
    fManagerHP->GetDataStream(ost.str().c_str(), theXSData);
    if (theXSData) {
      G4int i1, i2, n;
      theXSData >> i1 >> i2 >> n;
      if (fManagerHP->GetVerboseLevel() > 1) {
	G4cout << "## G4CrossSectionHP::Initialise for Z=" << Z
	       << " A=" << A << " Npoints=" << n << G4endl;
      }
      G4double x, y;
      G4PhysicsFreeVector* v = new G4PhysicsFreeVector(n);
      for (G4int i=0; i<n; ++i) {
	theXSData >> x >> y;
	x *= CLHEP::eV;
	y *= CLHEP::barn;
	//G4cout << "  e=" << x << "  xs=" << y << G4endl;
	v->PutValues((std::size_t)i, x, y);
      }
      v->EnableLogBinSearch(binSearch);
      if (noComp) {
	G4int nmax = amax[Z] - A + 1;
	fData->InitialiseForComponent(Z - minZ, nmax);
	noComp = false;
      }
      fData->AddComponent(Z - minZ, A, v);
    }
  }   
  if (noComp) { fData->InitialiseForComponent(Z - minZ, 0); }
  l.unlock();
}

