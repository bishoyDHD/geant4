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
// $Id: G4KaonZeroShort.cc,v 1.11 2001-10-28 05:01:34 kurasige Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// ----------------------------------------------------------------------
//      GEANT 4 class implementation file
//
//      History: first implementation, based on object model of
//      4th April 1996, G.Cosmo
//                              H.Kurashige   7 Jul 96
// **********************************************************************

#include "g4std/fstream"
#include "g4std/iomanip"

#include "G4KaonZeroShort.hh"

#include "G4PhaseSpaceDecayChannel.hh"
#include "G4DecayTable.hh"

// ######################################################################
// ###                      KAONZEROSHORT                             ###
// ######################################################################

G4KaonZeroShort::G4KaonZeroShort(
       const G4String&     aName,        G4double            mass,
       G4double            width,        G4double            charge,   
       G4int               iSpin,        G4int               iParity,    
       G4int               iConjugation, G4int               iIsospin,   
       G4int               iIsospin3,    G4int               gParity,
       const G4String&     pType,        G4int               lepton,      
       G4int               baryon,       G4int               encoding,
       G4bool              stable,       G4double            lifetime,
       G4DecayTable        *decaytable )
 : G4VMeson( aName,mass,width,charge,iSpin,iParity,
             iConjugation,iIsospin,iIsospin3,gParity,pType,
             lepton,baryon,encoding,stable,lifetime,decaytable )
{
  SetParticleSubType("kaon");
  // Anti-particle of K0Short is K0Short itself  
  SetAntiPDGEncoding(encoding);
 
  //create Decay Table 
  G4DecayTable*   table = GetDecayTable();
  if (table!=NULL) delete table;
  table = new G4DecayTable();

  // create decay channels
  G4VDecayChannel** mode = new G4VDecayChannel*[2];
  // kaon0s -> pi+ + pi-
  mode[0] = new G4PhaseSpaceDecayChannel("kaon0S",0.686,2,"pi+","pi-");
  // kaon0s -> pi0 + pi0
  mode[1] = new G4PhaseSpaceDecayChannel("kaon0S",0.313,2,"pi0","pi0");

  for (G4int index=0; index <2; index++ ) table->Insert(mode[index]);  
  delete [] mode;

  SetDecayTable(table);
}

// ......................................................................
// ...                 static member definitions                      ...
// ......................................................................
//     
//    Arguments for constructor are as follows
//               name             mass          width         charge
//             2*spin           parity  C-conjugation
//          2*Isospin       2*Isospin3       G-parity
//               type    lepton number  baryon number   PDG encoding
//             stable         lifetime    decay table 

G4KaonZeroShort G4KaonZeroShort::theKaonZeroShort(
	     "kaon0S",    0.497672*GeV,  7.373e-12*MeV,         0.0, 
		    0,              -1,             0,          
		    1,               0,             0,             
	      "meson",               0,             0,         310,
	 	false,      0.08926*ns,          NULL
);

G4KaonZeroShort* G4KaonZeroShort::KaonZeroShortDefinition(){return &theKaonZeroShort;}

G4KaonZeroShort* G4KaonZeroShort::KaonZeroShort(){return &theKaonZeroShort;}

// **********************************************************************
// **************************** SetCuts *********************************
// **********************************************************************
//  In this version Input Cut Value is meaning less
//  theKineticEnergyCuts for all materials are set to LowestEnergy

void G4KaonZeroShort::CalcEnergyCuts( const G4Material* )
{
  

  // Set Energy Cut values to lowest  for all materials
  SetEnergyCutValues(LowestEnergy);
}

