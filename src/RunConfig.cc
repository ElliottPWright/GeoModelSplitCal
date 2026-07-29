#include "RunConfig.hh"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cctype>

static inline std::string trim(std::string s) {
  size_t b = 0;
  while (b < s.size() && std::isspace((unsigned char)s[b])) ++b;
  size_t e = s.size();
  while (e > b && std::isspace((unsigned char)s[e-1])) --e;
  return s.substr(b, e-b);
}

static inline bool starts_with(const std::string& s, const std::string& p) {
  return s.rfind(p, 0) == 0;
}

static std::array<double,3> parse3(const std::string& v) {
  std::istringstream iss(v);
  std::array<double,3> a{};
  if (!(iss >> a[0] >> a[1] >> a[2])) {
    throw std::runtime_error("Expected 3 numbers, got: " + v);
  }
  return a;
}

RunConfig readRunConfigFile(const std::string& path) {
  std::ifstream in(path);
  if (!in) throw std::runtime_error("Cannot open run.cfg: " + path);

  RunConfig cfg;
  std::string line;
  int ln = 0;

  while (std::getline(in, line)) {
    ++ln;
    line = trim(line);
    if (line.empty() || starts_with(line, "#")) continue;

    auto eq = line.find('=');
    if (eq == std::string::npos)
      throw std::runtime_error("run.cfg parse error line " + std::to_string(ln) + ": " + line);

    std::string key = trim(line.substr(0, eq));
    std::string val = trim(line.substr(eq+1));

    if      (key == "n_events")       cfg.n_events = std::stoi(val);
    else if (key == "particle")       cfg.particle = val;
    else if (key == "half_lengths")   cfg.half_lengths = parse3(val);  // EPWL 20/07: to generate rectangular starting points 
    else if (key == "energy_MeV")     cfg.energy_MeV = std::stod(val);
    else if (key == "position_mm")    cfg.position_mm = parse3(val);
    else if (key == "direction")      cfg.direction = parse3(val);
    else if (key == "sigma_xy_mm")    cfg.sigma_xy_mm = std::stod(val);
    else if (key == "gaussian")       cfg.gaussian = std::stoi(val);
    else if (key == "macro")          cfg.macro = val;
    else if (key == "seed")           cfg.seed = std::stol(val);
    else if (key == "write_gdml")     cfg.write_gdml = std::stoi(val);
    else if (key == "visualize") {
      std::string v = val;
      for (auto& c : v) c = std::tolower((unsigned char)c);
      cfg.visualize = (v=="1" || v=="true" || v=="yes" || v=="on");
    }
    else if (key == "vis_macro") {
      cfg.vis_macro = val;
    }
    else if (key == "vis_mode") {
      cfg.vis_mode = std::stoi(val);
    }

    else
      throw std::runtime_error("Unknown run.cfg key: " + key);
  }


  return cfg;
}


