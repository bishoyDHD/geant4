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
// $Id: G4GDMLWrite.cc,v 1.44 2008-07-03 07:33:43 gcosmo Exp $
// GEANT4 tag $Name: not supported by cvs2svn $
//
// class G4GDMLWrite Implementation
//
// Original author: Zoltan Torzsok, November 2007
//
// --------------------------------------------------------------------

#include "G4GDMLWrite.hh"

G4bool G4GDMLWrite::addPointerToName = true;

G4bool G4GDMLWrite::FileExists(const G4String& fname) const {

  struct stat FileInfo;
  return (stat(fname.c_str(),&FileInfo) == 0); 
}

G4GDMLWrite::VolumeMapType& G4GDMLWrite::volumeMap() {

   static VolumeMapType instance;
   return instance;
}
// GC - 2
// Added !

G4GDMLWrite::VolumeListType& G4GDMLWrite::volumeList() {
// GC
// G4GDMLWrite::VolumeMapType& G4GDMLWrite::volumeMap() {

   static VolumeListType instance;
// GC
// static VolumeMapType instance;

   return instance;
}

G4GDMLWrite::DepthListType& G4GDMLWrite::depthList() {
// GC
// G4GDMLWrite::DepthMapType& G4GDMLWrite::depthMap() {

   static DepthListType instance;
// GC
// static DepthMapType instance;

   return instance;
}

G4String G4GDMLWrite::GenerateName(const G4String& name,const void* const ptr) {

   std::stringstream stream; stream << name;
   if (addPointerToName) stream << ptr; 
   return G4String(stream.str());
}

xercesc::DOMAttr* G4GDMLWrite::newAttribute(const G4String& name,const G4String& value) {

   xercesc::XMLString::transcode(name,tempStr,99);
   xercesc::DOMAttr* att = doc->createAttribute(tempStr);
   xercesc::XMLString::transcode(value,tempStr,99);
   att->setValue(tempStr);
   return att;
}

xercesc::DOMAttr* G4GDMLWrite::newAttribute(const G4String& name,const G4double& value) {

   xercesc::XMLString::transcode(name,tempStr,99);
   xercesc::DOMAttr* att = doc->createAttribute(tempStr);
   std::ostringstream ostream;
   ostream.precision(15);
   ostream << value;
   G4String str = ostream.str();
   xercesc::XMLString::transcode(str,tempStr,99);
   att->setValue(tempStr);
   return att;
}

xercesc::DOMElement* G4GDMLWrite::newElement(const G4String& name) {

   xercesc::XMLString::transcode(name,tempStr,99);
   return doc->createElement(tempStr);
}

G4Transform3D G4GDMLWrite::Write(const G4String& fname, const G4LogicalVolume* const logvol,
                                 const G4String& setSchemaLocation, const G4int depth) {

   SchemaLocation = setSchemaLocation;

   if (depth==0) G4cout << "G4GDML: Writing '" << fname << "'..." << G4endl;
   else G4cout << "G4GDML: Writing module '" << fname << "'..." << G4endl;
   
   if (FileExists(fname)) G4Exception("G4GDML: ERROR! File '"+fname+"' already exists!");

   if (depth==0) volumeMap().clear(); // The module map is global for all modules, so clear it only at once!
// GC - 2
// Added!

   xercesc::XMLString::transcode("LS", tempStr, 99);
   xercesc::DOMImplementation* impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);
   xercesc::DOMWriter* writer = ((xercesc::DOMImplementationLS*)impl)->createDOMWriter();

   xercesc::XMLString::transcode("Range", tempStr, 99);
   impl = xercesc::DOMImplementationRegistry::getDOMImplementation(tempStr);

   xercesc::XMLString::transcode("gdml", tempStr, 99);
   doc = impl->createDocument(0,tempStr,0);
   xercesc::DOMElement* gdml = doc->getDocumentElement();

   if (writer->canSetFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint,true))
      writer->setFeature(xercesc::XMLUni::fgDOMWRTFormatPrettyPrint,true);

   gdml->setAttributeNode(newAttribute("xmlns:xsi","http://www.w3.org/2001/XMLSchema-instance"));
   gdml->setAttributeNode(newAttribute("xsi:noNamespaceSchemaLocation",SchemaLocation));

   defineWrite(gdml);
   materialsWrite(gdml);
   solidsWrite(gdml);
   structureWrite(gdml);
   setupWrite(gdml,logvol);

   G4Transform3D R = TraverseVolumeTree(logvol,depth);

   xercesc::XMLFormatTarget *myFormTarget = new xercesc::LocalFileFormatTarget(fname.c_str());

   try {
      writer->writeNode(myFormTarget,*doc);
   } catch (const xercesc::XMLException& toCatch) {
   
      char* message = xercesc::XMLString::transcode(toCatch.getMessage());
      G4cout << "G4GDML: Exception message is: " << message << G4endl;
      xercesc::XMLString::release(&message);
      return G4Transform3D::Identity;
   } catch (const xercesc::DOMException& toCatch) {
   
      char* message = xercesc::XMLString::transcode(toCatch.msg);
      G4cout << "G4GDML: Exception message is: " << message << G4endl;
      xercesc::XMLString::release(&message);
      return G4Transform3D::Identity;
   } catch (...) {
      
      G4cout << "G4GDML: Unexpected Exception!" << G4endl;
      return G4Transform3D::Identity;
   }        

   delete myFormTarget;
   writer->release();

   if (depth==0) G4cout << "G4GDML: Writing '" << fname << "' done!" << G4endl;
   else G4cout << "G4GDML: Writing module '" << fname << "' done!" << G4endl;

   return R;
}

