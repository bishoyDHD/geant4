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

#include "G4CascadeSigmaPlusPChannel.hh"

namespace {
  // Outgoing particle types of a given multiplicity

  static const G4int spp2bfs[1][2] =
    {{1,23}};

  static const G4int spp3bfs[6][3] =
    {{1,1,17}, {1,3,21}, {1,7,23}, {2,3,23},  {1,3,25},  {1,11,29}};

  static const G4int spp4bfs[20][4] =
    {{1,1,7,17},   {1,1,3,13},   {1,2,3,17}, {1,3,7,21}, {2,3,3,21},
     {1,11,17,21}, {1,7,7,23},   {1,3,5,23}, {2,3,7,23}, {1,15,17,23}, 
     {1,11,13,23}, {2,11,17,23}, {1,3,7,25}, {2,3,3,25}, {1,11,17,25},
     {1,3,3,27},   {1,7,11,29},  {1,3,15,29},{2,3,11,29},{1,3,11,31}}; 

  static const G4int spp5bfs[42][5] =
    {{1,1,7,7,17},   {1,1,3,5,17},   {1,1,3,7,13},   {1,2,3,7,17},   
     {1,2,3,3,13},   {2,2,3,3,17},   {1,3,7,7,21},   {1,3,3,5,21},   
     {2,3,3,7,21},   {1,7,11,17,21}, {1,3,11,13,21}, {1,3,15,17,21}, 
     {2,3,11,17,21}, {1,7,7,7,23},   {1,3,5,7,23},   {2,3,7,7,23},   
     {2,3,3,5,23},   {1,7,15,17,23}, {1,3,13,15,23}, {1,5,11,17,23}, 
     {1,7,11,13,23}, {2,7,11,17,23}, {2,3,11,13,23}, {2,3,15,17,23},
     {1,3,7,7,25},   {1,3,3,5,25},   {2,3,3,7,25},   {1,7,11,17,25}, 
     {1,3,11,13,25}, {1,3,15,17,25}, {2,3,11,17,25}, {1,3,3,7,27},
     {2,3,3,3,27},   {1,3,11,17,27}, {1,7,7,11,29},  {1,3,5,11,29},
     {1,3,7,15,29},  {2,3,7,11,29},  {2,3,3,15,29},  {1,3,7,11,31},
     {1,3,3,15,31},  {2,3,3,11,31}};

  static const G4int spp6bfs[25][6] =
    {{1,1,7,7,7,17}, {1,1,3,5,7,17}, {1,1,3,7,7,13}, {1,1,3,3,5,13},
     {1,2,3,7,7,17}, {1,2,3,3,5,17}, {1,2,3,3,7,13}, {2,2,3,3,7,17},
     {2,2,3,3,3,13}, {1,3,7,7,7,21}, {1,3,3,5,7,21}, {2,3,3,7,7,21},
     {2,3,3,3,5,21}, {1,7,7,7,7,23}, {1,3,5,7,7,23}, {1,3,3,5,5,23},
     {2,3,7,7,7,23}, {2,3,3,5,7,23}, {1,3,7,7,7,25}, {1,3,3,5,7,25},
     {2,3,3,7,7,25}, {2,3,3,3,5,25}, {1,3,3,7,7,27}, {1,3,3,3,5,27},
     {2,3,3,3,7,27}};

  static const G4int spp7bfs[17][7] =
    {{1,1,7,7,7,7,17}, {1,1,3,5,7,7,17}, {1,1,3,3,5,5,17},
     {1,1,3,7,7,7,13}, {1,1,3,3,5,7,13}, {1,2,3,7,7,7,17}, 
     {1,2,3,3,5,7,17}, {1,2,3,3,7,7,13}, {1,2,3,3,3,5,13}, 
     {2,2,3,3,7,7,17}, {2,2,3,3,3,5,17}, {2,2,3,3,3,7,13}, 
     {1,3,7,7,7,7,21}, {1,3,3,5,7,7,21}, {1,3,3,3,5,5,21},
     {2,3,3,7,7,7,21}, {2,3,3,3,5,7,21}};

