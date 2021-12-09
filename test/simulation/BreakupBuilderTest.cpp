#include "gtest/gtest.h"

#include <vector>
#include <optional>
#include <memory>
#include "breakupModel/simulation/BreakupBuilder.h"
#include "breakupModel/input/RuntimeInputSource.h"

class BreakupBuilderTest : public ::testing::Test {

protected:

    double _expectedMinimalLength = 0.05;
    const std::vector<Satellite> _satellites1{Satellite{1}};
    const std::vector<Satellite> _satellites2{Satellite{1}, Satellite{2}};
    const std::vector<Satellite> _satellites3{Satellite{1}, Satellite{2}, Satellite{3}};
    const std::vector<Satellite> _satellites4{Satellite{2}, Satellite{3}};

};

TEST_F(BreakupBuilderTest, ConfigExplosionWeak) {
    const std::shared_ptr<RuntimeInputSource> config =
            std::make_shared<RuntimeInputSource>(_expectedMinimalLength, _satellites1);
    BreakupBuilder breakupBuilder {config};

    auto breakup = breakupBuilder.getBreakup();

    ASSERT_EQ(breakup->getMinimalCharacteristicLength(), _expectedMinimalLength);
    ASSERT_EQ(breakup->getInput(), _satellites1);
    ASSERT_EQ(breakup->getCurrentMaxGivenId(), 1);
}

TEST_F(BreakupBuilderTest, ConfigCollisionWeak) {
    const std::shared_ptr<RuntimeInputSource> config =
            std::make_shared<RuntimeInputSource>(_expectedMinimalLength, _satellites2);
    BreakupBuilder breakupBuilder {config};

    auto breakup = breakupBuilder.getBreakup();

    ASSERT_EQ(breakup->getMinimalCharacteristicLength(), _expectedMinimalLength);
    ASSERT_EQ(breakup->getInput(), _satellites2);
    ASSERT_EQ(breakup->getCurrentMaxGivenId(), 2);
}

TEST_F(BreakupBuilderTest, ConfigCollisionInvalid) {
    const std::shared_ptr<RuntimeInputSource> config =
            std::make_shared<RuntimeInputSource>(_expectedMinimalLength, _satellites3);
    BreakupBuilder breakupBuilder {config};

    ASSERT_THROW(breakupBuilder.getBreakup(), std::runtime_error) << "This should throw an "
                                                                      "exception, because 3 satellites is invalid for "
                                                                      "a collision";
}

TEST_F(BreakupBuilderTest, ConfigExplosionInvalid) {
    const std::shared_ptr<RuntimeInputSource> config =
            std::make_shared<RuntimeInputSource>(_expectedMinimalLength, _satellites2,
                                                 SimulationType::EXPLOSION,
                                                 120,
                                                 std::nullopt);
    BreakupBuilder breakupBuilder {config};

    ASSERT_THROW(breakupBuilder.getBreakup(), std::runtime_error) << "This should throw an "
                                                                     "exception, because a explosion can only have "
                                                                     "one participating satellite";
}
TEST_F(BreakupBuilderTest, ConfigExplosionStrong) {
    const std::shared_ptr<RuntimeInputSource> config =
            std::make_shared<RuntimeInputSource>(_expectedMinimalLength, _satellites1,
                                                 SimulationType::EXPLOSION,
                                                 120, std::nullopt);
    BreakupBuilder breakupBuilder {config};

    auto breakup = breakupBuilder.getBreakup();

    ASSERT_EQ(breakup->getMinimalCharacteristicLength(), _expectedMinimalLength);
    ASSERT_EQ(breakup->getInput(), _satellites1);
    ASSERT_EQ(breakup->getCurrentMaxGivenId(), 120);
}

TEST_F(BreakupBuilderTest, ConfigCollisionFilter1) {
    const std::shared_ptr<RuntimeInputSource> config =
            std::make_shared<RuntimeInputSource>(_expectedMinimalLength, _satellites3,
                                                 SimulationType::COLLISION,
                                                 120,
                                                 std::optional<std::set<size_t>>{{2, 3}});
    BreakupBuilder breakupBuilder {config};

    auto breakup = breakupBuilder.getBreakup();

    ASSERT_EQ(breakup->getMinimalCharacteristicLength(), _expectedMinimalLength);
    ASSERT_EQ(breakup->getInput(), _satellites4);
    ASSERT_EQ(breakup->getCurrentMaxGivenId(), 120);
}

TEST_F(BreakupBuilderTest, ConfigCollisionFilter2) {
    const std::shared_ptr<RuntimeInputSource> config =
            std::make_shared<RuntimeInputSource>(_expectedMinimalLength, _satellites3,
                                                 SimulationType::COLLISION,
                                                 std::nullopt,
                                                 std::optional<std::set<size_t>>{{1, 2}});
    BreakupBuilder breakupBuilder {config};

    auto breakup = breakupBuilder.getBreakup();

    ASSERT_EQ(breakup->getMinimalCharacteristicLength(), _expectedMinimalLength);
    ASSERT_EQ(breakup->getInput(), _satellites2);
    ASSERT_EQ(breakup->getCurrentMaxGivenId(), 3);
}
