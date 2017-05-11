#ifndef TUDAT_ONEWAYDOPPLEROBSERVATIONMODEL_H
#define TUDAT_ONEWAYDOPPLEROBSERVATIONMODEL_H


#include <map>

#include <boost/function.hpp>
#include <boost/make_shared.hpp>

#include <Eigen/Core>

#include "Tudat/Astrodynamics/BasicAstrodynamics/physicalConstants.h"
#include "Tudat/Astrodynamics/ObservationModels/observationModel.h"
#include "Tudat/Astrodynamics/ObservationModels/lightTimeSolution.h"

namespace tudat
{

namespace observation_models
{

//! Function to compute the component of a (velocity) vector projected along a unit vector, divided by speed of light.
/*!
 *  Function to compute the component of a (velocity) vector projected along a unit vector, divided by speed of light.
 *  \param lineOfSightUnitVector Unit vector along which velocityVector is to be projected
 *  \param velocityVector Vector for which component along lineOfSightUnitVector is to be computed
 *  \return The component of a (velocity) vector projected along a unit vector, divided by speed of light.
 */
template< typename ObservationScalarType = double >
ObservationScalarType calculateLineOfSightVelocityAsCFraction(
        const Eigen::Matrix< ObservationScalarType, 3, 1 >& lineOfSightUnitVector,
        const Eigen::Matrix< ObservationScalarType, 3, 1 >& velocityVector )
{
    return lineOfSightUnitVector.dot( velocityVector ) / physical_constants::getSpeedOfLight< ObservationScalarType >( );
}

//! Function to compute component of transmitter velocity projected along line-of-sight vector, divided by speed of light.
/*!
 *  Function to compute component of transmitter velocity projected along line-of-sight vector, divided by speed of light,
 *  from the receiver position and transmitter state function. The unit vector is computed in the direction from the
 *  transmitter to the receiver
 *  \param receiverPosition Cartesian position of receiver
 *  \param transmitterStateFunction Function returning the Cartesian state of the transmitter.
 *  \param currentTime Time at which transmitterStateFunction is to be evaluated.
 *  \return Component of transmitter velocity projected along line-of-sight vector to receiver, divided by speed of light.
 */
template< typename ObservationScalarType = double, typename TimeType = double >
ObservationScalarType calculateLineOfSightVelocityAsCFractionFromTransmitterStateFunction(
        const Eigen::Matrix< ObservationScalarType, 3, 1 >& receiverPosition,
        const boost::function< Eigen::Matrix< ObservationScalarType, 6, 1 >( const double ) >& transmitterStateFunction,
        const TimeType currentTime )
{
    Eigen::Matrix< ObservationScalarType, 6, 1 > currentState = transmitterStateFunction( currentTime );
    return calculateLineOfSightVelocityAsCFraction< ObservationScalarType >(
                ( receiverPosition - currentState.segment( 0, 3 ) ).normalized( ), currentState.segment( 3, 3 ) );
}

//! Function to compute component of transmitter velocity projected along line-of-sight vector, divided by speed of light.
/*!
 *  Function to compute component of transmitter velocity projected along line-of-sight vector, divided by speed of light,
 *  from the receiver state function and transmitter position. The unit vector is computed in the direction from the
 *  transmitter to the receiver
 *  \param receiverStateFunction Function returning the Cartesian state of the receiver.
 *  \param transmitterPosition Cartesian position of transmitter
 *  \param currentTime Time at which transmitterStateFunction is to be evaluated.
 *  \return Component of transmitter velocity projected along line-of-sight vector to receiver, divided by speed of light.
 */
template< typename ObservationScalarType = double, typename TimeType = double >
ObservationScalarType calculateLineOfSightVelocityAsCFractionFromReceiverStateFunction(
        const boost::function< Eigen::Matrix< ObservationScalarType, 6, 1 >( const double ) >& receiverStateFunction,
        const Eigen::Matrix< ObservationScalarType, 3, 1 >& transmitterPosition,
        const TimeType currentTime )
{
    Eigen::Matrix< ObservationScalarType, 6, 1 > currentState = receiverStateFunction( currentTime );
    return calculateLineOfSightVelocityAsCFraction< ObservationScalarType >(
                ( currentState.segment( 0, 3 ) - transmitterPosition ).normalized( ), currentState.segment( 3, 3 ) );
}

//! Function to compute first-order (radial) Doppler term from a Taylor series expansion
/*!
 *  Function to compute first-order (radial) Doppler term from a Taylor series expansion. The function computes the
 *  (dt1/dt2 -1) term, with t2 the coordinate reception time and t1 the coordinate transmission time of the signal. Light
 *  time corrections are not included in this function. The Taylor series of the denominator of dt1/dt2 is used in the
 *  calculation, to an order that is provided as input.
 *  \param transmitterState Cartesian state of the transmitter at t1
 *  \param receiverState Cartesian state of the receiver at t1
 *  \param taylorSeriesOrder Order to which Taylor series is to be expanded
 *  \return First-order Doppler effect for electromagnetic signal transmission from transmitter to receiver as: (dt1/dt2 -1)
 *   with t2 the coordinate reception time and t1 the coordinate transmission time of the signal.
 */
template< typename ObservationScalarType = double >
ObservationScalarType computeOneWayFirstOrderDopplerTaylorSeriesExpansion(
        Eigen::Matrix< ObservationScalarType, 6, 1 >& transmitterState,
        Eigen::Matrix< ObservationScalarType, 6, 1 >& receiverState,
        const int taylorSeriesOrder )
{
    // Compute line of sight unit vector
    Eigen::Matrix< ObservationScalarType, 3, 1 > relativePostion =
            ( ( receiverState - transmitterState ).segment( 0, 3 ) ).normalized( );

    // Compute projected velocity components
    ObservationScalarType transmitterTerm  =
            ( relativePostion.dot( transmitterState.segment( 3, 3 ) ) /
              physical_constants::getSpeedOfLight< ObservationScalarType >( )  );
    ObservationScalarType receiverTerm =
            ( relativePostion.dot( receiverState.segment( 3, 3 ) ) /
              physical_constants::getSpeedOfLight< ObservationScalarType >( ) );

    // Compute Taylor series of 1/(1-r21*v2) up to required order
    ObservationScalarType currentTaylorSeriesTerm =
            mathematical_constants::getFloatingInteger< ObservationScalarType >( 1 );
    ObservationScalarType currentTaylorSeries = mathematical_constants::getFloatingInteger< ObservationScalarType >( 0 );
    for( int i = 0; i < taylorSeriesOrder; i++ )
    {
        currentTaylorSeriesTerm *= receiverTerm;
        currentTaylorSeries += currentTaylorSeriesTerm;
    }

    // Compute Doppler term
    return -transmitterTerm + currentTaylorSeries *
            ( mathematical_constants::getFloatingInteger< ObservationScalarType >( 1 ) - transmitterTerm );

}

//! Computes observable the (simplified) one-way Doppler observation between two link ends, omitting proper time rates and
//! light time corrections.
/*!
 *  Computes observable the (simplified) one-way Doppler observation between two link ends, omitting proper time rates and
 *  light time corrections. The observable is defined as d f_{B}/d_f_{A} - 1, with A the transmitter and B the receiver, and
 *  f the frequency of the signal.
 */
template< typename ObservationScalarType = double, typename TimeType = double >
class OneWayDopplerObservationModel: public ObservationModel< 1, ObservationScalarType, TimeType >
{
public:

    typedef Eigen::Matrix< ObservationScalarType, 6, 1 > StateType;
    typedef Eigen::Matrix< ObservationScalarType, 3, 1 > PositionType;


    //! Constructor.
    /*!
     *  Constructor,
     *  \param lightTimeCalculator Object to compute the light-time (including any corrections w.r.t. Euclidean case)
     *  \param observationBiasCalculator Object for calculating system-dependent errors in the
     *  observable, i.e. deviations from the physically ideal observable between reference points (default none).
     */
    OneWayDopplerObservationModel(
            const boost::shared_ptr< observation_models::LightTimeCalculator< ObservationScalarType, TimeType > >
            lightTimeCalculator,
            const boost::shared_ptr< ObservationBias< 1 > > observationBiasCalculator = NULL ):
        ObservationModel< 1, ObservationScalarType, TimeType >( one_way_doppler, observationBiasCalculator ),
        lightTimeCalculator_( lightTimeCalculator )
    {
        one_ = mathematical_constants::getFloatingInteger< ObservationScalarType >( 1 );
        taylorSeriesExpansionOrder_ = 3;
    }

    //! Destructor
    ~OneWayDopplerObservationModel( ){ }

    //! Function to compute ideal one-way Doppler observation  without any corrections at given time.
    /*!
     *  This function compute ideal the one-way observation  without any corrections at a given time.
     *  The time argument can be either the reception or transmission time (defined by linkEndAssociatedWithTime input).
     *  It does not include system-dependent measurement
     *  errors, such as biases or clock errors.
     *  \param time Time at which observation is to be simulated
     *  \param linkEndAssociatedWithTime Link end at which given time is valid, i.e. link end for which associated time
     *  is kept constant (to input value)
     *  \return Calculated observed one-way Doppler value.
     */
    Eigen::Matrix< ObservationScalarType, 1, 1 > computeIdealObservations(
            const TimeType time,
            const LinkEndType linkEndAssociatedWithTime )

