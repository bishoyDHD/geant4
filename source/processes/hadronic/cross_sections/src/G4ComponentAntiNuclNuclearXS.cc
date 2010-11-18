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
//  Calculation of the total, elastic and inelastic cross-sections
//  of anti-nucleon and anti-nucleus interactions with nuclei
//  based on Glauber approach and V. Grishine formulaes for
//  interpolations (ref. V.M.Grichine, Eur.Phys.J., C62(2009) 399;
//  NIM, B267 (2009) 2460) and our parametrization of hadron-nucleon
//  cross-sections
// 
// 
//   Created by A.Galoyan and V. Uzhinsky, 18.11.2010


#include "G4ComponentAntiNuclNuclearXS.hh"

#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"

///////////////////////////////////////////////////////////////////////////////


G4ComponentAntiNuclNuclearXS::G4ComponentAntiNuclNuclearXS() 
: fUpperLimit( 10000 * GeV ),
  fLowerLimit( 10 * MeV )
{
  theAProton       = G4AntiProton::AntiProton();
  theANeutron      = G4AntiNeutron::AntiNeutron();
  theADeuteron     = G4AntiDeuteron::AntiDeuteron();
  theATriton       = G4AntiTriton::AntiTriton();
  theAAlpha        = G4AntiAlpha::AntiAlpha();
  theAHe3          = G4AntiHe3::AntiHe3();

 Mn       = 0.93827231;           // GeV
 b0       = 11.92;                // GeV^(-2)
 b2       = 0.3036;               // GeV^(-2)
 SqrtS0   = 20.74;                // GeV
 S0       = 33.0625;              // GeV^2

}

///////////////////////////////////////////////////////////////////////////////////////
//
//

G4ComponentAntiNuclNuclearXS::~G4ComponentAntiNuclNuclearXS()
{
}

////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
//
// Calculation of total CrossSection of Anti-Nucleus - Nucleus 


G4double G4ComponentAntiNuclNuclearXS:: ComputeTotalCrossSection
(const G4DynamicParticle* aParticle, G4int Z, G4double A)
{
  G4double xsection,   sigmaTotal, sigmaElastic;

 const G4ParticleDefinition* theParticle = aParticle->GetDefinition();

    sigmaTotal        = GetAntiHadronNucleonTotCrSc(aParticle);
    sigmaElastic      = GetAntiHadronNucleonElCrSc(aParticle);

// calculation of squared radius of  NN-collision
   fRadiusNN2=sigmaTotal*sigmaTotal*0.1/(8.*sigmaElastic*pi) ;  //fm^2   

// calculation of effective nuclear radius for Pbar and Nbar interactions (can be changed)

  if ( (theParticle == theAProton) || (theParticle == theANeutron) )
 {   
     if(A==1)
     { fTotalXsc = sigmaTotal * millibarn;
        return fTotalXsc;  }
 
   fRadiusEff = 1.34*std::pow(A,0.23)+1.35/std::pow(A,1./3.);   //fm
  
   if( (Z==1) && (A==2) ) fRadiusEff = 3.800;     //fm
   if( (Z==1) && (A==3) ) fRadiusEff = 3.300;  
   if( (Z==2) && (A==3) ) fRadiusEff = 3.300;  
   if( (Z==2) && (A==4) ) fRadiusEff = 2.376;     


 }
      
//calculation of effective nuclear radius for AntiDeuteron interaction (can be changed)
  if (theParticle == theADeuteron) 
 { fRadiusEff = 1.46 * std::pow(A,0.21) + 1.45 / std::pow(A,1./3.);

    if( (Z==1) && (A==2) ) fRadiusEff = 3.238;     //fm
    if( (Z==1) && (A==3) ) fRadiusEff = 3.144;     
    if( (Z==2) && (A==3) ) fRadiusEff = 3.144;      
    if( (Z==2) && (A==4) ) fRadiusEff = 2.544;     
 }
// calculation of effective nuclear radius for AntiHe3 interaction (can be changed)

  if( (theParticle ==theAHe3) || (theParticle ==theATriton) )
 { fRadiusEff = 1.40* std::pow(A,0.21)+1.63/std::pow(A,1./3.);

    if( (Z==1) && (A==2) ) fRadiusEff = 3.144;     //fm
    if( (Z==1) && (A==3) ) fRadiusEff = 3.075;  
    if( (Z==2) && (A==3) ) fRadiusEff = 3.075;  
    if( (Z==2) && (A==4) ) fRadiusEff = 2.589;  
  }

//calculation of effective nuclear radius for AntiAlpha interaction (can be changed)

  if (theParticle == theAAlpha) 
 {
  fRadiusEff = 1.35* std::pow(A,0.21)+1.1/std::pow(A,1./3.);
  
    if( (Z==1) && (A==2) ) fRadiusEff = 2.544;     //fm
    if( (Z==1) && (A==3) ) fRadiusEff = 2.589;   
    if( (Z==2) && (A==3) ) fRadiusEff = 2.589;   
    if( (Z==2) && (A==4) ) fRadiusEff = 2.241;    
  
 }

   G4double R2 = fRadiusEff*fRadiusEff;
   G4double REf2  = R2+fRadiusNN2;
   G4double ApAt = std::abs(theParticle->GetBaryonNumber())  *  A;

 xsection = 2*pi*REf2*10.*std::log(1+(ApAt*sigmaTotal/(2*pi*REf2*10.)));  //mb
 xsection =xsection *millibarn; 
 fTotalXsc   = xsection;

  return fTotalXsc; 
}

