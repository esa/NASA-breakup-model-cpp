#pragma once

#include <cmath>
#include "UtilityFunctions.h"

/*
 * Functions adapted from pykep:
 * -https://github.com/esa/pykep/blob/master/include/keplerian_toolbox/core_functions/convert_anomalies.hpp
 * -https://github.com/esa/pykep/blob/master/include/keplerian_toolbox/numerics/newton_raphson.hpp
 * -https://github.com/esa/pykep/blob/master/include/keplerian_toolbox/core_functions/kepler_equations.hpp
 * [Links accessed 23.06.2021]
 */

namespace util {

    /**
    * Gravitational Parameter of the earth in [m^3/s^2]
    */
    constexpr double GRAVITATIONAL_PARAMETER_EARTH = 398600441880000.0;

    namespace detail {

        constexpr double accuracy = 1e-16;


        // With the eccentric anomaly (EA)
        inline double kepE(double EA, double MA, double eccentricity) {
            return (EA - eccentricity * std::sin(EA) - MA);
        }

        inline double d_kepE(double EA, double eccentricity) {
            return (1.0 - eccentricity * std::cos(EA));
        }

        inline double newtonRaphson(double EA, double MA, double e) {
            int max_loop = 100;
            double term;
            // double start = x; //DS: unused
            // main iteration
            do {
                term = kepE(EA, MA, e) / d_kepE(EA, e);
                EA = EA - term;
            }
                // check if term is within required accuracy or loop limit is exceeded
            while ((std::fabs(term / std::max(std::fabs(EA), 1.)) > accuracy) && (--max_loop));
            return EA;
        }

        /**
         * Norms the angle to positive values.
         * @param angle in [rad], might be negative
         * @return angle in [rad], positive
         */
        inline double normAngle(double angle) {
            return angle < 0.0 ? angle + PI2 : angle;
        }

    }

    /**
     * Converts the Mean Anomaly to the Eccentric Anomaly.
     * @param MA - Mean Anomaly [rad]
     * @param e - eccentricity
     * @return Eccentric Anomaly in [rad]
     */
    inline double meanAnomalyToEccentricAnomaly(double MA, double e) {
        using namespace detail;
        double EA = MA + e * std::sin(MA);
        return normAngle(newtonRaphson(EA, MA, e));
    }

    /**
     * Converts the Eccentric Anomaly to the Mean Anomaly.
     * @param EA - Eccentric Anomaly [rad]
     * @param e - eccentricity
     * @return Mean Anomaly in [rad]
     */
    inline double eccentricAnomalyToMeanAnomaly(double EA, double e) {
        using namespace detail;
        return normAngle(EA - e * std::sin(EA));
    }

    /**
     * Converts the True Anomaly to the Eccentric Anomaly.
     * @param TA - True Anomaly [rad]
     * @param e  - eccentricity
     * @return Eccentric Anomaly in [rad]
     */
    inline double trueAnomalyToEccentricAnomaly(double TA, double e) {
        using namespace detail;
        return normAngle(2.0 * std::atan(std::sqrt((1.0 - e) / (1.0 + e)) * std::tan(TA / 2.0)));
    }

    /**
     * Converts the Eccentric Anomaly to the True Anomaly.
     * @param EA - Eccentric Anomaly [rad]
     * @param e  - eccentricity
     * @return True Anomaly in [rad]
     */
    inline double eccentricAnomalyToTrueAnomaly(double EA, double e) {
        using namespace detail;
        return normAngle(2.0 * std::atan(std::sqrt((1.0 + e) / (1.0 - e)) * std::tan(EA / 2.0)));
    }

    /**
     * Transforms the mean Motion to the semi major axis.
     * The mean motion in the TLE format is given in [rev/day]
     * @param meanMotion in [rev/day] (1 rev/day = 2*pi/86400 rad/s)
     * @return semi major axis in [m]
     */
    inline double meanMotionToSemiMajorAxis(double meanMotion) {
        //GRAVITATIONAL_PARAMETER_EARTH^(1/3)
        static constexpr double thirdRoot = 73594.59595000199;
        static constexpr double twoThird = 2.0 / 3.0;
        static constexpr double fac = PI2 / 86400.0;

        return thirdRoot / std::pow(fac * meanMotion, twoThird);
    }

}
