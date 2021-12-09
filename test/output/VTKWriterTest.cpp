#include "gtest/gtest.h"

#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include "breakupModel/model/Satellite.h"
#include "breakupModel/output/VTKWriter.h"

class VTKWriterTest : public ::testing::Test {

protected:

    virtual void SetUp() {
        for (unsigned int i = 1; i <= 4; ++i) {
            auto d = static_cast<double>(i);
            Satellite satellite{i};
            satellite.setPosition({d, d, d});
            satellite.setVelocity({d, d, d});
            satellite.setEjectionVelocity({d*100, d, d});
            satellite.setMass(d * 10);
            satellite.setCharacteristicLength(d * 100);
            _satelliteCollection.push_back(satellite);
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

    const std::string _filePath{"resources/VTKWriterTestActual.vtu"};

    const std::string _expectedFilePath{"resources/VTKWriterTest.vtu"};

    std::vector<Satellite> _satelliteCollection{};

};

TEST_F(VTKWriterTest, FileCreation) {
    ASSERT_FALSE(std::filesystem::exists(_filePath)) << "File should not yet exist!";

    auto vtkTestLogger = spdlog::basic_logger_mt("VTKWriterTest", _filePath, true);
    VTKWriter vtkWriter{vtkTestLogger};

    ASSERT_TRUE(std::filesystem::exists(_filePath)) << "A CSV file should have been generated!";
}

TEST_F(VTKWriterTest, DataCheck) {
    auto vtkTestLogger = spdlog::basic_logger_mt("VTKWriterTest", _filePath, true);
    VTKWriter vtkWriter{vtkTestLogger};
    vtkWriter.printResult(_satelliteCollection);
    vtkTestLogger->flush();

    std::ifstream expectedFile{_expectedFilePath};
    std::ifstream actualFile(_filePath);

    std::string expectedLine;
    std::string actualLine;

    unsigned int actualLineNumber = 1;
    while(std::getline(expectedFile, expectedLine) && std::getline(actualFile, actualLine)) {
        ASSERT_EQ(actualLine, expectedLine) << "The error was in line " << actualLineNumber;
        actualLineNumber += 1;
    }
    ASSERT_EQ(actualLineNumber, 58);
}