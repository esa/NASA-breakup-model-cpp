# NASA Breakup Model in modern C++

![Build and Test](https://github.com/esa/NASA-breakup-model-cpp/actions/workflows/ctest.yml/badge.svg)

This code is a validated implementation of the [NASA Standard Breakup Model by Johnson et al.](https://www.sciencedirect.com/science/article/abs/pii/S0273117701004239). It was created in a [collaborative thesis](https://mediatum.ub.tum.de/1624604) project between TU Munich and ESA's Advanced Concepts Team. Please refer to the thesis for extensive information about the theoretical background, related work, implementation & design decisions, application, verification, and runtime measurements of the presented code.

Comments to Equations or similar in the source code reference the [Paper by Johnson et al.](https://www.sciencedirect.com/science/article/abs/pii/S0273117701004239). For any question in relation to the code, feel free to open an issue or reach out to `pablo.gomez@esa.int`.

## Requirements
The project uses the following dependencies:
- GoogleTest-1.10.0 (Only required for Testing, Automatically set-up by CMake)
- yaml-cpp-0.6.3 (Required for Input, Automatically set-up by CMake)
- spdlog Version 1.8.5 (Required for output and logging, Automatically set-up by CMake)

Furthermore, the Breakup simulation uses ``std::execution`` to parallelize
the fragment calculation. Not every compiler implements the C++ 17 feature
"Standardization of Parallelism TS". To avoid this issue, install:

      sudo apt install libtbb-dev


## Build
The program is build by using CMake. So first make sure that you installed
CMake and then follow these steps:

    mkdir build
    cd build
    cmake ..
    make

## Execution
After the build, the simulation can be run by executing:

    ./breakupModel [yaml-file]

where the yaml-file contains a Configuration. Examples for Configuration Files
can be found in the folder example-config of this repository.

### Brief overview on the available options

- _minimalCharacteristicLength_
  - The simulation will only produce fragments greater than this lower bound
  - Usefully because, real radar detection does not allow us to track infinitesimal
    small particles in space
  - Set this to reasonable values, small values will lead to lots of debris!
- _simulationType_
  - OPTIONAL 
  - Acts like a guard to protect the user from running an undesired simulation
  - Ensures Mode: COLLISION or EXPLOSION
- _currentMaxID_
  - OPTIONAL
  - The debris fragments will have unique IDs, this option should contain the
    currently highest NORAD Catalog ID
  - If not given the Simulation tries to derive the current highest ID from
    the given input
- _inputSource_
  - Takes the input data as a list: Either Data-YAML file or TLE + Satcat
- _idFilter_
  - OPTIONAL
  - Applies a filter over the inputSource
  - Useful if you give the full Satcat + TLE data and don't want to manually
    extract the Satellites which should collide or similar
- _enforceMassConservation_
  - OPTIONAL (default false)
  - The simulation does produce the debris according to power law distribution
  - If this is enabled more fragments than specified for the minimal L_c will be
    produced if the former simulation ended with outputMass < inputMass
  - **Caution!** The filling of the mass budget is not always reasonable! E.g. In case
    of a non-catastrophic collision the bigger object is NOT fully fragmented,
    hence a fulfilling inputMass == outputMass in the end is not desirable!
  - Notice that to maintain the correct distribution, outputMass == inputMass will
    not be produced but approximated
  - Notice that the case outputMass > inputMass is not affected by this option;
    the removal of a mass excess is always applied
  - Notice that this option kills some performance because of the random nature, there
    is no possibility to schedule how many particles will be produced in the end

### Input

The given yaml-file should look like this:

```yaml
  simulation:
    minimalCharacteristicLength: 0.05 #minimal fragment L_c in [m]
    simulationType: COLLISION         #Option (Alias): COLLISION (CO) or EXPLOSION (EX)
                                      #If not given type is determined
                                      # by number of input satellites 
    currentMaxID: 48514               #For determining fragment ID
                               
                                      #Should be the currently largest given NORAD-Catalog ID
                                      #If not given, zero is assumed
    inputSource: ["../data.yaml"]     #Path to input file(s) - One of the following:
                                      #1) ["data.yaml"]
                                      #2) ["satcat.csv", "tle.txt"]
    idFilter: [1, 2]                  #Only the satellites with these IDs will be
                                      #recognized by the simulation.
                                      #If not given, no filter is applied
    enforceMassConservation: True     #When this is set to true, the simulation will
                                      #try to enforce mass conservation
                                      #if not given, this is always false
  inputOutput:                        #If you want to print out the input data into specific file (optional)
    target: ["input.csv", "input.vtu"]#Target files
    #kepler: True                     #CSV with Kepler elements
    #csvPattern: "IL"                 #Override "kepler" setting and prints CSV output according to pattern
  resultOutput:                       #If you want a result, you should define here some target file for the
    target: ["result.csv", "result.vtu"]#fragements (like vtk or csv)
    #kepler: True                     #Option like above
    #csvPattern: "IL"                 #Option like above, available Patterns: see below
```    
A "data.yaml" should have the following form (for example):

```yaml
    satellites:
    - name: "Example Satellite 1"   #Optional
      id: 24946                     #A must e. g. NORAD Catalog ID)
      satType: SPACECRAFT           #Optional (Default: SPACECRAFT); Option (Alias):
                                    #SPACECRAFT (SC), ROCKET_BODY (RB), DEBRIS (DEB), UNKNOWN
      mass: 700.0                   #Either mass or area is a must
                                    #Mass in [kg], Area (Radar Cross Section) in [m^2]
      velocity: [1.0, 1.0, 1.0]     #The cartesian velcoity vector [m/s]
      position: [0.0, 0.0, 0.0]     #The cartesian position [m] (Optional)
    - name: "Example Satellite 2"   #Second example, again name is optional
      id: 24947                     #A must
      satType: ROCKET_BODY          #Another satellite type
      area: 3.5                     #Either mass or area is a must
                                    #Mass in [kg], Area (Radar Cross Section) in [m^2]
      kepler:                       #Either velocity (+ position) or
        semi-major-axis: 6798505.86 #Keplerian Elements
        eccentricity: 0.0002215     #Angles should be given in [rad]
        inclination: 0.9013735469   #semi-major-axis in [m]
        longitude-of-the-ascending-node: 4.724103630312
        argument-of-periapsis: 2.237100203348
        true-anomaly: (double)      #You don't have to give all three of them to the simulation
        eccentric-anomaly: (double) #But one of them is a must.
        mean-anomaly: 0.2405604761  #The program ignores if multipe anomalies are given
                                    #expect one.The precedence is: Eccentric > Mean > True
    - name: "Example Satellite 3"   
      id: 24948                     
      satType: SPACECRAFT           
      mass: 700.0                   
      kepler: "../tle-file.txt"     #It is also possible to parse the Kepler elements from
                                    #a TLE file
    - ...
```

A "satcat.csv" has the following form:

    OBJECT_NAME,OBJECT_ID,NORAD_CAT_ID,OBJECT_TYPE,OPS_STATUS_CODE,OWNER,LAUNCH_DATE,LAUNCH_SITE,DECAY_DATE,PERIOD,INCLINATION,APOGEE,PERIGEE,RCS,DATA_STATUS_CODE,ORBIT_CENTER,ORBIT_TYPE
    SL-1 R/B,1957-001A,1,R/B,D,CIS,1957-10-04,TYMSC,1957-12-01,96.19,65.10,938,214,20.4200,,EA,IMP
    SPUTNIK 1,1957-001B,2,PAY,D,CIS,1957-10-04,TYMSC,1958-01-03,96.10,65.00,1080,64,,,EA,IMP
    SPUTNIK 2,1957-002A,3,PAY,D,CIS,1957-11-03,TYMSC,1958-04-14,103.74,65.33,1659,211,0.0800,,EA,IMP
    EXPLORER 1,1958-001A,4,PAY,D,US,1958-02-01,AFETR,1970-03-31,88.48,33.15,215,183,,,EA,IMP
    ...

It can be obtained from the following source:
https://celestrak.com/pub/satcat.csv

A "tle.txt" has the following form:

    ISS (ZARYA)
    1 25544U 98067A   08264.51782528 -.00002182  00000-0 -11606-4 0  2927
    2 25544  51.6416 247.4627 0006703 130.5360 325.0288 15.72125391563537
    ...

Example taken from https://en.wikipedia.org/wiki/Two-line_element_set.
The format specification can also be read there.

Notice that if you decide to take the satcat + TLE approach that the satellite(s)
which should explode (collide) must be present in both files. The mapping is done
via their IDs. You find the IDs in the satcat in the third column (NORAD_CAT_ID) and
in the TLE optionally in first and second row in the beginning (example: 25544).

### Output
The simulation can produce CSV and VTK files, depending on the configuration
it produces both, either of them or none (see YAML Configuration File)

#### Available Patterns for the CSVPatternWriter:

| Pattern | Meaning                   | Pattern | Meaning |
| ---     | ---                       | ---     | --- |
| I       | ID                        | a       | Semi-Major-Axis [m] |
| n       | Name                      | e       | Eccentricity |
| t       | Satellite Type            | i       | Inclination [rad] |
| L       | Characteristic Length [m] | W       | Longitude of the ascending node [rad] |
| R       | A/M [m^2/kg]              | w       | Argument of periapsis [rad] |
| A       | Area [m^2]                | M       | Mean Anomaly [rad] |
| m       | Mass [kg]                 | E       | Eccentric Anomaly [rad] |
| v       | Velocity [m/s]            | T       | True Anomaly [rad] |
| j       | Ejection Velocity [m/s]   |         | |
| p       | Position [m]              |         | |

## How to use the project as a library
The Breakup Simulation can be used directly from any C++ project
without the need of file input, etc.
To create a new Breakup Simulation just follow these four steps:
    
1. Create a new RuntimeInputSource and define the parameter
   (use the SatelliteBuilder to easier create new Satellites
   and get feedback about the completeness)
2. Give this RuntimeInputSource to the BreakupBuilder
3. Create your Breakup Simulation
4. Run and get the Result!

### Example CMake integration:
```cmake
cmake_minimum_required(VERSION 3.16)
project(FOO)

# ------------------------- get the breakup model -------------------------
# Enable ExternalProject CMake module
include(FetchContent)

# OPTIONAL: disable stuff not needed when used as a lib
set(BUILD_BREAKUP_MODEL_TESTS OFF CACHE INTERNAL "")
set(BUILD_BREAKUP_MODEL_SIM OFF CACHE INTERNAL "")

FetchContent_Declare(
        BreakupModelfetch
        GIT_REPOSITORY git@github.com:esa/NASA-breakup-model-cpp.git
        # GIT_TAG master
)

# Get Breakup Model source and binary directories from CMake project
FetchContent_MakeAvailable(BreakupModelfetch)

# OPTIONAL: Disable potential warnings
# from the library target
target_compile_options(breakupModel_lib PRIVATE -w)
# from included headers
get_target_property(propval breakupModel_lib INTERFACE_INCLUDE_DIRECTORIES)
target_include_directories(breakupModel_lib SYSTEM PUBLIC "${propval}")

# -------------------- define your target and link it ---------------------

add_executable(fooExe foo.cpp)
target_compile_features(fooExe PUBLIC cxx_std_17)
target_link_libraries(fooExe PUBLIC breakupModel_lib)
```

### Example C++ Code integration:
```cpp
#include <breakupModel/input/RuntimeInputSource.h>
#include <breakupModel/model/Satellite.h>
#include <breakupModel/model/SatelliteBuilder.h>
#include <breakupModel/simulation/BreakupBuilder.h>

int main() {
    // generate satellites to collide
    SatelliteBuilder satelliteBuilder;
    std::vector<Satellite> satellites{
        satelliteBuilder
            .setID(0)
            .setPosition({0.,0.,0.})  // [m]
            .setVelocity({0.,1.,0.})  // [m/s^2]
            .setMass(500.)            // [kg]
            .getResult(),
        satelliteBuilder
            .setID(1)
            .setPosition({0.,0.,0.})  // [m]
            .setVelocity({0.,0.,1.})  // [m/s^2]
            .setMass(500.)            // [kg]
            .getResult(),
    };

    // Minimal RuntimeInputSource only with minLc = 0.05 [m] and all involved satellites.
    auto configSource = std::make_shared<RuntimeInputSource>(0.05, satellites);
    // Initialize a BreakupBuilder with a configuration object
    // (YMALConfigurationReader or RuntimeInputSource or your own derived source)
    BreakupBuilder breakupBuilder{configSource};
    // Create the actual breakup simulation
    auto breakup = breakupBuilder.getBreakup();
    // Run it and collect the result
    breakup->run();
    std::vector<Satellite> debris = breakup->getResult();
}
```
## Testing
The tests use the framework GoogleTest and
can simply be run by executing in the build directory:

    ctest
