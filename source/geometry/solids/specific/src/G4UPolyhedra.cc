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
// Implementation of G4UPolyhedra wrapper class
//
// 31.10.13 G.Cosmo, CERN
// --------------------------------------------------------------------

#include "G4Polyhedra.hh"
#include "G4UPolyhedra.hh"

#if ( defined(G4GEOM_USE_USOLIDS) || defined(G4GEOM_USE_PARTIAL_USOLIDS) )

#include "G4GeomTools.hh"
#include "G4GeometryTolerance.hh"
#include "G4AffineTransform.hh"
#include "G4VPVParameterisation.hh"
#include "G4BoundingEnvelope.hh"

using namespace CLHEP;

////////////////////////////////////////////////////////////////////////
//
// Constructor (GEANT3 style parameters)
//
// GEANT3 PGON radii are specified in the distance to the norm of each face.
//
G4UPolyhedra::G4UPolyhedra(const G4String& name,
                                 G4double phiStart,
                                 G4double phiTotal,
                                 G4int numSide,
                                 G4int numZPlanes,
                           const G4double zPlane[],
                           const G4double rInner[],
                           const G4double rOuter[]  )
  : Base_t(name, phiStart, phiTotal, numSide,
           numZPlanes, zPlane, rInner, rOuter)
{
  fGenericPgon = false;
  SetOriginalParameters();
  wrStart = phiStart;
  while (wrStart < 0)
  {
    wrStart += twopi;
  }
  wrDelta = phiTotal;
  if (wrDelta <= 0. || wrDelta >= twopi*(1-DBL_EPSILON))
  {
    wrDelta = twopi;
  }
  wrNumSide = numSide;
  G4double convertRad = 1./std::cos(0.5*wrDelta/wrNumSide);
  rzcorners.resize(0);
  for (G4int i=0; i<numZPlanes; ++i)
  {
    G4double z = zPlane[i];
    G4double r = rOuter[i]*convertRad;
    rzcorners.emplace_back(r,z);
  }
  for (G4int i=numZPlanes-1; i>=0; --i)
  {
    G4double z = zPlane[i];
    G4double r = rInner[i]*convertRad;
    rzcorners.emplace_back(r,z);
  }
  std::vector<G4int> iout;
  G4GeomTools::RemoveRedundantVertices(rzcorners,iout,2*kCarTolerance);
}


////////////////////////////////////////////////////////////////////////
//
// Constructor (generic parameters)
//
G4UPolyhedra::G4UPolyhedra(const G4String& name,
                                 G4double phiStart,
                                 G4double phiTotal,
                                 G4int    numSide,
                                 G4int    numRZ,
                           const G4double r[],
                           const G4double z[]   )
  : Base_t(name, phiStart, phiTotal, numSide, numRZ, r, z)
{
  fGenericPgon = true;
  SetOriginalParameters();
  wrStart = phiStart;
  while (wrStart < 0.)
  {
    wrStart += twopi;
  }
  wrDelta = phiTotal;
  if (wrDelta <= 0. || wrDelta >= twopi*(1-DBL_EPSILON))
  {
    wrDelta = twopi;
  }
  wrNumSide = numSide;
  rzcorners.resize(0);
  for (G4int i=0; i<numRZ; ++i)
  {
    rzcorners.emplace_back(r[i],z[i]);
  }
  std::vector<G4int> iout;
  G4GeomTools::RemoveRedundantVertices(rzcorners,iout,2*kCarTolerance);
}


////////////////////////////////////////////////////////////////////////
//
// Destructor
//
G4UPolyhedra::~G4UPolyhedra() = default;


////////////////////////////////////////////////////////////////////////
//
// Copy constructor
//
G4UPolyhedra::G4UPolyhedra( const G4UPolyhedra& source )
  : Base_t( source )
{
  fGenericPgon = source.fGenericPgon;
  fOriginalParameters = source.fOriginalParameters;
  wrStart   = source.wrStart;
  wrDelta   = source.wrDelta;
  wrNumSide = source.wrNumSide;
  rzcorners = source.rzcorners;
}


