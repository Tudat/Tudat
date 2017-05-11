/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#include <iostream>

#include <boost/lexical_cast.hpp>

#include "Tudat/Astrodynamics/ObservationModels/observableTypes.h"

namespace tudat
{

namespace observation_models
{

//! Function to get the name (string) associated with a given observable type.
std::string getObservableName( const ObservableType observableType )
{
    std::string observableName;
    switch( observableType )
    {
    case one_way_range:
        observableName = "OneWayRange";
        break;
    case angular_position:
        observableName = "AngularPosition";
        break;
    case position_observable:
        observableName = "CartesianPosition";
        break;
    case one_way_doppler:
        observableName = "OneWayDoppler";
        break;
    default:
        std::string errorMessage =
                "Error, could not find observable type "+ boost::lexical_cast< std::string >( observableType ) +
                " when getting name from type";
        throw std::runtime_error( errorMessage );
    }
    return observableName;
}

//! Function to get the observable type.ssociated with the name (string) of observable.
ObservableType getObservableType( const std::string& observableName )
{
    ObservableType observableType;

    if( observableName == "OneWayRange" )
    {
        observableType = one_way_range;
    }
    else if( observableName == "AngularPosition" )
    {
        observableType = angular_position;
    }
    else if( observableName == "CartesianPosition" )
    {
        observableType = position_observable;
    }
    else if( observableName ==  "OneWayDoppler" )
    {
        observableType = one_way_doppler;
    }
    else
    {
        std::string errorMessage =
                "Error, could not find observable name "+ observableName +
                " when getting type from name";
        throw std::runtime_error( errorMessage );
    }

    return observableType;
}

//! Function to get the indices in link end times/states for a given link end type and observable type
std::vector< int > getLinkEndIndicesForLinkEndTypeAtObservable(
        const ObservableType observableType, const LinkEndType linkEndType )
{
    std::vector< int > linkEndIndices;

    switch( observableType )
    {
    case one_way_range:
        switch( linkEndType )
        {
        case transmitter:
            linkEndIndices.push_back( 0 );
            break;
        case receiver:
            linkEndIndices.push_back( 1 );
            break;
        default:
            std::string errorMessage =
                    "Error, could not find link end type index for link end " +
                    boost::lexical_cast< std::string >( linkEndType ) + " of observable " +
                    boost::lexical_cast< std::string >( observableType );
            throw std::runtime_error( errorMessage );
        }
        break;
    case one_way_doppler:
        switch( linkEndType )
        {
        case transmitter:
            linkEndIndices.push_back( 0 );
            break;
        case receiver:
            linkEndIndices.push_back( 1 );
            break;
        default:
            std::string errorMessage =
                    "Error, could not find link end type index for link end " +
                    boost::lexical_cast< std::string >( linkEndType ) + " of observable " +
                    boost::lexical_cast< std::string >( observableType );
            throw std::runtime_error( errorMessage );
        }
        break;
    case angular_position:
        switch( linkEndType )
        {
        case transmitter:
            linkEndIndices.push_back( 0 );
            break;
        case receiver:
            linkEndIndices.push_back( 1 );
            break;
        default:
            std::string errorMessage =
                    "Error, could not find link end type index for link end " +
                    boost::lexical_cast< std::string >( linkEndType ) + " of observable " +
                    boost::lexical_cast< std::string >( observableType );
            throw std::runtime_error( errorMessage );
        }
        break;
    case position_observable:
        if( linkEndType == observed_body )
        {
            linkEndIndices.push_back( 0 );
        }
        else
        {
            std::string errorMessage =
                    "Error, could not find link end type index for link end " +
                    boost::lexical_cast< std::string >( linkEndType ) + " of observable " +
                    boost::lexical_cast< std::string >( observableType );
            throw std::runtime_error( errorMessage );
        }

    default:
        std::string errorMessage =
                "Error, could not find link end type index for link end types of observable " +
                boost::lexical_cast< std::string >( observableType );
        throw std::runtime_error( errorMessage );
    }

    return linkEndIndices;
}

}

}
