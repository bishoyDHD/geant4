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
// $Id: G4JTPolynomialSolver.hh,v 1.4 2005-03-15 19:11:35 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// Class description:
//
// G4JTPolynomialSolver implements the Jenkins-Traub algorithm
// for real polynomial root finding.
// The solver returns -1, if the leading coefficient is zero,
// the number of roots found, otherwise.
//
// ----------------------------- INPUT --------------------------------
//
//    op     - double precision vector of coefficients in order of
//             decreasing powers
//    degree - integer degree of polynomial
// 
// ----------------------------- OUTPUT -------------------------------
//
//    zeror,zeroi - double precision vectors of the
//                  real and imaginary parts of the zeros
// 
// ---------------------------- EXAMPLE -------------------------------
// 
//    G4JTPolynomialSolver trapEq ;
//    G4double coef[8] ;
//    G4double zr[7] , zi[7] ;
//    G4int num = trapEq.FindRoots(coef,7,zr,zi);

// ---------------------------- HISTORY -------------------------------
//
// Translated from original TOMS493 Fortran77 routine (ANSI C, by C.Bond).
// Translated to C++ and adapted to use STL vectors,
// by Oliver Link (Oliver.Link@cern.ch)
//
// --------------------------------------------------------------------

#ifndef G4JTPOLYNOMIALSOLVER_HH
#define G4JTPOLYNOMIALSOLVER_HH

#include <cmath>
#include <vector>

#include "globals.hh"

class G4JTPolynomialSolver 
{

  public:

    G4JTPolynomialSolver();
    ~G4JTPolynomialSolver();
  
    G4int FindRoots(G4double *op, G4int degree,
                    G4double *zeror, G4double *zeroi);

  private:

    std::vector<G4double> p;
    std::vector<G4double> qp;
    std::vector<G4double> k;
    std::vector<G4double> qk;
    std::vector<G4double> svk;

    G4double sr;
    G4double si;
    G4double u,v; 
    G4double a,b,c,d;
    G4double a1,a2,a3,a6,a7;
    G4double e,f,g,h;
    G4double szr,szi;
    G4double lzr,lzi;
    G4int n,nn,nmi,zerok;
  
    /*  The following statements set machine constants */

    static const G4double base;
    static const G4double eta;
    static const G4double infin;
    static const G4double smalno;
    static const G4double are;
    static const G4double mre;
    static const G4double lo;

    void Quadratic(G4double a,G4double b1,G4double c,
                   G4double *sr,G4double *si, G4double *lr,G4double *li);
    void ComputeFixedShiftPolynomial(G4int l2, G4int *nz);
    void QuadraticPolynomialIteration(G4double *uu,G4double *vv,G4int *nz);
    void RealPolynomialIteration(G4double *sss, G4int *nz, G4int *iflag);
    void ComputeScalarFactors(G4int *type);
    void ComputeNextPolynomial(G4int *type);
    void ComputeNewEstimate(G4int type,G4double *uu,G4double *vv);
    void QuadraticSyntheticDivision(G4int n, G4double *u, G4double *v,
                                    std::vector<G4double> &p, 
                                    std::vector<G4double> &q, 
                                    G4double *a, G4double *b);
};

#endif