////////////////////////////////////////////////////////////////////////
//
// Assignment operator
//
G4UPolyhedra& G4UPolyhedra::operator=( const G4UPolyhedra& source )
{
  if (this == &source) return *this;

  Base_t::operator=( source );
  fGenericPgon = source.fGenericPgon;
  fOriginalParameters = source.fOriginalParameters;
  wrStart   = source.wrStart;
  wrDelta   = source.wrDelta;
  wrNumSide = source.wrNumSide;
  rzcorners = source.rzcorners;

  return *this;
}


////////////////////////////////////////////////////////////////////////
//
// Accessors & modifiers
//
G4int G4UPolyhedra::GetNumSide() const
{
  return wrNumSide;
}
G4double G4UPolyhedra::GetStartPhi() const
{
  return wrStart;
}
G4double G4UPolyhedra::GetEndPhi() const
{
  return (wrStart + wrDelta);
}
G4double G4UPolyhedra::GetSinStartPhi() const
{
  G4double phi = GetStartPhi();
  return std::sin(phi);
}
G4double G4UPolyhedra::GetCosStartPhi() const
{
  G4double phi = GetStartPhi();
  return std::cos(phi);
}
G4double G4UPolyhedra::GetSinEndPhi() const
{
  G4double phi = GetEndPhi();
  return std::sin(phi);
}
G4double G4UPolyhedra::GetCosEndPhi() const
{
  G4double phi = GetEndPhi();
  return std::cos(phi);
}
G4bool G4UPolyhedra::IsOpen() const
{
  return (wrDelta < twopi);
}
G4bool G4UPolyhedra::IsGeneric() const
{
  return fGenericPgon;
}
G4int G4UPolyhedra::GetNumRZCorner() const
{
  return rzcorners.size();
}
G4PolyhedraSideRZ G4UPolyhedra::GetCorner(G4int index) const
{
  G4TwoVector rz = rzcorners.at(index);
  G4PolyhedraSideRZ psiderz = { rz.x(), rz.y() };

  return psiderz;
}
G4PolyhedraHistorical* G4UPolyhedra::GetOriginalParameters() const
{
  return new G4PolyhedraHistorical(fOriginalParameters);
}
void G4UPolyhedra::SetOriginalParameters()
{
  G4double startPhi = GetPhiStart();
  G4double deltaPhi = GetPhiDelta();
  G4int numPlanes   = GetZSegmentCount() + 1;
  G4int numSides    = GetSideCount();

  fOriginalParameters.Start_angle   = startPhi;
  fOriginalParameters.Opening_angle = deltaPhi;
  fOriginalParameters.Num_z_planes  = numPlanes;
  fOriginalParameters.numSide       = numSides;

  delete [] fOriginalParameters.Z_values;
  delete [] fOriginalParameters.Rmin;
  delete [] fOriginalParameters.Rmax;
  fOriginalParameters.Z_values = new G4double[numPlanes];
  fOriginalParameters.Rmin = new G4double[numPlanes];
  fOriginalParameters.Rmax = new G4double[numPlanes];

  G4double convertRad = fGenericPgon
                      ? 1.0 : std::cos(0.5*deltaPhi/numSides);
  for (G4int i=0; i<numPlanes; ++i)
  {
    fOriginalParameters.Z_values[i] = GetZPlanes()[i];
    fOriginalParameters.Rmax[i]     = GetRMax()[i]/convertRad;
    fOriginalParameters.Rmin[i]     = GetRMin()[i]/convertRad;
  }
}
void G4UPolyhedra::SetOriginalParameters(G4PolyhedraHistorical* pars)
{
  fOriginalParameters = *pars;
  fRebuildPolyhedron = true;
  Reset();
}

