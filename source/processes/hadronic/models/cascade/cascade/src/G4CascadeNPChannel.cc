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
// $Id: G4CascadeT0npChannel.cc,v 1.3 2010-08-04 05:28:24 mkelsey Exp $
//
// 20100804  M. Kelsey -- Add name string to ctor
// 20110719  M. Kelsey -- Add initial state code to ctor
// 20110725  M. Kelsey -- Instantiate cross-section object for self-registration
// 20110916  M. Kelsey -- Drop self-registration due to platform inconsistencies
// 20120907  M. Kelsey -- Subclass and overload findCrossSection() function.

#include "G4CascadeNPChannel.hh"
#include "G4InuclParticleNames.hh"
using namespace G4InuclParticleNames;

namespace {
  // n p and p n : Outgoing particle types of a given multiplicity
  static const G4int np2bfs[1][2] =
  {{pro,neu}};

  static const G4int np3bfs[9][3] =
  {{pro,pro,pim}, {pro,neu,pi0}, {neu,neu,pip}, {pro,lam,k0},
   {pro,s0,k0},   {pro,sm,kpl},  {neu,lam,kpl}, {neu,s0,kpl},
   {neu,sp,k0}};

  static const G4int np4bfs[22][4] =
  {{pro,neu,pip,pim}, {pro,pro,pim,pi0}, {pro,neu,pi0,pi0},
   {neu,neu,pip,pi0}, {pro,lam,kpl,pim}, {pro,s0,kpl,pim},
   {pro,lam,k0,pi0},  {pro,s0,k0,pi0},   {pro,sp,k0,pim},
   {pro,sm,kpl,pi0},  {pro,sm,k0,pip},   {neu,lam,kpl,pi0},
   {neu,lam,k0,pip},  {neu,sp,kpl,pim},  {neu,sp,k0,pi0},
   {neu,s0,kpl,pi0},  {neu,s0,k0,pip},   {neu,sm,kpl,pip},
   {pro,neu,kpl,kmi}, {pro,neu,k0,k0b},  {pro,pro,k0,kmi},
   {neu,neu,kpl,k0b}};

  static const G4int np5bfs[38][5] =
  {{pro,neu,pip,pim,pi0}, {pro,neu,pi0,pi0,pi0}, {pro,pro,pip,pim,pim},
   {pro,pro,pim,pi0,pi0}, {neu,neu,pip,pip,pim}, {neu,neu,pip,pi0,pi0},
   {pro,lam,kpl,pim,pi0}, {pro,lam,k0,pip,pim},  {pro,lam,k0,pi0,pi0},
   {pro,s0,k0,pip,pim},   {pro,s0,k0,pi0,pi0},   {pro,s0,kpl,pim,pi0},
   {pro,sp,kpl,pim,pim},  {pro,sp,k0,pim,pi0},   {pro,sm,k0,pip,pi0},
   {pro,sm,kpl,pip,pim},  {pro,sm,kpl,pi0,pi0},  {neu,lam,kpl,pip,pim},
   {neu,lam,kpl,pi0,pi0}, {neu,lam,k0,pip,pi0},  {neu,s0,kpl,pip,pim},
   {neu,s0,kpl,pi0,pi0},  {neu,s0,k0,pip,pi0},   {neu,sp,k0,pip,pim},
   {neu,sp,k0,pi0,pi0},   {neu,sp,kpl,pim,pi0},  {neu,sm,kpl,pip,pi0},
   {neu,sm,k0,pip,pip},   {pro,neu,kpl,kmi,pi0}, {pro,neu,k0,k0b,pi0},
   {pro,neu,k0,kmi,pip},  {pro,neu,kpl,k0b,pim}, {pro,pro,k0,k0b,pim},
   {pro,pro,kpl,kmi,pim}, {pro,pro,k0,kmi,pi0},  {neu,neu,kpl,kmi,pip},
   {neu,neu,k0,k0b,pip},  {neu,neu,kpl,k0b,pi0}};

  static const G4int np6bfs[7][6] =
  {{pro,neu,pip,pip,pim,pim}, {pro,neu,pip,pim,pi0,pi0},
   {pro,neu,pi0,pi0,pi0,pi0}, {pro,pro,pip,pim,pim,pi0},
   {pro,pro,pim,pi0,pi0,pi0}, {neu,neu,pip,pip,pim,pi0},
   {neu,neu,pip,pi0,pi0,pi0}};

