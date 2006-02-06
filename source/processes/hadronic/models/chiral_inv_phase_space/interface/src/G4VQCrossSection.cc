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
// $Id: G4VQCrossSection.cc,v 1.4 2006-02-06 09:35:57 mkossov Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
//
// CHIPS virtual class: G4VQCrossSection for the collision cross sections
// Created: M.V. Kossov, CERN/ITEP(Moscow), 10-OCT-04
// The last update: M.V. Kossov, CERN/ITEP (Moscow) 27-Nov-04
// 
//=========================================================================================
// ****************************************************************************************
// ********** This CLASS is temporary moved from the photolepton_hadron directory *********
// ******* DO NOT MAKE ANY CHANGE! With time it'll move back to photolepton...(M.K.) ******
// ****************************************************************************************

//#define debug
//#define edebug
//#define pdebug
//#define ppdebug
//#define tdebug
//#define sdebug

#include "G4VQCrossSection.hh"

// Initialization of the
G4int     G4VQCrossSection::lastPDG=0; // The last PDG code of the projectile
G4int     G4VQCrossSection::lastN=0;   // The last N of calculated nucleus
G4int     G4VQCrossSection::lastZ=0;   // The last Z of calculated nucleus
G4double  G4VQCrossSection::lastTH=0.; // Last threshold momentum
G4double  G4VQCrossSection::lastCS=0.;// Last value of the Cross Section
G4double  G4VQCrossSection::lastP=0.;  // Last used in cross section TheMomentum
//G4int     G4VQCrossSection::lastN1=0;  // Last used in cross section TheNumOfBin1
//G4int     G4VQCrossSection::lastF1=0;  // Last used in cross section TheFirstBin1
//G4int     G4VQCrossSection::lastL1=0;  // Last used in cross section TheLastBin1
//G4int     G4VQCrossSection::lastN2=0;  // Last used in cross section TheNumOfBin1
//G4int     G4VQCrossSection::lastF2=0;  // Last used in cross section TheFirstBin1
//G4int     G4VQCrossSection::lastL2=0;  // Last used in cross section TheLastBin1
//G4double  G4VQCrossSection::lastBP=0.; // Last value of the Boundary Momentum
//G4double  G4VQCrossSection::lastMP=0.; // Last value of the Maximum Momentum

G4int     G4VQCrossSection::lastI=0;          // The last position in the DAMDB
G4double  G4VQCrossSection::tolerance=.001;   // The last position in the DAMDB

// Set the new tolerance (abs(p_old/p_new-1)<tolerance)
void G4VQCrossSection::setTolerance(G4double tol)
//   ============================================
{
		tolerance=tol;
}

// Gives the threshold energy for different isotopes (can be improved in the derived class)
G4double G4VQCrossSection::ThresholdEnergy(G4int , G4int, G4int) {return 0.;} // Fake use