G4bool G4UPolyhedra::Reset()
{
  if (fGenericPgon)
  {
    std::ostringstream message;
    message << "Solid " << GetName() << " built using generic construct."
            << G4endl << "Not applicable to the generic construct !";
    G4Exception("G4UPolyhedra::Reset()", "GeomSolids1001",
                JustWarning, message, "Parameters NOT reset.");
    return true;  // error code set
  }

  //
  // Rebuild polyhedra based on original parameters
  //
  wrStart = fOriginalParameters.Start_angle;
  while (wrStart < 0.)
  {
    wrStart += twopi;
  }
  wrDelta = fOriginalParameters.Opening_angle;
  if (wrDelta <= 0. || wrDelta >= twopi*(1-DBL_EPSILON))
  {
    wrDelta = twopi;
  }
  wrNumSide = fOriginalParameters.numSide;
  rzcorners.resize(0);
  for (G4int i=0; i<fOriginalParameters.Num_z_planes; ++i)
  {
    G4double z = fOriginalParameters.Z_values[i];
    G4double r = fOriginalParameters.Rmax[i];
    rzcorners.emplace_back(r,z);
  }
  for (G4int i=fOriginalParameters.Num_z_planes-1; i>=0; --i)
  {
    G4double z = fOriginalParameters.Z_values[i];
    G4double r = fOriginalParameters.Rmin[i];
    rzcorners.emplace_back(r,z);
  }
  std::vector<G4int> iout;
  G4GeomTools::RemoveRedundantVertices(rzcorners,iout,2*kCarTolerance);

  return false;  // error code unset
}


////////////////////////////////////////////////////////////////////////
//
// Dispatch to parameterisation for replication mechanism dimension
// computation & modification.
//
void G4UPolyhedra::ComputeDimensions(G4VPVParameterisation* p,
                                     const G4int n,
                                     const G4VPhysicalVolume* pRep)
{
  p->ComputeDimensions(*(G4Polyhedra*)this,n,pRep);
}


//////////////////////////////////////////////////////////////////////////
//
// Make a clone of the object

G4VSolid* G4UPolyhedra::Clone() const
{
  return new G4UPolyhedra(*this);
}


//////////////////////////////////////////////////////////////////////////
//
// Get bounding box

