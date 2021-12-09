#pragma once

#include <string>
#include <vector>
#include "breakupModel/model/Satellite.h"

/**
 * Pure virtual Interface for Data Input.
 * Provides methods to get satellites from an specific input file.
 */
class DataSource {

public:

    virtual ~DataSource() = default;

    /**
     * Returns a satellite collection. Input form varies depending on subclass
     * @return SatelliteCollection
     */
    virtual std::vector<Satellite> getSatelliteCollection() const = 0;

};

