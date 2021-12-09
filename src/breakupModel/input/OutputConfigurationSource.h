#pragma once

#include <vector>
#include <memory>
#include "breakupModel/output/OutputWriter.h"

/**
 * Pure virtual interface for the definition of OutputSources
 */
class OutputConfigurationSource {

public:

    /**
     * Returns a vector of pointers to OutputTargets defined by a given configuration source.
     * This vector contains the OutputWriter which should be used to print the result of the breakup event.
     * @return OutputTargets
     */
    virtual std::vector<std::shared_ptr<OutputWriter>> getOutputTargets () const = 0;

    /**
    * Returns a vector of pointers to InputTargets defined by a given configuration source.
     * This vector contains the OutputWriter which should be used to print the input data.
    * @return InputTargets
    */
    virtual std::vector<std::shared_ptr<OutputWriter>> getInputTargets () const = 0;

};