void G4GDMLWrite::AddModule(const G4VPhysicalVolume* const physvol) {
// GC
// void G4GDMLWrite::AddModule(const G4VPhysicalVolume* const physvol,const G4String& fname) {

   G4cout << "G4GDML: Adding module '" << GenerateName(physvol->GetName(),physvol) << "'..." << G4endl;
// GC
// G4cout << "G4GDML: Adding module '" << fname << "'..." << G4endl;

   if (physvol == 0) G4Exception("G4GDML: ERROR! Invalid pointer is specified for modularization!");
// GC
// if (volumeMap().find(physvol) != volumeMap().end()) G4Exception("G4GDML: ERROR! Module name '"+fname+"' already defined!");

   if (dynamic_cast<const G4PVDivision* const>(physvol)) G4Exception("G4GDML: ERROR! It is not possible to modularize by divisionvol!");
   if (physvol->IsParameterised()) G4Exception("G4GDML: ERROR! It is not possible to modularize by parameterised volume!");
   if (physvol->IsReplicated()) G4Exception("G4GDML: ERROR! It is not possible to modularize by replicated volume!");

   volumeList().push_back(physvol);
// GC
// volumeMap()[physvol] = fname;

}

void G4GDMLWrite::AddModule(const G4int depth) {

   G4cout << "G4GDML: Adding module(s) at depth " << depth << "..." << G4endl;
// GC
// if (depth<0) G4Exception("G4GDML: ERROR! Depth must be a positive number!");
// if (depthMap().find(depth) != depthMap().end()) G4Exception("G4GDML: ERROR! Adding module(s) at this depth is already requested!");

   depthList().push_back(depth);
// GC
// depthMap()[depth] = 0;

}

G4bool G4GDMLWrite::Modularize(const G4VPhysicalVolume* const topvol, const G4int depth) {

   for (size_t i = 0;i<volumeList().size();i++)
      if (volumeList()[i] == topvol) return true;

   for (size_t i = 0;i<depthList().size();i++)
      if (depthList()[i] == depth) return true;

   return false;
}
// GC
// G4String G4GDMLWrite::Modularize(const G4VPhysicalVolume* const physvol,const G4int depth) {
//
//   if (volumeMap().find(physvol) != volumeMap().end()) return volumeMap()[physvol]; // Modularize via physvol
//
//   if (depthMap().find(depth) != depthMap().end()) { // Modularize via depth
//
//     std::stringstream stream;
//     stream << "depth" << depth << "_module" << depthMap()[depth] << ".gdml";
//     depthMap()[depth]++; // There can be more modules at this depth!
//     return G4String(stream.str());
//   }
//   return G4String(""); // Empty string for module name = no modularization was requested at that level/physvol!
// }

void G4GDMLWrite::SetAddPointerToName(G4bool set) {

   addPointerToName = set;
}
