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
/// \file electromagnetic/TestEm4/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DUTSD.hh"
#include "FitpixSD.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4AssemblyVolume.hh"

#include "G4SDManager.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  :G4VUserDetectorConstruction(),
   fANbofLayers(110),
   fBNbofLayers(150),
   fFitpixNbofLayers(100)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  // Define materials 
  DefineMaterials();
  
  // Define volumes
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
  // Lead material defined using NIST Manager
  G4NistManager* man = G4NistManager::Instance();
  man->FindOrBuildMaterial("G4_AIR");
  man->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");
  man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  man->FindOrBuildMaterial("G4_Al");
  man->FindOrBuildMaterial("G4_Ti");
  man->FindOrBuildMaterial("G4_Si");
  man->FindOrBuildMaterial("G4_SILICON_DIOXIDE");


  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::DefineVolumes()
{  
  // Create required elements:
  auto H = new G4Element("Hydrogen", "H", 1., 1.01 * CLHEP::g / CLHEP::mole);
  auto C = new G4Element("Carbon", "C", 6., 12.01 * CLHEP::g / CLHEP::mole);
  auto O = new G4Element("Oxygen", "O", 8., 16.0 * CLHEP::g / CLHEP::mole);
  auto Cl = new G4Element("Chlorine", "Cl", 17., 35.45 * CLHEP::g / CLHEP::mole);
  auto Zn = new G4Element("Zinc", "Zn", 30., 65.38 * CLHEP::g / CLHEP::mole);
  auto Al = new G4Element("Aluminum", "Al", 13., 26.982 * CLHEP::g / CLHEP::mole);

  // Get materials
  auto defaultMaterial  = G4Material::GetMaterial("G4_AIR");
  auto sapphireMat = G4Material::GetMaterial("G4_ALUMINUM_OXIDE");
  auto plasticMat = G4Material::GetMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
  auto AlMat = G4Material::GetMaterial("G4_Al");
  auto TiMat = G4Material::GetMaterial("G4_Ti");
  auto SiMat = G4Material::GetMaterial("G4_Si");
  auto SiOMat = G4Material::GetMaterial("G4_SILICON_DIOXIDE");

  // Create Zamak 3 material
  auto Zamak3Mat = new G4Material("Zamak3", 6.7 * CLHEP::g / CLHEP::cm3, 2);
  Zamak3Mat->AddElement(Zn, 96);
  Zamak3Mat->AddElement(Al, 4);

  // Create Epoxy material
  auto EpoxyMat = new G4Material("Epoxy", 1.3 * CLHEP::g / CLHEP::cm3, 3);
  EpoxyMat->AddElement(H, 44);
  EpoxyMat->AddElement(C, 15);
  EpoxyMat->AddElement(O, 7);

  // Create PCB G-10 material
  auto pcbMat = new G4Material("G10", 1.7 * CLHEP::g / CLHEP::cm3, 3);
  pcbMat->AddMaterial(SiOMat, 0.773);
  pcbMat->AddMaterial(EpoxyMat, 0.147);
  pcbMat->AddElement(Cl, 0.08);


  if ( ! defaultMaterial || ! sapphireMat || ! plasticMat || ! pcbMat ) {
    G4ExceptionDescription msg;
    msg << "Cannot retrieve materials already defined."; 
    G4Exception("DetectorConstruction::DefineVolumes()",
      "MyCode0001", FatalException, msg);
  }  

  // World
  const double worldSize = 150*cm; 
  G4Box* worldS = new G4Box("World", worldSize /2., worldSize /2., worldSize /2.);
  G4LogicalVolume* worldL = new G4LogicalVolume(worldS, defaultMaterial, "World");
  G4VPhysicalVolume* worldP = new G4PVPlacement(0, G4ThreeVector(), worldL, "World", 0, false, 0);


  
  // Sapphire pad 110 um parameters
  auto fPadMetalizationThickness = 100 * nm;
	auto fWaferThickness = 110 * um;
  const double fWaferLayerNb = fANbofLayers;
  const double inch = 25.4*mm;
  // Sapphire pad 150 um parameters
	auto fWaferBThickness = 150 * um;
  const double fWaferBLayerNb = fBNbofLayers;
  // PCB parameters
  auto pcbThickness = 1.57*mm;
  auto pcbSizeXY = 15*cm;
  //
  // Sensor pad 110 um assembly
  auto sapphire110WrapperS = new G4Box("Sapphire 110um wrapper", pcbSizeXY / 2 , pcbSizeXY /2, (fWaferThickness+2*fPadMetalizationThickness+pcbThickness)/2);
  auto sapphire110WrapperL = new G4LogicalVolume(sapphire110WrapperS, defaultMaterial, "Sapphire 110um wrapper");
  //
  // Sensor pad 150 um assembly
  auto sapphire150WrapperS = new G4Box("Sapphire 110um wrapper", pcbSizeXY / 2 , pcbSizeXY /2, (fWaferBThickness+2*fPadMetalizationThickness+pcbThickness)/2);
  auto sapphire150WrapperL = new G4LogicalVolume(sapphire150WrapperS, defaultMaterial, "Sapphire 150um wrapper");
  //
  // Sapphire pad 110 um
  auto sapphireWaferS = new G4Tubs("Sapphire wafer", 0, 1*inch, fWaferThickness / 2., 0, 2*pi);
  auto sapphireWaferLayerS = new G4Tubs("Sapphire wafer", 0, 1*inch, (fWaferThickness/fWaferLayerNb) / 2., 0, 2*pi);
  auto sapphirePadLargeMetS = new G4Tubs("Sapphire pad large metallization (d=5.50)", 0, (5.50/2)*mm, fPadMetalizationThickness / 2., 0, 2*pi);
  auto sapphirePadSmallMetS = new G4Tubs("Sapphire pad small metallization (d=1.60)", 0, (1.60/2)*mm, fPadMetalizationThickness / 2., 0, 2*pi);
  auto sapphirePadHalfMoonBoolAS = new G4Tubs("Sapphire pad half-moon (boolA)", 0, 22*mm, fPadMetalizationThickness/2, 0, pi);
  auto sapphirePadHalfMoonBoolBS = new G4Box("Sapphire pad half-moon (boolB)", 44*mm /2, 24*mm /2, 1*cm /2);
  G4SubtractionSolid* sapphirePadHalfMoonS = new G4SubtractionSolid("Sapphire pad half-moon", sapphirePadHalfMoonBoolAS, sapphirePadHalfMoonBoolBS, 0, G4ThreeVector(0));  
  G4LogicalVolume* sapphireWaferL = new G4LogicalVolume(sapphireWaferS, sapphireMat, "Sapphire wafer 110 um");
  sapphireWaferLayerL = new G4LogicalVolume(sapphireWaferLayerS, sapphireMat, "Sapphire wafer 110 um (layer)");
  G4LogicalVolume* sapphirePadLargeMetL = new G4LogicalVolume(sapphirePadLargeMetS, AlMat, "Sapphire pad large metallization (d=5.50)");
  G4LogicalVolume* sapphirePadSmallMetL = new G4LogicalVolume(sapphirePadSmallMetS, AlMat, "Sapphire pad small metallization (d=1.60)");
  G4LogicalVolume* sapphirePadHalfMoonL = new G4LogicalVolume(sapphirePadHalfMoonS, AlMat, "Sapphire pad half-moon");
  G4ThreeVector sapphireWaferPos(0,0,0);
  G4ThreeVector halfMoonPos = sapphireWaferPos;
  G4ThreeVector padLarge1Pos = G4ThreeVector(-10.5*mm, -15*mm, 0);
  G4ThreeVector padLarge2Pos = G4ThreeVector(10.5*mm, -15*mm, 0);
  G4ThreeVector padLarge3Pos = G4ThreeVector(10.5*mm, 15*mm, 0);
  G4ThreeVector padLarge4Pos = G4ThreeVector(-10.5*mm, 15*mm, 0);
  G4ThreeVector padSmall1Pos = G4ThreeVector(-5*mm, -20*mm, 0);
  G4ThreeVector padSmall2Pos = G4ThreeVector(5*mm, -20*mm, 0);
  G4ThreeVector padSmall3Pos = G4ThreeVector(5*mm, 20*mm, 0);
  G4ThreeVector padSmall4Pos = G4ThreeVector(-5*mm, 20*mm, 0);
  double zMetSurfBoundaryPos = fWaferThickness/2+fPadMetalizationThickness/2;
  new G4PVPlacement(0, sapphireWaferPos, sapphireWaferL, "Sapphire wafer 110 um", sapphire110WrapperL, false, 0, false);
  static double layerThickness = fWaferThickness/fWaferLayerNb;
  G4ThreeVector layerPos = G4ThreeVector(0, 0, fWaferThickness/2 - layerThickness/2);
  for(G4int i=0; i<fWaferLayerNb; i++){
    new G4PVPlacement(0, layerPos, sapphireWaferLayerL, "Sapphire wafer 110 um (layer)", sapphireWaferL, false, i, false);
    layerPos -= G4ThreeVector(0, 0, layerThickness);
  }
  new G4PVPlacement(0, padLarge1Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadLargeMetL, "Sapphire pad 1 large metallization top", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padLarge1Pos - G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadLargeMetL, "Sapphire pad 1 large metallization bottom", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padLarge2Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadLargeMetL, "Sapphire pad 2 large metallization top", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padLarge2Pos - G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadLargeMetL, "Sapphire pad 2 large metallization bottom", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padLarge3Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadLargeMetL, "Sapphire pad 3 large metallization top", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padLarge4Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadLargeMetL, "Sapphire pad 4 large metallization top", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, halfMoonPos  - G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadHalfMoonL, "Sapphire half-moon metallization", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padSmall1Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadSmallMetL, "Sapphire pad 1 small metallization top", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padSmall1Pos - G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadSmallMetL, "Sapphire pad 1 small metallization bottom", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padSmall2Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadSmallMetL, "Sapphire pad 2 small metallization top", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padSmall2Pos - G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadSmallMetL, "Sapphire pad 2 small metallization bottom", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padSmall3Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadSmallMetL, "Sapphire pad 3 small metallization top", sapphireWaferL, false, 0, false);
  new G4PVPlacement(0, padSmall4Pos + G4ThreeVector(0,0,zMetSurfBoundaryPos), sapphirePadSmallMetL, "Sapphire pad 4 small metallization top", sapphireWaferL, false, 0, false);
  

  // Sapphire pad 150 um
  auto sapphireWaferBS = new G4Tubs("Sapphire wafer", 0, 1*inch, fWaferBThickness / 2., 0, 2*pi);
  auto sapphireWaferBLayerS = new G4Tubs("Sapphire Wafer", 0, 1*inch, (fWaferBThickness/fWaferBLayerNb) / 2., 0, 2*pi);
  G4LogicalVolume* sapphireWaferBL = new G4LogicalVolume(sapphireWaferBS, sapphireMat, "Sapphire wafer 150 um");
  G4LogicalVolume* sapphireWaferBLayerL = new G4LogicalVolume(sapphireWaferBLayerS, sapphireMat, "Sapphire wafer 150 um (layer)");

  double zMetSurfBoundaryBPos = fWaferBThickness/2+fPadMetalizationThickness/2;
  new G4PVPlacement(0, sapphireWaferPos, sapphireWaferBL, "Sapphire wafer 150 um", sapphire150WrapperL, false, 0, false);
  static double layerBThickness = fWaferBThickness/fWaferBLayerNb;
  layerPos = G4ThreeVector(0, 0, fWaferBThickness/2 - layerBThickness/2);
  for(G4int i=0; i<fWaferBLayerNb; i++){
    new G4PVPlacement(0, layerPos, sapphireWaferBLayerL, "Sapphire wafer 150 um (layer)", sapphireWaferBL, false, i, false);
    layerPos -= G4ThreeVector(0, 0, layerBThickness);
  }
  new G4PVPlacement(0, padLarge1Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadLargeMetL, "Sapphire pad 1 large metallization top", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padLarge1Pos - G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadLargeMetL, "Sapphire pad 1 large metallization bottom", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padLarge2Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadLargeMetL, "Sapphire pad 2 large metallization top", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padLarge2Pos - G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadLargeMetL, "Sapphire pad 2 large metallization bottom", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padLarge3Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadLargeMetL, "Sapphire pad 3 large metallization top", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padLarge4Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadLargeMetL, "Sapphire pad 4 large metallization top", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, halfMoonPos  - G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadHalfMoonL, "Sapphire half-moon metallization", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padSmall1Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadSmallMetL, "Sapphire pad 1 small metallization top", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padSmall1Pos - G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadSmallMetL, "Sapphire pad 1 small metallization bottom", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padSmall2Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadSmallMetL, "Sapphire pad 2 small metallization top", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padSmall2Pos - G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadSmallMetL, "Sapphire pad 2 small metallization bottom", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padSmall3Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadSmallMetL, "Sapphire pad 3 small metallization top", sapphireWaferBL, false, 0, false);
  new G4PVPlacement(0, padSmall4Pos + G4ThreeVector(0,0,zMetSurfBoundaryBPos), sapphirePadSmallMetL, "Sapphire pad 4 small metallization top", sapphireWaferBL, false, 0, false);



  // PCB support
  G4Box* pcbS = new G4Box("PCB", pcbSizeXY /2, pcbSizeXY /2, pcbThickness /2);
  G4LogicalVolume* pcbL = new G4LogicalVolume(pcbS, pcbMat, "PCB");
  G4ThreeVector pcb110Pos = - G4ThreeVector(0, 0, (fWaferThickness+fPadMetalizationThickness+pcbThickness)/2);
  G4ThreeVector pcb150Pos = - G4ThreeVector(0, 0, (fWaferBThickness+fPadMetalizationThickness+pcbThickness)/2);
  new G4PVPlacement(0, pcb110Pos, pcbL, "PCB 110um", sapphire110WrapperL, false, 0, false);
  new G4PVPlacement(0, pcb150Pos, pcbL, "PCB 150um", sapphire150WrapperL, false, 0, false);

 

  // Canister box for the detector assembly
  double boxSizeXY = 190*mm;
  double boxSizeZ = 70*mm;
  double boxThickness = 3*mm;
  double boxHoleRadius = 180*mm;
  //double boxHoleRadius = 185*mm;
  G4Box* dutBoxVolA_S = new G4Box("DUTbox_VolA", boxSizeXY / 2., boxSizeXY / 2., boxSizeZ /2.);
  G4Box* dutBoxVolB_S = new G4Box("DUTbox_VolB", (boxSizeXY-boxThickness) / 2., (boxSizeXY-boxThickness) / 2., (boxSizeZ-boxThickness) /2.);
  G4Tubs* dutBoxVolC_S = new G4Tubs("DUTbox_VolC (hole)", 0, boxHoleRadius / 2., boxThickness, 0, 2*pi);
  G4SubtractionSolid* dutBoxS = new G4SubtractionSolid("DUTboxVolA-B-C", dutBoxVolA_S, dutBoxVolB_S, 0, G4ThreeVector());  
  G4ThreeVector holePos(0, 0, (boxSizeZ - boxThickness)/2);
  G4SubtractionSolid* dutBoxS2 = new G4SubtractionSolid("DUT box", dutBoxS, dutBoxVolC_S, 0, holePos);
  //
  G4LogicalVolume* dutBoxL = new G4LogicalVolume(dutBoxS2, Zamak3Mat, "DUT box");

  
  // Aluminum foil (thickness 3/10mm)
  double dutBoxCoverFoilThickness = 3.0/10*mm;
  G4Box* dutBoxCoverS = new G4Box("DUT box cover (Al 0.3mm)", boxSizeXY /2, boxSizeXY /2, dutBoxCoverFoilThickness /2);
  G4LogicalVolume* dutBoxCoverL = new G4LogicalVolume(dutBoxCoverS, AlMat, "DUT box cover (Al 0.3mm)");
  G4ThreeVector dutBoxCoverPos = G4ThreeVector(0, 0, (boxSizeZ+dutBoxCoverFoilThickness)/2);
  new G4PVPlacement(0, dutBoxCoverPos, dutBoxCoverL, "Box cover (Al 0.3mm)", dutBoxL, false, 0, false);
  

  // Main placement of the DUTs assembly
  G4AssemblyVolume* dutBoxWrapper = new G4AssemblyVolume();
  G4Transform3D sapphire110WrapperRotPos = G4Transform3D(G4RotationMatrix(0,0,0), G4ThreeVector(0,0,1*cm));
  dutBoxWrapper->AddPlacedVolume(sapphire110WrapperL, sapphire110WrapperRotPos);
  G4Transform3D sapphire150WrapperRotPos = G4Transform3D(G4RotationMatrix(0,0,0), G4ThreeVector(0,0,-1*cm));
  dutBoxWrapper->AddPlacedVolume(sapphire150WrapperL, sapphire150WrapperRotPos);
  G4Transform3D canisterRotPos = G4Transform3D(G4RotationMatrix(0,0,0), G4ThreeVector(0,0,0));
  dutBoxWrapper->AddPlacedVolume(dutBoxL, canisterRotPos);
  //
  G4Transform3D dutBoxRotPos = G4Transform3D(G4RotationMatrix(0,0,0), -padSmall4Pos);
  dutBoxWrapper->MakeImprint(worldL, dutBoxRotPos);


  // Additional geometry in the beamline (from @Luca's email)
  // Ti surface 50um
  // 20 cm air
  // Silicon pixel detector (300um) with asic (400um)
  // 10 cm air
  //
  // measured air gaps(using a ruler) are 33cm(window-pixel) and 22cm(pixel-box)
  double transvSize = 5*cm;
  double tiSurfThickness = 50*um;
  double siPixelDetThickness = (300+400)*um;
  G4Box* tiSurfS = new G4Box("Ti surface", transvSize / 2., transvSize / 2., tiSurfThickness / 2.);
  G4LogicalVolume* tiSurfL = new G4LogicalVolume(tiSurfS, TiMat, "Ti surface");
  G4Box* siPixelDetS = new G4Box("Silicon pixel detector", transvSize / 2., transvSize / 2., (siPixelDetThickness-400*um) / 2.);
  G4Box* siPixelDetLayerS = new G4Box("Silicon pixel detector (layer)", transvSize / 2., transvSize / 2., (siPixelDetThickness-400*um)/fFitpixNbofLayers / 2.);
  G4Box* siPixelDetAsicS = new G4Box("Silicon pixel detector (asic)", transvSize / 2., transvSize / 2., (siPixelDetThickness-300*um) / 2.);
  G4LogicalVolume* siPixelDetL = new G4LogicalVolume(siPixelDetS, SiMat, "Silicon pixel detector");
  G4LogicalVolume* siPixelDetLayerL = new G4LogicalVolume(siPixelDetLayerS, SiMat, "Silicon pixel detector (layer)");
  G4LogicalVolume* siPixelDetAsicL = new G4LogicalVolume(siPixelDetAsicS, SiMat, "Silicon pixel detector (asic)");
  G4ThreeVector siPixelDetPos = sapphireWaferPos + G4ThreeVector(0, 0, 22*cm);
  G4ThreeVector tiSurfPos = siPixelDetPos + G4ThreeVector(0, 0, 33*cm);
  //
  new G4PVPlacement(0, tiSurfPos, tiSurfL, "Ti surface", worldL, 0, false);
  //
  new G4PVPlacement(0, siPixelDetPos, siPixelDetL, "Silicon pixel detector", worldL, 0, false);
  static double layerFitpixThickness = (siPixelDetThickness-400*um)/fFitpixNbofLayers;
  layerPos = G4ThreeVector(0, 0, (siPixelDetThickness-400*um)/2 - layerFitpixThickness/2);
  for(G4int i=0; i<fFitpixNbofLayers; i++){
    new G4PVPlacement(0, layerPos, siPixelDetLayerL, "Silicon pixel detector (layer)", siPixelDetL, false, i, false);
    layerPos -= G4ThreeVector(0, 0, layerFitpixThickness);
  }
  new G4PVPlacement(0, G4ThreeVector(0, 0, -siPixelDetThickness/2), siPixelDetAsicL, "Silicon pixel detector (asic)", siPixelDetL, false, 0);



	// Detector placement
	// X-Axis detector (strips parallel to the x)


  // Vis attributes
  // Wrapper
  G4VisAttributes wrapperVisAtt = G4VisAttributes(G4Color(1, 0, 0, 0.1)); // Red
  wrapperVisAtt.SetVisibility(false);
  double alpha = 0.4;
  // Container
  auto containerBoxColor = G4Color(0.36, 0.36, 0.36, alpha); // Greyish
  G4VisAttributes containerBoxVisAtt = G4VisAttributes(containerBoxColor);
  containerBoxVisAtt.SetLineWidth(1);
  containerBoxVisAtt.SetForceSolid(false);
  // Support
  auto supportColor = G4Color(0.36, 0.66, 0.055, alpha); // Greenish
  G4VisAttributes supportVisAtt = G4VisAttributes(supportColor);
  supportVisAtt.SetLineWidth(1);
  supportVisAtt.SetForceSolid(false);
  // Sensors, ie pixels
  auto sensorColor = G4Color(0.18, 0.2, 0.21, alpha); // Blackish
  G4VisAttributes SensorVisAtt = G4VisAttributes(sensorColor);
  SensorVisAtt.SetForceSolid(false);
  // Chip
  auto chipColor = G4Color(0.7, 0.03, 0.03, 1.0); // Blackish
  G4VisAttributes ChipVisAtt = G4VisAttributes(chipColor);
  ChipVisAtt.SetForceSolid(false);
  // Pad/strip/metallization
  auto metColor = G4Color(0.36, 0.055, 0.66, 1.0); // Bluish
  G4VisAttributes StripVisAtt = G4VisAttributes(metColor);
  StripVisAtt.SetLineWidth(1);
  StripVisAtt.SetForceSolid(true);

  worldL->SetVisAttributes(wrapperVisAtt);
  sapphire110WrapperL->SetVisAttributes(wrapperVisAtt);
  sapphire150WrapperL->SetVisAttributes(wrapperVisAtt);

  sapphireWaferL->SetVisAttributes(SensorVisAtt);
  sapphireWaferLayerL->SetVisAttributes(SensorVisAtt);
  sapphireWaferBL->SetVisAttributes(SensorVisAtt);
  sapphireWaferBLayerL->SetVisAttributes(SensorVisAtt);
  
  sapphirePadLargeMetL->SetVisAttributes(StripVisAtt);
  sapphirePadSmallMetL->SetVisAttributes(StripVisAtt);
  sapphirePadHalfMoonL->SetVisAttributes(StripVisAtt);

  siPixelDetL->SetVisAttributes(SensorVisAtt);
  siPixelDetAsicL->SetVisAttributes(ChipVisAtt);
  dutBoxL->SetVisAttributes(containerBoxVisAtt);
  dutBoxCoverL->SetVisAttributes(containerBoxVisAtt);

  pcbL->SetVisAttributes(supportVisAtt);

  
  //
  // print parameters
  //
  G4cout
    << G4endl 
    << "------------------------------------------------------------" << G4endl
    << "---> a) The DUT is " << fWaferLayerNb << " layers of: "
    << layerThickness/mm << "mm of " << sapphireMat->GetName() 
    << G4endl
    << "------------------------------------------------------------" << G4endl;
  //
  G4cout
    << G4endl 
    << "------------------------------------------------------------" << G4endl
    << "---> b) The DUT is " << fWaferBLayerNb << " layers of: "
    << layerBThickness/mm << "mm of " << sapphireMat->GetName() 
    << G4endl
    << "------------------------------------------------------------" << G4endl;


  // Create Target G4Region and add logical volume
  //fRegion = new G4Region("Active sensor region");
  //fRegion->AddRootLogicalVolume(sapphireWaferLayerL);

  //
  //always return the physical World
  return worldP;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
  G4SDManager::GetSDMpointer()->SetVerboseLevel(1);
  
  // Sensitive detectors
  auto sensor110 = new DUTSD("Sensor 110 um", "DUTAHitsCollection", fANbofLayers);
  auto sensor150 = new DUTSD("Sensor 150 um", "DUTBHitsCollection", fBNbofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(sensor110);
  G4SDManager::GetSDMpointer()->AddNewDetector(sensor150);
  SetSensitiveDetector("Sapphire wafer 110 um (layer)", sensor110);
  SetSensitiveDetector("Sapphire wafer 150 um (layer)", sensor150);
  
  // Fitpix detector
  auto fitpix = new FitpixSD("Fitpix", "FitpixHitsCollection", fFitpixNbofLayers);
  G4SDManager::GetSDMpointer()->AddNewDetector(fitpix);
  SetSensitiveDetector("Silicon pixel detector (layer)", fitpix);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......