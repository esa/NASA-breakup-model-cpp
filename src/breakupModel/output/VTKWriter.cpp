#include "VTKWriter.h"

void VTKWriter::printResult(const std::vector<Satellite> &satelliteCollection) const {
    //Header
    this->printHeader(satelliteCollection.size());

    //Point properties
    this->printProperty<double, Satellite>("characteristic-length", &Satellite::getCharacteristicLength, satelliteCollection);
    this->printProperty<double, Satellite>("mass", &Satellite::getMass, satelliteCollection);
    this->printProperty<double, Satellite>("area", &Satellite::getArea, satelliteCollection);
    this->printProperty<double, Satellite>("area-to-mass", &Satellite::getAreaToMassRatio, satelliteCollection);
    this->printProperty<std::array<double, 3>, Satellite>("velocity", &Satellite::getVelocity, satelliteCollection);
    this->printProperty<std::array<double, 3>, Satellite>("ejection-velocity", &Satellite::getEjectionVelocity, satelliteCollection);

    //Separator between point and point-tore-cell data
    this->printSeparator();

    //Point properties related to cell (position)
    this->printProperty<std::array<double, 3>, Satellite>("position", &Satellite::getPosition, satelliteCollection);

    //Footer
    this->printFooter();
}

void VTKWriter::printHeader(size_t size) const {
    _logger->info(R"(<?xml version="1.0" encoding="UTF-8" standalone="no" ?>)");
    _logger->info(R"(<VTKFile byte_order="LittleEndian" type="UnstructuredGrid" version="0.1">)");
    _logger->info(R"(  <UnstructuredGrid>)");
    _logger->info(R"(    <Piece NumberOfCells="0" NumberOfPoints="{}">)", size);
    _logger->info(R"(      <PointData>)");
}

void VTKWriter::printSeparator() const {
    _logger->info(R"(      </PointData>)");
    _logger->info(R"(      <CellData/>)");
    _logger->info(R"(      <Points>)");
}

void VTKWriter::printFooter() const {
    _logger->info(R"(      </Points>)");
    _logger->info(R"(      <Cells>)");
    _logger->info(R"(        <DataArray Name="types" NumberOfComponents="0" format="ascii" type="Float32"/>)");
    _logger->info(R"(      </Cells>)");
    _logger->info(R"(    </Piece>)");
    _logger->info(R"(  </UnstructuredGrid>)");
    _logger->info(R"(</VTKFile>)");
}

