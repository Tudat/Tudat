/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 *
 *    Notes
 *      To run this unit tests, a number of spice kernels need to be placed in the
 *      Spice kernel folder, by default External/SpiceInterface/Kernels or the
 *      SPICE_KERNEL_CUSTOM_FOLDER folder set as an argument to CMake or in UserSetings.txt.
 *      The required kernels are:
 *           de421.bsp
 *           pck00009.tpc
 *           naif0009.tls
 *           de-403-masses.tpc
 *      They can be found in a single zip file on the wiki at
 *      http://tudat.tudelft.nl/projects/tudat/wiki/SpiceInterface/ on the Tudat website or,
 *      alternatively, on the NAIF server at ftp://naif.jpl.nasa.gov/pub/naif/generic_kernels/.
 *
 */

#define BOOST_TEST_MAIN

#include "unitTestSupport.h"
#include <Tudat/External/JsonInterface/Environment/atmosphere.h>

namespace tudat
{

namespace unit_tests
{

BOOST_AUTO_TEST_SUITE( test_json_atmosphere )

// Test 1: atmosphere models
BOOST_AUTO_TEST_CASE( test_json_atmosphere_models )
{
    BOOST_CHECK_EQUAL_ENUM( "atmosphere_models",
                            simulation_setup::atmosphereTypes,
                            simulation_setup::unsupportedAtmosphereTypes );
}

// Test 2: exponential atmosphere
BOOST_AUTO_TEST_CASE( test_json_atmosphere_exponential )
{
    using namespace simulation_setup;
    using namespace json_interface;

    // Create AtmosphereSettings from JSON file
    const boost::shared_ptr< AtmosphereSettings > fromFileSettings =
            readInputFile< boost::shared_ptr< AtmosphereSettings > >( "atmosphere_exponential" );

    // Create AtmosphereSettings manually
    const double densityScaleHeight = 5.0;
    const double constantTemperature = 290.0;
    const double densityAtZeroAltitude = 1.0;
    const double specificGasConstant = 3.0;
    const boost::shared_ptr< AtmosphereSettings > manualSettings =
            boost::make_shared< ExponentialAtmosphereSettings >( densityScaleHeight,
                                                                 constantTemperature,
                                                                 densityAtZeroAltitude,
                                                                 specificGasConstant );

    // Compare
    BOOST_CHECK_EQUAL_JSON( fromFileSettings, manualSettings );
}

// Test 3: tabulated atmosphere
BOOST_AUTO_TEST_CASE( test_json_atmosphere_tabulated )
{
    using namespace simulation_setup;
    using namespace json_interface;

    // Create AtmosphereSettings from JSON file
    const boost::shared_ptr< AtmosphereSettings > fromFileSettings =
            readInputFile< boost::shared_ptr< AtmosphereSettings > >( "atmosphere_tabulated" );

    // Create AtmosphereSettings manually
    const boost::shared_ptr< AtmosphereSettings > manualSettings =
            boost::make_shared< TabulatedAtmosphereSettings >( "atmosphereTable.foo" );

    // Compare
    BOOST_CHECK_EQUAL_JSON( fromFileSettings, manualSettings );
}

// Test 4: NRLMSISE00 atmosphere
BOOST_AUTO_TEST_CASE( test_json_atmosphere_nrlmsise00 )
{
    using namespace simulation_setup;
    using namespace json_interface;

    // Create AtmosphereSettings from JSON file
    const boost::shared_ptr< AtmosphereSettings > fromFileSettings =
            readInputFile< boost::shared_ptr< AtmosphereSettings > >( "atmosphere_nrlmsise00" );

    // Create AtmosphereSettings manually
    const boost::shared_ptr< AtmosphereSettings > manualSettings =
            boost::make_shared< AtmosphereSettings >( nrlmsise00 );

    // Compare
    BOOST_CHECK_EQUAL_JSON( fromFileSettings, manualSettings );
}

// Test 5: NRLMSISE00 atmosphere (custom space weather file)
BOOST_AUTO_TEST_CASE( test_json_atmosphere_nrlmsise00_custom )
{
    using namespace simulation_setup;
    using namespace json_interface;

    // Create AtmosphereSettings from JSON file
    const boost::shared_ptr< AtmosphereSettings > fromFileSettings =
            readInputFile< boost::shared_ptr< AtmosphereSettings > >( "atmosphere_nrlmsise00_custom" );

    // Create AtmosphereSettings manually
    const boost::shared_ptr< AtmosphereSettings > manualSettings =
            boost::make_shared< NRLMSISE00AtmosphereSettings >( "spaceWeatherFile.foo" );

    // Compare
    BOOST_CHECK_EQUAL_JSON( fromFileSettings, manualSettings );
}

BOOST_AUTO_TEST_SUITE_END( )

} // namespace unit_tests

} // namespace tudat