#pragma once
#include <string>
#include <array>

struct RunConfig {
  int n_events = 0;

  std::string particle = "e-";
  double energy_MeV = 1000.0;

  // EPWL 20/07: Added member halflengths
  std::array<double,3> half_lengths    {0.0, 0.0, 0.0};

  // EPWL 24/07: Added member gaussian
  bool gaussian = false;

  std::array<double,3> position_mm  {0.0, 0.0, -2500.0};
  std::array<double,3> direction    {0.0, 0.0, 1.0};
  double sigma_xy_mm = 0.0;
  bool getBool(const std::string& key, bool def=false) const;
  std::string getString(const std::string& key, const std::string& def="") const;


  std::string macro = "";
  long seed = 0;
  int  write_gdml = 0;
  bool visualize = false;
  std::string vis_macro = "../vis.mac";
  int vis_mode = 0;  // 0=module envelopes, 2=layer envelopes, 1=full detail

};

RunConfig readRunConfigFile(const std::string& path);

