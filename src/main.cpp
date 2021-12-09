#include <iostream>
#include <string>
#include <memory>
#include <exception>
#include <chrono>

#include "breakupModel/input/YAMLDataReader.h"
#include "breakupModel/input/YAMLConfigurationReader.h"
#include "breakupModel/simulation/BreakupBuilder.h"
#include "breakupModel/output/CSVWriter.h"
#include "breakupModel/output/VTKWriter.h"
#include "spdlog/spdlog.h"

int main(int argc, char *argv[]) {

    //Enable to get debug messages
    //spdlog::default_logger()->set_level(spdlog::level::debug);

    if (argc != 2) {
        spdlog::error(
                "Wrong program call. Please call the program in the following way:\n"
                "./breakupModel [yaml-file]");
        return 0;
    }
    try {
        //The fileName of the YAML file
        std::string fileName{argv[1]};

        //Load an Configuration Source containing the input arguments required for the BreakupBuilder
        //The YAMLConfigurationReader as a special case is also able to load the Configuration for Output
        auto configSource = std::make_shared<YAMLConfigurationReader>(fileName);

        //The SimulationFactory which builds our breakup simulation
        BreakupBuilder breakupBuilder{configSource};

        //Create and run the simulation or catch an exception in case something is wrong with the simulation
        //Creates and Runs the simulation
        auto breakUpSimulation = breakupBuilder.getBreakup();
        auto start = std::chrono::high_resolution_clock::now();
        breakUpSimulation->run();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = end - start;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
        spdlog::info("The simulation took {} ms", ms.count());
        spdlog::info("The simulation produced {} fragments", breakUpSimulation->getResultSoA().size());

        //Prints the the output to files defined by the OutputConfigurationSource aka the YAMLConfigurationReader
        auto outputTargets = configSource->getOutputTargets();
        for (auto &out : outputTargets) {
            out->printResult(breakUpSimulation->getResult());
        }
        //Print output for the input defined by the OutputConfigurationSource aka the YAMLConfigurationReader
        auto inputTargets = configSource->getInputTargets();
        for (auto &inOut : inputTargets) {
            inOut->printResult(breakUpSimulation->getInput());
        }
    } catch (std::exception &e) {
        spdlog::error(e.what());
    }
    return 0;
}