void G4UPolyhedra::BoundingLimits(G4ThreeVector& pMin,
                                  G4ThreeVector& pMax) const
{
  static G4bool checkBBox = true;
  static G4bool checkPhi  = true;

  G4double rmin = kInfinity, rmax = -kInfinity;
  G4double zmin = kInfinity, zmax = -kInfinity;
  for (G4int i=0; i<GetNumRZCorner(); ++i)
  {
    G4PolyhedraSideRZ corner = GetCorner(i);
    if (corner.r < rmin) rmin = corner.r;
    if (corner.r > rmax) rmax = corner.r;
    if (corner.z < zmin) zmin = corner.z;
    if (corner.z > zmax) zmax = corner.z;
  }

  G4double sphi    = GetStartPhi();
  G4double ephi    = GetEndPhi();
  G4double dphi    = IsOpen() ? ephi-sphi : twopi;
  G4int    ksteps  = GetNumSide();
  G4double astep   = dphi/ksteps;
  G4double sinStep = std::sin(astep);
  G4double cosStep = std::cos(astep);

  G4double sinCur = GetSinStartPhi();
  G4double cosCur = GetCosStartPhi();
  if (!IsOpen()) rmin = 0.;
  G4double xmin = rmin*cosCur, xmax = xmin;
  G4double ymin = rmin*sinCur, ymax = ymin;
  for (G4int k=0; k<ksteps+1; ++k)
  {
    G4double x = rmax*cosCur;
    if (x < xmin) xmin = x;
    if (x > xmax) xmax = x;
    G4double y = rmax*sinCur;
    if (y < ymin) ymin = y;
    if (y > ymax) ymax = y;
    if (rmin > 0.)
    {
      G4double xx = rmin*cosCur;
      if (xx < xmin) xmin = xx;
      if (xx > xmax) xmax = xx;
      G4double yy = rmin*sinCur;
      if (yy < ymin) ymin = yy;
      if (yy > ymax) ymax = yy;
    }
    G4double sinTmp = sinCur;
    sinCur = sinCur*cosStep + cosCur*sinStep;
    cosCur = cosCur*cosStep - sinTmp*sinStep;
  }
  pMin.set(xmin,ymin,zmin);
  pMax.set(xmax,ymax,zmax);

  // Check correctness of the bounding box
  //
  if (pMin.x() >= pMax.x() || pMin.y() >= pMax.y() || pMin.z() >= pMax.z())
  {
    std::ostringstream message;
    message << "Bad bounding box (min >= max) for solid: "
            << GetName() << " !"
            << "\npMin = " << pMin
            << "\npMax = " << pMax;
    G4Exception("G4UPolyhedra::BoundingLimits()", "GeomMgt0001",
                JustWarning, message);
    StreamInfo(G4cout);
  }

  // Check consistency of bounding boxes
  //
  if (checkBBox)
  {
    U3Vector vmin, vmax;
    Extent(vmin,vmax);
    if (std::abs(pMin.x()-vmin.x()) > kCarTolerance ||
        std::abs(pMin.y()-vmin.y()) > kCarTolerance ||
        std::abs(pMin.z()-vmin.z()) > kCarTolerance ||
        std::abs(pMax.x()-vmax.x()) > kCarTolerance ||
        std::abs(pMax.y()-vmax.y()) > kCarTolerance ||
        std::abs(pMax.z()-vmax.z()) > kCarTolerance)
    {
      std::ostringstream message;
      message << "Inconsistency in bounding boxes for solid: "
              << GetName() << " !"
              << "\nBBox min: wrapper = " << pMin << " solid = " << vmin
              << "\nBBox max: wrapper = " << pMax << " solid = " << vmax;
      G4Exception("G4UPolyhedra::BoundingLimits()", "GeomMgt0001",
                  JustWarning, message);
      checkBBox = false;
    }
  }

  // Check consistency of angles
  //
  if (checkPhi)
  {
    if (GetStartPhi() != GetPhiStart()  ||
        GetEndPhi()   != GetPhiEnd()    ||
        GetNumSide()  != GetSideCount() ||
        IsOpen()      != (Base_t::GetPhiDelta() < twopi))
    {
      std::ostringstream message;
      message << "Inconsistency in Phi angles or # of sides for solid: "
              << GetName() << " !"
              << "\nPhi start  : wrapper = " << GetStartPhi()
              << " solid = " <<     GetPhiStart()
              << "\nPhi end    : wrapper = " << GetEndPhi()
              << " solid = " <<     GetPhiEnd()
              << "\nPhi # sides: wrapper = " << GetNumSide()
              << " solid = " <<     GetSideCount()
              << "\nPhi is open: wrapper = " << (IsOpen() ? "true" : "false")
              << " solid = "
              << ((Base_t::GetPhiDelta() < twopi) ? "true" : "false");
      G4Exception("G4UPolyhedra::BoundingLimits()", "GeomMgt0001",
                  JustWarning, message);
      checkPhi = false;
    }
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Calculate extent under transform and specified limit

G4bool
G4UPolyhedra::CalculateExtent(const EAxis pAxis,
                              const G4VoxelLimits& pVoxelLimit,
                              const G4AffineTransform& pTransform,
                                    G4double& pMin, G4double& pMax) const
{
  G4ThreeVector bmin, bmax;
  G4bool exist;

  // Check bounding box (bbox)
  //
  BoundingLimits(bmin,bmax);
  G4BoundingEnvelope bbox(bmin,bmax);
#ifdef G4BBOX_EXTENT
  if (true) return bbox.CalculateExtent(pAxis,pVoxelLimit,pTransform,pMin,pMax);
#endif
  if (bbox.BoundingBoxVsVoxelLimits(pAxis,pVoxelLimit,pTransform,pMin,pMax))
  {
    return exist = pMin < pMax;
  }

  // To find the extent, RZ contour of the polycone is subdivided
  // in triangles. The extent is calculated as cumulative extent of
  // all sub-polycones formed by rotation of triangles around Z
  //
  G4TwoVectorList contourRZ;
  G4TwoVectorList triangles;
  std::vector<G4int> iout;
  G4double eminlim = pVoxelLimit.GetMinExtent(pAxis);
  G4double emaxlim = pVoxelLimit.GetMaxExtent(pAxis);

  // get RZ contour, ensure anticlockwise order of corners
  for (G4int i=0; i<GetNumRZCorner(); ++i)
  {
    G4PolyhedraSideRZ corner = GetCorner(i);
    contourRZ.emplace_back(corner.r,corner.z);
  }
  G4GeomTools::RemoveRedundantVertices(contourRZ,iout,2*kCarTolerance);
  G4double area = G4GeomTools::PolygonArea(contourRZ);
  if (area < 0.) std::reverse(contourRZ.begin(),contourRZ.end());

  // triangulate RZ countour
  if (!G4GeomTools::TriangulatePolygon(contourRZ,triangles))
  {
    std::ostringstream message;
    message << "Triangulation of RZ contour has failed for solid: "
            << GetName() << " !"
            << "\nExtent has been calculated using boundary box";
    G4Exception("G4UPolyhedra::CalculateExtent()",
                "GeomMgt1002",JustWarning,message);
    return bbox.CalculateExtent(pAxis,pVoxelLimit,pTransform,pMin,pMax);
  }

  // set trigonometric values
  G4double sphi     = GetStartPhi();
  G4double ephi     = GetEndPhi();
  G4double dphi     = IsOpen() ? ephi-sphi : twopi;
  G4int    ksteps   = GetNumSide();
  G4double astep    = dphi/ksteps;
  G4double sinStep  = std::sin(astep);
  G4double cosStep  = std::cos(astep);
  G4double sinStart = GetSinStartPhi();
  G4double cosStart = GetCosStartPhi();

  // allocate vector lists
  std::vector<const G4ThreeVectorList *> polygons;
  polygons.resize(ksteps+1);
  for (G4int k=0; k<ksteps+1; ++k)
  {
    polygons[k] = new G4ThreeVectorList(3);
  }

  // main loop along triangles
  pMin =  kInfinity;
  pMax = -kInfinity;
  G4int ntria = triangles.size()/3;
  for (G4int i=0; i<ntria; ++i)
  {
    G4double sinCur = sinStart;
    G4double cosCur = cosStart;
    G4int i3 = i*3;
    for (G4int k=0; k<ksteps+1; ++k) // rotate triangle
    {
      auto ptr = const_cast<G4ThreeVectorList*>(polygons[k]);
      auto iter = ptr->begin();
      iter->set(triangles[i3+0].x()*cosCur,
                triangles[i3+0].x()*sinCur,
                triangles[i3+0].y());
      iter++;
      iter->set(triangles[i3+1].x()*cosCur,
                triangles[i3+1].x()*sinCur,
                triangles[i3+1].y());
      iter++;
      iter->set(triangles[i3+2].x()*cosCur,
                triangles[i3+2].x()*sinCur,
                triangles[i3+2].y());

      G4double sinTmp = sinCur;
      sinCur = sinCur*cosStep + cosCur*sinStep;
      cosCur = cosCur*cosStep - sinTmp*sinStep;
    }

    // set sub-envelope and adjust extent
    G4double emin,emax;
    G4BoundingEnvelope benv(polygons);
    if (!benv.CalculateExtent(pAxis,pVoxelLimit,pTransform,emin,emax)) continue;
    if (emin < pMin) pMin = emin;
    if (emax > pMax) pMax = emax;
    if (eminlim > pMin && emaxlim < pMax) break; // max possible extent
  }
  // free memory
  for (G4int k=0; k<ksteps+1; ++k) { delete polygons[k]; polygons[k]=nullptr;}
  return (pMin < pMax);
}


////////////////////////////////////////////////////////////////////////
//
// CreatePolyhedron
//
G4Polyhedron* G4UPolyhedra::CreatePolyhedron() const
{
  // Check the validity of the delta phi
  G4double deltaPhi = wrDelta;
  if (deltaPhi <= 0. || deltaPhi >= twopi*(1-DBL_EPSILON))
  {
    deltaPhi = twopi;
  }
  return new G4PolyhedronPgon(wrStart, deltaPhi, wrNumSide, rzcorners);
}

#endif  // G4GEOM_USE_USOLIDS
