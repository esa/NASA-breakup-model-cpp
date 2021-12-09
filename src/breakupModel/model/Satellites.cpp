#include "Satellites.h"

std::vector<std::tuple<double &, std::array<double, 3> &, std::array<double, 3> &>> Satellites::getVelocityTuple() {
    std::vector<std::tuple<double &, std::array<double, 3> &, std::array<double, 3> &>> vector{};
    vector.reserve(size());
    for (size_t i = 0; i < size(); ++i) {
        vector.emplace_back(areaToMassRatio[i], velocity[i], ejectionVelocity[i]);
    }
    return vector;
}

std::vector<std::tuple<double &, double &, double &, double &>> Satellites::getAreaMassTuple() {
    std::vector<std::tuple<double &, double &, double &, double &>> vector{};
    vector.reserve(size());
    for (size_t i = 0; i < size(); ++i) {
        vector.emplace_back(characteristicLength[i], areaToMassRatio[i], area[i], mass[i]);
    }
    return vector;
}

std::vector<std::tuple<double &, double &, std::array<double, 3> &, std::shared_ptr<const std::string> &>>
Satellites::getCMVNTuple() {
    std::vector<std::tuple<double &, double &, std::array<double, 3> &, std::shared_ptr<const std::string> &>> vector{};
    vector.reserve(size());
    for (size_t i = 0; i < size(); ++i) {
        vector.emplace_back(characteristicLength[i], mass[i], velocity[i], name[i]);
    }
    return vector;
}

std::vector<std::tuple<std::array<double, 3> &, std::shared_ptr<const std::string> &>> Satellites::getVNTuple() {
    std::vector<std::tuple<std::array<double, 3> &, std::shared_ptr<const std::string> &>> vector{};
    vector.reserve(size());
    for (size_t i = 0; i < size(); ++i) {
        vector.emplace_back(velocity[i], name[i]);
    }
    return vector;
}

std::vector<Satellite> Satellites::getAoS() const {
    std::vector<Satellite> vector{};
    size_t size = this->size();
    size_t id = startId;
    vector.reserve(size);

    auto nameIt = name.begin();
    auto lcIt = characteristicLength.begin();
    auto amIt = areaToMassRatio.begin();
    auto mIt = mass.begin();
    auto aIt = area.begin();
    auto vIt = velocity.begin();
    auto evIt = ejectionVelocity.begin();

    for (; lcIt != characteristicLength.end(); ++nameIt, ++lcIt, ++amIt, ++mIt, ++aIt, ++vIt, ++evIt) {
        vector.emplace_back(id++, *nameIt, satType, *lcIt, *amIt, *mIt, *aIt, *vIt, *evIt, position);
    }
    return vector;
}

void Satellites::popBack() {
    this->resize(this->size() - 1);
}

std::tuple<double &, double &, double &, double &> Satellites::appendElement() {
    this->resize(this->size() + 1);
    return std::tuple<double &, double &, double &, double &>
            {characteristicLength.back(), areaToMassRatio.back(), area.back(), mass.back()};
}

