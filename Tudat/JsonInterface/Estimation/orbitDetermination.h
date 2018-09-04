/*    Copyright (c) 2010-2018, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_JSONINTERFACE_ORBITDETERMINATION_H
#define TUDAT_JSONINTERFACE_ORBITDETERMINATION_H

#include "Tudat/Astrodynamics/OrbitDetermination/podInputOutputTypes.h"
#include "Tudat/JsonInterface/Support/valueAccess.h"
#include "Tudat/JsonInterface/Support/valueConversions.h"

namespace tudat
{

namespace simulation_setup
{

//! Create a `json` object from a shared pointer to a `EstimatableParameterSettings` object.
template< typename ObservationScalarType = double, typename TimeType = double >
void to_json( nlohmann::json& jsonObject, const std::shared_ptr< PodSettings< ObservationScalarType, TimeType > >& parameterSettings )
{

}

//! Create a shared pointer to a `EstimatableParameterSettings` object from a `json` object.
template< typename ObservationScalarType = double, typename TimeType = double >
void from_json( const nlohmann::json& jsonObject, std::shared_ptr< PodSettings< ObservationScalarType, TimeType > >& parameterSettings )
{
    using namespace json_interface;
    using K = Keys::Estimation;

    const bool reintegrateEquationsOnFirstIteration =
            getValue< bool >( jsonObject, K::reintegrateEquationsOnFirstIteration, true );
    const bool reintegrateVariationalEquations =
            getValue< bool >( jsonObject, K::reintegrateVariationalEquations, true );
    const bool saveInformationMatrix =
            getValue< bool >( jsonObject, K::saveInformationMatrix, true );
    const bool printOutput =
            getValue< bool >( jsonObject, K::printOutput, true );
    const bool saveResidualsAndParametersFromEachIteration =
            getValue< bool >( jsonObject, K::saveResidualsAndParametersFromEachIteration, true );
    const bool saveStateHistoryForEachIteration =
            getValue< bool >( jsonObject, K::saveStateHistoryForEachIteration, false );


    const int maximumNumberOfIterations =
            getValue< int >( jsonObject, K::maximumNumberOfIterations, 5 );
    const double minimumResidualChange =
            getValue< double >( jsonObject, K::reintegrateEquationsOnFirstIteration, 0.0 );
    const double minimumResidual =
            getValue< double >( jsonObject, K::reintegrateEquationsOnFirstIteration, 1.0E-20 );
    const int numberOfIterationsWithoutImprovement =
            getValue< int >( jsonObject, K::reintegrateEquationsOnFirstIteration, 2 );

    parameterSettings = std::make_shared< PodSettings< ObservationScalarType, TimeType > >(
                std::make_shared< EstimationConvergenceChecker >(
                    maximumNumberOfIterations, minimumResidualChange, minimumResidual, numberOfIterationsWithoutImprovement ),
                reintegrateEquationsOnFirstIteration, reintegrateVariationalEquations, saveInformationMatrix,
                printOutput, saveResidualsAndParametersFromEachIteration, saveStateHistoryForEachIteration );
}

void updateInverseAPrioriCovarianceFromJSON(
        const nlohmann::json& jsonObject, const int numberOfParameters, Eigen::MatrixXd& inverseAprioriCovariance );

void updateObservationWeightsFromJSON(
        const nlohmann::json& jsonObject,
        const std::map< observation_models::ObservableType, std::map< observation_models::LinkEnds, int > > numberOfObservations,
        std::map< observation_models::ObservableType, std::map< observation_models::LinkEnds, Eigen::VectorXd > >& observableWeights );

} // namespace propagators

} // namespace tudat

#endif // TUDAT_JSONINTERFACE_ORBITDETERMINATION_H
