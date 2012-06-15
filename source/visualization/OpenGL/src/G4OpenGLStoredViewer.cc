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
//
// $Id: G4OpenGLStoredViewer.cc,v 1.29 2010-10-06 10:05:52 allison Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// 
// Andrew Walkden  7th February 1997
// Class G4OpenGLStoredViewer : Encapsulates the `storedness' of
//                            an OpenGL view, for inheritance by
//                            derived (X, Xm...) classes.

#ifdef G4VIS_BUILD_OPENGL_DRIVER

#include "G4OpenGLStoredViewer.hh"

#include "G4OpenGLStoredSceneHandler.hh"
#include "G4Text.hh"
#include "G4Circle.hh"
#include "G4UnitsTable.hh"
#include "G4Scene.hh"

G4OpenGLStoredViewer::G4OpenGLStoredViewer
(G4OpenGLStoredSceneHandler& sceneHandler):
G4VViewer (sceneHandler, -1),  
G4OpenGLViewer (sceneHandler), 
fG4OpenGLStoredSceneHandler (sceneHandler)
{
  fLastVP = fDefaultVP; // Not sure if this gets executed before or
  // after G4VViewer::G4VViewer!!  Doesn't matter much.
}

G4OpenGLStoredViewer::~G4OpenGLStoredViewer () {}

void G4OpenGLStoredViewer::KernelVisitDecision () {
  
  // If there's a significant difference with the last view parameters
  // of either the scene handler or this viewer, trigger a rebuild.

  if (!fG4OpenGLStoredSceneHandler.fTopPODL ||
      CompareForKernelVisit(fLastVP)) {
    NeedKernelVisit ();
  }      
  fLastVP = fVP;
}

G4bool G4OpenGLStoredViewer::CompareForKernelVisit(G4ViewParameters& lastVP) {

  if (
      (lastVP.GetDrawingStyle ()    != fVP.GetDrawingStyle ())    ||
      (lastVP.IsAuxEdgeVisible ()   != fVP.IsAuxEdgeVisible ())   ||
      (lastVP.GetRepStyle ()        != fVP.GetRepStyle ())        ||
      (lastVP.IsCulling ()          != fVP.IsCulling ())          ||
      (lastVP.IsCullingInvisible () != fVP.IsCullingInvisible ()) ||
      (lastVP.IsDensityCulling ()   != fVP.IsDensityCulling ())   ||
      (lastVP.IsCullingCovered ()   != fVP.IsCullingCovered ())   ||
      (lastVP.IsSection ()          != fVP.IsSection ())          ||
      // Section (DCUT) implemented locally.  But still need to visit
      // kernel if status changes so that back plane culling can be
      // switched.
      (lastVP.IsCutaway ()          != fVP.IsCutaway ())          ||
      // Cutaways implemented locally.  But still need to visit kernel
      // if status changes so that back plane culling can be switched.
      (lastVP.IsExplode ()          != fVP.IsExplode ())          ||
      (lastVP.GetNoOfSides ()       != fVP.GetNoOfSides ())       ||
      (lastVP.IsMarkerNotHidden ()  != fVP.IsMarkerNotHidden ())  ||
      (lastVP.GetDefaultVisAttributes()->GetColour() !=
       fVP.GetDefaultVisAttributes()->GetColour())                ||
      (lastVP.GetDefaultTextVisAttributes()->GetColour() !=
       fVP.GetDefaultTextVisAttributes()->GetColour())            ||
      (lastVP.GetBackgroundColour ()!= fVP.GetBackgroundColour ())||
      (lastVP.IsPicking ()          != fVP.IsPicking ())
      )
    return true;
  
  if (lastVP.IsDensityCulling () &&
      (lastVP.GetVisibleDensity () != fVP.GetVisibleDensity ()))
    return true;

  /**************************************************************
  Section (DCUT) implemented locally.  No need to visit kernel if
  section plane itself changes.
  if (lastVP.IsSection () &&
      (lastVP.GetSectionPlane () != fVP.GetSectionPlane ()))
    return true;
  ***************************************************************/

  /**************************************************************
  Cutaways implemented locally.  No need to visit kernel if cutaway
  planes themselves change.
  if (lastVP.IsCutaway ()) {
    if (lastVP.GetCutawayPlanes ().size () !=
	fVP.GetCutawayPlanes ().size ()) return true;
    for (size_t i = 0; i < lastVP.GetCutawayPlanes().size(); ++i)
      if (lastVP.GetCutawayPlanes()[i] != fVP.GetCutawayPlanes()[i])
	return true;
  }
  ***************************************************************/

  if (lastVP.IsExplode () &&
      (lastVP.GetExplodeFactor () != fVP.GetExplodeFactor ()))
    return true;

  return false;
}

