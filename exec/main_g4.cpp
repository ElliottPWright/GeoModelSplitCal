#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisExecutive.hh"
#include "G4PhysListFactory.hh"

#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "EventAction.hh"
#include "EventStore.hh"
#include "RunConfig.hh"
#include "PrimaryGeneratorAction.hh"


// Minimal primary generator
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"

//CLI
#include <cstring>
#include <string>
#include <vector>
#include <iostream>



int main(int argc, char** argv)
{
  // Usage:
  //   ./run_g4 calo.cfg [macro]
 
  std::string caloCfg = "../calo.cfg";
  std::string runCfg  = "../run.cfg";
  if (argc > 1) caloCfg = argv[1];
  if (argc > 2) runCfg  = argv[2];

  auto run = readRunConfigFile(runCfg);
  auto require = [&](int& i, const char* opt) -> const char* {
    if (i + 1 >= argc) {
      std::cerr << "Missing value after " << opt << "\n";
      std::exit(2);
    }
    return argv[++i];
  };
  
  auto require3 = [&](int& i, const char* opt, double& a, double& b, double& c) {
    if (i + 3 >= argc) {
      std::cerr << "Missing 3 values after " << opt << "\n";
      std::exit(2);
    }
    a = std::stod(argv[++i]);
    b = std::stod(argv[++i]);
    c = std::stod(argv[++i]);
  };
  
  for (int i = 3; i < argc; ++i) {  // argv[1]=calo.cfg, argv[2]=run.cfg
    const std::string opt = argv[i];
  
    if (opt == "--n-events") {
      run.n_events = std::stoi(require(i, "--n-events"));
    } else if (opt == "--particle") {
      run.particle = require(i, "--particle");
    } else if (opt == "--energy-MeV") {
      run.energy_MeV = std::stod(require(i, "--energy-MeV"));
      } 
    else if (opt == "--dir") {
        double x,y,z;
        require3(i, "--dir", x, y, z);
        run.direction = {x, y, z};
    } 
    else if (opt == "--pos-mm") {
      double x,y,z;
      require3(i, "--pos-mm", x, y, z);
      run.position_mm = {x, y, z};
    } 
    else if (opt == "--sigma-xy-mm") {
      run.sigma_xy_mm = std::stod(require(i, "--sigma-xy-mm"));
    } else if (opt.rfind("--", 0) == 0) {
      std::cerr << "Unknown option: " << opt << "\n";
      std::exit(2);
    }
  }
  // optional seed
  if (run.seed != 0) {
    CLHEP::HepRandom::setTheSeed(run.seed);
  }

  


     
 // const std::string cfgFile = (argc > 1) ? argv[1] : "../calo.cfg";
  const std::string cfgFile = caloCfg;

  auto* runManager = new G4RunManager;

  // Shared event store
  auto* store = new EventStore;

  auto* detCon = new DetectorConstruction(store, cfgFile, run.write_gdml);
  detCon->SetVisMode(run.vis_mode);
  runManager->SetUserInitialization(detCon);

  G4PhysListFactory factory;
  runManager->SetUserInitialization(factory.GetReferencePhysList("FTFP_BERT"));
  runManager->SetUserAction(new PrimaryGeneratorAction(run));

  runManager->SetUserAction(new RunAction(static_cast<int>(run.energy_MeV),static_cast<std::string>(run.particle)));
  runManager->SetUserAction(new EventAction(store));

  runManager->Initialize();
  
  // Visualization + UI (optional)
  // Visualization manager only if requested (or if you enter interactive)
  G4VisExecutive* vis = nullptr;
  if (run.visualize || run.macro.empty()) {
    vis = new G4VisExecutive;
    vis->Initialize();
  }
  
  auto* UImanager = G4UImanager::GetUIpointer();
  
  if (run.visualize) {

    // Run the visualization macro without starting a Qt/X11 UI
    UImanager->ApplyCommand("/control/execute " + G4String(run.vis_macro));

    // Run the requested number of events
    if (run.n_events > 0) {
        UImanager->ApplyCommand(
            "/run/beamOn " + G4String(std::to_string(run.n_events))
        );
    }

     // Force final offscreen image generation after the run
    UImanager->ApplyCommand("/vis/viewer/rebuild");
    UImanager->ApplyCommand("/vis/viewer/update");

    if (vis != nullptr) delete vis;

} else {
    // batch behavior
    if (!run.macro.empty()) {
      UImanager->ApplyCommand("/control/execute " + G4String(run.macro));
      if (run.n_events > 0) {
        UImanager->ApplyCommand("/run/beamOn " + G4String(std::to_string(run.n_events)));
      }
    } else {
      // if you want: pure non-vis interactive disabled -> just beamOn or exit
      // (keep your old interactive branch if you still want it)
    }
  }

//delete vis;
   if (!run.macro.empty()) {
  }  

  delete runManager;
  delete store;
  return 0;
}
