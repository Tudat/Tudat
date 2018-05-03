/*    Copyright (c) 2010-2018, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_CREATEATMOSPHEREMODEL_H
#define TUDAT_CREATEATMOSPHEREMODEL_H

#include <string>
#include <map>

#include <boost/shared_ptr.hpp>

#include "Tudat/Astrodynamics/Aerodynamics/atmosphereModel.h"
#include "Tudat/Astrodynamics/BasicAstrodynamics/physicalConstants.h"
#include "Tudat/Mathematics/Interpolators/interpolator.h"

namespace tudat
{

namespace simulation_setup
{

//! List of wind models available in simulations
/*!
 *  List of wind models available in simulations. Wind models not defined by this
 *  given enum cannot be used for automatic model setup.
 */
enum WindModelTypes
{
    custom_wind_model
};

//! Class for providing settings for wind model.
/*!
 *  Class for providing settings for automatic wind model creation. This class is a
 *  functional (base) class for settings of wind models that require no information in
 *  addition to their type. Wind model classes defining requiring additional information
 *  must be created using an object derived from this class.
 */
class WindModelSettings
{
public:

    //! Constructor
    /*!
     * Constructor
     * \param windModelType Type of wind model that is to be created
     */
    WindModelSettings( const WindModelTypes windModelType ):
        windModelType_( windModelType ){ }

    //! Destructor
    virtual ~WindModelSettings( ){ }

    //! Function to retrieve type of wind model that is to be created
    /*!
     * Function to retrieve type of wind model that is to be created
     * \return Type of wind model that is to be created
     */
    WindModelTypes getWindModelType( )
    {
        return windModelType_;
    }

protected:

    //! Type of wind model that is to be created
    WindModelTypes windModelType_;
};

//! Class to define settings for a custom, user-defined, wind model
class CustomWindModelSettings: public WindModelSettings
{
public:

    //! Constructor
    /*!
     * Constructor
     * \param windFunction Function that returns wind vector as a function of altitude, longitude, latitude and time (in that
     * order).
     */
    CustomWindModelSettings(
            const boost::function< Eigen::Vector3d( const double, const double, const double, const double ) > windFunction ):
        WindModelSettings( custom_wind_model ), windFunction_( windFunction ){ }

    //! Destructor
    ~CustomWindModelSettings( ){ }

    //! Function to retrieve function that returns wind vector as a function of altitude, longitude, latitude and time
    /*!
     * Function to retrieve function that returns wind vector as a function of altitude, longitude, latitude and time
     * \return Function that returns wind vector as a function of altitude, longitude, latitude and time
     */
    boost::function< Eigen::Vector3d( const double, const double, const double, const double ) > getWindFunction( )
    {
        return windFunction_;
    }

    //! Function to reset function that returns wind vector as a function of altitude, longitude, latitude and time
    /*!
     * Function to reset function that returns wind vector as a function of altitude, longitude, latitude and time
     * \param windFunction New function that returns wind vector as a function of altitude, longitude, latitude and time
     */
    void setWindFunction(
            const boost::function< Eigen::Vector3d( const double, const double, const double, const double ) > windFunction )
    {
        windFunction_ = windFunction;
    }

protected:

    //! Function that returns wind vector as a function of altitude, longitude, latitude and time (in that order).
    boost::function< Eigen::Vector3d( const double, const double, const double, const double ) > windFunction_;
};

//! List of atmosphere models available in simulations
/*!
 *  List of atmosphere models available in simulations. Atmosphere models not defined by this
 *  given enum cannot be used for automatic model setup.
 */
enum AtmosphereTypes
{
    exponential_atmosphere,
    tabulated_atmosphere,
    nrlmsise00
};

//! Class for providing settings for atmosphere model.
/*!
 *  Class for providing settings for automatic atmosphere model creation. This class is a
 *  functional (base) class for settings of atmosphere models that require no information in
 *  addition to their type. Atmosphere model classes defining requiring additional information
 *  must be created using an object derived from this class.
 */