void G4OpenGLStoredViewer::DrawDisplayLists () {
#ifdef G4DEBUG_VIS_OGL
  printf("G4OpenGLStoredViewer::DrawDisplayLists \n");
#endif

  // Temporarily disable display list creation because we wish to
  // re-use G4OpenGLStoredSceneHandler::AddPrimitive without creating
  // new display lists.  But if we do re-use
  // G4OpenGLStoredSceneHandler::AddPrimitive, we must supply an
  // object transform and 2D if applicable.
  G4bool tempMFDL = fG4OpenGLStoredSceneHandler.fMemoryForDisplayLists;
  fG4OpenGLStoredSceneHandler.fMemoryForDisplayLists = false;

  const G4Planes& cutaways = fVP.GetCutawayPlanes();
  G4bool cutawayUnion = fVP.IsCutaway() &&
    fVP.GetCutawayMode() == G4ViewParameters::cutawayUnion;
  const size_t nPassesForCutaways = cutawayUnion? cutaways.size(): 1;
#ifdef G4DEBUG_VIS_OGL
  printf("G4OpenGLStoredViewer::DrawDisplayLists");
#endif
  G4int iPassForTransparency = 1;
  G4bool secondPassForTransparencyRequested = false;
  do {
    for (size_t iPass = 0; iPass < nPassesForCutaways; ++iPass) {

      if (cutawayUnion) {
	double a[4];
	a[0] = cutaways[iPass].a();
	a[1] = cutaways[iPass].b();
	a[2] = cutaways[iPass].c();
	a[3] = cutaways[iPass].d();
	glClipPlane (GL_CLIP_PLANE2, a);
	glEnable (GL_CLIP_PLANE2);
      }

      G4bool isPicking = fVP.IsPicking();

      for (size_t iPO = 0;
	   iPO < fG4OpenGLStoredSceneHandler.fPOList.size(); ++iPO) {
	if (POSelected(iPO)) {
	  G4OpenGLStoredSceneHandler::PO& po =
	    fG4OpenGLStoredSceneHandler.fPOList[iPO];
	  if (iPassForTransparency == 1) {  // First pass for transparency.
	    if (po.fDisplayOnSecondPassForTransparency) {
	      secondPassForTransparencyRequested = true;
	      continue;
	    }
	  } else {  // Second pass for transparency.
	    if (!po.fDisplayOnSecondPassForTransparency) {
	      continue;
	    }
	  }
	  if (isPicking) glLoadName(po.fPickName);
	  if (po.fpG4TextPlus) {
	    // Re-use G4OpenGLStoredSceneHandler::AddPrimitive...
	    fG4OpenGLStoredSceneHandler.fObjectTransformation = po.fTransform;
	    fG4OpenGLStoredSceneHandler.fProcessing2D =
	      po.fpG4TextPlus->fProcessing2D;
	    fG4OpenGLStoredSceneHandler.AddPrimitive(po.fpG4TextPlus->fG4Text);
	    fG4OpenGLStoredSceneHandler.fProcessing2D = false;
	  } else {
	    glPushMatrix();
	    G4OpenGLTransform3D oglt (po.fTransform);
	    glMultMatrixd (oglt.GetGLMatrix ());
	    glCallList (po.fDisplayListId);
	    glPopMatrix();
	  }
	}
      }

      G4Transform3D lastMatrixTransform;
      G4bool first = true;

      for (size_t iTO = 0;
	   iTO < fG4OpenGLStoredSceneHandler.fTOList.size(); ++iTO) {
	if (TOSelected(iTO)) {
	  G4OpenGLStoredSceneHandler::TO& to =
	    fG4OpenGLStoredSceneHandler.fTOList[iTO];
	  if (iPassForTransparency == 1) {  // First pass for transparency.
	    if (to.fDisplayOnSecondPassForTransparency) {
	      secondPassForTransparencyRequested = true;
	      continue;
	    }
	  } else {  // Second pass for transparency.
	    if (!to.fDisplayOnSecondPassForTransparency) {
	      continue;
	    }
	  }
	  if (to.fEndTime >= fStartTime && to.fStartTime <= fEndTime) {
	    if (fVP.IsPicking()) glLoadName(to.fPickName);
	    if (to.fpG4TextPlus) {
	      // Re-use G4OpenGLStoredSceneHandler::AddPrimitive...
	      fG4OpenGLStoredSceneHandler.fObjectTransformation =
		to.fTransform;
	      fG4OpenGLStoredSceneHandler.fProcessing2D =
		to.fpG4TextPlus->fProcessing2D;
	      fG4OpenGLStoredSceneHandler.AddPrimitive
		(to.fpG4TextPlus->fG4Text);
	      fG4OpenGLStoredSceneHandler.fProcessing2D = false;
	    } else {
	      if (to.fTransform != lastMatrixTransform) {
		if (! first) {
		  glPopMatrix();
		}
		first = false;
		glPushMatrix();
		G4OpenGLTransform3D oglt (to.fTransform);
		glMultMatrixd (oglt.GetGLMatrix ());
	      }
	      const G4Colour& c = to.fColour;
	      if (fFadeFactor > 0. && to.fEndTime < fEndTime) {
		// Brightness scaling factor
		G4double bsf = 1. - fFadeFactor *
		  ((fEndTime - to.fEndTime) / (fEndTime - fStartTime));
		const G4Colour& bg = fVP.GetBackgroundColour();
		glColor3d
		  (bsf * c.GetRed() + (1. - bsf) * bg.GetRed(),
		   bsf * c.GetGreen() + (1. - bsf) * bg.GetGreen(),
		   bsf * c.GetBlue() + (1. - bsf) * bg.GetBlue());
	      } else {
		glColor3d(c.GetRed(), c.GetGreen(), c.GetBlue());
	      }
	      glCallList (to.fDisplayListId);
	    }
	    if (to.fTransform != lastMatrixTransform) {
	      lastMatrixTransform = to.fTransform;
	    }
	  }
	}
      }
      if (! first) {
	glPopMatrix();
      }

      if (cutawayUnion) glDisable (GL_CLIP_PLANE2);
    }  // nPassesForCutaways
  } while (secondPassForTransparencyRequested &&
	   ++iPassForTransparency <= 2);

  // Display time at "head" of time range, which is fEndTime...
  if (fDisplayHeadTime && fEndTime < DBL_MAX) {
    glMatrixMode (GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho (-1., 1., -1., 1., -G4OPENGL_FLT_BIG, G4OPENGL_FLT_BIG);
    glMatrixMode (GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
    G4Text headTimeText(G4BestUnit(fEndTime,"Time"),
			G4Point3D(fDisplayHeadTimeX, fDisplayHeadTimeY, 0.));
    headTimeText.SetScreenSize(fDisplayHeadTimeSize);
    G4VisAttributes visAtts (G4Colour
			     (fDisplayHeadTimeRed,
			      fDisplayHeadTimeGreen,
			      fDisplayHeadTimeBlue));
    headTimeText.SetVisAttributes(&visAtts);
    static_cast<G4OpenGLSceneHandler&>(fSceneHandler).
      G4OpenGLSceneHandler::AddPrimitive(headTimeText);
    glMatrixMode (GL_PROJECTION);
    glPopMatrix();
    glMatrixMode (GL_MODELVIEW);
    glPopMatrix();
  }

  // Display light front...
  if (fDisplayLightFront && fEndTime < DBL_MAX) {
    G4double lightFrontRadius = (fEndTime - fDisplayLightFrontT) * c_light;
    if (lightFrontRadius > 0.) {
      G4Point3D lightFrontCentre(fDisplayLightFrontX, fDisplayLightFrontY, fDisplayLightFrontZ);
      G4Point3D circleCentre = lightFrontCentre;
      G4double circleRadius = lightFrontRadius;
      if (fVP.GetFieldHalfAngle() > 0.) {
	// Perspective view.  Find horizon centre and radius...
	G4Point3D targetPoint = fSceneHandler.GetScene()->GetStandardTargetPoint() +
	  fVP.GetCurrentTargetPoint();
	G4double sceneRadius = fSceneHandler.GetScene()->GetExtent().GetExtentRadius();
	if(sceneRadius <= 0.) sceneRadius = 1.;
	G4double cameraDistance = fVP.GetCameraDistance(sceneRadius);
	G4Point3D cameraPosition = targetPoint + cameraDistance * fVP.GetViewpointDirection().unit();
	G4Vector3D lightFrontToCameraDirection = cameraPosition - lightFrontCentre;
	G4double lightFrontCentreDistance = lightFrontToCameraDirection.mag();
	/*
	G4cout << "cameraPosition: " << cameraPosition
	       << ", lightFrontCentre: " << lightFrontCentre
	       << ", lightFrontRadius: " << lightFrontRadius
	       << ", lightFrontCentreDistance: " << lightFrontCentreDistance
	       << ", dot: " << lightFrontToCameraDirection * fVP.GetViewpointDirection()
	       << G4endl;
	*/
	if (lightFrontToCameraDirection * fVP.GetViewpointDirection() > 0. && lightFrontRadius < lightFrontCentreDistance) {
	  // Light front in front of camera...
	  G4double sineHorizonAngle = lightFrontRadius / lightFrontCentreDistance;
	  circleCentre = lightFrontCentre + (lightFrontRadius * sineHorizonAngle) * lightFrontToCameraDirection.unit();
	  circleRadius = lightFrontRadius * std::sqrt(1. - std::pow(sineHorizonAngle, 2));
	  /*
	  G4cout << "sineHorizonAngle: " << sineHorizonAngle
		 << ", circleCentre: " << circleCentre
		 << ", circleRadius: " << circleRadius
		 << G4endl;
	  */
	} else {
	  circleRadius = -1.;
	}
      }
      if (circleRadius > 0.) {
	G4Circle lightFront(circleCentre);
	lightFront.SetWorldRadius(circleRadius);
	glColor3d(fDisplayLightFrontRed,
		  fDisplayLightFrontGreen,
		  fDisplayLightFrontBlue);
	static_cast<G4OpenGLSceneHandler&>(fSceneHandler).
	  G4OpenGLSceneHandler::AddPrimitive(lightFront);
      }
    }
  }

  // Restore current flag...
  fG4OpenGLStoredSceneHandler.fMemoryForDisplayLists = tempMFDL;
}

#endif