    {
        ObservationScalarType lightTime;

        bool isTimeAtReception = -1;
        switch( linkEndAssociatedWithTime )
        {
        case receiver:
            isTimeAtReception = true;
            break;
        case transmitter:
            isTimeAtReception = false;
            break;
        default:
            throw std::runtime_error(
                        "Error when calculating one way Doppler observation, link end is not transmitter or receiver" );
        }

        // Compute light time
        lightTime = lightTimeCalculator_->calculateLightTimeWithLinkEndsStates(
                    receiverState_, transmitterState_, time, true );

        // Compute one-way Doppler
        return ( Eigen::Matrix<  ObservationScalarType, 1, 1  >( )
                 << computeOneWayFirstOrderDopplerTaylorSeriesExpansion(
                     transmitterState_, receiverState_, taylorSeriesExpansionOrder_ ) ).finished( );
    }

    //! Function to compute one-way Doppler observable without any corrections.
    /*!
     *  Function to compute one-way Doppler  observable without any corrections, i.e. the true physical Doppler as computed
     *  from the defined link ends. It does not include system-dependent measurement
     *  errors, such as biases or clock errors.
     *  The times and states of the link ends are also returned in double precision. These states and times are returned by
     *  reference.
     *  \param time Time at which observable is to be evaluated.
     *  \param linkEndAssociatedWithTime Link end at which given time is valid, i.e. link end for which associated time
     *  is kept constant (to input value)
     *  \param linkEndTimes List of times at each link end during observation.
     *  \param linkEndStates List of states at each link end during observation.
     *  \return Ideal one-way Doppler observable.
     */
    Eigen::Matrix< ObservationScalarType, 1, 1 > computeIdealObservationsWithLinkEndData(
            const TimeType time,
            const LinkEndType linkEndAssociatedWithTime,
            std::vector< double >& linkEndTimes,
            std::vector< Eigen::Matrix< double, 6, 1 > >& linkEndStates )
    {
        ObservationScalarType lightTime;
        TimeType transmissionTime, receptionTime;

        bool fixTransmissionTime;

        // Compute light time
        switch( linkEndAssociatedWithTime )
        {
        case receiver:
            lightTime = lightTimeCalculator_->calculateLightTimeWithLinkEndsStates(
                        receiverState_, transmitterState_, time, true );
            transmissionTime = time - lightTime;
            receptionTime = time;
            fixTransmissionTime = 0;
            break;

        case transmitter:
            lightTime = lightTimeCalculator_->calculateLightTimeWithLinkEndsStates(
                        receiverState_, transmitterState_, time, false );
            transmissionTime = time;
            receptionTime = time + lightTime;
            fixTransmissionTime = 1;
            break;
        default:
            throw std::runtime_error(
                        "Error when calculating one way Doppler observation, link end is not transmitter or receiver" );
        }

        // Save link end times and states
        linkEndTimes.push_back( transmissionTime );
        linkEndTimes.push_back( receptionTime );

        linkEndStates.push_back( transmitterState_.template cast< double >( ) );
        linkEndStates.push_back( receiverState_.template cast< double >( ) );

        // Compute and return one-way Doppler observable
        ObservationScalarType dopplerObservable = computeOneWayFirstOrderDopplerTaylorSeriesExpansion<
                ObservationScalarType >( transmitterState_, receiverState_, taylorSeriesExpansionOrder_ );
        return ( Eigen::Matrix<  ObservationScalarType, 1, 1  >( ) << dopplerObservable ).finished( );


    }

    //! Function to return the object to calculate light time.
    /*!
     * Function to return the object to calculate light time.
     * \return Object to calculate light time.
     */
    boost::shared_ptr< observation_models::LightTimeCalculator< ObservationScalarType, TimeType > > getLightTimeCalculator( )
    {
        return lightTimeCalculator_;
    }


private:

    //! Object to calculate light time.
    /*!
     *  Object to calculate light time, including possible corrections from troposphere, relativistic corrections, etc.
     */
    boost::shared_ptr< observation_models::LightTimeCalculator< ObservationScalarType, TimeType > > lightTimeCalculator_;

    //! Templated precision value of 1.0
    ObservationScalarType one_;

    //! Order to which Doppler effect Taylor series is to be expanded.
    int taylorSeriesExpansionOrder_;

    //! Pre-declared receiver state, to prevent many (de-)allocations
    StateType receiverState_;

    //! Pre-declared transmitter state, to prevent many (de-)allocations
    StateType transmitterState_;

};

}

}

#endif // TUDAT_ONEWAYDOPPLEROBSERVATIONMODEL_H
