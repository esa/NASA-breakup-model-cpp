#pragma once

#include "Breakup.h"

/**
 * A Explosion Breakup of one satellite.
 */
class Explosion : public Breakup {

public:

    using Breakup::Breakup;

private:

    void init() final;

    void calculateFragmentCount() final;

    void assignParentProperties() final;

};

