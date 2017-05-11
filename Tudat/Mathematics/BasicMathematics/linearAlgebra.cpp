/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 *
 */

#include <cmath>

#include <Eigen/LU>

#include "Tudat/Mathematics/BasicMathematics/linearAlgebra.h"
#include "Tudat/Basics/utilities.h"

namespace tudat
{

namespace linear_algebra
{

//! Function that returns that 'cross-product matrix'
Eigen::Matrix3d getCrossProductMatrix( const Eigen::Vector3d& vector )
{
    Eigen::Matrix3d crossProductMatrix = Eigen::Matrix3d::Zero( );
    crossProductMatrix( 1, 0 ) = vector.z( );
    crossProductMatrix( 0, 1 ) = -vector.z( );
    crossProductMatrix( 2, 0 ) = -vector.y( );
    crossProductMatrix( 0, 2 ) = vector.y( );
    crossProductMatrix( 2, 1 ) = vector.x( );
    crossProductMatrix( 1, 2 ) = -vector.x( );
    return crossProductMatrix;
}

//! Compute cosine of the angle between two vectors.
double computeCosineOfAngleBetweenVectors( const Eigen::VectorXd& vector0,
                                           const Eigen::VectorXd& vector1 )
{
    if( !( vector0.size( ) == vector1.size( ) ) )
    {
        throw std::runtime_error( "Error when computing angle between vectors; size is incompatible" );
    }

    // Get the cosine of the angle by dotting the normalized vectors.
    double dotProductOfNormalizedVectors = vector0.normalized( ).dot( vector1.normalized( ) );

    // Explicitly define the extreme cases, which can give problems with the acos function.
    if ( dotProductOfNormalizedVectors >= 1.0 )
    {
        return 1.0;
    }

    else if ( dotProductOfNormalizedVectors <= -1.0 )
    {
        return -1.0;
    }
    // Determine the actual angle.
    else
    {
        return dotProductOfNormalizedVectors;
    }
}

//! Compute angle between two vectors.
double computeAngleBetweenVectors( const Eigen::VectorXd& vector0, const Eigen::VectorXd& vector1 )
{
    // Determine the cosine of the angle by using another routine.
    double dotProductOfNormalizedVectors = computeCosineOfAngleBetweenVectors( vector0, vector1 );

    // Return arccosine of the above, which is effectively the angle.
    return std::acos( dotProductOfNormalizedVectors );
}

//! Computes the difference between two 3d vectors.
Eigen::Vector3d computeVectorDifference( const Eigen::Vector3d& vector0,
                                         const Eigen::Vector3d& vector1 )
{
    return ( vector0 - vector1 );
}

//! Computes norm of the the difference between two 3d vectors.
double computeNormOfVectorDifference( const Eigen::Vector3d& vector0,
                                      const Eigen::Vector3d& vector1 )
{
    return ( vector0 - vector1 ).norm( );
}

//! Computes the norm of a 3d vector
double getVectorNorm( const Eigen::Vector3d& vector )
{
    return vector.norm( );
}

Eigen::Vector3d evaluateSecondBlockInStateVector(
        const boost::function< Eigen::Vector6d( const double ) > stateFunction,
        const double time )
{
    return stateFunction( time ).segment( 3, 3 );
}

//! Computes the norm of a 3d vector from a vector-returning function.
double getVectorNormFromFunction( const boost::function< Eigen::Vector3d( ) > vectorFunction )
{
    return getVectorNorm( vectorFunction( ) );
}


} // namespace linear_algebra

} // namespace tudat
