#include "CalorimeterBuilder.h"
#include "MaterialManager.h"
#include "BarLayer.h"
#include "Fibre_HPLayer.h"

#include "GeoModelKernel/GeoBox.h"
#include "GeoModelKernel/GeoLogVol.h"
#include "GeoModelKernel/GeoPhysVol.h"
#include "GeoModelKernel/GeoNameTag.h"
#include "GeoModelKernel/GeoTransform.h"
#include "GeoModelKernel/Units.h"

#include <string>

using namespace GeoModelKernelUnits;

void CalorimeterBuilder::buildStack(GeoVPhysVol* world, MaterialManager& MM, const CalorimeterConfig& cfg, int mx, int my)
{
  auto* leadMat = MM.lead();
  auto* pvtMat  = MM.pvt();
  auto* psMat   = MM.polystyrene();

  const double plateXY = cfg.plate_xy_mm * mm;

  const double totalX = cfg.plate_xy_mm * mm;
  const double totalY = cfg.plate_xy_mm * mm;

  const double moduleGap = cfg.module_gap_mm * mm;

  const double moduleX =
    (totalX - (cfg.module_nx - 1) * moduleGap)
    / cfg.module_nx;

  const double moduleY =
    (totalY - (cfg.module_ny - 1) * moduleGap)
    / cfg.module_ny;
  const double leadZ   = cfg.lead_thickness_mm * mm;
  const double scintZ  = cfg.scint_thickness_mm * mm;
  const double airGapZ = cfg.airgap_mm * mm;

  const double ironZ = cfg.iron_thickness_mm * mm;

  const double wideW = 60.0 * mm;
  const double thinW = 10.0 * mm;

  const double hplZ    = cfg.hpl_thickness_mm * mm;

  auto makeLayerEnv = [&](GeoVPhysVol* module,
                          const std::string& envName,
                          double halfZ,          // in GeoModel length units (already *mm)
                          double zCenter,
			  double centerX,
                        double centerY) -> GeoPhysVol*
  {
      auto* envShape = new GeoBox(0.5*plateXY, 0.5*plateXY, halfZ);
      auto* envLog   = new GeoLogVol((envName + "_LOG").c_str(), envShape, MM.air());
      auto* envPhys  = new GeoPhysVol(envLog);
  
      module->add(new GeoNameTag(envName.c_str()));
      module->add(new GeoTransform(GeoTrf::Translate3D(centerX, centerY, zCenter)));
      module->add(envPhys);
  
      return envPhys;
  };




  const std::string mtag = "_MX" + std::to_string(mx) + "Y" + std::to_string(my);

  // Wide PVT horizontal (along X)
  auto* wideHShape = new GeoBox(0.5*wideW, 0.5*plateXY, 0.5*scintZ);
  auto* wideHLog   = new GeoLogVol("WidePVT_H_Log", wideHShape, pvtMat);
  
  // Wide PVT vertical (along Y)
  auto* wideVShape = new GeoBox(0.5*plateXY, 0.5*wideW, 0.5*scintZ);
  auto* wideVLog   = new GeoLogVol("WidePVT_V_Log", wideVShape, pvtMat);
  
  // Thin PS horizontal
  auto* thinHShape = new GeoBox(0.5*thinW, 0.5*plateXY, 0.5*scintZ);
  auto* thinHLog   = new GeoLogVol("ThinPS_H_Log", thinHShape, psMat);
  
  // Thin PS vertical
  auto* thinVShape = new GeoBox(0.5*plateXY, 0.5*thinW, 0.5*scintZ);
  auto* thinVLog   = new GeoLogVol("ThinPS_V_Log", thinVShape, psMat);



  // Reusable logvols
  auto* leadShape = new GeoBox(0.5*plateXY, 0.5*plateXY, 0.5*leadZ);
  auto* leadLog   = new GeoLogVol("LeadPlateLog", leadShape, leadMat);


  auto* ironShape = new GeoBox(0.5*plateXY, 0.5*plateXY, 0.5*ironZ);
  auto* ironLog   = new GeoLogVol("IronPlateLog", ironShape, MM.iron());


  auto* wideShape = new GeoBox(0.5*(60.0*mm), 0.5*plateXY, 0.5*scintZ);
  auto* wideLog   = new GeoLogVol("WidePVTBarLog", wideShape, pvtMat);

  auto* thinShape = new GeoBox(0.5*(10.0*mm), 0.5*plateXY, 0.5*scintZ);
  auto* thinLog   = new GeoLogVol("ThinPSBarLog", thinShape, psMat);

  // Total thickness for centering
  double totalZ = 0.0;
  
  // --- section 1: code 7 = lead ---
  for (int code : cfg.layers) {
    if (code == 7) totalZ += leadZ;
    else if (code == 1 || code == 2 || code == 3 || code == 4) totalZ += scintZ;
    else if (code == 5) totalZ += hplZ;
    else if (code == 6) totalZ += hplZ;
    else if (code == 8) totalZ += airGapZ;
    else throw std::runtime_error("Unknown layer code in layers: " + std::to_string(code));
  }
  totalZ += cfg.gap_ecal_hcal_mm; 
  // --- section 2: code 7 = iron ---
  for (int code : cfg.layers2) {
    if (code == 7) totalZ += ironZ;
    else if (code == 1 || code == 2 || code == 3 || code == 4) totalZ += scintZ;
    else if (code == 5) totalZ += hplZ; 
    else if (code == 6) totalZ += hplZ; 
    else if (code == 8) totalZ += airGapZ;
    else throw std::runtime_error("Unknown layer code in layers2: " + std::to_string(code));
  }

  int iWideH=0, iWideV=0, iThinH=0, iThinV=0, iLead=0, iGap=0, iHPL=0;
  
  double zCursor = cfg.center_stack ? -0.5 * totalZ : 0.0;

  for (int mx = 0; mx < cfg.module_nx; ++mx) {
	  for (int my = 0; my < cfg.module_ny; ++my) {
		  const double moduleCenterX =
			  -0.5 * totalX
                          + 0.5 * moduleX
                          + mx * (moduleX + moduleGap);
		  const double moduleCenterY =
			  -0.5 * totalY
                          + 0.5 * moduleY
                          + my * (moduleY + moduleGap);
		  int globalsensLayer = 0;
		  int globalLayer = 0;
		  for (int code : cfg.layers) {
	
        if (code == 7) {
        const double zCenter = zCursor + 0.5*leadZ;
        const std::string envName =
          "ECAL_GL" + std::to_string(globalLayer) +
          "_Lead" + mtag;
        
        auto* env = makeLayerEnv(
    world,
    envName,
    0.5 * leadZ,
    zCenter,
    moduleCenterX,
    moduleCenterY);
        
        auto* platePhys = new GeoPhysVol(leadLog);
        env->add(new GeoNameTag((envName).c_str()));
        env->add(new GeoTransform(GeoTrf::Translate3D(0,0,0)));
        env->add(platePhys);
        
        zCursor += leadZ;
        ++iLead;
        globalLayer++;


    }
    else if (code == 1) {
        const double zCenter = zCursor + 0.5*scintZ;

        const std::string envName =
          "ECAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_WidePVT_H" + mtag;
        
        auto* env = makeLayerEnv(
    world,
    envName,
    0.5 * leadZ,
    zCenter,
    moduleCenterX,
    moduleCenterY);
        
        // place bars inside env at local z=0
        BarLayer::place(env, wideHLog, 60.0, 36,
                        /*zCenter_mm=*/0.0,
                        ("ECAL_GL"+std::to_string(globalLayer)+
                         "_SL"+std::to_string(globalsensLayer)+
                         "_WidePVT_H").c_str(),
                        iWideH, BarAxis::AlongX, mtag);
        
        zCursor += scintZ;
        ++iWideH;
        globalLayer++;
        globalsensLayer++;
    }
    else if (code == 2) {
        const double zCenter = zCursor + 0.5*scintZ;
        
        const std::string envName =
          "ECAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_WidePVT_V" + mtag;
        
        auto* env = makeLayerEnv(
    world,
    envName,
    0.5 * leadZ,
    zCenter,
    moduleCenterX,
    moduleCenterY);
        
        // place bars inside env at local z=0
        BarLayer::place(env, wideVLog, 60.0, 36,
                        /*zCenter_mm=*/0.0,
                        ("ECAL_GL"+std::to_string(globalLayer)+
                         "_SL"+std::to_string(globalsensLayer)+
                         "_WidePVT_V").c_str(),
                        iWideV, BarAxis::AlongY, mtag);
        
        zCursor += scintZ;
        ++iWideV;
        globalLayer++;
        globalsensLayer++;
    }
    else if (code == 3) {
        const double zCenter = zCursor + 0.5*scintZ;
        
        const std::string envName =
          "ECAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_ThinPS_H" + mtag;
        
        auto* env = makeLayerEnv(
    world,
    envName,
    0.5 * leadZ,
    zCenter,
    moduleCenterX,
    moduleCenterY);
        
        // place bars inside env at local z=0
        BarLayer::place(env, thinHLog, 10.0, 216,
                        /*zCenter_mm=*/0.0,
                        ("ECAL_GL"+std::to_string(globalLayer)+
                         "_SL"+std::to_string(globalsensLayer)+
                         "_ThinPS_H").c_str(),
                        iThinH, BarAxis::AlongX, mtag);
        
        zCursor += scintZ;
        ++iThinH;
        globalLayer++;
        globalsensLayer++;
    }
    else if (code == 4) {
        const double zCenter = zCursor + 0.5*scintZ;
        
        const std::string envName =
          "ECAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_ThinPS_V" + mtag;
        
        auto* env = makeLayerEnv(
    world,
    envName,
    0.5 * leadZ,
    zCenter,
    moduleCenterX,
    moduleCenterY);
        
        // place bars inside env at local z=0
        BarLayer::place(env, thinVLog, 10.0, 216,
                        /*zCenter_mm=*/0.0,
                        ("ECAL_GL"+std::to_string(globalLayer)+
                         "_SL"+std::to_string(globalsensLayer)+
                         "_ThinPS_V").c_str(),
                        iThinH, BarAxis::AlongY, mtag);
        
        zCursor += scintZ;
        ++iThinV;
        globalLayer++;
        globalsensLayer++;
    }
    else if (code == 5) {
        const double zCenter = zCursor + 0.5*hplZ;

        const std::string envName =
          "ECAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_HPL" + mtag;
        
        auto* env = makeLayerEnv(
    world,
    envName,
    0.5 * leadZ,
    zCenter,
    moduleCenterX,
    moduleCenterY);
        
        Fibre_HPLayer::build(env, MM.aluminum(), MM.polystyrene(),
                             ("ECAL_GL"+std::to_string(globalLayer)+
                              "_SL"+std::to_string(globalsensLayer)).c_str(),
                             /*zCenter_mm=*/0.0,
                             iHPL, cfg.plate_xy_mm, cfg.hpl_thickness_mm,
                             cfg.fiber_diameter_mm,
                             cfg.fiber_core_diameter_mm,
                             /*fibresAlongY=*/true,
                             mtag);
        
        zCursor += hplZ;
        ++iHPL;
        globalLayer++;
        globalsensLayer++;
    }
    else if (code == 6) {
        const double zCenter = zCursor + 0.5*hplZ;
        
        const std::string envName =
          "ECAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_HPL" + mtag;
        
        auto* env = makeLayerEnv(
    world,
    envName,
    0.5 * leadZ,
    zCenter,
    moduleCenterX,
    moduleCenterY);
        
        Fibre_HPLayer::build(env, MM.aluminum(), MM.polystyrene(),
                             ("ECAL_GL"+std::to_string(globalLayer)+
                              "_SL"+std::to_string(globalsensLayer)).c_str(),
                             /*zCenter_mm=*/0.0,
                             iHPL, cfg.plate_xy_mm, cfg.hpl_thickness_mm,
                             cfg.fiber_diameter_mm,
                             cfg.fiber_core_diameter_mm,
                             /*fibresAlongY=*/false,
                             mtag);
        
        zCursor += hplZ;
        ++iHPL;
        globalLayer++;
        globalsensLayer++;
    }
    else if (code == 8) {
      // airgap: no volume needed; just advance z
      zCursor += airGapZ;
      ++iGap;
    }
   }
  }
  }
  int iIron = 0;  // iron counter
 
  zCursor += cfg.gap_ecal_hcal_mm;
    
  int globalLayer=0;
  int globalsensLayer=0;
  for (int code : cfg.layers2) {
  
    if (code == 7) {
      auto* ironPhys = new GeoPhysVol(ironLog);
      world->add(new GeoNameTag((("HCAL_GL"+std::to_string(globalLayer)+"_SL"+std::to_string(globalsensLayer))+"_Iron_" + std::to_string(iIron)).c_str()));
      world->add(new GeoTransform(GeoTrf::Translate3D(0, 0, zCursor + 0.5*ironZ)));
      world->add(ironPhys);
      zCursor += ironZ;
      ++iIron;
      globalLayer++;
    }
    else if (code == 5) {
        const double zCenter = zCursor + 0.5*hplZ;

        const std::string envName =
          "HCAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_HPL" + mtag;
        
        auto* env = makeLayerEnv(world, envName, 0.5*hplZ, zCenter, 0.0, 0.0);
        
        Fibre_HPLayer::build(env, MM.aluminum(), MM.polystyrene(),
                             ("HCAL_GL"+std::to_string(globalLayer)+
                              "_SL"+std::to_string(globalsensLayer)).c_str(),
                             /*zCenter_mm=*/0.0,
                             iHPL, cfg.plate_xy_mm, cfg.hpl_thickness_mm,
                             cfg.fiber_diameter_mm,
                             cfg.fiber_core_diameter_mm,
                             /*fibresAlongY=*/true,
                             mtag);
        
        zCursor += hplZ;
        ++iHPL;
        globalLayer++;
        globalsensLayer++;
    
    }
    else if (code == 6) {
        const double zCenter = zCursor + 0.5*hplZ;
        
        const std::string envName =
          "HCAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_HPL" + mtag;
        
        auto* env = makeLayerEnv(world, envName, 0.5*hplZ, zCenter, 0.0, 0.0);
        
        Fibre_HPLayer::build(env, MM.aluminum(), MM.polystyrene(),
                             ("HCAL_GL"+std::to_string(globalLayer)+
                              "_SL"+std::to_string(globalsensLayer)).c_str(),
                             /*zCenter_mm=*/0.0,
                             iHPL, cfg.plate_xy_mm, cfg.hpl_thickness_mm,
                             cfg.fiber_diameter_mm,
                             cfg.fiber_core_diameter_mm,
                             /*fibresAlongY=*/false,
                             mtag);
        
        zCursor += hplZ;
        ++iHPL;
        globalLayer++;
        globalsensLayer++;
   
    }
  
    else if (code == 1) {

        const double zCenter = zCursor + 0.5*scintZ;

        const std::string envName =
          "HCAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_WidePVT_H" + mtag;
        
        auto* env = makeLayerEnv(world, envName, 0.5*scintZ, zCenter, 0.0, 0.0);
        
        // place bars inside env at local z=0
        BarLayer::place(env, wideHLog, 60.0, 36,
                        /*zCenter_mm=*/0.0,
                        ("HCAL_GL"+std::to_string(globalLayer)+
                         "_SL"+std::to_string(globalsensLayer)+
                         "_WidePVT_H").c_str(),
                        iWideH, BarAxis::AlongX, mtag);
        
        zCursor += scintZ;
        ++iWideH;
        globalLayer++;
        globalsensLayer++;
    }

    else if (code == 2) {

        const double zCenter = zCursor + 0.5*scintZ;
        
        const std::string envName =
          "HCAL_GL" + std::to_string(globalLayer) +
          "_SL" + std::to_string(globalsensLayer) +
          "_WidePVT_V" + mtag;
        
        auto* env = makeLayerEnv(world, envName, 0.5*scintZ, zCenter, 0.0, 0.0);
        
        // place bars inside env at local z=0
        BarLayer::place(env, wideVLog, 60.0, 36,
                        /*zCenter_mm=*/0.0,
                        ("HCAL_GL"+std::to_string(globalLayer)+
                         "_SL"+std::to_string(globalsensLayer)+
                         "_WidePVT_V").c_str(),
                        iWideV, BarAxis::AlongY, mtag);
        
        zCursor += scintZ;
        ++iWideV;
        globalLayer++;
        globalsensLayer++;

    }
  
    // optional: allow air gaps in iron section too
    else if (code == 8) {
      zCursor += airGapZ;
      ++iGap;
      globalLayer++;
    }
  
    // optional: if you truly only want wide layers in the back, then forbid 3/4/5
    else {
      throw std::runtime_error("Unsupported layer code in layers2: " + std::to_string(code));
    }
  }
};
















