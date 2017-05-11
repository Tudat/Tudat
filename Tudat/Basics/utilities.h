/*    Copyright (c) 2010-2017, Delft University of Technology
 *    All rigths reserved
 *
 *    This file is part of the Tudat. Redistribution and use in source and
 *    binary forms, with or without modification, are permitted exclusively
 *    under the terms of the Modified BSD license. You should have received
 *    a copy of the license with this file. If not, please or visit:
 *    http://tudat.tudelft.nl/LICENSE.
 */

#ifndef TUDAT_UTILITIES_H
#define TUDAT_UTILITIES_H

#include <vector>

#include <iostream>
#include <map>

#include <boost/function.hpp>
#include <boost/multi_array.hpp>

#include <Eigen/Core>

namespace tudat
{

namespace utilities
{

//! Function to create a vector from the values of a map
/*!
 *  Function to create a vector from the values of a map. The output vector is in the order of the map entries, i.e. as provided by a forward iterator.
 *  The map keys are not used for the return vector.
 *  \param inputMap Original map from which the vector is to be created
 *  \return Vector created from the map values
 */
template< typename VectorArgument, typename KeyType >
std::vector< VectorArgument > createVectorFromMapValues( const std::map< KeyType, VectorArgument >& inputMap )
{
    // Create and size return vector.
    std::vector< VectorArgument > outputVector;
    outputVector.resize( inputMap.size( ) );

    // Iterate over all map entries and create vector
    int currentIndex = 0;
    for( typename std::map< KeyType, VectorArgument >::const_iterator mapIterator = inputMap.begin( );
         mapIterator != inputMap.end( ); mapIterator++ )
    {
        outputVector[ currentIndex ] = mapIterator->second;
        currentIndex++;
    }

    return outputVector;

}

//! Function to create a vector from the keys of a map
/*!
 *  Function to create a vector from the keys of a map. The output vector is in the order of the map entries, i.e. as provided by a forward iterator.
 *  The map values are not used for the return vector.
 *  \param inputMap Original map from which the vector is to be created
 *  \return Vector created from the map keys
 */
template< typename VectorArgument, typename KeyType >
std::vector< KeyType > createVectorFromMapKeys( const std::map< KeyType, VectorArgument >& inputMap )
{
    // Create and size return vector.
    std::vector< KeyType > outputVector;
    outputVector.resize( inputMap.size( ) );

    // Iterate over all map entries and create vector
    int currentIndex = 0;
    for( typename std::map< KeyType, VectorArgument >::const_iterator mapIterator = inputMap.begin( );
         mapIterator != inputMap.end( ); mapIterator++ )
    {
        outputVector[ currentIndex ] = mapIterator->first;
        currentIndex++;
    }

    return outputVector;
}

//! Function to sum the return values of two boost function with empty input argument list.
/*!
 * Function to sum the return values of two boost function with empty input argument list.
 * \param function1 First function to be added.
 * \param function2 Second function to be added.
 * \return Sum of return values of function1 and function2
 */
template< typename S >
S sumFunctionReturn( const boost::function< S( ) > function1, const boost::function< S( ) > function2 )
{
    return function1( ) + function2( );
}

//! Function to subtract the return values of two boost function with empty input argument list.
/*!
 * Function to subtract the return values of two boost function with empty input argument list.
 * \param function1 First function to be subtracted from.
 * \param function2 Second function to be subtracted.
 * \return Return values of function1 - return value of function2
 */
template< typename S >
S subtractFunctionReturn( const boost::function< S( ) > function1, const boost::function< S( ) > function2 )
{
    return function1( ) - function2( );
}

//! Function to create a vector block history from full matrix history.
/*!
 *  Function to create a vector matrix block history from full matrix history.
 *  \param matrixHistory Full matrix history
 *  \param blockMatrixHistory Block vector history (return by reference).
 *  \param startIndices Starting point (row,column) in matrix of return vector blocks.
 *  \param segmentSize Number of rows in vector.
 */
template< typename S, typename T >
void createVectorBlockMatrixHistory(
        const std::map< S, Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > >& matrixHistory,
        std::map< S, Eigen::Matrix< T, Eigen::Dynamic, 1 > >& blockMatrixHistory,
        const std::pair< int, int > startIndices, const int segmentSize )
{
    blockMatrixHistory.clear( );

    // Loop over integration output and put results in corresponding data structures.
    for( typename std::map< S, Eigen::Matrix< T, Eigen::Dynamic, Eigen::Dynamic > >::const_iterator matrixIterator = matrixHistory.begin( );
         matrixIterator != matrixHistory.end( ); matrixIterator++ )
    {
        // Set numerically integrated states of bodies.
        blockMatrixHistory[ matrixIterator->first ] =
                matrixIterator->second.block( startIndices.first, startIndices.second, segmentSize, 1 );
    }
}

//! Function to print the contents of a map, line by line
/*!
 *  Function to print the contents of a map, line by line. Both the key and value types must have the << operator defined
 *  \param mapToPrint Map that is to be printed.
 */
template< typename S, typename T >
void printMapContents( const std::map< S, T >& mapToPrint)
{
    for( typename std::map< S, T >::const_iterator mapIterator = mapToPrint.begin( );
         mapIterator != mapToPrint.end( ); mapIterator++ )
    {
        std::cout<<mapIterator->first<<", "<<mapIterator->second<<std::endl;
    }
}

//! Function to cast a map of Eigen matrices from one key/matrix scalar type set to another set
/*!
 *  Function to produce a map of Eigen matrices, cast from one set of key/matrix scalar type set to another set.
 *  \param originalMap Map in original types
 *  \param newTypesMap Map that is to be created (returned by reference).
 */
template< typename S, typename T, typename U, typename V, int Rows, int Columns >
void castMatrixMap( const std::map< S, Eigen::Matrix< T, Rows, Columns > >& originalMap,
                          std::map< U, Eigen::Matrix< V, Rows, Columns > >& newTypesMap )
{
    newTypesMap.clear( );
    for( typename std::map< S, Eigen::Matrix< T, Rows, Columns > >::const_iterator mapIterator = originalMap.begin( );
         mapIterator != originalMap.end( ); mapIterator++ )
    {
        newTypesMap[ static_cast< U >( mapIterator->first ) ] = mapIterator->second.template cast< V >( );
    }
}

template< typename KeyType, typename ScalarType, int NumberOfRows, int NumberOfColumns = 1 >
Eigen::Matrix< ScalarType, Eigen::Dynamic, NumberOfColumns > createConcatenatedEigenMatrixFromMapValues(
        const std::map< KeyType, Eigen::Matrix< ScalarType, NumberOfRows, NumberOfColumns > >& inputMap )
{
    // Create and size return vector.
    Eigen::Matrix< ScalarType, Eigen::Dynamic, NumberOfColumns > outputVector;

    int columns;
    if( NumberOfColumns != Eigen::Dynamic )
    {
        columns = NumberOfColumns;
    }
    else
    {
        columns = inputMap.begin( )->second.cols( );
    }

    if( NumberOfRows != Eigen::Dynamic )
    {
        int counter = 0;

        outputVector.resize( inputMap.size( ) * NumberOfRows, columns );
        for( typename std::map< KeyType, Eigen::Matrix< ScalarType, NumberOfRows, NumberOfColumns > >::const_iterator mapIterator =
             inputMap.begin( ); mapIterator != inputMap.end( ); mapIterator++ )
        {
            outputVector.block( counter, 0, NumberOfRows, columns ) = mapIterator->second;
            counter += NumberOfRows;
        }
    }
    else
    {
        int concatenatedSize = 0;

        for( typename std::map< KeyType, Eigen::Matrix< ScalarType, NumberOfRows, NumberOfColumns > >::const_iterator mapIterator =
             inputMap.begin( ); mapIterator != inputMap.end( ); mapIterator++ )
        {
            concatenatedSize += mapIterator->second.rows( );
        }

        outputVector.resize( concatenatedSize, columns );
        int currentSize;
        int counter = 0;

        for( typename std::map< KeyType, Eigen::Matrix< ScalarType, NumberOfRows, NumberOfColumns > >::const_iterator mapIterator =
             inputMap.begin( ); mapIterator != inputMap.end( ); mapIterator++ )
        {
            currentSize = mapIterator->second.rows( );
            outputVector.block( counter, 0, currentSize, columns ) = mapIterator->second;
            counter += currentSize;
        }
    }

    return outputVector;
}

template< typename T >
std::vector< T > convertEigenVectorToStlVector( const Eigen::Matrix< T, Eigen::Dynamic, 1 >& eigenVector )
{
    std::vector< T > stlVector;
    stlVector.resize( eigenVector.rows( ) );
    for( int i = 0; i < eigenVector.rows( ); i++ )
    {
        stlVector[ i ] = eigenVector( i );
    }
    return stlVector;
}


template< typename T >
Eigen::Matrix< T, Eigen::Dynamic, 1 > convertStlVectorToEigenVector( const std::vector< T >& stlVector )
{
    Eigen::Matrix< T, Eigen::Dynamic, 1 > eigenVector = Eigen::Matrix< T, Eigen::Dynamic, 1 >::Zero( stlVector.size( ) );
    for( unsigned int i = 0; i < stlVector.size( ); i++ )
    {
        eigenVector[ i ] = stlVector[ i ];
    }
    return eigenVector;
}

//! Function to add a double to all entries in an STL vector.
/*!
 *  Function to add a double to all entries in an STL vector (addition of a double must be defined for Argument type).
 *  \param vector Vector to which a double is to be added.
 *  \param scalar Value that is to be added to vector
 *  \return New vector with scalar added to all entries of input vector.
 */
template< typename Argument >
std::vector< Argument > addScalarToVector( const std::vector< Argument >& vector, const double scalar )
{
    // Declare and resize return vector.
    std::vector< Argument > addedVector;
    addedVector.resize( vector.size( ) );

    // Add scalar to all entries of input vector
    for( unsigned int i = 0; i < vector.size( ); i++ )
    {
        addedVector[ i ] = vector[ i ] + scalar;
    }

    return addedVector;
}



//! Function to copy a multi-array into another multi-array
/*!
 *  Function to copy a multi-array into another multi-array, resizing the new multi-array accordingly
 *  \param arrayToCopy Multi-array that is to be copied
 *  \param targetArray New multi-array into which arrayToCopy is to be copied (returned by reference).
 */
template< typename S, int NumberOfDimensions >
void copyMultiArray( const boost::multi_array< S, NumberOfDimensions >& arrayToCopy,
                     boost::multi_array< S, NumberOfDimensions >& targetArray )
{
    std::vector< size_t > ex;
    const size_t* shape = arrayToCopy.shape( );
    ex.assign( shape, shape + arrayToCopy.num_dimensions( ) );
    targetArray.resize( ex );
    targetArray = arrayToCopy;
}

//! Get index in nth direction of pointer to single entry in multi-array of doubles
/*!
 *  Get index in nth direction of pointer to single entry in multi-array of doubles
 *  \param multiArray Multi-array for which the index is to be retrieved
 *  \param requestedElement Pointer to element for which index is to be retrieved
 *  \param direction Dimension of multi-array for which index is to be retrieved
 *  \return Index in nth direction of pointer to single entry in multi-array of doubles
 */
template< unsigned int NumberOfDimensions >
typename boost::multi_array< double ,NumberOfDimensions >::index getMultiArrayIndex(
        const typename boost::multi_array< double, NumberOfDimensions >& multiArray, const double* requestedElement,
        const unsigned short int direction )
{
    int offset = requestedElement - multiArray.origin( );
    return( offset / multiArray.strides( )[ direction] % multiArray.shape( )[ direction ] +
            multiArray.index_bases( )[direction] );
}

//! Get indices of pointer to single entry in multi-array (size 1) of doubles
/*!
 *  Get indices of pointer to single entry in multi-array (size 1) of doubles
 *  \param multiArray Multi-array for which the index is to be retrieved
 *  \param requestedElement Pointer to element for which index is to be retrieved
 *  \return Indices of pointer to single entry in multi-array of doubles
 */
boost::array< boost::multi_array< double, 1 >::index, 1 > getMultiArrayIndexArray(
        const boost::multi_array< double, 1 >& multiArray, const double* requestedElement );

//! Get indices of pointer to single entry in multi-array (size 2) of doubles
/*!
 *  Get indices of pointer to single entry in multi-array (size 2) of doubles
 *  \param multiArray Multi-array for which the index is to be retrieved
 *  \param requestedElement Pointer to element for which index is to be retrieved
 *  \return Indices of pointer to single entry in multi-array of doubles
 */
boost::array< boost::multi_array< double, 2 >::index, 2 > getMultiArrayIndexArray(
        const boost::multi_array< double, 2 >& multiArray, const double* requestedElement );

//! Get indices of pointer to single entry in multi-array (size 3) of doubles
/*!
 *  Get indices of pointer to single entry in multi-array (size 3) of doubles
 *  \param multiArray Multi-array for which the index is to be retrieved
 *  \param requestedElement Pointer to element for which index is to be retrieved
 *  \return Indices of pointer to single entry in multi-array of doubles
 */
boost::array< boost::multi_array< double, 3 >::index, 3 > getMultiArrayIndexArray(
        const boost::multi_array< double, 3 >& multiArray, const double* requestedElement );



} // namespace utilities

} // namespace tudat

#endif // TUDAT_UTILITIES_H