// The main member function giving the collision cross section (P is in IU, CS is in mb)
// Make pMom in independent units !
G4double G4VQCrossSection::GetCrossSection(G4bool fCS, G4double pMom, G4int tgZ, G4int tgN,
                                                                                G4int pPDG)
{
  static std::vector <G4int>    colPDG;// Vector of the projectile PDG code
  static std::vector <G4int>    colN;  // Vector of N for calculated nuclei (isotops)
  static std::vector <G4int>    colZ;  // Vector of Z for calculated nuclei (isotops)
  static std::vector <G4double> colP;  // Vector of last momenta for the reaction
  static std::vector <G4double> colTH; // Vector of energy thresholds for the reaction
  static std::vector <G4double> colCS; // Vector of last cross sections for the reaction
  // ***---*** End of the mandatory Static Definitions of the Associative Memory ***---***
  G4bool in=false;                     // By default the isotope must be found in the AMDB
  if(tgN!=lastN || tgZ!=lastZ || pPDG!=lastPDG)// The nucleus was not the last used isotope
  {
    in = false;                        // By default the isotope haven't be found in AMDB  
    lastP    = 0.;                     // New momentum history (nothing to compare with)
    lastPDG  = pPDG;                   // The last PDG of the projectile
    lastN    = tgN;                    // The last N of the calculated nucleus
    lastZ    = tgZ;                    // The last Z of the calculated nucleus
    lastI    = colN.size();            // Size of the Associative Memory DB in the heap
    if(lastI) for(G4int i=0; i<lastI; i++) if(colN[i]==tgN&& colZ[i]==tgZ&& colPDG[i]==pPDG)
	   {                                  // The nucleus is found in AMDB
      in = true;                       // This is the case when the isotop is found in DB
      lastP  =colP [i];                // Last Momentum  (A-dependent)
      lastTH =colTH[i];                // Last THreshold (A-dependent)
      lastCS =colCS[i];                // Last CrossSect (A-dependent)
      if(pMom<=lastTH) return 0.;      // Momentum is below the Threshold value
      else if(std::fabs(lastP/pMom-1.)<tolerance) return lastCS*millibarn;// Use lastCS
      lastI  = i;                      // Make the found isotope to be current isotope
      // Momentum pMom is in IU ! @@ Units (?)
#ifdef pdebug
      G4cout<<"G4VQCS::GetCrosSec: (-1) P="<<pMom<<",fCS="<<fCS<<",lastI="<<lastI<<G4endl;
#endif
      lastCS=CalculateCrossSection(fCS,-1,lastI,lastPDG, lastN,lastZ,pMom);//read & update
      break;                           // Go out of the LOOP
	   }
	   if(!in)                            // This nucleus has not been calculated previously
	   {
      lastTH = ThresholdEnergy(tgZ, tgN); // The last Threshold Energy
      // lastI==colN.size() frome above
#ifdef pdebug
      G4cout<<"G4VQCS::GetCrosSec: (0) P="<<pMom<<",fCS="<<fCS<<",lastI="<<lastI<<G4endl;
#endif
      //!!The slave functions must provide cross-sections in millibarns (mb) !! (not in IU)
      lastCS = CalculateCrossSection(fCS,0,lastI,lastPDG,lastZ,lastN,pMom);//calcul&create
#ifdef pdebug
      G4cout<<"G4VQCS::GetCrosSec:CS(IU)="<<lastCS<<",lZ="<<lastN<<",lN="<<lastZ<<G4endl;
#endif
      colN.push_back(tgN);
      colZ.push_back(tgZ);
      colPDG.push_back(pPDG);
      colP.push_back(pMom);
      colTH.push_back(lastTH);
      colCS.push_back(lastCS);
      return lastCS*millibarn;
	   } // End of creation of the new set of parameters
  } // End of parameters udate
  else if(pMom<=lastTH) return 0.; // Momentum is below the Threshold value
  else if(std::fabs(lastP/pMom-1.)<tolerance) return lastCS*millibarn;     // Use theLastCS
  else
  {
#ifdef pdebug
    G4cout<<"G4VQCS::GetCrosSec: (1) P="<<pMom<<",fCS="<<fCS<<",lastI="<<lastI<<G4endl;
#endif
    lastCS=CalculateCrossSection(fCS,1,lastI,lastPDG,lastZ,lastN,pMom); // Only UpdateDB
  }
  colP[lastI]=pMom;
  colPDG[lastI]=pPDG;
  colCS[lastI]=lastCS;
  return lastCS*millibarn;
}

G4double G4VQCrossSection::GetDirectPart(G4double) {return 0.;} // Direct interaction

G4double G4VQCrossSection::GetNPartons(G4double) {return 3.;} // Direct interaction

G4double G4VQCrossSection::GetLastTOTCS() {return 0.;} // Get the last total CS

G4double G4VQCrossSection::GetLastQELCS() {return 0.;} // Get the last quasi-elast CS

G4double G4VQCrossSection::GetExchangeEnergy() {return 0.;}

G4double G4VQCrossSection::GetExchangeQ2(G4double) {return 0.;}

G4double G4VQCrossSection::GetExchangeT(G4int,G4int,G4int) {return 0.;}

G4double G4VQCrossSection::GetQEL_ExchangeQ2() {return 0.;}

G4double G4VQCrossSection::GetNQE_ExchangeQ2() {return 0.;}

G4int G4VQCrossSection::GetExchangePDGCode() {return 0;}

G4double G4VQCrossSection::GetVirtualFactor(G4double nu, G4double Q2) {return 0.*nu*Q2;}

// This function finds  the linear approximation Y-point for the XN(N), YN(N) table
G4double G4VQCrossSection::LinearFit(G4double X, G4int N, G4double* XN, G4double* YN)
{
  G4double Xj=XN[0];
  G4double Xh=XN[N-1];
  if(X<=Xj) return YN[0]; 
  else if(X>=Xh) return YN[N-1];
  G4double Xp=0.; G4int j=0; while (X>Xj && j<N) {j++; Xp=Xj; Xj=XN[j];}
  return YN[j]-(Xj-X)*(YN[j]-YN[j-1])/(Xj-Xp);
}

// This function finds the linear approximation Y-point for equidistant bins: XI=X0+I*DX
G4double G4VQCrossSection::EquLinearFit(G4double X, G4int N, G4double X0, G4double DX,
                                        G4double* Y)
{
  if(DX<=0. || N<2)
  {
    G4cout<<"***G4VQCrossSection::EquLinearFit: DX="<<DX<<", N="<<N<<G4endl;
    return Y[0];
  }
  G4int    N2=N-2;
  G4double d=(X-X0)/DX;
  G4int         j=static_cast<int>(d);
  if     (j<0)  j=0;
  else if(j>N2) j=N2;
  d-=j; // excess
  G4double yi=Y[j];
  G4double sigma=yi+(Y[j+1]-yi)*d;
  return sigma;
}
