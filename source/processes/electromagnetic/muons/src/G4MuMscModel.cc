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
// $Id: G4MuMscModel.cc,v 1.8 2007-11-29 10:50:12 vnivanch Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// -------------------------------------------------------------------
//
// GEANT4 Class file
//
//
// File name:   G4MuMscModel
//
// Author:      Laszlo Mu
//
// Creation date: 03.03.2001
//
// Modifications:
//
// 27-03-03 Move model part from G4MultipleScattering80 (V.Ivanchenko)
//

// Class Description:
//
// Implementation of the model of multiple scattering based on
// H.W.Lewis Phys Rev 78 (1950) 526 and others

// -------------------------------------------------------------------
//


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "G4MuMscModel.hh"
#include "Randomize.hh"
#include "G4Electron.hh"
#include "G4LossTableManager.hh"
#include "G4ParticleChangeForMSC.hh"
#include "G4TransportationManager.hh"
#include "G4SafetyHelper.hh"
#include "G4eCoulombScatteringModel.hh"
#include "G4PhysicsTableHelper.hh"
#include "G4ElementVector.hh"
#include "G4ProductionCutsTable.hh"
#include "G4PhysicsLogVector.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;

G4MuMscModel::G4MuMscModel(G4double frange, 
			   G4double thetaMax, 
			   G4double tMax,  
			   const G4String& nam)
  : G4eCoulombScatteringModel(0.0,thetaMax,false,tMax,nam),
    theLambdaTable(0),
    theLambda2Table(0),
    dtrl(0.05),
    facrange(frange),
    thetaLimit(thetaMax),
    numlimit(0.2),
    lowBinEnergy(keV),
    highBinEnergy(PeV),
    nbins(60),
    nwarnings(0),
    nwarnlimit(50),
    currentCouple(0),
    isInitialized(false),
    buildTables(true),
    newrun(true),
    inside(false)
{
  invsqrt12 = 1./sqrt(12.);
  tlimitminfix = 1.e-6*mm;
  theManager = G4LossTableManager::Instance(); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4MuMscModel::~G4MuMscModel()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4MuMscModel::Initialise(const G4ParticleDefinition* p,
			      const G4DataVector& cuts)
{
  SetupParticle(p);
  newrun = true;
  xSection = currentRange = targetZ = ecut = tkin = 0.0;
  // set values of some data members
  if(!isInitialized) {
    isInitialized = true;
    if(p->GetParticleType() == "nucleus") buildTables = false;

    if (pParticleChange)
      fParticleChange = reinterpret_cast<G4ParticleChangeForMSC*>(pParticleChange);
    else
      fParticleChange = new G4ParticleChangeForMSC();

    safetyHelper = G4TransportationManager::GetTransportationManager()
      ->GetSafetyHelper();
    safetyHelper->InitialiseHelper();
  }
  G4eCoulombScatteringModel::Initialise(p, cuts);
  currentCuts = &cuts;
  if(buildTables)
    theLambda2Table = G4PhysicsTableHelper::PreparePhysicsTable(theLambda2Table);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4MuMscModel::BuildTables()
{
  //G4cout << "G4MuMscModel::BuildTables flags newrun= " << newrun 
  //	 << "  buildTables= " << buildTables << G4endl;
  newrun = false;
  if(!buildTables) return;

  // Access to materials
  const G4ProductionCutsTable* theCoupleTable=
        G4ProductionCutsTable::GetProductionCutsTable();
  size_t numOfCouples = theCoupleTable->GetTableSize();
  G4double e, s, cut;

  for(size_t i=0; i<numOfCouples; i++) {

    if (theLambda2Table->GetFlag(i)) {

      // create physics vector and fill it
      DefineMaterial(theCoupleTable->GetMaterialCutsCouple(i));
      cut = (*currentCuts)[currentMaterialIndex];
      G4PhysicsVector* aVector =
	new G4PhysicsLogVector(lowBinEnergy, highBinEnergy, nbins);
      for(G4int j=0; j<nbins; j++) {
        e = aVector->GetLowEdgeEnergy(j);
        s = ComputeLambda2(e, cut);
        //G4cout << j << "  " << currentCouple->GetMaterial()->GetName() 
	//       << "  e(MeV)= " << e << " cut(MeV)= " << cut
	//       << " L2= " << s << G4endl;  
	aVector->PutValue(j, s);
      }
      
      G4PhysicsTableHelper::SetPhysicsVector(theLambda2Table, i, aVector);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4MuMscModel::ComputeCrossSectionPerAtom( 
                             const G4ParticleDefinition* p,
			     G4double kinEnergy,
			     G4double Z, G4double A,
			     G4double cutEnergy, G4double)
{
  if(p == particle && kinEnergy == tkin && Z == targetZ &&
     cutEnergy == ecut) return xSection;
  ecut = cutEnergy;
  xSection = 0.0;
  SetupParticle(p);
  //  G4double ekin = kinEnergy;
  G4double ekin = std::max(keV, kinEnergy);
  SetupTarget(Z, A, ekin);

  G4double tmax = tkin;
  if(p == theElectron) tmax *= 0.5;
  else if(p != thePositron) {
    G4double ratio = electron_mass_c2/mass;
    G4double tau = tkin/mass;
    tmax = 2.0*electron_mass_c2*tau*(tau + 2.)/
      (1.0 + 2.0*ratio*(tau + 1.0) + ratio*ratio); 
  }
  G4double t = std::min(cutEnergy, tmax);
  G4double mom21 = t*(t + 2.0*electron_mass_c2);
  t = tkin - t;
  G4double mom22 = t*(t + 2.0*mass);
  cosTetMaxElec = (mom2 + mom22 - mom21)*0.5/sqrt(mom2*mom22);
  if(cosTetMaxElec < cosTetMaxNuc) cosTetMaxElec = cosTetMaxNuc;

  G4double x, y, x1, x2, x3, x4;
  if(cosTetMaxElec < 1.0) {
    x = (1.0 - cosTetMaxElec)/screenZ;
    if(x < numlimit) y = 0.5*x*x*(1.0 - 1.3333333*x + 1.5*x*x); 
    else             y = log(1.0 + x) - x/(1.0 + x);
    if(y < 0.0) {
      nwarnings++;
      if(nwarnings < nwarnlimit /*&& y < -1.e-10*/) {
	G4cout << "Electron scattering <0 for L1 " << y 
	       << " e(MeV)= " << tkin << " p(MeV/c)= " << sqrt(mom2) 
	       << " Z= " << Z << "  " 
	       << particle->GetParticleName() << G4endl;
	G4cout << " z= " << 1.0-cosTetMaxElec << " screenZ= " << screenZ 
	       << " x= " << x << G4endl;
      }
      y = 0.0;
    }
    xSection += y/Z;
  }
  //  G4cout << "cut= " << ecut << " e= " << tkin << " croosE= " 
  //  << xSection/barn << G4endl;

  if(cosTetMaxNuc < 1.0) {
    x  = 1.0 - cosTetMaxNuc;
    x1 = screenZ*formfactA;
    x2 = 1.0/(1.0 - x1); 
    x3 = x/screenZ;
    x4 = 1.0/formfactA + x;
    if(x3 < numlimit) 
      y = 0.5*x3*x3*x2*x2*x2*(1.0 - 1.333333*x3 + 1.5*x3*x3 
			      - 1.5*x1 + 3.0*x1*x1 + 2.666666*x3*x1);
    else {
      x3 = screenZ + x;
      y  = ((1.0 + x1)*x2*log(x3/(x4*x1)) - x*(1.0/x3 + 1.0/x4))*x2*x2; 
    }
    if(y < 0.0) {
      nwarnings++;
      if(nwarnings < nwarnlimit /*&& y < -1.e-10*/) { 
	G4cout << "Nuclear scattering <0 for L1 " << y 
	       << " e(MeV)= " << tkin << " Z= " << Z << "  " 
	       << particle->GetParticleName() << G4endl;
	G4cout << " formfactA= " << formfactA << " screenZ= " << screenZ 
	       << " x= " << " x1= " << x1 << " x2= " << x2 
	       << " x3= " << x3 << " x4= " << x4 <<G4endl;
      }
      y = 0.0;
    }
    xSection += y; 
  }
  xSection *= (coeff*Z*Z*chargeSquare*invbeta2/mom2); 
  //  G4cout << " croosE= " << xSection/barn << " screenZ= " 
  //	 << screenZ << " formF= " << formfactA << G4endl;
  return xSection; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4MuMscModel::ComputeLambda2(G4double kinEnergy,
				      G4double cutEnergy)
{
  G4double res = 0.0;
  G4double res0= 0.0;
  G4double ekin = std::max(keV, kinEnergy);
  //G4double ekin = kinEnergy;

  const G4Material* mat = currentCouple->GetMaterial();
  const G4ElementVector* theElementVector = mat->GetElementVector();
  const G4double* theAtomNumDensityVector = mat->GetVecNbOfAtomsPerVolume();
  size_t nelm = mat->GetNumberOfElements();

  G4double tmax = ekin;
  mom2 = ekin*(ekin + mass*2.0);
  if(particle == theElectron) tmax *= 0.5;
  else if(particle != thePositron) {
    G4double ratio = electron_mass_c2/mass;
    G4double tau = ekin/mass;
    tmax = 2.0*electron_mass_c2*tau*(tau + 2.)/
      (1.0 + 2.0*ratio*(tau + 1.0) + ratio*ratio); 
  }
  G4double t = std::min(cutEnergy, tmax);
  G4double mom21 = t*(t + 2.0*electron_mass_c2);
  t = tkin - t;
  G4double mom22 = t*(t + 2.0*mass);
  cosTetMaxElec = (mom2 + mom22 - mom21)*0.5/sqrt(mom2*mom22);
  if(cosTetMaxElec < 0.0) cosTetMaxElec = 0.0;

  G4double x, x1, x2, x3, x4, x5, x6, x23, y, y0, fac;

  for (size_t i=0; i<nelm; i++) {
    const G4Element* elm = (*theElementVector)[i];
    G4double Z = elm->GetZ();
    SetupTarget(Z, elm->GetN(), ekin);
    G4double s = 0.0;  
    G4double s0= 0.0;  
    G4double costm = cosTetMaxElec;
    if(costm < cosTetMaxNuc) costm = cosTetMaxNuc;
    if(costm < 1.0) {
      x = 1.0 - costm;
      x1= x/screenZ;
      s0 = x1/(Z*screenZ*(1.0 + x1));
      if(x1 < numlimit) {
	y = x*x1*x1*(1.0 - 1.5*x1 + 1.8*x1*x1)/3.0;
      } else {
	x1 += 1.0;
	y = (x*(1.0 + x1)/x1 - 2.0*screenZ*log(x1));
      }
      if(y < 0.0) {
        nwarnings++;
        if(nwarnings < nwarnlimit /*&& y < -1.e-10*/) {
	  G4cout << "Electron scattering <0 for L2 " << y 
		 << " e(MeV)= " << tkin << " p(MeV/c)= " << sqrt(mom2)
		 << " Z= " << Z << "  " 
		 << particle->GetParticleName() << G4endl;
          G4cout << " z= " << 1.0-costm << " screenZ= " << screenZ 
		 << " x= " << x << " x1= " << x1 << G4endl;
	}
        y = 0.0;
      }
      s = y/Z;
    }
    //  G4cout << "cut= " << cut << " e= " << tkin << " croosE= " 
    //  << xSection/barn << G4endl;

    // limit main integral because of nuclear size effect

    if(cosTetMaxNuc < 1.0) {
      // Compute second moment
      x   = 1.0 - cosTetMaxNuc;
      x1  = screenZ*formfactA;
      x2  = 1.0/(1.0 - x1); 
      x23 = x2*x2*x2;
      x3  = x/screenZ;
      x5  = 1.0 + x3;
      x4  = 1.0 + x*formfactA;
      x6  = 2.0*log(x3/x4);
      y0  = (x3*(1.0/x5 + x1*x1/x4)/(screenZ*x2) - x6)*x23;
      if(x3 < numlimit) {
	y = x*x3*x3*x23*(1.0 - 1.5*x3 - 3.0*x1 + 
			 3.0*x3*x1 + 3.0*x1*x1 + 1.8*x3*x3)/3.0;
      } else {
	y   = (x*(1.0/x3 + 1.0/x4)/x2 - screenZ*x6)*x23; 
      }
      if(y < 0.0) {
        nwarnings++;
        if(nwarnings < nwarnlimit /*&& y < -1.e-10*/) { 
	  G4cout << "Nuclear scattering <0 for L2 " << y 
		 << " e(MeV)= " << tkin << " Z= " << Z << "  " 
		 << particle->GetParticleName() << G4endl;
          G4cout << " formfactA= " << formfactA << " screenZ= " << screenZ 
		 << " x= " << x << " x1= " << x1 << " x2= " << x2 
		 << " x3-1= " << x3-1.0 << " x4-1.0= " << x4-1.0 <<G4endl;
	}
        y = 0.0;
      }
      s0 += y0;
      s  += y;
    }

    fac   = Z*Z*theAtomNumDensityVector[i];
    res  += s*fac; 
    res0 += s0*fac; 
  }
  fac   = coeff*chargeSquare*invbeta2/mom2;
  res0 *= fac;
  res  *= (0.25*fac);
  G4double res1 = 0.5/GetLambda(ekin);
  if(res0 > 0.0) res *= (4.0*res0/(9.0*res1*res1));
  //  G4cout << "e= " << ekin << " sig0= " << res0 << " sig1= " << res1 << " sig2= " << res
  // << " screenZ= " << screenZ << " formF= " << formfactA 
  // << " " << particle->GetParticleName() << " in " << mat->GetName() << G4endl;
  return res; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4MuMscModel::ComputeTruePathLengthLimit(
                             const G4Track& track,
			     G4PhysicsTable* theTable,
			     G4double currentMinimalStep)
{
  G4double tlimit = currentMinimalStep;
  const G4DynamicParticle* dp = track.GetDynamicParticle();

  // initialisation for 1st step  
  if(track.GetCurrentStepNumber() == 1) {
    inside = false;
    SetupParticle(dp->GetDefinition());
    theLambdaTable = theTable;
    if(newrun) BuildTables();
  }

  // initialisation for each step  
  preKinEnergy = dp->GetKineticEnergy();
  DefineMaterial(track.GetMaterialCutsCouple());
  lambda0 = GetLambda(preKinEnergy);
  currentRange = 
    theManager->GetRangeFromRestricteDEDX(particle,preKinEnergy,currentCouple);

  // extra check for abnormal situation
  // this check needed to run MSC with eIoni and eBrem inactivated
  if(tlimit > currentRange) tlimit = currentRange;

  // stop here if small range particle
  if(inside) return tlimit;   

  // pre step
  G4StepPoint* sp = track.GetStep()->GetPreStepPoint();
  G4StepStatus stepStatus = sp->GetStepStatus();
  G4double presafety = sp->GetSafety();

  // compute presafety again if presafety <= 0 and no boundary
  // i.e. when it is needed for optimization purposes
  if(stepStatus != fGeomBoundary && presafety < tlimitminfix) 
    presafety = safetyHelper->ComputeSafety(sp->GetPosition()); 

  //  G4cout << "G4MuMscModel::ComputeTruePathLengthLimit tlimit= " 
  //	 <<tlimit<<" safety= " << presafety
  //	 << " range= " <<currentRange<<G4endl;

  // far from geometry boundary
  if(currentRange < presafety) {
    inside = true;  
    
    // limit mean scattering angle
  } else {
    G4double rcut = currentCouple->GetProductionCuts()->GetProductionCut(1);
    G4double rlimit = std::max(facrange*lambda0,sqrt(3.4*rcut*lambda0));
    tlimit = std::min(rlimit, tlimit);
  }
  /*
  G4cout << particle->GetParticleName() << " e= " << preKinEnergy
	 << " L0= " << lambda0 << " R= " << currentRange
	 << "tlimit= " << tlimit  
  	 << " currentMinimalStep= " << currentMinimalStep << G4endl;
  */
  return tlimit;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4MuMscModel::ComputeGeomPathLength(G4double truelength)
{
  tPathLength  = truelength;
  zPathLength  = tPathLength;

  G4double tau = tPathLength/lambda0;
  lambdaeff    = lambda0;
  //G4cout << "ComputeGeomPathLength: tLength= " << tPathLength
  //	 << " lambda0= " << lambda0 << " tau= " << tau << G4endl; 
  // small step
  if(tau < numlimit) {
    par1 = -1. ;  
    par2 = par3 = 0. ;  
    zPathLength *= (1.0 - 0.5*tau + tau*tau/6.0);

    // medium step
  } else if(tPathLength < currentRange*dtrl) {
    zPathLength = lambda0*(1.0 - exp(-tau));

  } else if(tkin < mass) {

    par1 = 1./currentRange;
    par2 = 1./(par1*lambda0);
    par3 = 1.+ par2;
    lambdaeff = 1.0/(par1*par3);
    G4double x = tPathLength/currentRange;
    G4double x1;
    if(x < numlimit) x1 = x*(1.0  - 0.5*x + x*x/3.0);
    else             x1 = log(1.0 - x); 
    zPathLength = lambdaeff*(1.-exp(par3*x1));

  } else {

    G4double T1 = theManager->GetEnergy(particle,
					currentRange-tPathLength,
					currentCouple);
    G4double lambda1 = GetLambda(T1);

    par1 = (lambda0-lambda1)/(lambda0*tPathLength) ;
    par2 = 1./(par1*lambda0) ;
    par3 = 1.+ par2 ;
    lambdaeff = 1.0/(par1*par3);
    zPathLength = lambdaeff*(1.-exp(par3*log(lambda1/lambda0)));
  }

  //  if(zPathLength > lambda0) zPathLength = lambda0;
  if(zPathLength > tPathLength) zPathLength = tPathLength;

  return zPathLength;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double G4MuMscModel::ComputeTrueStepLength(G4double geomStepLength)
{
  // step defined other than transportation 
  if(geomStepLength == zPathLength) return tPathLength;

  tPathLength  = geomStepLength;
  zPathLength  = geomStepLength;
  G4double tau = geomStepLength/lambda0;
  if(tau < numlimit) {
    tPathLength *= (1.0 + 0.5*tau - tau*tau/3.0); 
  
  } else if(par1 <  0.) {
    tPathLength = -lambda0*log(1.0 - tau); 

  } else {
    G4double x = par1*par3*geomStepLength;
    if(x < numlimit) 
      tPathLength = (1.- exp(- x*(1.- 0.5*x + x*x/3.0)/par3))/par1 ;
    else if (x < 1.0)  
      tPathLength = (1.-exp(log(1.- x)/par3))/par1;
    else 
      tPathLength = currentRange;
  }
  if(tPathLength < geomStepLength) tPathLength = geomStepLength;

  return tPathLength;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4MuMscModel::SampleScattering(const G4DynamicParticle* dynParticle,
				    G4double safety)
{
  G4double kinEnergy = dynParticle->GetKineticEnergy();
  if(kinEnergy == 0.0 || tPathLength == 0.0) return;
  G4double x1  = 0.5*tPathLength/lambdaeff;
  //G4double x1  = 0.5*tPathLength/lambda0;
  
  /*
  G4cout << "G4MuMscModel::SampleScattering t(mm)= " << tPathLength
	 << " 1/lambdaeff= " << 1.0/lambdaeff 
	 << " matIdx= " << currentMaterialIndex << G4endl;
  */
  /*
  G4double z1 = x1;
  G4double x2 = tPathLength*GetLambda2(0.5*(preKinEnergy + kinEnergy)); 
  G4double x3 = 0.0;
  G4double grej = 1.0;
  if(x2 < 1.0) {
    z1 = 0.75*x1*(1.0 + sqrt(1.0 - x2));
    x3 = (x1 - z1)/(z1*z1);
    grej += x3;
  } 
  */
  /*
  G4double y1  = 1.0 - x1;
  G4double x2  = tPathLength*GetLambda2(0.5*(preKinEnergy + kinEnergy)); 
  //G4double x2 = tPathLength*GetLambda2(preKinEnergy); 
  G4double x3 = x2/(x1*y1);
  if(x3 <= 0.0 || x3 >= 0.33) {
    nwarnings++;
    if(nwarnings < nwarnlimit) 
      G4cout << "G4MuMscModel::SampleScattering: ePre(MeV)= " << preKinEnergy/MeV
	     << " ePost(MeV)= " << kinEnergy/MeV
	     << " <x>= " << x1 << " x2= " << x2
	     << " x3= " << x3
	     << G4endl;
    x3 = std::min(1.0/y1,0.16666);
  }
  G4double x4 = 0.25*(3.0*x3 + sqrt(x3*(x3 + 8.0)))/(1.0 - x3);
  //  G4cout << "x1= " << x1 << " x3= " << x3 << " x4= " << x4 << G4endl;
  */
  G4double x, z;  
  x  = G4UniformRand();
  z = -x1*log(x);

  //  G4cout << " z1= " << z1 << " grej= " << grej << " x3= " << x3 << " x1= " << x1 << " x2= " << x2 << G4endl;
  /*
  do {
    z = -z1*log(G4UniformRand());
    x  = G4UniformRand(); 
  } while (grej*x > 1.0 + x3*z);
  */

  /*
  G4double z1;  
  if(x <= y1) {
    z1 = x4*log(x/y1);
    if(z1 < numlimit) z = x1*(1.0 + z1 + 0.5*z1*z1);
    else z = x1*exp(z1);

  } else {
    z1 = x4*log((1.0 - x)/x1);
    if(z1 < numlimit) z = x1 - z1 + x1*z1 - 0.5*z1*z1*y1;
    else z = 1.0 - y1*exp(z1);
  }
  */

  G4double cost = 1.0 - 2.0*z;
  if(cost < -1.0) cost = -1.0;
  else if(cost > 1.0) cost = 1.0;
  G4double sint = sqrt((1.0 - cost)*(1.0 + cost));

  G4double phi  = twopi*G4UniformRand();

  G4double dirx = sint*cos(phi);
  G4double diry = sint*sin(phi);
  /*
  G4cout << "G4MuMscModel: step(mm)= " << tPathLength/mm
	 << " e= " << kinEnergy << " Epre= " << preKinEnergy
  	 << " lambdaeff= " << lambdaeff
	 << " sint= " << sint
  	 << " x1= " << x1 << " sqrt(x2)= " << sqrt(x2) << " x4= " << x4 << G4endl;
  */
  G4ThreeVector oldDirection = dynParticle->GetMomentumDirection();
  G4ThreeVector newDirection(dirx,diry,cost);
  newDirection.rotateUz(oldDirection);
  fParticleChange->ProposeMomentumDirection(newDirection);

  if (latDisplasment && safety > tlimitminfix) {
    G4double rms= sqrt(2.0*x1);
    G4double rx = zPathLength*(0.5*dirx + invsqrt12*G4RandGauss::shoot(0.0,rms));
    G4double ry = zPathLength*(0.5*diry + invsqrt12*G4RandGauss::shoot(0.0,rms));
    G4double r  = sqrt(rx*rx + ry*ry);
    /*
    G4cout << " r(mm)= " << r << " safety= " << safety
           << " trueStep(mm)= " << tPathLength
           << " geomStep(mm)= " << zPathLength
           << G4endl;
    */
    G4ThreeVector latDirection(rx,ry,0.0);
    latDirection.rotateUz(oldDirection);

    G4ThreeVector Position = *(fParticleChange->GetProposedPosition());
    G4double fac = 1.;
    if(r >  safety) {
      //  ******* so safety is computed at boundary too ************
      G4double newsafety = safetyHelper->ComputeSafety(Position);
      if(r > newsafety)
	fac = newsafety/r ;
    }  

    if(fac > 0.) {
      // compute new endpoint of the Step
      G4ThreeVector newPosition = Position+fac*latDirection;

      // definitely not on boundary
      if(1. == fac) {
	safetyHelper->ReLocateWithinVolume(newPosition);
	    
      } else {
	// check safety after displacement
	G4double postsafety = safetyHelper->ComputeSafety(newPosition);

	// displacement to boundary
	if(postsafety <= 0.0) {
	  safetyHelper->Locate(newPosition, newDirection);

	  // not on the boundary
	} else { 
	      safetyHelper->ReLocateWithinVolume(newPosition);
	}
      }
      fParticleChange->ProposePosition(newPosition);
    } 
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void G4MuMscModel::SampleSecondaries(std::vector<G4DynamicParticle*>*,
                                     const G4MaterialCutsCouple*,
				     const G4DynamicParticle*,
				     G4double,
				     G4double)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