////////////////////////////////////////////////////////////////
// Calculation of inelastic CrossSection of Anti-Nucleus - Nucleus
////////////////////////////////////////////////////////////////

G4double G4ComponentAntiNuclNuclearXS:: ComputeInelasticCrossSection
(const G4DynamicParticle* aParticle, G4int Z, G4double A)
{
  G4double  inelxsection,  sigmaTotal, sigmaElastic;

  const G4ParticleDefinition* theParticle = aParticle->GetDefinition();

    sigmaTotal        = GetAntiHadronNucleonTotCrSc(aParticle);
    sigmaElastic      = GetAntiHadronNucleonElCrSc(aParticle);
  
// calculation of sqr of radius NN-collision
   fRadiusNN2=sigmaTotal*sigmaTotal*0.1/(8.*sigmaElastic*pi);   // fm^2   


// calculation of effective nuclear radius for Pbar and Nbar interaction (can be changed)

  if ( (theParticle == theAProton) || (theParticle == theANeutron) )
 {
  if (A==1)
      { fInelasticXsc = (sigmaTotal - sigmaElastic) * millibarn;
        return fInelasticXsc;  
      } 
 fRadiusEff = 1.31*std::pow(A, 0.22)+0.9/std::pow(A, 1./3.);  //fm
    
    if( (Z==1) && (A==2) ) fRadiusEff = 3.582;               //fm
    if( (Z==1) && (A==3) ) fRadiusEff = 3.105;               
    if( (Z==2) && (A==3) ) fRadiusEff = 3.105;
    if( (Z==2) && (A==4) ) fRadiusEff = 2.209;
 }

//calculation of effective nuclear radius for AntiDeuteron interaction (can be changed)

  if (theParticle ==theADeuteron) 
{ 
 fRadiusEff = 1.38*std::pow(A, 0.21)+1.55/std::pow(A, 1./3.);
  
    if( (Z==1) && (A==2) ) fRadiusEff = 3.169;            //fm
    if( (Z==1) && (A==3) ) fRadiusEff = 3.066;
    if( (Z==2) && (A==3) ) fRadiusEff = 3.066;
    if( (Z==2) && (A==4) ) fRadiusEff = 2.498;
 }

//calculation of effective nuclear radius for AntiHe3 interaction (can be changed)

  if( (theParticle ==theAHe3) || (theParticle ==theATriton) )
 {
  fRadiusEff = 1.34 * std::pow(A, 0.21)+1.51/std::pow(A, 1./3.);
  
    if( (Z==1) && (A==2) ) fRadiusEff = 3.066;           //fm
    if( (Z==1) && (A==3) ) fRadiusEff = 2.973;
    if( (Z==2) && (A==3) ) fRadiusEff = 2.973;
    if( (Z==2) && (A==4) ) fRadiusEff = 2.508;
  
 }

//calculation of effective nuclear radius for AntiAlpha interaction (can be changed)

  if (theParticle == theAAlpha) 
 {
  fRadiusEff = 1.3*std::pow(A, 0.21)+1.05/std::pow(A, 1./3.);
    
    if( (Z==1) && (A==2) ) fRadiusEff = 2.498;            //fm
    if( (Z==1) && (A==3) ) fRadiusEff = 2.508;
    if( (Z==2) && (A==3) ) fRadiusEff = 2.508;
    if( (Z==2) && (A==4) ) fRadiusEff = 2.158;
 }
  G4double R2 = fRadiusEff*fRadiusEff;
  G4double REf2  = R2+fRadiusNN2;
  G4double  ApAt= std::abs(theParticle->GetBaryonNumber())  *  A;

 inelxsection  = pi*REf2 *10* std::log(1+(ApAt*sigmaTotal/(pi*REf2*10.))); //mb
 inelxsection  = inelxsection * millibarn;  
   fInelasticXsc =  inelxsection; 
   return fInelasticXsc;
}