class AtmosphereSettings
{
public:

    //! Constructor, sets type of atmosphere model.
    /*!
     *  Constructor, sets type of atmosphere model. Settings for atmosphere models requiring
     *  additional information should be defined in a derived class.
     *  \param atmosphereType Type of atmosphere model that is to be created.
     */
    AtmosphereSettings( const AtmosphereTypes atmosphereType ):
        atmosphereType_( atmosphereType ){ }

    //! Destructor
    virtual ~AtmosphereSettings( ){ }

    //! Function to return type of atmosphere model that is to be created.
    /*!
     *  Function to return type of atmosphere model that is to be created.
     *  \return Type of atmosphere model that is to be created.
     */
    AtmosphereTypes getAtmosphereType( ){ return atmosphereType_; }

    //! Function to return settings for the atmosphere's wind model.
    /*!
     *  Function to return settings for the atmosphere's wind model.
     *  \return Settings for the atmosphere's wind model.
     */
    boost::shared_ptr< WindModelSettings > getWindSettings( )
    {
        return windSettings_;
    }

    //! Function to (re)set settings for the atmosphere's wind model.
    /*!
     *  Function to (re)set settings for the atmosphere's wind model.
     *  \param windSettings Settings for the atmosphere's wind model.
     */
    void setWindSettings( const boost::shared_ptr< WindModelSettings > windSettings )
    {
        windSettings_ = windSettings;
    }

private:

    //!  Type of atmosphere model that is to be created.
    AtmosphereTypes atmosphereType_;

    //! Settings for the atmosphere's wind model.
    boost::shared_ptr< WindModelSettings > windSettings_;
};

//! AtmosphereSettings for defining an exponential atmosphere.
class ExponentialAtmosphereSettings: public AtmosphereSettings
{
public:
    //! Constructor.
    /*!
     *  Constructor.
     *  \param densityScaleHeight Scale height for density profile of atmosphere.
     *  \param constantTemperature Constant atmospheric temperature.
     *  \param densityAtZeroAltitude Atmospheric density at ground level.
     *  \param specificGasConstant Specific gas constant for (constant) atmospheric chemical
     *  composition.
     */

    ExponentialAtmosphereSettings(
            const double densityScaleHeight, const double constantTemperature,
            const double densityAtZeroAltitude, const double specificGasConstant,
            const double ratioOfSpecificHeats = 1.4 ):
        AtmosphereSettings( exponential_atmosphere ),
        densityScaleHeight_( densityScaleHeight ), constantTemperature_( constantTemperature ),
        densityAtZeroAltitude_( densityAtZeroAltitude ), specificGasConstant_( specificGasConstant ),
        ratioOfSpecificHeats_( ratioOfSpecificHeats )
    { }

    //! Function to return scale heigh for density profile of atmosphere.
    /*!
     *  Function to return scale heigh for density profile of atmosphere.
     *  \return Scale heigh for density profile of atmosphere.
     */
    double getDensityScaleHeight( ){ return densityScaleHeight_; }

    //! Function to return constant atmospheric temperature.
    /*!
     *  Function to return constant atmospheric temperature.
     *  \return Constant atmospheric temperature.
     */
    double getConstantTemperature( ){ return constantTemperature_; }

    //! Function to return atmospheric density at ground level.
    /*!
     *  Function to return atmospheric density at ground level.
     *  \return Atmospheric density at ground level.
     */
    double getDensityAtZeroAltitude( ){ return densityAtZeroAltitude_; }

    //! Function to return specific gas constant for (constant) atmospheric chemical
    /*!
     *  Function to return specific gas constant for (constant) atmospheric chemical
     *  \return Specific gas constant for (constant) atmospheric chemical
     */
    double getSpecificGasConstant( ){ return specificGasConstant_; }

