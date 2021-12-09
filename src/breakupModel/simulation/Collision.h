#pragma once

#include "Breakup.h"

/**
 * A collision Breakup of two satellites.
 * @attention There is no check if the two satellites are actually at the same position!
 */
class Collision : public Breakup {

    bool _isCatastrophic;

public:

    using Breakup::Breakup;

private:

    void init() final;

    void calculateFragmentCount() final;

    void assignParentProperties() final;

public:

    bool isIsCatastrophic() const {
        return _isCatastrophic;
    }

};

