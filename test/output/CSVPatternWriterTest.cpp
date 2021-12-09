#include "gtest/gtest.h"

#include <filesystem>
#include <vector>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/output/CSVPatternWriter.h"
#include "breakupModel/input/CSVReader.h"

class CSVPatternWriterTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        double area = 1.0;
        double mass = 100;
        size_t id = 0;
        _satelliteCollection.resize(static_cast<size_t>(4), Satellite("DebrisTestFragment", SatType::DEBRIS));
        for (auto &sat : _satelliteCollection) {
            sat.setId(++id);
            sat.setCharacteristicLength(0.25);
            sat.setArea(area);
            sat.setMass(mass);
            sat.setAreaToMassRatio(1.0 / 100.0);
            sat.setVelocity({1.0, 2.0, 3.0});
            area += 1.0;
            mass += 100.0;
        }

    }

    virtual void TearDown() {
        _satelliteCollection.clear();
        try {
            std::filesystem::remove(_filePath);
        } catch (std::exception &e) {
            std::cerr << e.what() << std::endl;
        }
    }

    const std::string _filePath{"resources/csvPatternTestFile.csv"};

    const std::string _pattern{"IntLRAmvp"};

    std::vector<Satellite> _satelliteCollection{};

};

TEST_F(CSVPatternWriterTest, FileCreation) {
    ASSERT_FALSE(std::filesystem::exists(_filePath)) << "File should not yet exist!";

    auto csvTestLogger = spdlog::basic_logger_mt("CSVWriterTest", _filePath, true);
    CSVPatternWriter csvWriter{csvTestLogger, _pattern};

    ASSERT_TRUE(std::filesystem::exists(_filePath)) << "A CSV file should have been generated!";
}

TEST_F(CSVPatternWriterTest, HeaderCheck) {
    auto csvTestLogger = spdlog::basic_logger_mt("CSVWriterTest", _filePath, true);
    CSVPatternWriter csvWriter{csvTestLogger, _pattern};
    csvTestLogger->flush();

    csvWriter.printResult(_satelliteCollection);
    csvTestLogger->flush();

    CSVReader<size_t, std::string, SatType,
            double, double, double, double,
            std::string, std::string>
            csvReader{_filePath, true};

    auto header = csvReader.getHeader();

    ASSERT_EQ(header.at(0), "ID");
    ASSERT_EQ(header.at(1), "Name");
    ASSERT_EQ(header.at(2), "Satellite Type");
    ASSERT_EQ(header.at(3), "Characteristic Length [m]");
    ASSERT_EQ(header.at(4), "A/M [m^2/kg]");
    ASSERT_EQ(header.at(5), "Area [m^2]");
    ASSERT_EQ(header.at(6), "Mass [kg]");
    ASSERT_EQ(header.at(7), "Velocity [m/s]");
    ASSERT_EQ(header.at(8), "Position [m]");

}

TEST_F(CSVPatternWriterTest, DataCheck) {
    auto csvTestLogger = spdlog::basic_logger_mt("CSVWriterTest", _filePath, true);
    CSVPatternWriter csvWriter{csvTestLogger, _pattern};

    csvWriter.printResult(_satelliteCollection);
    csvTestLogger->flush();

    CSVReader<size_t, std::string, SatType,
            double, double, double, double,
            std::string, std::string> csvReader{_filePath, true};

    size_t expectedID = 1;
    const std::string expectedName{"DebrisTestFragment"};
    const std::string expectedType{"DEBRIS"};
    double expectedCharacteristicLength = 0.25;
    double expectedArea = 1.0;
    double expectedMass = 100.0;
    double expectedAM = 1.0 / 100.0;
    const std::string expectedVelocity{"[1 2 3]"};
    const std::string expectedPosition{"[0 0 0]"};

    auto lines = csvReader.getLines();
    auto line = lines.begin();
    for (auto &sat : _satelliteCollection) {
        ASSERT_EQ(std::get<0>(*line), expectedID);
        ASSERT_EQ(std::get<1>(*line), expectedName);
        ASSERT_EQ(Satellite::satTypeToString.at(std::get<2>(*line)), expectedType);
        ASSERT_DOUBLE_EQ(std::get<3>(*line), expectedCharacteristicLength);
        ASSERT_DOUBLE_EQ(std::get<4>(*line), expectedAM);
        ASSERT_DOUBLE_EQ(std::get<5>(*line), expectedArea);
        ASSERT_DOUBLE_EQ(std::get<6>(*line), expectedMass);
        ASSERT_EQ(std::get<7>(*line), expectedVelocity);
        ASSERT_EQ(std::get<8>(*line), expectedPosition);
        expectedID += 1;
        expectedArea += 1.0;
        expectedMass += 100.0;

        ++line;
    }
}