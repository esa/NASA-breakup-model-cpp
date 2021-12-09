#include "Satellite.h"

std::ostream &operator<<(std::ostream &os, SatType satType) {
    return os << Satellite::satTypeToString.at(satType);
}

std::istream &operator>>(std::istream &istream, SatType &satType) {
    std::string string;
    istream >> string;
    try {
        satType = Satellite::stringToSatType.at(string);
        return istream;
    } catch (std::exception &e) {
        throw std::runtime_error{"SatType could not be parsed correctly from string: \"" + string + "\"!"};
    }
}

const std::map<std::string, SatType> Satellite::stringToSatType{{"SPACECRAFT",  SatType::SPACECRAFT},
                                                                {"SC",          SatType::SPACECRAFT},
                                                                {"PAY",         SatType::SPACECRAFT},
                                                                {"ROCKET_BODY", SatType::ROCKET_BODY},
                                                                {"RB",          SatType::ROCKET_BODY},
                                                                {"R/B",         SatType::ROCKET_BODY},
                                                                {"DEBRIS",      SatType::DEBRIS},
                                                                {"DEB",         SatType::DEBRIS},
                                                                {"UNKNOWN",     SatType::UNKNOWN},
                                                                {"UNK",         SatType::UNKNOWN}
};

const std::map<SatType, std::string> Satellite::satTypeToString{{SatType::SPACECRAFT,  "SPACECRAFT"},
                                                                {SatType::ROCKET_BODY, "ROCKET_BODY"},
                                                                {SatType::DEBRIS,      "DEBRIS"},
                                                                {SatType::UNKNOWN,     "UNKNOWN"}
};

void Satellite::setCartesianByOrbitalElements(const OrbitalElements &orbitalElements) {
    using namespace util;
    //Sets the Orbital Elements cache
    _orbitalElementsCache = std::make_optional(orbitalElements);

    auto keplerianElements = orbitalElements.getAsArray();

    double a = keplerianElements[0];
    double e = keplerianElements[1];
    double i = keplerianElements[2];
    double omg = keplerianElements[3];
    double omp = keplerianElements[4];
    double EA = keplerianElements[5];
    double b, n, xper, yper, xdotper, ydotper;
    std::array<std::array<double, 3>, 3> R{};
    double cosomg, cosomp, sinomg, sinomp, cosi, sini;
    double dNdZeta;

    // semi-major axis is assumed to be positive here we apply the convention of having it negative as for
    // computations to result in higher elegance
    if (e > 1.0) {
        a = -a;
    }

    // 1 - We start by evaluating position and velocity in the perifocal reference system

    const double cosEA = std::cos(EA);
    if (e < 1.0) {
        // EA is the eccentric anomaly
        const double sinEA = std::sin(EA);

        b = a * std::sqrt(1.0 - e * e);
        n = std::sqrt(GRAVITATIONAL_PARAMETER_EARTH / (a * a * a));

        xper = a * (cosEA - e);
        yper = b * sinEA;
        xdotper = -(a * n * sinEA) / (1.0 - e * cosEA);
        ydotper = (b * n * cosEA) / (1.0 - e * cosEA);
    } else {
        // EA is the Gudermannian
        const double tanEA = tan(EA);
        const double tanEA_PI_4 = std::tan(0.5 * EA + PI_4);

        b = -a * sqrt(e * e - 1.0);
        n = sqrt((-GRAVITATIONAL_PARAMETER_EARTH) / (a * a * a));

        dNdZeta = e * (1.0 + tanEA * tanEA) - (0.5 + 0.5 * tanEA_PI_4 * tanEA_PI_4) / tanEA_PI_4;

        xper = a / cosEA - a * e;
        yper = b * tanEA;

        xdotper = a * tanEA / cosEA * n / dNdZeta;
        ydotper = b / cosEA * cosEA * n / dNdZeta;
    }

    // 2 - We then built the rotation matrix from perifocal reference frame to inertial
    cosomg = std::cos(omg);
    cosomp = std::cos(omp);
    sinomg = std::sin(omg);
    sinomp = std::sin(omp);
    cosi = std::cos(i);
    sini = std::sin(i);

    R[0][0] = cosomg * cosomp - sinomg * sinomp * cosi;
    R[0][1] = -cosomg * sinomp - sinomg * cosomp * cosi;
    R[0][2] = sinomg * sini;
    R[1][0] = sinomg * cosomp + cosomg * sinomp * cosi;
    R[1][1] = -sinomg * sinomp + cosomg * cosomp * cosi;
    R[1][2] = -cosomg * sini;
    R[2][0] = sinomp * sini;
    R[2][1] = cosomp * sini;
    R[2][2] = cosi;

    // 3 - We end by transforming according to this rotation matrix
    std::array<double, 3> temp{xper, yper, 0.0};
    std::array<double, 3> temp2{xdotper, ydotper, 0};

    for (int j = 0; j < 3; j++) {
        _position[j] = 0.0;
        _velocity[j] = 0.0;
        for (int k = 0; k < 3; k++) {
            _position[j] += R[j][k] * temp[k];
            _velocity[j] += R[j][k] * temp2[k];
        }
    }
}