  // Cross sections for S+ p -> 2-7 body final states
  // 
  // first index:      0: channel for mult = 2
  //                 1-6: channels for mult = 3 
  //                7-26: channels for mult = 4
  //               27-68: channels for mult = 5
  //               69-93: channels for mult = 6
  //              94-110: channels for mult = 7
  //
  // second index: kinetic energy
  // 
  static const G4double sppCrossSections[111][31] = {
    //
    // multiplicity 2 (1 channel)
    //
    // S+ p
    {150.0,130.0,110.0, 90.0, 80.0, 70.0, 60.0, 50.0, 40.0, 45.0,
     40.0, 23.30,20.14,16.90,11.80, 9.80, 7.80, 7.00, 6.60, 6.40,
     6.20, 6.10, 5.90, 5.70, 5.50, 5.30, 5.10, 5.00, 4.98, 4.96, 4.70},

    //
    //  multiplicity 3 (6 channels)
    //
    // p p K0bar
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  1.00, 1.50, 1.80, 1.70, 1.50, 1.26, 1.02, 0.93, 0.84,
       0.76, 0.68, 0.61, 0.53, 0.45, 0.38, 0.31, 0.23, 0.21, 0.19, 0.09},

    // L p pi+
    {  0.0,  0.0,  0.0,  0.10, 0.20, 0.30, 0.50, 0.70, 1.00, 1.20,
       1.30, 2.50, 4.00, 4.50, 4.30, 4.00, 3.32, 2.68, 2.45, 2.20,
       1.98, 1.78, 1.60, 1.38, 1.18, 0.96, 0.77, 0.58, 0.53, 0.49, 0.22},
 
