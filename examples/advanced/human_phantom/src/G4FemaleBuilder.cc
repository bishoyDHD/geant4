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
#include"G4FemaleBuilder.hh"

#include "G4VBodyFactory.hh"
#include "G4MIRDBodyFactory.hh"
#include "G4ORNLBodyFactory.hh"

G4FemaleBuilder::G4FemaleBuilder()
{  
}

G4FemaleBuilder::~G4FemaleBuilder()
{
} 

void G4FemaleBuilder::BuildBreast(G4bool sensitivity)
{
 if (motherVolume == 0)
   G4Exception("The world volume is missing !!!!!");

 body -> CreateBreast(motherVolume,sex,sensitivity);
}

void G4FemaleBuilder::BuildParameterisedBreast(G4bool sensitivity)
{
G4cout << "Builder: build parameterised breast!!!!" <<G4endl;

   if (motherVolume == 0)
   G4Exception("The world volume is missing !!!!!");
   
   body -> CreateParameterisedBreast(motherVolume,sex,sensitivity);
}

void G4FemaleBuilder::BuildOvary(G4bool sensitivity)
{   
if (trunkVolume == 0)
   G4Exception("The trunk volume is missing !!!!!");
   
   body -> CreateOvary(trunkVolume,sex,sensitivity);  
}

void G4FemaleBuilder::BuildUterus(G4bool sensitivity )
{
if (trunkVolume == 0)
   G4Exception("The trunk volume is missing !!!!!");

   body -> CreateUterus(trunkVolume,sex,sensitivity); 
}