OrbitalElements Satellite::getOrbitalElements() const {
    using namespace util;

    //Save same expensive operations by using the cache
    if (_orbitalElementsCache.has_value()) {
        return _orbitalElementsCache.value();
    }

    std::array<double, 6> keplerianElements{};

    std::array<double, 3> k = {0.0, 0.0, 1.0};
    // build generic arrays to size - init values don't matter:
    std::array<double, 3> h{};
    std::array<double, 3> Dum_Vec{};
    std::array<double, 3> n{};
    std::array<double, 3> evett{};

    double p, temp, R0, ni;

    // 1 - We compute h: the orbital angular momentum vector
    h = cross(_position, _velocity);

    // 2 - We compute p: the orbital parameter
    p = dot(h, h) / GRAVITATIONAL_PARAMETER_EARTH; // h^2 / mu

    // 3 - We compute n: the vector of the node line
    // This operation is singular when inclination is zero, in which case the orbital parameters
    // are not defined
    n = cross(k, h);
    // vers(x, y) = unit vector of y -> x
    n = n / euclideanNorm(n);

    // 4 - We compute evett: the eccentricity vector
    R0 = euclideanNorm(_position);
    Dum_Vec = cross(_velocity, h);
    evett = Dum_Vec / GRAVITATIONAL_PARAMETER_EARTH - _position / R0;

    // The eccentricity is calculated and stored as the second orbital element
    keplerianElements[1] = euclideanNorm(evett);

    // The semi-major axis (positive quantity) is calculated and stored as the first orbital element
    keplerianElements[0] = std::abs(p / (1.0 - keplerianElements[1] * keplerianElements[1]));

    // Inclination is calculated and stored as the third orbital element
    keplerianElements[2] = std::acos(h[2] / euclideanNorm(h));

    // Argument of pericentrum is calculated and stored as the fifth orbital element
    temp = dot(n, evett);
    keplerianElements[4] = std::acos(temp / keplerianElements[1]);
    if (evett[2] < 0.0) {
        keplerianElements[4] = PI2 - keplerianElements[4];
    }

    // Argument of longitude is calculated and stored as the fourth orbital element
    keplerianElements[3] = std::acos(n[0]);
    if (n[1] < 0.0) {
        keplerianElements[3] = PI2 - keplerianElements[3];
    }

    temp = dot(evett, _position);

    // 4 - We compute ni: the true anomaly (in 0, 2*PI)
    ni = std::acos(temp / keplerianElements[1] / R0);

    temp = dot(_position, _velocity);

    if (temp < 0.0) {
        ni = PI2 - ni;
    }

    // Eccentric anomaly or the gudermannian is calculated and stored as the sixth orbital element
    // algebraic equivalent of kepler's equation (else case: in terms of the Gudermannian)
    const double tanNi_2 = std::tan(ni / 2.0);
    double root;
    if (keplerianElements[1] < 1.0) {
        root = (1.0 - keplerianElements[1]) / (1.0 + keplerianElements[1]);

    } else {
        root = (keplerianElements[1] - 1.0) / (keplerianElements[1] + 1.0);
    }
    keplerianElements[5] = 2.0 * atan(sqrt(root) * tanNi_2);

    //Add 2*pi if the angle is negative in order to norm it to positive values
    if (keplerianElements[5] < 0) {
        keplerianElements[5] += PI2;
    }

    OrbitalElements orbitalElements {keplerianElements};

    //Sets the orbital elements cache to the new calculated values
    _orbitalElementsCache = std::make_optional(orbitalElements);
    return orbitalElements;
}

std::ostream &operator<<(std::ostream &os, const Satellite &satellite) {
    using util::operator<<;
    os << "Satellite{"
       << "_id: " << satellite._id << " _name: " << satellite._name << " _satType: " << satellite._satType
       << " _characteristicLength: " << satellite._characteristicLength << " _velocity: " << satellite._velocity
       << " _position: " << satellite._position << "}";
    return os;
}