  static const G4int np7bfs[9][7] =
  {{pro,neu,pip,pip,pim,pim,pi0}, {pro,neu,pip,pim,pi0,pi0,pi0},
   {pro,neu,pi0,pi0,pi0,pi0,pi0}, {pro,pro,pip,pip,pim,pim,pim},
   {pro,pro,pip,pim,pim,pi0,pi0}, {pro,pro,pim,pi0,pi0,pi0,pi0},
   {neu,neu,pip,pip,pip,pim,pim}, {neu,neu,pip,pip,pim,pi0,pi0},
   {neu,neu,pip,pi0,pi0,pi0,pi0}};

  static const G4int np8bfs[10][8] =
  {{pro,neu,pip,pip,pip,pim,pim,pim}, {pro,neu,pip,pip,pim,pim,pi0,pi0},
   {pro,neu,pip,pim,pi0,pi0,pi0,pi0}, {pro,neu,pi0,pi0,pi0,pi0,pi0,pi0},
   {pro,pro,pip,pip,pim,pim,pim,pi0}, {pro,pro,pip,pim,pim,pi0,pi0,pi0},
   {pro,pro,pim,pi0,pi0,pi0,pi0,pi0}, {neu,neu,pip,pip,pip,pim,pim,pi0},
   {neu,neu,pip,pip,pim,pi0,pi0,pi0}, {neu,neu,pip,pi0,pi0,pi0,pi0,pi0}};

  static const G4int np9bfs[12][9] =
  {{pro,neu,pip,pip,pip,pim,pim,pim,pi0},{pro,neu,pip,pip,pim,pim,pi0,pi0,pi0},
   {pro,neu,pip,pim,pi0,pi0,pi0,pi0,pi0},{pro,neu,pi0,pi0,pi0,pi0,pi0,pi0,pi0},
   {pro,pro,pip,pip,pip,pim,pim,pim,pim},{pro,pro,pip,pip,pim,pim,pim,pi0,pi0},
   {pro,pro,pip,pim,pim,pi0,pi0,pi0,pi0},{pro,pro,pim,pi0,pi0,pi0,pi0,pi0,pi0},
   {neu,neu,pip,pip,pip,pip,pim,pim,pim},{neu,neu,pip,pip,pip,pim,pim,pi0,pi0},
   {neu,neu,pip,pip,pim,pi0,pi0,pi0,pi0},{neu,neu,pip,pi0,pi0,pi0,pi0,pi0,pi0}};
}

namespace {
  // Total n p cross section as a function of kinetic energy
  static const G4double npTotXSec[30] = 
  // Stepanov cross sections below 400 MeV 
  {20360.0, 302.4, 235.9, 173.3, 132.0, 100.7,  74.9,  51.9,  38.8,  31.6,  
      27.8,  25.2,  24.0,  23.3,  29.1,  35.0,  37.5,  39.02, 40.29, 40.72,
      42.36, 41.19, 42.04, 41.67, 40.96, 39.48, 39.79, 39.39, 39.36, 39.34};

  static const G4double npCrossSections[108][30] = {
  //
  // multiplicity 2 (1 channel)
  //
  //  p n (p n)
  // Stepanov cross sections below 400 MeV 
  {20360.0, 302.4, 235.9, 173.3, 132.0, 100.7,  74.9,  51.9,  38.8,  31.6,       
      27.8,  25.2,  24.0,  23.3,  27.0,  27.0,  23.0,  19.0,  17.0,  15.5,
      14.0,  13.0,  12.0,  11.0,  10.0,   9.5,   9.0,   8.5,   8.0,   7.7},
  //
  // multiplicity 3 (9 channels)
  //
  //  p p pi- (n n pi+)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.2,  0.9,  1.75, 2.3,  2.8,  2.8,
     2.2,  1.9,  1.6,  1.35, 1.1,  0.95, 0.8,  0.7,  0.6,  0.53 },

