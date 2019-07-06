/*    Copyright (c) 2010-2018, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 *
 *    References
 *      Wakker, K. F. (2007), Lecture Notes Astrodynamics II (Chapter 18), TU Delft course AE4-874,
 *          Delft University of technology, Delft, The Netherlands.
 *
 */

#define BOOST_TEST_MAIN

#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/unit_test.hpp>
#include <Eigen/Dense>
#include <math.h>
#include <iostream>

#include "Tudat/Astrodynamics/ShapeBasedMethods/compositeFunctionHodographicShaping.h"
#include "Tudat/Astrodynamics/ShapeBasedMethods/hodographicShaping.h"
#include "Tudat/Astrodynamics/ShapeBasedMethods/baseFunctionsHodographicShaping.h"
#include "Tudat/Astrodynamics/BasicAstrodynamics/physicalConstants.h"
#include "Tudat/Mathematics/BasicMathematics/mathematicalConstants.h"
#include "Tudat/Mathematics/BasicMathematics/basicMathematicsFunctions.h"
#include "Tudat/Mathematics/BasicMathematics/coordinateConversions.h"
#include "Tudat/Astrodynamics/Ephemerides/approximatePlanetPositions.h"
#include "Tudat/SimulationSetup/tudatSimulationHeader.h"

namespace tudat
{
namespace unit_tests
{

//! Test hodographic shaping implementation.
BOOST_AUTO_TEST_SUITE( test_hodographic_shaping )

//! Test.
BOOST_AUTO_TEST_CASE( test_hodographic_shaping1 )
{        
    double numberOfRevolutions = 1.0;

    double julianDate = 2458849.5;

    // Ephemeris departure body.
    ephemerides::EphemerisPointer pointerToDepartureBodyEphemeris = std::make_shared< ephemerides::ApproximatePlanetPositions>(
                ephemerides::ApproximatePlanetPositionsBase::BodiesWithEphemerisData::earthMoonBarycenter );

    // Ephemeris arrival body.
    ephemerides::EphemerisPointer pointerToArrivalBodyEphemeris = std::make_shared< ephemerides::ApproximatePlanetPositions >(
                ephemerides::ApproximatePlanetPositionsBase::BodiesWithEphemerisData::mars );

    // Retrieve ephemerides for departure and arrival bodies.
    std::map< double, Eigen::Vector3d > cartesianPositionDepartureBody;
    std::map< double, Eigen::Vector3d > cartesianPositionArrivalBody;
    for ( int i = 0 ; i < 690 ; i++ ){
        cartesianPositionDepartureBody[ i * physical_constants::JULIAN_DAY ] =
                pointerToDepartureBodyEphemeris->getCartesianState( julianDate + i * physical_constants::JULIAN_DAY ).segment(0,3);
        cartesianPositionArrivalBody[ i * physical_constants::JULIAN_DAY ] =
                pointerToArrivalBodyEphemeris->getCartesianState( julianDate + i * physical_constants::JULIAN_DAY ).segment(0,3);
    }

    // Compute cylindrical state of departure body.
    Eigen::VectorXd cylindricalStateOfDepartureBody = coordinate_conversions::convertCartesianToCylindricalState(
                pointerToDepartureBodyEphemeris->getCartesianState( julianDate ) );

    std::cout << "cartesian state departure body = Earth: " << pointerToDepartureBodyEphemeris->getCartesianState( julianDate ).segment(0,3)
              << "\n\n";

    std::cout << "radial distance Earth departure: " << pointerToDepartureBodyEphemeris->getCartesianState( julianDate ).segment(0,3).norm()
              << "\n\n";

    std::cout << "cartesian state arrival body = Mars: " << pointerToArrivalBodyEphemeris->getCartesianState( julianDate ).segment(0,3) << "\n\n";

    std::cout << "radial distance Mars departure: " << pointerToArrivalBodyEphemeris->getCartesianState( julianDate ).segment(0,3).norm() << "\n\n";

    // Compute cylindrical state of arrival body.
    Eigen::VectorXd cylindricalStateOfArrivalBody = coordinate_conversions::convertCartesianToCylindricalState(
                pointerToArrivalBodyEphemeris->getCartesianState( julianDate ) );

    double timeOfFlight = 500.0;

    // If transfer angle crossed 2*PI.
    double transferAngle = basic_mathematics::computeModulo(
                ( cylindricalStateOfArrivalBody( 1 ) - cylindricalStateOfDepartureBody( 1 ) ), 2.0 * mathematical_constants::PI );

    // Set boundary conditions.
    std::vector< double > boundaryTimes(2);
    boundaryTimes[0] = 0.0;                                                   //* Initial time
    boundaryTimes[1] = timeOfFlight * tudat::physical_constants::JULIAN_DAY;  //* Final time

    // Set boundary conditions for the radial direction.
    std::vector< double > boundaryConditionsRadial(4);
    boundaryConditionsRadial[0] = cylindricalStateOfDepartureBody(0);   //* Initial radial distance
    boundaryConditionsRadial[1] = cylindricalStateOfArrivalBody(0);     //* Final radial distance
    boundaryConditionsRadial[2] = cylindricalStateOfDepartureBody(3);   //* Initial radial velocity
    boundaryConditionsRadial[3] = cylindricalStateOfArrivalBody(3);     //* Final radial velocity

    std::cout << "cylindrical state of departure body: " << cylindricalStateOfDepartureBody.segment(0,3) << "\n\n";
    std::cout << "cylindrical state of arrival body: " << cylindricalStateOfArrivalBody.segment(0,3) << "\n\n";

    // Set boundary conditions for the normal direction.
    std::vector< double > boundaryConditionsNormal(3);
    boundaryConditionsNormal[0] = cylindricalStateOfDepartureBody(4);   //* Initial normal velocity
    boundaryConditionsNormal[1] = cylindricalStateOfArrivalBody(4);     //* Final normal velocity
    boundaryConditionsNormal[2] = numberOfRevolutions * 2.0 * mathematical_constants::PI + transferAngle; //* Final polar angle

    // Set boundary conditions for the axial direction.
    std::vector< double > boundaryConditionsAxial(4);
    boundaryConditionsAxial[0] = cylindricalStateOfDepartureBody(2);    //* Initial axial distance
    boundaryConditionsAxial[1] = cylindricalStateOfArrivalBody(2);      //* Final axial distance
    boundaryConditionsAxial[2] = cylindricalStateOfDepartureBody(5);    //* Initial axial velocity
    boundaryConditionsAxial[3] = cylindricalStateOfArrivalBody(5);      //* Final axial velocity

    std::cout << "initial axial distance: " << cylindricalStateOfDepartureBody[2] << "\n\n";
    std::cout << "final axial distance: " << cylindricalStateOfArrivalBody[2] << "\n\n";

    // Set velocity functions.
    double frequency = 2.0 * mathematical_constants::PI / ( boundaryTimes[1] - boundaryTimes[0] );


    // Set components for the radial velocity function.
    double scaleFactor = 1.0 / ( boundaryTimes[1] - boundaryTimes[0] );

    std::vector< std::shared_ptr< shape_based_methods::BaseFunctionHodographicShaping > > radialVelocityFunctionComponents;
    radialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::constant ) );
    radialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPower, 1.0, 0.0, scaleFactor ) );
    radialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPower, 2.0, 0.0, std::pow( scaleFactor, 2 ) ) );
    radialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPowerSine, 1.0, 0.5 * frequency, scaleFactor ) );
    radialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPowerCosine, 1.0, 0.5 * frequency, scaleFactor ) );

    Eigen::VectorXd radialVelocityComponentsCoefficients;
    radialVelocityComponentsCoefficients.resize( 5 );
    radialVelocityComponentsCoefficients[0] = 500.0;
    radialVelocityComponentsCoefficients[1] = 500.0;
    radialVelocityComponentsCoefficients[2] = 500.0;
    radialVelocityComponentsCoefficients[3] = 500.0;
    radialVelocityComponentsCoefficients[4] = 500.0;

    shape_based_methods::CompositeFunction radialVelocityFunction( radialVelocityFunctionComponents, radialVelocityComponentsCoefficients );


    // Set components for the normal velocity function.
    std::vector< std::shared_ptr< shape_based_methods::BaseFunctionHodographicShaping > > normalVelocityFunctionComponents;
    normalVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                     shape_based_methods::constant ) );
    normalVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPower, 1.0, 0.0, scaleFactor ) );
    normalVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPower, 2.0, 0.0, std::pow( scaleFactor, 2 ) ) );
    normalVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPowerSine, 1.0, 0.5 * frequency, scaleFactor ) );
    normalVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                    shape_based_methods::scaledPowerCosine, 1.0, 0.5 * frequency, scaleFactor ) );

    Eigen::VectorXd normalVelocityComponentsCoefficients;
    normalVelocityComponentsCoefficients.resize( 5 );
    normalVelocityComponentsCoefficients[0] = 500.0;
    normalVelocityComponentsCoefficients[1] = 500.0;
    normalVelocityComponentsCoefficients[2] = 500.0;
    normalVelocityComponentsCoefficients[3] = 500.0;
    normalVelocityComponentsCoefficients[4] = - 200.0;

    shape_based_methods::CompositeFunction normalVelocityFunction( normalVelocityFunctionComponents, normalVelocityComponentsCoefficients );


    // Set components for the axial velocity function.
    std::vector< std::shared_ptr< shape_based_methods::BaseFunctionHodographicShaping > > axialVelocityFunctionComponents;
    axialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                   shape_based_methods::cosine, 0.0, ( numberOfRevolutions + 0.5 ) * frequency ) );
    axialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                   shape_based_methods::scaledPowerCosine, 3.0,
                                                   ( numberOfRevolutions + 0.5 ) * frequency , std::pow( scaleFactor, 3 ) ) );
    axialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                   shape_based_methods::scaledPowerSine, 3.0,
                                                   ( numberOfRevolutions + 0.5 ) * frequency , std::pow( scaleFactor, 3 ) ) );
    axialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                   shape_based_methods::scaledPowerCosine, 4.0,
                                                   ( numberOfRevolutions + 0.5 ) * frequency, std::pow( scaleFactor, 4 ) ) );
    axialVelocityFunctionComponents.push_back( shape_based_methods::createBaseFunctionHodographicShaping(
                                                   shape_based_methods::scaledPowerSine, 4.0,
                                                   ( numberOfRevolutions + 0.5 ) * frequency, std::pow( scaleFactor, 4 ) ) );

    Eigen::VectorXd axialVelocityComponentsCoefficients;
    axialVelocityComponentsCoefficients.resize( 5 );
    axialVelocityComponentsCoefficients[0] = 500.0;
    axialVelocityComponentsCoefficients[1] = 500.0;
    axialVelocityComponentsCoefficients[2] = 500.0;
    axialVelocityComponentsCoefficients[3] = 500.0;
    axialVelocityComponentsCoefficients[4] = 2000.0;

    shape_based_methods::CompositeFunction axialVelocityFunction( axialVelocityFunctionComponents, axialVelocityComponentsCoefficients );

    Eigen::Vector6d cartesianStateDepartureBody = pointerToDepartureBodyEphemeris->getCartesianState( julianDate );

    // Create hodographic-shaping object with defined velocity functions and boundary conditions.
    shape_based_methods::HodographicShaping VelocityShapingMethod( radialVelocityFunction,
                                                                normalVelocityFunction,
                                                                axialVelocityFunction,
                                                                cartesianStateDepartureBody,
                                                                boundaryConditionsRadial,
                                                                boundaryConditionsNormal,
                                                                boundaryConditionsAxial,
                                                                boundaryTimes,
                                                                0.0,
                                                                timeOfFlight * tudat::physical_constants::JULIAN_DAY );

    // Compute number of free coefficients for the radial velocity function.
    int numberOfFreeCoefficientsRadial = radialVelocityFunction.getNumberOfCompositeFunctionComponents()
                                    - std::min( 3, static_cast< int >( boundaryConditionsRadial.size() ) );

    // Compute number of free coefficients for the normal velocity function.
    int numberOfFreeCoefficientsNormal = normalVelocityFunction.getNumberOfCompositeFunctionComponents()
                                    - std::min( 3, static_cast< int >( boundaryConditionsNormal.size() ) );

    std::cout << "boundary conditions normal: " <<  static_cast< int >( boundaryConditionsNormal.size() ) << "\n\n";

    // Compute number of free coefficients for the normal velocity function.
    int numberOfFreeCoefficientsAxial = axialVelocityFunction.getNumberOfCompositeFunctionComponents()
                                    - std::min( 3, static_cast< int > (boundaryConditionsAxial.size()) );

    // Compute total number of free coefficients.
    int numberOfFreeCoefficients = numberOfFreeCoefficientsRadial + numberOfFreeCoefficientsNormal
                                   + numberOfFreeCoefficientsAxial;

    std::cout << "number free coefficients normal: " << numberOfFreeCoefficientsNormal << "\n\n";

    // Initialize free coefficients vector.
    Eigen::VectorXd freeCoefficients = Eigen::VectorXd::Zero( numberOfFreeCoefficients );
    freeCoefficients[0] = 500.0;
    freeCoefficients[1] = 500.0;
    freeCoefficients[2] = 500.0;
    freeCoefficients[3] = -200.0;
    freeCoefficients[4] = 500.0;
    freeCoefficients[5] = 2000.0;


    std::map< double, Eigen::VectorXd > fullPropagationResults;
    std::map< double, Eigen::VectorXd > shapingMethodResults;
    std::map< double, Eigen::VectorXd > dependentVariables;

    spice_interface::loadStandardSpiceKernels( );

    // Create central, departure and arrival bodies.
    std::vector< std::string > bodiesToCreate;
    bodiesToCreate.push_back( "Sun" );
    bodiesToCreate.push_back( "Earth" );
    bodiesToCreate.push_back( "Mars" );
    bodiesToCreate.push_back( "Jupiter" );


    std::map< std::string, std::shared_ptr< simulation_setup::BodySettings > > bodySettings =
            simulation_setup::getDefaultBodySettings( bodiesToCreate );

    std::string frameOrigin = "SSB";
    std::string frameOrientation = "ECLIPJ2000";


    // Define central body ephemeris settings.
    bodySettings[ "Sun" ]->ephemerisSettings = std::make_shared< simulation_setup::ConstantEphemerisSettings >(
                ( Eigen::Vector6d( ) << 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 ).finished( ), frameOrigin, frameOrientation );

    bodySettings[ "Sun" ]->ephemerisSettings->resetFrameOrientation( frameOrientation );
    bodySettings[ "Sun" ]->rotationModelSettings->resetOriginalFrame( frameOrientation );


    // Create body map.
    simulation_setup::NamedBodyMap bodyMap = createBodies( bodySettings );

    bodyMap[ "Vehicle" ] = std::make_shared< simulation_setup::Body >( );
    bodyMap.at( "Vehicle" )->setEphemeris( std::make_shared< ephemerides::TabulatedCartesianEphemeris< > >(
                                                         std::shared_ptr< interpolators::OneDimensionalInterpolator
                                                         < double, Eigen::Vector6d > >( ), frameOrigin, frameOrientation ) );


    setGlobalFrameBodyEphemerides( bodyMap, frameOrigin, frameOrientation );



    std::vector< std::string > bodiesToPropagate;
    bodiesToPropagate.push_back( "Vehicle" );
    std::vector< std::string > centralBodies;
    centralBodies.push_back( "Sun" );

    // Acceleration from the central body.
    std::map< std::string, std::vector< std::shared_ptr< simulation_setup::AccelerationSettings > > > bodyToPropagateAccelerations;
    bodyToPropagateAccelerations[ "Sun" ].push_back( std::make_shared< simulation_setup::AccelerationSettings >(
                                                                basic_astrodynamics::central_gravity ) );
    bodyToPropagateAccelerations[ "Mars" ].push_back( std::make_shared< simulation_setup::AccelerationSettings >(
                                                          basic_astrodynamics::central_gravity ) );
    bodyToPropagateAccelerations[ "Earth" ].push_back( std::make_shared< simulation_setup::AccelerationSettings >(
                                                          basic_astrodynamics::central_gravity ) );
    bodyToPropagateAccelerations[ "Jupiter" ].push_back( std::make_shared< simulation_setup::AccelerationSettings >(
                                                          basic_astrodynamics::central_gravity ) );

    simulation_setup::SelectedAccelerationMap accelerationMap;
    accelerationMap[ "Vehicle" ] = bodyToPropagateAccelerations;

    // Create the acceleration map.
    basic_astrodynamics::AccelerationMap accelerationModelMap = createAccelerationModelsMap(
                bodyMap, accelerationMap, bodiesToPropagate, centralBodies );

    VelocityShapingMethod.satisfyRadialBoundaryConditions( freeCoefficients );
    VelocityShapingMethod.satisfyNormalBoundaryConditionsWithFinalPolarAngle( freeCoefficients );
    VelocityShapingMethod.satisfyAxialBoundaryConditions( freeCoefficients );
    std::map< double, double > outputRadialDistance;
    std::map< double, double > outputAxialDistance;
    std::map< double, double > outputPolarAngle;
    std::map< double, Eigen::Vector3d > outputCartesianPosition;
    std::map< double, Eigen::Vector3d > outputAccelerationVector;

    double stepSize = ( boundaryTimes[1] - boundaryTimes[0] ) / static_cast< double >( 50 );
    for ( int currentStep = 0 ; currentStep <= 50 ; currentStep++ ){
        double currentTime = boundaryTimes[0] + currentStep * stepSize;

        outputRadialDistance[ currentStep ] = VelocityShapingMethod.computeRadialDistanceCurrentTime( currentTime );
        outputAxialDistance[ currentStep ] = VelocityShapingMethod.computeAxialDistanceCurrentTime( currentTime );
        outputPolarAngle[ currentStep ] = VelocityShapingMethod.computePolarAngle( currentTime ); // + cylindricalStateOfDepartureBody( 1 );

        outputCartesianPosition[ currentStep ] = coordinate_conversions::convertCylindricalToCartesian(
                    ( Eigen::Vector3d() << outputRadialDistance[ currentStep ], outputPolarAngle[ currentStep ],
                      outputAxialDistance[ currentStep ] ).finished() );

        outputAccelerationVector[ currentStep ] = VelocityShapingMethod.computeThrustAccelerationComponents( currentTime );

    }

    // Define integrator settings
    std::shared_ptr< numerical_integrators::IntegratorSettings< double > > integratorSettings =
            std::make_shared< numerical_integrators::IntegratorSettings< double > > ( numerical_integrators::rungeKutta4, 0.0, stepSize / 100.0 );

    // Define mass function of the vehicle.
    std::function< double( const double ) > newMassFunction = [ = ]( const double currentTime )
    {
        return 200.0 - 50.0 / ( timeOfFlight * physical_constants::JULIAN_DAY ) * currentTime ;
    };
    bodyMap[ "Vehicle" ]->setBodyMassFunction( newMassFunction );

    // Compute shaped trajectory and propagated trajectory.
    VelocityShapingMethod.computeShapingTrajectoryAndFullPropagation( bodyMap, accelerationModelMap, cartesianStateDepartureBody,
                                                                      "Sun", "Vehicle", propagators::cowell, integratorSettings,
                                                                      fullPropagationResults, shapingMethodResults, dependentVariables );





    tudat::input_output::writeDataMapToTextFile( outputRadialDistance,
                                          "outputRadialDistance.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( outputAxialDistance,
                                          "outputAxialDistance.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( outputPolarAngle,
                                          "outputPolarAngle.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( outputCartesianPosition,
                                          "outputCartesianPosition.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( outputAccelerationVector,
                                          "outputAccelerationVector.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( cartesianPositionDepartureBody,
                                          "cartesianPositionDepartureBody.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( fullPropagationResults,
                                          "fullPropagationResults.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( shapingMethodResults,
                                          "shapingMethodResults.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    tudat::input_output::writeDataMapToTextFile( dependentVariables,
                                          "dependentVariables.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    std::map< double, Eigen::VectorXd > thrustAccelerationMap;
    for ( std::map< double, Eigen::VectorXd >::iterator itr = dependentVariables.begin() ; itr != dependentVariables.end() ; itr++ )
    {
        thrustAccelerationMap[ itr->first ] = VelocityShapingMethod.computeCartesianAcceleration( itr->first );
    }

    tudat::input_output::writeDataMapToTextFile( thrustAccelerationMap,
                                          "thrustAccelerationMap.dat",
                                           "C:/Users/chamb/Documents/Master_2/SOCIS/HodographicShapingTest/",
                                          "",
                                          std::numeric_limits< double >::digits10,
                                          std::numeric_limits< double >::digits10,
                                          "," );

    std::cout << "final polar angle: " << VelocityShapingMethod.computeFinalPolarAngle() << "\n\n";
    std::cout << "final cartesian state: " << coordinate_conversions::convertCylindricalToCartesian(
                     ( Eigen::Vector3d() << outputRadialDistance[ 50 ], VelocityShapingMethod.computeFinalPolarAngle(),
                       outputAxialDistance[ 50 ] ).finished() ) << "\n\n";

    std::cout << "deltaV: " << VelocityShapingMethod.computeDeltaV() << "\n\n";
    std::cout << "compute acceleration cylindrical coordinates: " << VelocityShapingMethod.computeThrustAccelerationComponents( 100.0 ) << "\n\n" ;
    std::cout << "compute acceleration cartesian coordinates: " << VelocityShapingMethod.computeCartesianAcceleration( 100.0 ) << "\n\n";
    std::cout << "norm cylindrical acceleration: " << VelocityShapingMethod.computeThrustAccelerationCurrentTime( 100.0 ) << "\n\n";
    std::cout << "norm cartesian acceleration: " << VelocityShapingMethod.computeMagnitudeCartesianAcceleration(100.0 ) << "\n\n";

    Eigen::Vector3d cylindricalAcceleration = VelocityShapingMethod.computeThrustAccelerationComponents( 100.0 );
    Eigen::Vector3d cylindricalState = VelocityShapingMethod.computeCurrentCylindricalState( 100.0 ).segment(0,3);
    Eigen::Vector3d cartesianState = VelocityShapingMethod.computeCurrentCartesianState( 100.0 ).segment(0,3);

    Eigen::Vector3d cartesianAcceleration;
    cartesianAcceleration[ 0 ] = ( cartesianState[ 0 ] / std::pow( cylindricalState[ 0 ], 2 ) ) * ( cylindricalState[ 0 ] * cylindricalAcceleration[ 0 ]
            - ( cartesianState[ 1 ] / cartesianState[ 0 ] ) * std::pow( cylindricalState[ 0 ], 1 ) * cylindricalAcceleration[ 1 ] );
    cartesianAcceleration[ 1 ] = ( std::pow( cylindricalState[ 0 ], 1 ) * cylindricalAcceleration[ 1 ]
            + cartesianState[ 1 ] * cartesianAcceleration[ 0 ] ) / cartesianState[ 0 ];
    cartesianAcceleration[ 2 ] = cylindricalAcceleration[ 2 ];

    Eigen::Vector3d testCylindricalAcceleration;
    testCylindricalAcceleration[ 0 ] = ( cartesianState[ 0 ] * cartesianAcceleration[ 0 ] + cartesianState[ 1 ] * cartesianAcceleration[ 1 ] )
            / cylindricalState[ 0 ];
    testCylindricalAcceleration[ 1 ] = ( cartesianState[ 0 ] * cartesianAcceleration[ 1 ] - cartesianState[ 1 ] * cartesianAcceleration[ 0 ] )
            / std::pow( cylindricalState[ 0 ], 1 );
    testCylindricalAcceleration[ 2 ] = testCylindricalAcceleration[ 2 ];

    std::cout << "cylindrical acceleration: " << cylindricalAcceleration << "\n\n";
    std::cout << "cartesian acceleration: " << cartesianAcceleration << "\n\n";
    std::cout << "test cylindrical acceleration: " << testCylindricalAcceleration << "\n\n";

}

BOOST_AUTO_TEST_SUITE_END( )

} // namespace unit_tests
} // namespace tudat