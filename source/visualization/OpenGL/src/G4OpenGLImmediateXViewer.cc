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
// $Id: G4OpenGLImmediateXViewer.cc,v 1.9 2004-04-07 15:18:22 gbarrand Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// Andrew Walkden  7th February 1997
// Class G4OpenGLImmediateXViewer : a class derived from G4OpenGLXViewer and
//                                G4OpenGLImmediateViewer.

#ifdef G4VIS_BUILD_OPENGLX_DRIVER

#include "G4OpenGLImmediateXViewer.hh"

#include "G4ios.hh"

G4OpenGLImmediateXViewer::
G4OpenGLImmediateXViewer (G4OpenGLImmediateSceneHandler& scene,
			  const G4String&  name)
 : G4VViewer (scene, scene.IncrementViewCount (), name),
   G4OpenGLViewer (scene),
   G4OpenGLXViewer (scene),
   G4OpenGLImmediateViewer (scene)
{
  if (fViewId < 0) return;  // In case error in base class instantiation.

// ensure a suitable window was found
  if (!vi_immediate) {
    G4cerr << "G4OpenGLImmediateXViewer::G4OpenGLImmediateXViewer -"
      " G4OpenGLXViewer couldn't get a visual." << G4endl;  
    fViewId = -1;  // This flags an error.
    return;
  }
}

G4OpenGLImmediateXViewer::~G4OpenGLImmediateXViewer () {}

void G4OpenGLImmediateXViewer::Initialise () {

  CreateGLXContext (vi_immediate);

  CreateMainWindow ();

  InitializeGLView ();

  // clear the buffers and window.
  ClearView ();
  FinishView ();

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //The following code was in the constructor and found not to work for
  //the reasons below.  It was moved to DrawView(), but has now been moved
  //to Initialise().
  //The following code is useless in its current position, as the 
  //G4OpenGLXmViewer constructor gets called *after* it, and hence sets
  //doublebuffer to true or false there, after our little test to correct
  //it in the case of a double buffer being got for an immediate view.
  //Hence, code moved to DrawView.
  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

  // If a double buffer context has been forced upon us, ignore the
  // back buffer for this OpenGLImmediate view.
  if (doublebuffer) {
    doublebuffer = false;
    glDrawBuffer (GL_FRONT);
  }

  glDepthFunc (GL_LEQUAL);
  glDepthMask (GL_TRUE);
}

void G4OpenGLImmediateXViewer::DrawView () {

  if (white_background == true) {
    glClearColor (1., 1., 1., 1.);
  } else {
    glClearColor (0., 0., 0., 1.);
  }
  glClearDepth (1.0);

  G4ViewParameters::DrawingStyle style = GetViewParameters().GetDrawingStyle();

  //Make sure current viewer is attached and clean...
  glXMakeCurrent (dpy, win, cx);
  glViewport (0, 0, WinSize_x, WinSize_y);
  ClearView ();

  if(style!=G4ViewParameters::hlr &&
     haloing_enabled) {

    HaloingFirstPass ();
    NeedKernelVisit ();
    ProcessView ();
    glFlush ();

    HaloingSecondPass ();

  }

  NeedKernelVisit ();  // Always need to visit G4 kernel.
  ProcessView ();
  FinishView ();

}

#endif