  //  p n pi0 (p n pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  1.2,  4.7,  8.3, 11.3, 12.0, 10.2,
     8.2,  6.0,  4.9,  3.6,  2.5,  2.0,  1.6,  1.2,  1.0,  0.08 },

  //  n n pi+ (p p pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.7,  2.4,  4.2,  5.6,  6.1,  5.1,
     4.1,  3.0,  2.5,  1.8,  1.2,  1.0,  0.8,  0.6,  0.5,  0.41},

  //  p L K0 (n L K+)
   { 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.01, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},

  //  p S0 K0 (n S0 K+)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.01, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},

  //  p S- K+ (n S+ K0)
   { 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},

  //  n L K+ (p L K0)
   {  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},

  //  n S0 K+ (p S0 K0)
   { 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.01, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},

  //  n S+ K0 (p S- K+)
   { 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
  //
  // multiplicity 4 (22 channels)
  //
  //  p n pi+ pi- (p n pi+ pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.12, 0.38, 1.1,  3.5,
     5.9,  5.9,  5.1,  4.2,  3.7,  3.0,  2.6,  2.1,  1.8,  1.4 },

  //  p p pi- pi0 (n n pi+ pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.03, 0.1,  0.24, 0.55,
     1.2,  1.5,  1.45, 1.25, 1.0,  0.9,  0.8,  0.7,  0.6,  0.53 },

  //  p n pi0 pi0 (p n pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.07, 0.24, 0.66, 2.1,
     3.6,  3.6,  3.1,  2.5,  2.2,  1.8,  1.5,  1.2,  1.1,  0.84 },

  //  n n pi+ pi0 (p p pi- pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.03, 0.1,  0.24, 0.55,
     1.2,  1.5,  1.45, 1.25, 1.0,  0.9,  0.8,  0.7,  0.6,  0.53 },

  //  p L K+ pi- (n L K0 pi+)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.01, 0.02, 0.04, 0.04, 0.03, 0.03, 0.03, 0.02 },

  //  p S0 K+ pi- (n S0 K0 pi+)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.02 },

  //  p L K0 pi0 (n L K+ pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  p S0 K0 pi0 (n S0 K+ pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.01, 0.01, 0.0,  0.0 },

  //  p S+ K0 pi- (n S- K+ pi+)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01 },

  //  p S- K+ pi0 (n S+ K0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.03, 0.04, 0.04, 0.04, 0.03, 0.02, 0.02 },

  //  p S- K0 pi+ (n S+ K+ pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.03, 0.04, 0.04, 0.04, 0.03, 0.02, 0.02 },

  //  n L K+ pi0 (p L K0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.04, 0.04, 0.03, 0.03, 0.03, 0.02 },

  //  n L K0 pi+ (p L K+ pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  n S+ K+ pi- (p S- K0 pi+)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.02 },

  //  n S+ K0 pi0 (p S- K+ pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.0,  0.0 },

  //  n S0 K+ pi0 (p S0 K0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.01, 0.03, 0.04, 0.04, 0.04, 0.03, 0.02, 0.02 },

  //  n S0 K0 pi+ (p S0 K+ pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.03, 0.04, 0.04, 0.04, 0.03, 0.02, 0.02 },

  //  n S- K+ pi+ (p S+ K0 pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.01, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01 },

  //  p n K+ K- (p n K0 K0bar)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.01, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p n K0 K0bar (p n K+ K-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p p K0 K- (n n K+ K0bar)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  n n K+ K0bar (p p K0 K-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01 },
  //
  // multiplicity 5 (38 channels)
  //
  //  p n pi+ pi- pi0 (p n pi+ pi- pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.04,
     0.3,  0.82, 1.35, 1.8,  1.8,  1.65, 1.5,  1.28, 1.12, 0.98 },

  //  p n pi0 pi0 pi0 (p n pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02,
     0.15, 0.41, 0.68, 0.9,  0.9,  0.82, 0.75, 0.64, 0.55, 0.49 },

  //  p p pi+ pi- pi- (n n pi+ pi+ pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.09, 0.2,
     0.52, 1.2,  1.8,  2.0,  1.7,  1.5,  1.35, 1.2,  1.05, 0.9 },

  //  p p pi- pi0 pi0 (n n pi+ pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.04, 0.1,
     0.26, 0.6,  0.9,  0.98, 0.86, 0.75, 0.68, 0.6,  0.52, 0.45 },

  //  n n pi+ pi+ pi- (p p pi+ pi- pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.04,
     0.3,  0.82, 1.35, 1.8,  1.8,  1.65, 1.5,  1.28, 1.12, 0.98 },

  //  n n pi+ pi0 pi0 (p p pi- pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02,
     0.15, 0.41, 0.68, 0.9,  0.9,  0.82, 0.75, 0.64, 0.56, 0.49 },

  //  p L K+ pi- pi0 (n L K0 pi+ pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.01 },

  //  p L K0 pi+ pi- (n L K+ pi+ pi-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.01 },

  //  p L K0 pi0 pi0 (n L K+ pi0 pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.01, 0.01, 0.01, 0.02, 0.02, 0.02 },

  //  p S0 K0 pi+ pi- (n S0 K+ pi+ pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p S0 K0 pi0 pi0 (n S0 K+ pi0 pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.0 },

  //  p S0 K+ pi- pi0 (n S0 K0 pi+ pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p S+ K+ pi- pi- (n S- K0 pi+ pi+)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p S+ K0 pi- pi0 (n S- K+ pi+ pi0)
   { 0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p S- K0 pi+ pi0 (n S+ K+ pi- pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p S- K+ pi+ pi- (n S+ K0 pi+ pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p S- K+ pi0 pi0 (n S+ K0 pi0 pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.0 },

  //  n L K+ pi+ pi- (p L K0 pi+ pi-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.01 },

  //  n L K+ pi0 pi0 (p L K0 pi0 pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  n L K0 pi+ pi0 (p L K+ pi- pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.02, 0.02, 0.02, 0.01 },

  //  n S0 K+ pi+ pi- (p S0 K0 pi+ pi-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  n S0 K+ pi0 pi0 (p S0 K0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
     0.0,  0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0, 0.0,
     0.0,  0.0,  0.0,  0.0, 0.01, 0.01, 0.01, 0.01, 0.0, 0.0 },

  //  n S0 K0 pi+ pi0 (p S0 K+ pi- pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  n S+ K0 pi+ pi- (p S- K+ pi+ pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  n S+ K0 pi0 pi0 (p S- K+ pi0 pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.0 },

  //  n S+ K+ pi- pi0 (p S- K0 pi+ pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  n S- K+ pi+ pi0 (p S+ K0 pi- pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  n S- K0 pi+ pi+ (p S+ K+ pi- pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.01, 0.02, 0.02, 0.01, 0.01, 0.01 },

  //  p n K+ K- pi0 (p n K0 K0bar pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  p n K0 K0bar pi0 (p n K+ K- pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  p n K0 K- pi+ (p n K+ K0bar pi-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  p n K+ K0bar pi- (p n K0 K- pi+)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  p p K0 K0bar pi- (n n K+ K- pi+)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  p p K+ K- pi- (n n K0 K0bar pi+)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  p p K0 K- pi0 (n n K+ K0bar pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  n n K+ K- pi+ (p p K0 K0bar pi-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  n n K0 K0bar pi+ (p p K+ K- pi-)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },

  //  n n K+ K0bar pi0 (p p K0 K- pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.01, 0.01, 0.01, 0.01, 0.01, 0.01 },
  //
  // multiplicity 6 (7 channels)
  //
  //  p n pi+ pi+ pi- pi- (p n pi+ pi+ pi- pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.06, 0.1,  0.18, 0.38, 0.49, 0.46, 0.43, 0.40, 0.38, 0.36 },

  //  p n pi+ pi- pi0 pi0 (p n pi+ pi- pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.03, 0.05, 0.09, 0.19, 0.25, 0.23, 0.22, 0.2,  0.19, 0.18 },

  //  p n pi0 pi0 pi0 pi0 (p n pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.01, 0.02, 0.05, 0.1,  0.13, 0.12, 0.11, 0.1,  0.1,  0.09 },

  //  p p pi+ pi- pi- pi0 (n n pi+ pi+ pi- pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.06, 0.1,  0.18, 0.38, 0.49, 0.46, 0.43, 0.40, 0.38, 0.36 },

  //  p p pi- pi0 pi0 pi0 (n n pi+ pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.03, 0.05, 0.09, 0.19, 0.25, 0.23, 0.22, 0.2,  0.19, 0.18 },

  //  n n pi+ pi+ pi- pi0 (p p pi+ pi- pi- pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.06, 0.1,  0.18, 0.38, 0.49, 0.46, 0.43, 0.40, 0.38, 0.36 },

  //  n n pi+ pi0 pi0 pi0 (p p pi- pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.03, 0.05, 0.09, 0.19, 0.25, 0.23, 0.22, 0.2,  0.19, 0.18 },
  //
  // multiplicity 7 (9 channels)
  //
  //  p n pi+ pi+ pi- pi- pi0 (p n pi+ pi+ pi- pi- pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.06, 0.17, 0.5,  0.7,  0.7,  0.69, 0.66, 0.62 },

  //  p n pi+ pi- pi0 pi0 pi0 (p n pi+ pi- pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.03, 0.08, 0.25, 0.35, 0.35, 0.35, 0.33, 0.31 },

  //  p n pi0 pi0 pi0 pi0 pi0 (p n pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.02, 0.04, 0.12, 0.17, 0.18, 0.17, 0.16, 0.15 },

  //  p p pi+ pi+ pi- pi- pi- (n n pi+ pi+ pi+ pi- pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.06, 0.19, 0.31, 0.41, 0.44, 0.47, 0.45, 0.45 },

  //  p p pi+ pi- pi- pi0 pi0 (n n pi+ pi+ pi- pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.03, 0.1,  0.15, 0.2,  0.22, 0.23, 0.22, 0.22 },

  //  p p pi- pi0 pi0 pi0 pi0 (n n pi+ pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.02, 0.05, 0.07, 0.1,  0.11, 0.12, 0.11, 0.11 },

  //  n n pi+ pi+ pi+ pi- pi- (p p pi+ pi+ pi- pi- pi-)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.06, 0.17, 0.5,  0.7,  0.7,  0.69, 0.66, 0.62 },

  //  n n pi+ pi+ pi- pi0 pi0 (p p pi+ pi- pi- pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.03, 0.08, 0.25, 0.35, 0.35, 0.34, 0.33, 0.31 },

  //  n n pi+ pi0 pi0 pi0 pi0 (p p pi- pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.02, 0.05, 0.07, 0.1,  0.11, 0.12, 0.11, 0.11 },
  //
  // multiplicity 8 (10 channels)
  //
  //  p n pi+ pi+ pi+ pi- pi- pi- (p n pi+ pi+ pi+ pi- pi- pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.08, 0.18, 0.27, 0.30, 0.27, 0.24 },

  //  p n pi+ pi+ pi- pi- pi0 pi0 (p n pi+ pi+ pi- pi- pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.08, 0.18, 0.27, 0.30, 0.27, 0.24 },

  //  p n pi+ pi- pi0 pi0 pi0 pi0 (p n pi+ pi- pi0 pi0 pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.04, 0.12, 0.15, 0.18, 0.15, 0.15 },

  //  p n pi0 pi0 pi0 pi0 pi0 pi0 (p n pi0 pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.01, 0.02, 0.06, 0.09, 0.12, 0.09, 0.09 },

  //  p p pi+ pi+ pi- pi- pi- pi0 (n n pi+ pi+ pi+ pi- pi- pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.08, 0.18, 0.27, 0.30, 0.27, 0.24 },

  //  p p pi+ pi- pi- pi0 pi0 pi0 (n n pi+ pi+ pi- pi0 pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.04, 0.12, 0.15, 0.18, 0.15, 0.15 },

  //  p p pi- pi0 pi0 pi0 pi0 pi0 (n n pi+ pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.03, 0.06, 0.09, 0.12, 0.09, 0.09 },

  //  n n pi+ pi+ pi+ pi- pi- pi0 (p p pi+ pi+ pi- pi- pi- pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.08, 0.18, 0.27, 0.30, 0.27, 0.24 },

  //  n n pi+ pi+ pi- pi0 pi0 pi0 (p p pi+ pi- pi- pi0 pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.04, 0.12, 0.15, 0.18, 0.15, 0.15 },

  //  n n pi+ pi0 pi0 pi0 pi0 pi0 (p p pi- pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0, 0.01, 0.03, 0.06, 0.09, 0.12, 0.09, 0.09 },
  //
  // multiplicity 9 (12 channels)
  //
  //  p n pi+ pi+ pi+ pi- pi- pi- pi0 (p n pi+ pi+ pi+ pi- pi- pi- pi0)
   { 0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0, 0.0, 0.01, 0.03, 0.07, 0.11, 0.14, 0.15, 0.15, 0.15 },

  //  p n pi+ pi+ pi- pi- pi0 pi0 pi0 (p n pi+ pi+ pi- pi- pi0 pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.05, 0.07, 0.08, 0.09, 0.09, 0.09 },

  //  p n pi+ pi- pi0 pi0 pi0 pi0 pi0 (p n pi+ pi- pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.01, 0.03, 0.04, 0.05, 0.05, 0.05, 0.05 },

  //  p n pi0 pi0 pi0 pi0 pi0 pi0 pi0 (p n pi0 pi0 pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.03, 0.03, 0.03, 0.03 },

  //  p p pi+ pi+ pi+ pi- pi- pi- pi- (n n pi+ pi+ pi+ pi+ pi- pi- pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.06, 0.15, 0.19, 0.22, 0.22, 0.22 },

  //  p p pi+ pi+ pi- pi- pi- pi0 pi0 (n n pi+ pi+ pi+ pi- pi- pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.06, 0.15, 0.19, 0.22, 0.22, 0.22 },

  //  p p pi+ pi- pi- pi0 pi0 pi0 pi0 (n n pi+ pi+ pi- pi0 pi0 pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.01, 0.04, 0.09, 0.12, 0.13, 0.13, 0.13 },

  //  p p pi- pi0 pi0 pi0 pi0 pi0 pi0 (n n pi+ pi0 pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.01, 0.02, 0.05, 0.07, 0.08, 0.08, 0.08 },

  //  n n pi+ pi+ pi+ pi+ pi- pi- pi- (p p pi+ pi+ pi+ pi- pi- pi- pi-)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.06, 0.15, 0.19, 0.22, 0.22, 0.22 },

  //  n n pi+ pi+ pi+ pi- pi- pi0 pi0 (p p pi+ pi+ pi- pi- pi- pi0 pi0)
   { 0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.01, 0.02, 0.06, 0.15, 0.19, 0.22, 0.22, 0.22 },

  //  n n pi+ pi+ pi- pi0 pi0 pi0 pi0 (p p pi+ pi- pi- pi0 pi0 pi0 pi0)
   { 0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.01, 0.04, 0.09, 0.12, 0.13, 0.13, 0.13 },

  //  n n pi+ pi0 pi0 pi0 pi0 pi0 pi0 (p p pi- pi0 pi0 pi0 pi0 pi0 pi0)
   { 0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
     0.0,  0.0, 0.0, 0.01, 0.02, 0.05, 0.07, 0.08, 0.08, 0.08 }};
}

