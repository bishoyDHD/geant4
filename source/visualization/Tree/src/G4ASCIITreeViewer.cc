// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: G4ASCIITreeViewer.cc,v 1.2 2001-06-15 07:12:37 stanaka Exp $
// GEANT4 tag $Name: not supported by cvs2svn $

#include "G4ASCIITreeViewer.hh"

#include "G4ios.hh"
#include "g4std/strstream"

G4ASCIITreeViewer::G4ASCIITreeViewer
(G4VSceneHandler& sceneHandler, const G4String& name):
  G4VTreeViewer(sceneHandler, name) {}

G4ASCIITreeViewer::~G4ASCIITreeViewer() {}
