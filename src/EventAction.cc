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
/// \file electromagnetic/TestEm4/src/EventAction.cc
/// \brief Implementation of the EventAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"
#include "DUTSD.hh"
#include "DUTHit.hh"
#include "Analysis.hh"

#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4UnitsTable.hh"

#include "Randomize.hh"
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
:G4UserEventAction(),
 fdutAHCID(-1),
 fdutBHCID(-1),
 ffitpixHCID(-1),
 fTotalEnergyDeposit(0.),
 fTotalEnergyDeposit_dutB(0.),
 fTotalEnergyDeposit_fitpix(0.)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DUTHitsCollection* EventAction::GetHitsCollection(G4int hcID, const G4Event* event) const
{
  auto hitsCollection 
    = static_cast<DUTHitsCollection*>(
        event->GetHCofThisEvent()->GetHC(hcID));
  
  if ( ! hitsCollection ) {
    G4ExceptionDescription msg;
    msg << "Cannot access hitsCollection ID " << hcID; 
    G4Exception("EventAction::GetHitsCollection()",
      "MyCode0003", FatalException, msg);
  }         

  return hitsCollection;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::PrintEventStatistics(G4double dutEdep, G4double dutTrackLength) const
{
  // print event statistics
  G4cout
     << "   DUT: total energy: " 
     << std::setw(7) << G4BestUnit(dutEdep, "Energy")
     << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction( const G4Event* event)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event* event)
{                          
  auto primPart = event->GetPrimaryVertex()->GetPrimary();
  auto primPart_energy = primPart->GetKineticEnergy();

  // Get hits collections IDs (only once)
  if ( fdutAHCID == -1 || fdutBHCID == -1  ) {
    fdutAHCID = G4SDManager::GetSDMpointer()->GetCollectionID("DUTAHitsCollection");
    fdutBHCID = G4SDManager::GetSDMpointer()->GetCollectionID("DUTBHitsCollection");
  }

  // Get hits collections for the Fitpix SD (only once)
  if( ffitpixHCID == -1 ){
    ffitpixHCID = G4SDManager::GetSDMpointer()->GetCollectionID("FitpixHitsCollection");
  }

  // Get hits collections
  auto dutAHC = GetHitsCollection(fdutAHCID, event);
  auto dutBHC = GetHitsCollection(fdutBHCID, event);
  auto fitpixHC = GetHitsCollection(ffitpixHCID, event); 

  // Get hit with total values
  auto dutAHitsAll = (*dutAHC)[dutAHC->entries()-1];
  auto dutAHitsAllLarge = (*dutAHC)[dutAHC->entries()-3];
  auto dutAHitsAllSmall = (*dutAHC)[dutAHC->entries()-2];
  
  auto dutBHitsAll = (*dutBHC)[dutBHC->entries()-1];
  auto dutBHitsAllLarge = (*dutBHC)[dutBHC->entries()-3];
  auto dutBHitsAllSmall = (*dutBHC)[dutBHC->entries()-2];

  //auto fitpixHit = (*fitpixHC)[0];
  auto fitpixHitAll = (*fitpixHC)[fitpixHC->entries()-1];

  // Print per event (modulo n)
  //
  auto eventID = event->GetEventID();
  auto printModulo = G4RunManager::GetRunManager()->GetPrintProgress();
  if ( ( printModulo > 0 ) && ( eventID % printModulo == 0 ) ) {
    if(dutAHitsAll->GetEdep() > 0){
      G4cout << "---> End of event: " << eventID << G4endl;
      PrintEventStatistics(dutAHitsAll->GetEdep(), dutAHitsAll->GetTrackLength());
    }
  }  
  
  // Fill histograms, ntuple
  //
  // get analysis manager
  auto analysisManager = G4AnalysisManager::Instance();
  // fill primary vertex histogram
  analysisManager->FillH1(0, primPart_energy);
  //
  if(dutAHitsAll->GetEdep() > 0){  
    // fill histograms
    analysisManager->FillH1(1, dutAHitsAll->GetEdep());
    analysisManager->FillH1(2, dutAHitsAll->GetEdep()/(27.0*CLHEP::eV)/1000.0);
    //analysisManager->FillH1(3);
    if(dutAHitsAllLarge->GetEdep() > 0) analysisManager->FillH1(4, dutAHitsAllLarge->GetEdep());
    if(dutAHitsAllSmall->GetEdep() > 0) analysisManager->FillH1(5, dutAHitsAllSmall->GetEdep());
    //
    if(dutAHitsAllLarge->GetEdep() > 0){
      analysisManager->FillNtupleIColumn(2, 0, eventID);
      analysisManager->FillNtupleDColumn(2, 1, dutAHitsAllLarge->GetEdep());
      analysisManager->FillNtupleSColumn(2, 3, "110um");
      analysisManager->AddNtupleRow(2);
    }
    if(dutAHitsAllSmall->GetEdep() > 0){
      analysisManager->FillNtupleIColumn(2, 0, eventID);
      analysisManager->FillNtupleDColumn(2, 2, dutAHitsAllSmall->GetEdep());
      analysisManager->FillNtupleSColumn(2, 3, "110um");
      analysisManager->AddNtupleRow(2);
    }
    //
    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleDColumn(1, 1, dutAHitsAll->GetEdep()/CLHEP::keV);
    analysisManager->FillNtupleSColumn(1, 2, "110um");
    analysisManager->AddNtupleRow(1);

    // fill ntuple
    for(size_t i=0; i< dutAHC->entries()-1; i++){
      auto dutHit = (*dutAHC)[i];
      auto edep = dutHit->GetEdep()/CLHEP::keV;
      if(edep == 0) continue;
      
      auto xpos = dutHit->GetX()/CLHEP::mm;
      auto ypos = dutHit->GetY()/CLHEP::mm;
      auto zpos = dutHit->GetZ()/CLHEP::mm;
      //auto zpos = dutHit->GetZ();
      analysisManager->FillH2(0, xpos, ypos, edep);
      //
      analysisManager->FillNtupleIColumn(0, 0, eventID);
      analysisManager->FillNtupleIColumn(0, 1, i+1);
      analysisManager->FillNtupleDColumn(0, 2, edep);
      analysisManager->FillNtupleDColumn(0, 3, xpos);
      analysisManager->FillNtupleDColumn(0, 4, ypos);
      analysisManager->FillNtupleDColumn(0, 5, zpos);
      analysisManager->FillNtupleSColumn(0, 6, "110um");
      analysisManager->AddNtupleRow(0);
    }
  }

  if(dutBHitsAll->GetEdep() > 0){  
    // fill histograms
    analysisManager->FillH1(6, dutBHitsAll->GetEdep());
    analysisManager->FillH1(7, dutBHitsAll->GetEdep()/(27.0*CLHEP::eV)/1000.0);
    //analysisManager->FillH1(8);
    if(dutBHitsAllLarge->GetEdep() > 0) analysisManager->FillH1(9, dutBHitsAllLarge->GetEdep());
    if(dutBHitsAllSmall->GetEdep() > 0) analysisManager->FillH1(10, dutBHitsAllSmall->GetEdep());
    //
    if(dutBHitsAllLarge->GetEdep() > 0){
      analysisManager->FillNtupleIColumn(2, 0, eventID);
      analysisManager->FillNtupleDColumn(2, 1, dutBHitsAllLarge->GetEdep());
      analysisManager->FillNtupleSColumn(2, 3, "150um");
      analysisManager->AddNtupleRow(2);
    }
    if(dutBHitsAllSmall->GetEdep() > 0){
      analysisManager->FillNtupleIColumn(2, 0, eventID);
      analysisManager->FillNtupleDColumn(2, 2, dutBHitsAllSmall->GetEdep());
      analysisManager->FillNtupleSColumn(2, 3, "150um");
      analysisManager->AddNtupleRow(2);
    }
    //
    analysisManager->FillNtupleIColumn(1, 0, eventID);
    analysisManager->FillNtupleDColumn(1, 1, dutBHitsAll->GetEdep()/CLHEP::keV);
    analysisManager->FillNtupleSColumn(1, 2, "150um");
    analysisManager->AddNtupleRow(1);

    // fill ntuple
    for(size_t i=0; i< dutBHC->entries()-1; i++){
      auto dutHit = (*dutBHC)[i];
      auto edep = dutHit->GetEdep()/CLHEP::keV;
      if(edep == 0) continue;
      
      auto xpos = dutHit->GetX();
      auto ypos = dutHit->GetY();
      auto zpos = dutHit->GetZ();
      analysisManager->FillH2(1, xpos, ypos, edep);
      //
      analysisManager->FillNtupleIColumn(0, 0, eventID);
      analysisManager->FillNtupleIColumn(0, 1, i+1);
      analysisManager->FillNtupleDColumn(0, 2, edep);
      analysisManager->FillNtupleDColumn(0, 3, xpos/CLHEP::mm);
      analysisManager->FillNtupleDColumn(0, 4, ypos/CLHEP::mm);
      analysisManager->FillNtupleDColumn(0, 5, zpos/CLHEP::mm);
      analysisManager->FillNtupleSColumn(0, 6, "150um");
      analysisManager->AddNtupleRow(0);
    }
  }

  if(fitpixHitAll->GetEdep() > 0){
    // fill ntuple
    for(size_t i=0; i< fitpixHC->entries()-1; i++){
      auto fitpixHit = (*fitpixHC)[i];
      auto edep = fitpixHit->GetEdep()/CLHEP::keV;
      if(edep == 0) continue;
      
      auto xpos = fitpixHit->GetX();
      auto ypos = fitpixHit->GetY();
      auto zpos = fitpixHit->GetZ();
      analysisManager->FillH2(2, xpos, ypos, edep);
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