// Initialize n-p channel
typedef G4CascadeNPChannelData::data_t G4CascadeNPChannelData_t;
G4ThreadLocal G4CascadeNPChannelData_t *G4CascadeNPChannelData::data=0;

G4CascadeNPChannelData::data_t *G4CascadeNPChannelData::initializer()
{
  if (G4CascadeNPChannelData::data == 0)
    G4CascadeNPChannelData::data = 
      new G4CascadeNPChannelData::data_t(
					 np2bfs, np3bfs, np4bfs, np5bfs, np6bfs, np7bfs,
					 np8bfs, np9bfs, npCrossSections, npTotXSec,
					 neu*pro, "NeutronProton");
  return G4CascadeNPChannelData::data;
}

// Overload base class interpolator to use function for 0-10 MeV total, elastic

G4double 
G4CascadeNPChannel::findCrossSection(G4double ke,
                                     const G4double (&xsec)[30]) const {
  if (ke < 0.01 && (xsec == npTotXSec || xsec == npCrossSections[0])) {
    // Stepanov's function for ke < 10 MeV, up to zero-energy value
    const G4double kemin = 1.92/npTotXSec[0];
    return (ke>0.001 ? (3.0885 - 0.0011748/ke)/ke + 5.3107 :
	    ke>kemin ? 1.92/ke : npTotXSec[0]);
  }
  return G4PionNucSampler::findCrossSection(ke, xsec);	// Call through to base
}
