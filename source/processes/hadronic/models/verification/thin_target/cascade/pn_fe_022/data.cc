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
//
// -------------------------------------------------------------------
//      GEANT 4 class file --- Copyright CERN 1998
//      CERN Geneva Switzerland
//
//
//      File name:     Test30
//
//      Author:        V.Ivanchenko 
// 
//      Creation date: 12 March 2002
//
//      Modifications: 
//
// -------------------------------------------------------------------

#include "globals.hh"
#include "G4DataVector.hh"
#include "G4ios.hh"
#include <fstream>
#include <iomanip>
#include <string>
#include <iostream.h>
#include <stdlib.h>
#include <strstream.h>


int main(int argc, char** argv)
{
  int verbose = 1;

  // -------------------------------------------------------------------
  // Control on input

  if(argc < 2) {
    G4cout << "Input file is not specified! Exit" << G4endl;
    exit(1);
  }

  ifstream* fin = new ifstream();
  string fname = argv[1];
  fin->open(fname.c_str());
  if( !fin->is_open()) {
    G4cout << "Input file <" << fname << "> does not exist! Exit" << G4endl;
    exit(1);
  }

  ofstream* fout_a = new ofstream();
  string fname1 = "dsde.dat";
  fout_a->open(fname1.c_str(), std::ios::out|std::ios::trunc);
  ofstream* fout_b = new ofstream();
  string fname2 = "dsdtet.dat";
  fout_b->open(fname2.c_str(), std::ios::out|std::ios::trunc);
  ofstream* fout_c = new ofstream();
  string fname3 = "dsdedtet.dat";
  fout_c->open(fname3.c_str(), std::ios::out|std::ios::trunc);

  //there can't be lines longer than nmax characters
  const int nmax = 200;
  char line[nmax]; 
  std::string line1, line2, word1, word2, word3;
  G4bool end = true;

  G4DataVector* energy = new G4DataVector();
  int nbin = 38;
  int n, ibin, inum;
  int counter = 0;
  double bin = 0.5*MeV;
  double elim= 1.0*MeV;
  double an, e1, e2, y1, y2, ct1, ct2, xs;
  double e0 = 0.0;
  double x =  0.0;

  for(int i=0; i<=nbin; i++) {
    x += bin;
    energy->push_back(x);
  }

  G4DataVector* angle = new G4DataVector();
  std::vector<G4DataVector*> cs;

  // main loop 

  do {

    // read next line

    counter++;
    for(int ii = 0; ii < nmax; ii++) {line[ii] = ' ';}
    fin->getline( line, nmax);
    line1 = std::string("");
    line1 = std::string(line, nmax);
    if(2 < verbose) {
      cout << "Next line # " << counter << ": " << line1 << endl;
      cout << "First symbols= " << line[0] << line[1] << line[2] << endl;
    }  

    // analize line contence

    // end of file
    if(fin->eof() || line[0] == 'E' && line[1] == 'N' && line[2] == 'D') {
      end = false;
    } else if(line[0] == 'A' && line[1] == 'D' && line[2] == 'E') {
      (*fin) >> an;
      if(0 < verbose) {
        cout << " angle= " << an << endl;
      }  
      an *= degree;
      (*fin) >> word1 >> word2 >> n;
      angle->push_back(an);
      G4DataVector* cross = new G4DataVector();
      cross->push_back(0.0);

      (*fin) >> line2 >> word1 >> word2 >> word3;
      (*fin) >> line2 >> word1 >> word2 >> word3;

      ibin= 0;
      inum= 0;
      for(i=0; i<n; i++) {
        (*fin) >> e1 >> x >> e2 >> xs;
        if(1 < verbose) {
          cout << "e1= " << e1 << " e2= " << e2 << " cross= " << x << endl;
        }  
        for(int j=ibin; j<nbin; j++) {
          e0 = (*energy)[j];
          if(e1 >= e0 && e1 < e0 + bin) break;
          if(inum) (*cross)[j] /= (double)inum;
          cross->push_back(0.0);
          inum = 0;
        }
        (*cross)[j] += x;
        inum++;
        ibin = j;
      }
      if(inum) (*cross)[ibin] /= (double)inum;
      for(int j=ibin; j<n; j++) {
        cross->push_back(0.0);
      }
      cs.push_back(cross);
      if(0 < verbose) {
        (*fout_c) << "#####..Result.of.parcing..#######..E(MeV)=22.2 " 
                  << " Angle(degree)= " << an/degree
                  << G4endl;
        for(i=0; i<nbin; i++) {
          (*fout_c) << "e(MeV)= " << 0.5*((*energy)[i] + (*energy)[i+1]) 
                    << " cross(mb/MeV/sr)= " << (*cross)[i] << endl;
	}
      }  
    }
  } while (end);
  (*fout_c) << "#####..End..#####" << G4endl;

  angle->push_back(pi);
  int na = angle->size();
  G4DataVector* f1;
  G4DataVector* f2;
  
  if(0 < verbose) {
     (*fout_a) << "#####..Result.of.integration..#####.."
               << G4endl;
     (*fout_b) << "#####..Result.of.integration..#####.." 
               << " Elim(MeV)= " << elim
               << G4endl;
     for(i=0; i<nbin; i++) {
        
       x = 0.0;
       for(int j=0; j<na-1; j++) {
         f1  = cs[j];  
         y1  = (*f1)[i];  
         ct1 = cos((*angle)[j]);
            if(j == 0) {
              f2  = cs[j+1]; 
              y2  = (*f2)[i]; 
              ct2 = cos((*angle)[j+1]);
              y1 += (y2 - y1)*(1.0 - ct1)/(ct2 - ct1);
              ct1 = 1.0;
              if(y1 < 0.0) y1 = 0.0;
	    } else if (j == na-2) {
              f2  = cs[j-1]; 
              y2  = (*f2)[i]; 
              ct2 = cos((*angle)[j-1]);
              y2 -= (y2 - y1)*(ct2 + 1.0)/(ct2 - ct1);
              ct2 = -1.0;
              if(y2 < 0.0) y2 = 0.0;
            } else {
              f2  = cs[j+1]; 
              y2  = (*f2)[i]; 
              ct2 = cos((*angle)[j+1]);
	    }
	 //         cout << "f1= " << f1 << " f2= " << f2 << endl;
	 //         cout << "y1= " << y1 << " y2= " << y2 << endl;
	 //         cout << "ct1= " << ct1 << " ct2= " << ct2 << endl;
         x  += 0.5*(y1 + y2)*(ct1 - ct2);  
       }        
       x *= twopi;
       (*fout_a) << "e(MeV)= " << 0.5*((*energy)[i] + (*energy)[i+1]) 
            << " cross(mb/MeV)= " << x << endl;
     }
     (*fout_a) << "#####..End..#####" << G4endl;
     for(int j=0; j<na-1; j++) {
       f1  = cs[j];  
       an  = cos((*angle)[j]);
       x   = 0.0;
       for(i=0; i<nbin; i++) {
         y1  = (*f1)[i];
         e1  = (*energy)[i];
         e2  = (*energy)[i+1];
         if(e2 < elim) {
           y1 = 0.0;
         } else if (e1 < elim) {
           y1 *= (e2 - elim)/(e2 - e1);
	 }
         x += y1*(e2 - e1);
       }
       (*fout_b) << "cos(theta)= " << an 
                 << " cross(mb/sr)= " << x << endl;
     }
     (*fout_b) << "#####..End..#####" << G4endl;
  }  
}






