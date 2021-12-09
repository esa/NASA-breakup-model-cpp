#pragma once

#include "breakupModel/model/Satellite.h"

namespace util {

    /**
     * General Template for the if, else if, else statement used by equations 5, 6, 7.
     * @tparam Function a function taking a single argument
     * @param logLc - log_10(L_c)
     * @param lowerBound - the lowerBound of the interval
     * @param upperBound - the upper bound of the interval
     * @param lowerReturn - return value in case logLc is smaller or equal than the lowerBound
     * @param upperReturn - return value in case logLc is bigger or equal than the upperBound
     * @param midReturn - return value in case logLc is in the interval ]lowerBound; upperBound[
     * @return depending on the above
     */
    template<typename Function>
    inline double distributionConstant(double logLc, double lowerBound, double upperBound,
                                       double lowerReturn, double upperReturn, Function midReturn) {
        if (logLc <= lowerBound) {
            return lowerReturn;
        } else if (logLc >= upperBound) {
            return upperReturn;
        } else {
            return midReturn(logLc);
        }
    }

    /**
     * Returns the alpha for L_c > 11cm and the R/B or S/C Case depending on the given SatType.
     * @param logLc - log_10(L_c)
     * @return alpha as double
     */
    inline double alpha(SatType satType, double logLc) {
        return satType == SatType::ROCKET_BODY
               ? distributionConstant(logLc, -1.4, 0.0, 1.0, 0.5,
                                      [](double logLc) { return 1.0 - 0.3571 * (logLc + 1.4); })
               : distributionConstant(logLc, -1.95, 0.55, 0.0, 1.0,
                                      [](double logLc) { return 0.3 + 0.4 * (logLc + 1.2); });
    }

    /**
    * Returns the mu_1 for L_c > 11cm and the R/B or S/C Case depending on the given SatType.
    * @param logLc - log_10(L_c)
    * @return mu_1 as double
    */
    inline double mu_1(SatType satType, double logLc) {
        return satType == SatType::ROCKET_BODY
               ? distributionConstant(logLc, -0.5, 0.0, -0.45, -0.9,
                                      [](double logLc) { return -0.45 - 0.9 * (logLc + 0.5); })
               : distributionConstant(logLc, -1.1, 0.0, -0.6, -0.95,
                                      [](double logLc) { return -0.6 - 0.318 * (logLc + 1.1); });
    }

    /**
    * Returns the mu_1 for L_c > 11cm and the R/B or S/C Case depending on the given SatType.
    * @return sigma_1 as double
    */
    inline double sigma_1(SatType satType, double logLc) {
        return satType == SatType::ROCKET_BODY
               ? 0.55
               : distributionConstant(logLc, -1.3, -0.3, 0.1, 0.3,
                                      [](double logLc) { return 0.1 + 0.2 * (logLc + 1.3); });
    }

    /**
    * Returns the mu_2 for L_c > 11cm and the R/B or S/C Case depending on the given SatType.
    * @return mu_2 as double
    */
    inline double mu_2(SatType satType, double logLc) {
        return satType == SatType::ROCKET_BODY
               ? -0.9
               : distributionConstant(logLc, -0.7, -0.1, -1.2, -2.0,
                                      [](double logLc) { return -1.2 - 1.333 * (logLc + 0.7); });
    }

    /**
    * Returns the sigma_2 for L_c > 11cm and the R/B or S/C Case depending on the given SatType.
    * @param logLc - log_10(L_c)
    * @return sigma_2 as double
    */
    inline double sigma_2(SatType satType, double logLc) {
        return satType == SatType::ROCKET_BODY
               ? distributionConstant(logLc, -1.0, 0.1, 0.28, 0.1,
                                      [](double logLc) { return -0.28 - 0.1636 * (logLc + 1.0); })
               : distributionConstant(logLc, -0.5, -0.3, 0.5, 0.3,
                                      [](double logLc) { return 0.5 - (logLc + 0.5); });
    }

    /**
    * Returns the mu for L_c < 8cm.
    * @param logLc - log_10(L_c)
    * @return mu as double
    */
    inline double mu_soc(double logLc) {
        return distributionConstant(logLc, -1.75, -1.25, -0.3, -1.0,
                                    [](double logLc) { return -0.3 - 1.4 * (logLc + 1.75); });
    }

    /**
    * Returns the sigma for L_c < 8cm.
    * @param logLc - log_10(L_c)
    * @return sigma as double
    */
    inline double sigma_soc(double logLc) {
        return logLc <= -3.5 ? 0.2 : 0.2 + 0.1333 * (logLc + 3.5);
    }


}