///////////////////////////////////////////////////////////////////////////////
//
// Calculates elastic Anti-nucleus-Nucleus cross-section  as Total - Inelastic 
//
G4double G4ComponentAntiNuclNuclearXS::ComputeElasticCrossSection(const 
G4DynamicParticle* aParticle, G4int Z, G4double A)
{
 fElasticXsc = ComputeTotalCrossSection(aParticle, Z, A)-
                                  ComputeInelasticCrossSection(aParticle, Z, A);

 if (fElasticXsc < 0.) fElasticXsc = 0.;

 return fElasticXsc;
}
 
// /////////////////////////////////////////////////////////////////////////////////
// Calculation of  Antihadron - hadron Total Cross-section  

G4double G4ComponentAntiNuclNuclearXS :: 
GetAntiHadronNucleonTotCrSc(const G4DynamicParticle* aParticle)
{
  G4double xsection, momentum;
  const G4ParticleDefinition* theParticle = aParticle->GetDefinition();
  momentum=aParticle->GetMomentum().mag() /std::abs(theParticle->GetBaryonNumber());
  G4double Plab = momentum / GeV;

 G4double   B, SigAss;
 G4double   C, d1, d2, d3  ;

 Elab     = std::sqrt(Mn*Mn + Plab*Plab);   // GeV
 S        = 2.*Mn*Mn + 2. *Mn*Elab;         // GeV^2
 SqrtS    = std::sqrt(S);                   // GeV 

 B        = b0+b2*std::log(SqrtS/SqrtS0)*std::log(SqrtS/SqrtS0); //GeV^(-2)
 SigAss   = 36.04 +0.304*std::log(S/S0)*std::log(S/S0);          //mb 
 R0       = std::sqrt(0.40874044*SigAss - B);                   //GeV^(-2)
 
 C        = 13.55;
 d1       = -4.47;
 d2       = 12.38;
 d3       = -12.43;
 xsection = SigAss*(1 + 1./(std::sqrt(S-4.*Mn*Mn)) / (std::pow(R0, 3.))
  *C* (1+d1/SqrtS+d2/(std::pow(SqrtS,2.))+d3/(std::pow(SqrtS,3.)) ));  

//  xsection *= millibarn;

  fAntiHadronNucleonTotXsc = xsection;
  return fAntiHadronNucleonTotXsc;
}


//
// /////////////////////////////////////////////////////////////////////////////////
// Calculation of  Antihadron - hadron Elastic Cross-section  

G4double G4ComponentAntiNuclNuclearXS :: 
GetAntiHadronNucleonElCrSc(const G4DynamicParticle* aParticle)
{
 G4double xsection;

 G4double   SigAss;
 G4double   C, d1, d2, d3  ;

 GetAntiHadronNucleonTotCrSc(aParticle);

 SigAss   = 4.5 + 0.101*std::log(S/S0)*std::log(S/S0);            //mb
  
 C        = 59.27;
 d1       = -6.95;
 d2       = 23.54;
 d3       = -25.34;

 xsection = SigAss* (1 + 1. / (std::sqrt(S-4.*Mn*Mn)) / (std::pow(R0, 3.))
  *C* ( 1+d1/SqrtS+d2/(std::pow(SqrtS,2.))+d3/(std::pow(SqrtS,3.)) ));  

//  xsection *= millibarn;

  fAntiHadronNucleonElXsc = xsection;
  return fAntiHadronNucleonElXsc;
}