    //! Function to return ratio of specific heats for (constant) atmospheric chemical
    /*!
     *  Function to return ratio of specific heats for (constant) atmospheric chemical
     *  \return Specific gas constant for (constant) atmospheric chemical
     */
    double getRatioOfSpecificHeats( ){ return ratioOfSpecificHeats_; }

private:

    //! Scale heigh for density profile of atmosphere.
    double densityScaleHeight_;

    //! Constant atmospheric temperature.
    double constantTemperature_;

    //! Atmospheric density at ground level.
    double densityAtZeroAltitude_;

    //! Specific gas constant for (constant) atmospheric chemical
    double specificGasConstant_;

    //! Ratio of specific heats for (constant) atmospheric chemical
    double ratioOfSpecificHeats_;
};


//! AtmosphereSettings for defining an NRLMSISE00 atmosphere reading space weather data from a text file.
class NRLMSISE00AtmosphereSettings: public AtmosphereSettings
{
public:

    //! Constructor.
    /*!
     *  Constructor.
     *  \param spaceWeatherFile File containing space weather data, as in
     *  https://celestrak.com/SpaceData/sw19571001.txt
     */
    NRLMSISE00AtmosphereSettings( const std::string& spaceWeatherFile ):
        AtmosphereSettings( nrlmsise00 ), spaceWeatherFile_( spaceWeatherFile ){ }

    //! Function to return file containing space weather data.
    /*!
     *  Function to return file containing space weather data.
     *  \return Filename containing space weather data.
     */
    std::string getSpaceWeatherFile( ){ return spaceWeatherFile_; }

private:

    //! File containing space weather data.
    /*!
     *  File containing space weather data, as in https://celestrak.com/SpaceData/sw19571001.txt
     */
    std::string spaceWeatherFile_;
};


//! AtmosphereSettings for defining an atmosphere with tabulated data from file.
class TabulatedAtmosphereSettings: public AtmosphereSettings
{
public:

    //! Constructor.
    /*!
     *  Constructor.
     *  \param atmosphereFile File containing atmospheric properties, file should contain
     *  at least altitude, density, pressure and temperature, in a user-defined order (specified by dependentVariablesNames),
     *  which depend on the independent variables listed in independentVariablesNames.
     */
    TabulatedAtmosphereSettings( const std::map< int, std::string >& atmosphereFile,
                                 const std::vector< aerodynamics::AtmosphereDependentVariables > dependentVariablesNames =
    { aerodynamics::density_dependent_atmosphere, aerodynamics::pressure_dependent_atmosphere, aerodynamics::temperature_dependent_atmosphere },
                                 const std::vector< aerodynamics::AtmosphereIndependentVariables > independentVariablesNames =
    { aerodynamics::altitude_dependent_atmosphere },
                                 const double specificGasConstant = physical_constants::SPECIFIC_GAS_CONSTANT_AIR,
                                 const double ratioOfSpecificHeats = 1.4,
                                 const interpolators::BoundaryInterpolationType boundaryHandling =
                    interpolators::use_boundary_value_with_warning ):
        AtmosphereSettings( tabulated_atmosphere ), atmosphereFile_( atmosphereFile ), dependentVariables_( dependentVariablesNames ),
        independentVariables_( independentVariablesNames ), specificGasConstant_( specificGasConstant ),
        ratioOfSpecificHeats_( ratioOfSpecificHeats ), boundaryHandling_( boundaryHandling )
    { }

    //! Function to return file containing atmospheric properties.
    /*!
     *  Function to return file containing atmospheric properties.
     *  \return Filename containing atmospheric properties.
     */
    std::map< int, std::string > getAtmosphereFile( ){ return atmosphereFile_; }

    //! Function to return dependent variables names.
    /*!
     *  Function to return dependent variables names.
     *  \return Dependent variables.
     */
    std::vector< aerodynamics::AtmosphereDependentVariables > getDependentVariables( ){ return dependentVariables_; }