    // S+ p pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.03,
       0.10, 0.90, 1.50, 1.80, 1.70, 1.50, 1.26, 1.02, 0.93, 0.84,
       0.76, 0.68, 0.61, 0.53, 0.45, 0.38, 0.31, 0.23, 0.21, 0.19, 0.09},
 
    // S+ n pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.03,
       0.10, 0.90, 1.50, 1.80, 1.70, 1.50, 1.26, 1.02, 0.93, 0.84,
       0.76, 0.68, 0.61, 0.53, 0.45, 0.38, 0.31, 0.23, 0.21, 0.19, 0.09},
 
    // S0 p pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.03,
       0.10, 0.90, 1.50, 1.80, 1.70, 1.50, 1.26, 1.02, 0.93, 0.84,
       0.76, 0.68, 0.61, 0.53, 0.45, 0.38, 0.31, 0.23, 0.21, 0.19, 0.09},
 
    // X0 p K+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.02, 0.03, 0.03, 0.04, 0.04, 0.03, 0.03,
       0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01, 0.0,  0.0},
 
    //
    //  multiplicity 4 (20 channels)
    //
    // p p K0bar pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.10, 0.25, 0.40, 0.60, 0.80, 1.00, 1.20, 1.40, 1.54,
       1.49, 1.41, 1.36, 1.29, 1.24, 1.21, 1.15, 1.11, 1.11, 1.10, 0.98},
 
    // p p K- pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.10, 0.25, 0.40, 0.60, 0.80, 1.00, 1.20, 1.40, 1.54,
       1.49, 1.41, 1.36, 1.29, 1.24, 1.21, 1.15, 1.11, 1.11, 1.10, 0.98},
 
    // n p K0bar pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.10, 0.25, 0.40, 0.60, 0.80, 1.00, 1.20, 1.40, 1.54,
       1.49, 1.41, 1.36, 1.29, 1.24, 1.21, 1.15, 1.11, 1.11, 1.10, 0.98},

    // L p pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.03, 0.06,
       0.10, 0.25, 0.40, 0.50, 0.60, 0.75, 0.90, 1.05, 1.20, 1.32,
       1.28, 1.22, 1.18, 1.12, 1.08, 1.05, 1.00, 0.97, 0.97, 0.96, 0.85},
 
    // L n 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.03, 0.06,
       0.10, 0.25, 0.40, 0.50, 0.60, 0.75, 0.90, 1.05, 1.20, 1.32,
       1.28, 1.22, 1.18, 1.12, 1.08, 1.05, 1.00, 0.97, 0.97, 0.96, 0.85},

    // L p K+ K0bar
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},
 
    // S+ p 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.03, 0.07, 0.13, 0.25, 0.40, 0.60, 0.80, 1.00, 1.10,
       1.07, 1.02, 0.99, 0.94, 0.90, 0.88, 0.84, 0.81, 0.81, 0.80, 0.71},
 
    // S+ p pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.03, 0.07, 0.13, 0.25, 0.40, 0.60, 0.80, 1.00, 1.10,
       1.07, 1.02, 0.99, 0.94, 0.90, 0.88, 0.84, 0.81, 0.81, 0.80, 0.71},
 
    // S+ n pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.03, 0.07, 0.13, 0.25, 0.40, 0.60, 0.80, 1.00, 1.10,
       1.07, 1.02, 0.99, 0.94, 0.90, 0.88, 0.84, 0.81, 0.81, 0.80, 0.71},
 
    // S+ p K0 K0bar
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},
 
    // S+ p K+ K-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},
 
    // S+ n K+ K0bar
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},
 
    // S0 p pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.03, 0.07, 0.13, 0.25, 0.40, 0.60, 0.80, 1.00, 1.10,
       1.07, 1.02, 0.99, 0.94, 0.90, 0.88, 0.84, 0.81, 0.81, 0.80, 0.71},

    // S0 n 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.03, 0.07, 0.13, 0.25, 0.40, 0.60, 0.80, 1.00, 1.10,
       1.07, 1.02, 0.99, 0.94, 0.90, 0.88, 0.84, 0.81, 0.81, 0.80, 0.71},
 
    // S0 p K+ K0bar
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},

    // S- p 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.03, 0.07, 0.13, 0.25, 0.40, 0.60, 0.80, 1.00, 1.10,
       1.07, 1.02, 0.99, 0.94, 0.90, 0.88, 0.84, 0.81, 0.81, 0.80, 0.71},
 
    // X0 p K+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},
 
    // X0 p K0 pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},
                                                       
    // X0 n K+ pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},

    // X- p K+ pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.01, 0.01, 0.01, 0.01},

    //
    //  multiplicity 5 (42 channels)
    // 
    // 
    // p p K0bar 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.06, 0.07, 0.09, 0.13,
       0.15, 0.17, 0.18, 0.22, 0.21, 0.21, 0.21, 0.21, 0.21, 0.21, 0.20},
 
    // p p K0bar pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.03, 0.06, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},
 
    // p p K- pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.03, 0.06, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},

    // n p K0bar pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.03, 0.06, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},

    // n p K- 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.03, 0.06, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},

    // n n K0bar 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.03, 0.06, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},

    // L p pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.04, 0.04, 0.08, 0.12, 0.16, 0.17, 0.18, 0.23,
       0.26, 0.30, 0.32, 0.37, 0.36, 0.36, 0.36, 0.36, 0.35, 0.35, 0.33},
 
    // L p 2pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.04, 0.04, 0.14, 0.18, 0.24, 0.26, 0.28, 0.35,
       0.40, 0.45, 0.48, 0.56, 0.54, 0.54, 0.54, 0.53, 0.52, 0.51, 0.47},
 
    // L n 2pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.04, 0.04, 0.14, 0.18, 0.24, 0.26, 0.28, 0.35,
       0.40, 0.45, 0.48, 0.56, 0.54, 0.54, 0.54, 0.53, 0.52, 0.51, 0.47},

    // L p K+ K0bar pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // L p K+ K- pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // L p K0 K0bar pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
   
    // L n K+ K0bar pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S+ p 3pi0   
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.03, 0.06, 0.07, 0.09, 0.13,
       0.15, 0.17, 0.18, 0.22, 0.21, 0.21, 0.21, 0.21, 0.21, 0.21, 0.20},
 
    // S+ p pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.05, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},

    // S+ n pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.03, 0.06, 0.07, 0.09, 0.13,
       0.15, 0.17, 0.18, 0.22, 0.21, 0.21, 0.21, 0.21, 0.21, 0.21, 0.20},
 
    // S+ n 2pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.05, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},
 
    // S+ p K0 K0bar pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S+ p K0 K- pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S+ p K+ K0bar pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},

    // S+ p K+ K- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S+ n K+ K0bar pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S+ n K+ K- pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},

    // S+ n K0 K0bar pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S0 p pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.03, 0.06, 0.07, 0.09, 0.13,
       0.15, 0.17, 0.18, 0.22, 0.21, 0.21, 0.21, 0.21, 0.21, 0.21, 0.20},
 
    // S0 p 2pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.05, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},
 
    // S0 n 2pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.05, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},
 
    // S0 p K+ K0bar pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S0 p K+ K- pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S0 p K0 K0bar pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S0 n K+ K0bar pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // S- p 2pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.05, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},
 
    // S- n 3pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.05, 0.10, 0.11, 0.13, 0.20,
       0.23, 0.26, 0.28, 0.33, 0.32, 0.32, 0.32, 0.32, 0.32, 0.32, 0.30},
 
    // S- p K+ K0bar pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X0 p K+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X0 p K+ pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X0 p K0 pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X0 n K+ pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X0 n K0 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X- p K+ pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X- p K0 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    // X- n K+ 2pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.01, 0.02,
       0.02, 0.03, 0.03, 0.03, 0.03, 0.02, 0.02, 0.02, 0.02, 0.01, 0.01},
 
    //
    //  multiplicity 6 (25 channels)
    // 
    // p p K0bar 3pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // p p K0bar pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},

    // p p K- pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // p p K- 2pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},

    // n p K0bar pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // n p K0bar 2pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},

    // n p K- 2pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // n n K0bar 2pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
                                                                            
    // n n K- 3pi+
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // L p pi+ 3pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.03, 0.06, 0.08, 0.10, 0.12, 0.14,
       0.16, 0.20, 0.24, 0.28, 0.33, 0.34, 0.36, 0.38, 0.38, 0.38, 0.34},
        
    // L p 2pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.05, 0.10, 0.13, 0.15, 0.18, 0.22,
       0.25, 0.31, 0.36, 0.42, 0.50, 0.51, 0.54, 0.57, 0.58, 0.58, 0.52},
 
    // L n 2pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.03, 0.06, 0.08, 0.10, 0.12, 0.14,
       0.16, 0.20, 0.24, 0.28, 0.33, 0.34, 0.36, 0.38, 0.38, 0.38, 0.34},
 
    // L n 3pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.05, 0.10, 0.13, 0.15, 0.18, 0.22,
       0.25, 0.31, 0.36, 0.42, 0.50, 0.51, 0.54, 0.57, 0.58, 0.58, 0.52},
 
    // S+ p 4pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // S+ p pi+ pi- 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.03, 0.05, 0.06, 0.07, 0.08,
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},
 
    // S+ p 2pi+ 2pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},

    // S+ n pi+ 3pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // S+ n 2pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},
 
    // S0 p pi+ 3pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // S0 p 2pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},

    // S0 n 2pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // S0 n 3pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},
 
    // S- p 2pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.04, 0.05, 0.06,
       0.06, 0.07, 0.08, 0.10, 0.12, 0.12, 0.13, 0.13, 0.13, 0.13, 0.12},
 
    // S- p 3pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},
                               
    // S- n 3pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.05, 0.06, 0.07, 0.08, 
       0.09, 0.11, 0.13, 0.15, 0.18, 0.18, 0.19, 0.20, 0.20, 0.20, 0.18},

    //
    //  multiplicity 7 (17 channels)
    // 
    // p p K0bar 4pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.03,
       0.04, 0.04, 0.05, 0.07, 0.08, 0.09, 0.10, 0.12, 0.12, 0.12, 0.12},
 
    // p p K0bar pi+ pi- 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.03,
       0.04, 0.04, 0.05, 0.07, 0.08, 0.09, 0.10, 0.12, 0.12, 0.12, 0.12},

    // p p K0bar 2pi+ 2pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02, 0.03, 0.04, 0.04,
       0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.19, 0.19, 0.19, 0.19},
 
    // p p K- pi+ 3pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.03,
       0.04, 0.04, 0.05, 0.07, 0.08, 0.09, 0.10, 0.12, 0.12, 0.12, 0.12},
 
    // p p K- 2pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02, 0.03, 0.04, 0.04,
       0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.19, 0.19, 0.19, 0.19},
 
    // n p K0bar pi+ 3pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.03,
       0.04, 0.04, 0.05, 0.07, 0.08, 0.09, 0.10, 0.12, 0.12, 0.12, 0.12},
 
    // n p K0bar 2pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02, 0.03, 0.04, 0.04,
       0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.19, 0.19, 0.19, 0.19},
  
    // n p K- 2pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.03,
       0.04, 0.04, 0.05, 0.07, 0.08, 0.09, 0.10, 0.12, 0.12, 0.12, 0.12},
 
    // n p K- 3pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02, 0.03, 0.04, 0.04,
       0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.19, 0.19, 0.19, 0.19},
 
    // n n K0bar 2pi+ 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.03, 0.03,
       0.04, 0.04, 0.05, 0.07, 0.08, 0.09, 0.10, 0.12, 0.12, 0.12, 0.12},
 
    // n n K0bar 3pi+ pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02, 0.03, 0.04, 0.04,
       0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.19, 0.19, 0.19, 0.19},

    // n n K- 3pi+ pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.02, 0.03, 0.04, 0.04,
       0.05, 0.06, 0.08, 0.10, 0.12, 0.14, 0.16, 0.19, 0.19, 0.19, 0.19},
 
    // L p pi+ 4pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.0,  0.01, 0.03, 0.04, 0.05, 0.06,
       0.08, 0.10, 0.13, 0.14, 0.17, 0.19, 0.22, 0.26, 0.26, 0.26, 0.26},
 
    // L p 2pi+ pi- 2pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.04, 0.06, 0.07, 0.08,
       0.11, 0.13, 0.17, 0.22, 0.26, 0.29, 0.33, 0.40, 0.40, 0.40, 0.40},
 
    // L p 3pi+ 2pi-
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.06, 0.09, 0.11, 0.12,
       0.16, 0.20, 0.26, 0.33, 0.40, 0.47, 0.55, 0.60, 0.60, 0.60, 0.60},
 
    // L n 2pi+ 3pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.01, 0.02, 0.04, 0.06, 0.07, 0.08,
       0.11, 0.13, 0.17, 0.22, 0.26, 0.29, 0.33, 0.40, 0.40, 0.40, 0.40},
 
    // L n 3pi+ pi- pi0
    {  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
       0.0,  0.0,  0.0,  0.0,  0.02, 0.04, 0.06, 0.09, 0.11, 0.12,
       0.16, 0.20, 0.26, 0.33, 0.40, 0.47, 0.55, 0.60, 0.60, 0.60, 0.60}};
}

G4CascadeSigmaPlusPChannelData::data_t
G4CascadeSigmaPlusPChannelData::data(spp2bfs, spp3bfs, spp4bfs,
				     spp5bfs, spp6bfs, spp7bfs,
				     sppCrossSections);
