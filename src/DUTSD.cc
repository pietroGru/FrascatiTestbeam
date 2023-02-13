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
/// \file DUTSD.cc
/// \brief Implementation of the DUTSD class

#include "DUTSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4ios.hh"

#include "G4EventManager.hh"
#include "G4Event.hh"

#include "Analysis.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DUTSD::DUTSD(const G4String& name, const G4String& hitsCollectionName, G4int nofLayers)
 : G4VSensitiveDetector(name),
   fHitsCollection(nullptr),
   fNofLayers(nofLayers)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DUTSD::~DUTSD(){
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DUTSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection
  fHitsCollection = new DUTHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce
  auto hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 

  // Create hits
  // fNofLayers for cells + one more for total sums 
  for (G4int i=0; i<fNofLayers+3; i++ ) {
    fHitsCollection->insert(new DUTHit());
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool DUTSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
  // energy deposit
  auto edep = step->GetTotalEnergyDeposit();

  // step length
  G4double stepLength = 0.;
  if ( step->GetTrack()->GetDefinition()->GetPDGCharge() != 0. ) {
    stepLength = step->GetStepLength();
  }

  if ( edep==0. && stepLength == 0. ) return false;      

  G4ThreeVector edepPos = ( step->GetPostStepPoint()->GetPosition() + step->GetPreStepPoint()->GetPosition() ) / 2;

  auto touchable = (step->GetPreStepPoint()->GetTouchable());  
  // Get sensor layer id 
  auto layerNumber = touchable->GetCopyNumber(0);
  //G4cout << "Layer number: " << layerNumber << G4endl;
  
  // Get layer name. This is either "Sapphire wafer 110 um (layer)" or "Sapphire wafer 150 um (layer)"
  auto layerName = touchable->GetVolume()->GetName();
  G4String detectorName = (layerName=="Sapphire wafer 110 um (layer)") ? "110um" : "150um";

  // Get hit accounting data for this layer
  auto hit = (*fHitsCollection)[layerNumber];
  if ( ! hit ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hit " << layerNumber; 
    G4Exception("DUTSD::ProcessHits()",
      "MyCode0004", FatalException, msg);
  }         

  // Get hit for total accounting
  auto hitTotal = (*fHitsCollection)[fHitsCollection->entries()-1];
  
  // Get hit for large pad
  auto hitTotalLarge = (*fHitsCollection)[fHitsCollection->entries()-3];
  auto hitTotalSmall = (*fHitsCollection)[fHitsCollection->entries()-2];

  // Add values
  hit->Add(edep, edepPos);
  hitTotal->Add(edep);
  
  auto planeRadius2 = (edepPos.getX()*edepPos.getX() + edepPos.getY()*edepPos.getY())/mm2;
  auto preStep = step->GetPreStepPoint()->GetPosition();
  auto postStep = step->GetPostStepPoint()->GetPosition();
  auto planeRadius2Pre = preStep.getX()*preStep.getX() + preStep.getY()*preStep.getY();
  auto planeRadius2Post = postStep.getX()*postStep.getX() + postStep.getY()*postStep.getY();
  double largePadRadius2 = (5.50/2.0*mm); largePadRadius2 *= largePadRadius2;
  double smallPadRadius2 = (1.6/2*mm); smallPadRadius2 *= smallPadRadius2;
  if(planeRadius2 < largePadRadius2 && planeRadius2Pre < largePadRadius2 && planeRadius2Post < largePadRadius2){
    hitTotalLarge->Add(edep);
  }
  if(planeRadius2 <= (1.6/2)*(1.6/2)*mm2){
    hitTotalSmall->Add(edep);
  }
  
  // Kinetic energy of the track entering the DUT (accounting for energy lost in the 100 nm metal layer)
  if(step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary && layerNumber == 0){
    G4double ekinTrack = step->GetPreStepPoint()->GetKineticEnergy();    

    auto analysisManager = G4AnalysisManager::Instance();
    int histNm = (detectorName=="110um" ? 3 : 8);
    analysisManager->FillH1(histNm, ekinTrack);
  }

  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DUTSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     auto nofHits = fHitsCollection->entries();
     G4cout
       << G4endl 
       << "-------->Hits Collection: in this event they are " << nofHits 
       << " hits in the tracker chambers: " << G4endl;
     for ( std::size_t i=0; i<nofHits; ++i ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