    //! Function to return independent variables names.
    /*!
     *  Function to return independent variables names.
     *  \return Independent variables.
     */
    std::vector< aerodynamics::AtmosphereIndependentVariables > getIndependentVariables( ){ return independentVariables_; }

    //! Function to return specific gas constant of the atmosphere.
    /*!
     *  Function to return specific gas constant of the atmosphere.
     *  \return Specific gas constant of the atmosphere.
     */
    double getSpecificGasConstant( ){ return specificGasConstant_; }

    //! Function to return ratio of specific heats of the atmosphere.
    /*!
     *  Function to return ratio of specific heats of the atmosphere.
     *  \return Ratio of specific heats of the atmosphere at constant pressure and constant volume.
     */
    double getRatioOfSpecificHeats( ){ return ratioOfSpecificHeats_; }

    //! Function to return boundary handling method.
    /*!
     *  Function to return boundary handling method.
     *  \return Boundary handling method for when independent variables are outside specified range.
     */
    interpolators::BoundaryInterpolationType getBoundaryHandling( ){ return boundaryHandling_; }

private:

    //! File containing atmospheric properties.
    /*!
     *  File containing atmospheric properties, file should contain
     *  columns of atmospheric data with at least density, pressure and temperature,
     *  (whose order is specified in dependentVariables), and with at least one
     *  indendent variables.
     */
    std::map< int, std::string > atmosphereFile_;

    //! A vector of strings containing the names of the variables contained in the atmosphere file
    /*!
     * A vector of strings containing the names of the variables contained in the atmosphere file,
     * in the correct order (from left, being the first entry in the vector, to the right).
     */
    std::vector< aerodynamics::AtmosphereDependentVariables > dependentVariables_;

    //! A vector of strings containing the names of the independent variables contained in the atmosphere file
    /*!
     * A vector of strings containing the names of the independent variables contained in the atmosphere file,
     * in the correct order (from left, being the first entry in the vector, to the right).
     */
    std::vector< aerodynamics::AtmosphereIndependentVariables > independentVariables_;

    //! Specific gas constant of the atmosphere.
    /*!
     * Specific gas constant of the atmosphere.
     */
    double specificGasConstant_;

    //! Ratio of specific heats of the atmosphere at constant pressure and constant volume.
    /*!
     *  Ratio of specific heats of the atmosphere at constant pressure and constant volume.
     */
    double ratioOfSpecificHeats_;

    //! Behavior of interpolator when independent variable is outside range.
    /*!
     *  Behavior of interpolator when independent variable is outside range.
     */
    interpolators::BoundaryInterpolationType boundaryHandling_;
};

//! Function to create a wind model.
/*!
 *  Function to create a wind model based on model-specific settings for the wind model.
 *  \param windSettings Settings for the wind model that is to be created, defined
 *  a pointer to an object of class (derived from) WindModelSettings.
 *  \param body Name of the body for which the wind model is to be created.
 *  \return Wind model created according to settings in windSettings.
 */
boost::shared_ptr< aerodynamics::WindModel > createWindModel(
        const boost::shared_ptr< WindModelSettings > windSettings,
        const std::string& body );

//! Function to create an atmosphere model.
/*!
 *  Function to create an atmosphere model based on model-specific settings for the atmosphere.
 *  \param atmosphereSettings Settings for the atmosphere model that is to be created, defined
 *  a pointer to an object of class (derived from) AtmosphereSettings.
 *  \param body Name of the body for which the atmosphere model is to be created.
 *  \return Atmosphere model created according to settings in atmosphereSettings.
 */
boost::shared_ptr< aerodynamics::AtmosphereModel > createAtmosphereModel(
        const boost::shared_ptr< AtmosphereSettings > atmosphereSettings,
        const std::string& body );

} // namespace simulation_setup

} // namespace tudat


#endif // TUDAT_CREATEATMOSPHEREMODEL_H
