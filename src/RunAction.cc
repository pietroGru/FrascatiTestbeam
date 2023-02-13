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
/// \file electromagnetic/TestEm4/src/RunAction.cc
/// \brief Implementation of the RunAction class
//
// 
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "RunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction()
 : G4UserRunAction()
{
  // set printing event number per each event
  //G4RunManager::GetRunManager()->SetPrintProgress(1); 

  // Create analysis manager
  // The choice of analysis technology is done via selectin of a namespace
  // in Analysis.hh
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  //G4cout << "Using " << analysisManager->GetType() << G4endl;
  //analysisManager->SetVerboseLevel(1);  
  //analysisManager->SetFirstHistoId(1);

  // Create directories 
  analysisManager->SetHistoDirectoryName("histograms");
  analysisManager->SetNtupleDirectoryName("ntuple");
  analysisManager->SetVerboseLevel(1);
  analysisManager->SetNtupleMerging(true);
  // Note: merging ntuples is available only with Root output
  
  // Book histograms, ntuple
  //

  // Creating histograms
  //
  analysisManager->CreateH1("primary","Primary particle energy", 301, 0., 301 * MeV, "MeV");

  analysisManager->CreateH1("edepTotUp","Energy/event deposited in the upstream sensor", 500, 0, 500*keV, "keV");
  analysisManager->CreateH1("chargeUp","Charge deposited in the pad upstream sensor", 400, 0, 400);
  analysisManager->CreateH1("primaryUp","Track's energy entering upstream sensor", 301, 0, 301*MeV, "MeV");
  analysisManager->CreateH1("edepTotLargeUp","Energy/B deposited in the large pad 110 um sensor", 500, 0, 50*MeV, "MeV");
  analysisManager->CreateH1("edepTotSmallUp","Energy/B deposited in the small pad 110 um sensor", 500, 0, 50*MeV, "MeV");

  analysisManager->CreateH1("edepTotDown","Energy/event deposited in the downstream sensor", 50, 0, 500*keV, "keV");
  analysisManager->CreateH1("chargeDown","Charge deposited in the pad downstream sensor", 400, 0, 400);
  analysisManager->CreateH1("primaryDown","Track's energy entering downstream sensor", 301, 0., 301*MeV, "MeV");
  analysisManager->CreateH1("edepTotLargeDown","Energy/B deposited in the large pad 150um sensor", 500, 0, 100*MeV, "MeV");
  analysisManager->CreateH1("edepTotSmallDown","Energy/B deposited in the small pad 150um sensor", 500, 0, 100*MeV, "MeV");

  analysisManager->CreateH2("edepMapUp", "Spatial energy dep. distribution upstream sensor", 100, -25.4*mm, 25.4*mm, 100, -25.4*mm, 25.4*mm, "mm", "mm");
  analysisManager->CreateH2("edepMapDown", "Spatial energy dep. distribution downstream sensor", 100, -25.4*mm, 25.4*mm, 100, -25.4*mm, 25.4*mm, "mm", "mm");

  analysisManager->CreateH2("edepMapFitpix", "Spatial energy dep. distribution fitpix sensor", 200, -10.0*mm, 10.0*mm, 200, -10.0*mm, 10.0*mm, "mm", "mm");


  // Creating ntuple
  //
  analysisManager->CreateNtuple("DUTs", "Sensors tree");
  analysisManager->CreateNtupleIColumn(0, "event");
  analysisManager->CreateNtupleIColumn(0, "layer");
  analysisManager->CreateNtupleDColumn(0, "edep");
  analysisManager->CreateNtupleDColumn(0, "edepPosX");
  analysisManager->CreateNtupleDColumn(0, "edepPosY");
  analysisManager->CreateNtupleDColumn(0, "edepPosZ");
  analysisManager->CreateNtupleSColumn(0, "wafer");
  analysisManager->FinishNtuple(0);
  //
  analysisManager->CreateNtuple("RUN", "Run tree");
  analysisManager->CreateNtupleIColumn(1, "event");
  analysisManager->CreateNtupleDColumn(1, "etot");
  analysisManager->CreateNtupleSColumn(1, "wafer");
  analysisManager->FinishNtuple(1);
  //
  analysisManager->CreateNtuple("AUX", "Auxiliary tree");
  analysisManager->CreateNtupleIColumn(2, "event");
  analysisManager->CreateNtupleDColumn(2, "etotLP");
  analysisManager->CreateNtupleDColumn(2, "etotSP");
  analysisManager->CreateNtupleSColumn(2, "wafer");
  analysisManager->FinishNtuple(2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
   //delete G4AnalysisManager::Instance();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run*)
{
  // show Rndm status
  if (isMaster) G4Random::showEngineStatus();
  

   // Get analysis manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Open an output file
  //
  G4String fileName = analysisManager->GetFileName();
  if(fileName == "" || fileName == nullptr){
      fileName = "dutOut.root";
  }
  analysisManager->OpenFile(fileName);    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run*)
{  
  // show Rndm status
  if (isMaster) G4Random::showEngineStatus();         
  
  auto analysisManager = G4AnalysisManager::Instance();
  // print histogram statistics
  //
  if ( analysisManager->GetH1(0) ) {
    G4cout << G4endl << " ----> print histograms statistic ";
    if(isMaster) {
      G4cout << "for the entire run " << G4endl << G4endl; 
    }
    else {
      G4cout << "for the local thread " << G4endl << G4endl; 
    }
    
    G4cout << " ELay : mean = " 
       << G4BestUnit(analysisManager->GetH1(0)->mean(), "Energy") 
       << " rms = " 
       << G4BestUnit(analysisManager->GetH1(0)->rms(),  "Energy") << G4endl;
    }

  // save histograms & ntuple
  //
  analysisManager->Write();
  analysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
