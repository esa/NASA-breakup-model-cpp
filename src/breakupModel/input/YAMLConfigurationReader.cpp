#include "YAMLConfigurationReader.h"

double YAMLConfigurationReader::getMinimalCharacteristicLength() const {
    if (_file[SIMULATION_TAG][MIN_CHAR_LENGTH_TAG]) {
        return _file[SIMULATION_TAG][MIN_CHAR_LENGTH_TAG].as<double>();
    } else {
        throw std::runtime_error{"The minimal characteristic Length was not specified in the YAML Configuration file!"};
    }
}

SimulationType YAMLConfigurationReader::getTypeOfSimulation() const {
    if (_file[SIMULATION_TAG][SIMULATION_TYPE_TAG]) {
        try {
            SimulationType simulationType = InputConfigurationSource::stringToSimulationType.at(
                    _file[SIMULATION_TAG][SIMULATION_TYPE_TAG].as<std::string>());
            return simulationType;
        } catch (std::exception &e) {
            spdlog::warn("The simulation type could not be parsed from the YAML Configuration file! "
                         "SimulationType therefore UNKNOWN!");
            return SimulationType::UNKNOWN;
        }
    } else {
        spdlog::warn("The simulation type was not given in the YAML Configuration file! "
                     "SimulationType therefore UNKNOWN!");
        return SimulationType::UNKNOWN;
    }
}

std::optional<size_t> YAMLConfigurationReader::getCurrentMaximalGivenID() const {
    if (_file[SIMULATION_TAG][CURRENT_MAX_ID_TAG]) {
        return std::make_optional(_file[SIMULATION_TAG][CURRENT_MAX_ID_TAG].as<size_t>());
    } else {
        return std::nullopt;
    }
}

std::shared_ptr<const DataSource> YAMLConfigurationReader::getDataReader() const {
    std::vector<std::string> fileNames{};
    if (_file[SIMULATION_TAG][INPUT_SOURCE_TAG] && _file[SIMULATION_TAG][INPUT_SOURCE_TAG].IsSequence()) {
        for (auto inputSource : _file[SIMULATION_TAG][INPUT_SOURCE_TAG]) {
            fileNames.push_back(inputSource.as<std::string>());
        }
    }

    //fileName.yaml
    if (fileNames.size() == 1 && fileNames[0].find(".yaml")) {
        return std::make_shared<YAMLDataReader>(fileNames[0]);
    } else if (fileNames.size() == 2) {
        //fileName.csv (should be satcat) && fileName.txt (should be tle)
        if (fileNames[0].find(".csv") && fileNames[1].find(".txt")) {
            return std::make_shared<TLESatcatDataReader>(fileNames[0], fileNames[1]);
            //fileName.tle (should be tle) && fileName.csv (should be satcat)
        } else if (fileNames[0].find(".txt") && fileNames[1].find(".csv")) {
            return std::make_shared<TLESatcatDataReader>(fileNames[1], fileNames[0]);
        }
    }
    //Error Handling
    std::stringstream message{};
    message << "The YAML Configuration Reader parsed the following files as Data Input:\n";
    for (unsigned int i = 0; i < fileNames.size(); ++i) {
        message << '{' << i << ": " << fileNames[i] << "}\n";
    }
    message << "This is no valid configuration!";
    throw std::runtime_error{message.str()};
}

std::optional<std::set<size_t>> YAMLConfigurationReader::getIDFilter() const {
    if (_file[SIMULATION_TAG][ID_FILTER_TAG] && _file[SIMULATION_TAG][ID_FILTER_TAG].IsSequence()) {
        std::set<size_t> filterSet{};
        for (auto id : _file[SIMULATION_TAG][ID_FILTER_TAG]) {
            filterSet.insert(id.as<size_t>());
        }
        return std::make_optional(filterSet);
    }
    return std::nullopt;
}

bool YAMLConfigurationReader::getEnforceMassConservation() const {
    if (_file[SIMULATION_TAG][ENFORCE_MASS_CONSERVATION_TAG]) {
        return _file[SIMULATION_TAG][ENFORCE_MASS_CONSERVATION_TAG].as<bool>();
    } else {
        return false;
    }
}

std::vector<std::shared_ptr<OutputWriter>> YAMLConfigurationReader::getOutputTargets() const {
    if (_file[RESULT_OUTPUT_TAG]) {
        return extractOutputWriter(_file[RESULT_OUTPUT_TAG]);
    } else {
        spdlog::info("You have defined no way of output for the result of the simulation!");
    }
    return std::vector<std::shared_ptr<OutputWriter>>{};
}

std::vector<std::shared_ptr<OutputWriter>> YAMLConfigurationReader::getInputTargets() const {
    std::vector<std::shared_ptr<OutputWriter>> outputs{};
    if (_file[INPUT_OUTPUT_TAG]) {
        return extractOutputWriter(_file[INPUT_OUTPUT_TAG]);
    } //This param is optional, so no info, if no output for input is specified
    return std::vector<std::shared_ptr<OutputWriter>>{};
}

std::vector<std::shared_ptr<OutputWriter>>
YAMLConfigurationReader::extractOutputWriter(const YAML::Node &node) {
    //If no targets are given, we can save a lot of work
    if (!node[TARGET_TAG]) {
        throw std::runtime_error{"You specified an output tag, but did not give it any targets!"};
    }
    //Start extracting the OutputWriter
    std::vector<std::shared_ptr<OutputWriter>> outputs{};
    for (auto outputFile : node[TARGET_TAG]) {
        std::string filename{outputFile.as<std::string>()};
        if (filename.substr(filename.size() - 3) == "csv") {            //CSV Case
            if (node[CSV_PATTERN_TAG]) {
                auto pattern = node[CSV_PATTERN_TAG].as<std::string>();
                outputs.push_back(std::shared_ptr<OutputWriter>(new CSVPatternWriter(filename, pattern)));
            } else {
                bool kepler = false;
                if (node[KEPLER_TAG]) {
                    kepler = node[KEPLER_TAG].as<bool>();
                }
                outputs.push_back(std::shared_ptr<OutputWriter>(new CSVWriter(filename, kepler)));
            }
        } else if (filename.substr(filename.size() - 3) == "vtu") {     //VTK Case
            outputs.push_back(std::shared_ptr<OutputWriter>(new VTKWriter(filename)));
        } else {
            spdlog::warn("The file {} is no available output form. Available are csv and vtu Output", filename);
        }
    }
    //Lastly check, if we really extracted OutputWriter
    if (outputs.empty()) {
        spdlog::warn("You have defined ResultOutput/ InputOutput with no valid file formats!");
    }
    return outputs;
}
