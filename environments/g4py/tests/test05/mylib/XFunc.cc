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
// $Id: XFunc.cc,v 1.3 2006-06-04 21:35:59 kmura Exp $
// ====================================================================
//   XFunc.cc
//
//                                         2005 Q
// ====================================================================
#include "XFunc.hh"

// ====================================================================
//
// class description
//
// ====================================================================

int func1(int i)
{
  return i;
}

double func1(int i, double d)
{
  return i*d;
}


int func2(int i)
{
  return i;
}

int func2(int i0, int i1)
{
  return i0*i1;
}

int func3(int i)
{
  return 0;
}

int func3(double d)
{
  return 1;
}

