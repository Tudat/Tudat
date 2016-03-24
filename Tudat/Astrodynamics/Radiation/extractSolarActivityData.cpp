/*   Copyright (c) 2010-2012, Delft University of Technology
 *    All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without modification, are
 *    permitted provided that the following conditions are met:
 *      - Redistributions of source code must retain the above copyright notice, this list of
 *        conditions and the following disclaimer.
 *      - Redistributions in binary form must reproduce the above copyright notice, this list of
 *        conditions and the following disclaimer in the documentation and/or other materials
 *        provided with the distribution.
 *      - Neither the name of the Delft University of Technology nor the names of its contributors
 *        may be used to endorse or promote products derived from this software without specific
 *        prior written permission.
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 *    OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *    COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 *    GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 *    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *    NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 *    OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *    Changelog
 *      YYMMDD    Author            Comment
 *      120607    A. Ronse          Creation of code.
 *
 *    References
 *      Data file:
 *                        http://celestrak.com/SpaceData/sw19571001.txt
 *                        http://celestrak.com/SpaceData/sw20110101.txt
 *      Data format explanation:
 *                        http://celestrak.com/SpaceData/SpaceWx-format.asp
 *
 */

#include "Tudat/Astrodynamics/Radiation/extractSolarActivityData.h"
#include "Tudat/Astrodynamics/Radiation/parseSolarActivityData.h"  // For the FieldTypes


using namespace tudat::input_output::parsed_data_vector_utilities;
using namespace tudat::input_output::field_types::solar_activity;
using namespace tudat::input_output::field_types::time;

namespace tudat
{
namespace radiation
{
namespace solar_activity
{

boost::shared_ptr<SolarActivityData> ExtractSolarActivityData::extract(
        tudat::input_output::parsed_data_vector_utilities::ParsedDataLineMapPtr data)
{

    // Check if the required fields are available
    checkRequiredFieldType(data, 33, year, month, day, bartelsSolarRotationNumber,
                           dayOfBartelsCycle, planetaryRangeIndex0to3, planetaryRangeIndex3to6,
                           planetaryRangeIndex6to9, planetaryRangeIndex9to12,
                           planetaryRangeIndex12to15, planetaryRangeIndex15to18,
                           planetaryRangeIndex18to21, planetaryRangeIndex21to24,
                           planetaryRangeIndexSum, planetaryEquivalentAmplitude0to3,
                           planetaryEquivalentAmplitude3to6, planetaryEquivalentAmplitude6to9,
                           planetaryEquivalentAmplitude9to12, planetaryEquivalentAmplitude12to15,
                           planetaryEquivalentAmplitude15to18, planetaryEquivalentAmplitude18to21,
                           planetaryEquivalentAmplitude21to24, planetaryEquivalentAmplitudeAverage,
                           planetaryDailyCharacterFigure, planetaryDailyCharacterFigureConverted,
                           internationalSunspotNumber, solarRadioFlux107Adjusted, fluxQualifier,
                           centered81DaySolarRadioFlux107Adjusted,
                           last81DaySolarRadioFlux107Adjusted, solarRadioFlux107Observed,
                           centered81DaySolarRadioFlux107Observed,
                           last81DaySolarRadioFlux107Observed );

     // Create the resulting solar activity data object (will be returned at the end)
     boost::shared_ptr<SolarActivityData> solarActivityContainer(new SolarActivityData());

     // Convert string data and append to solar activity data object
     solarActivityContainer->year = getField<unsigned int>(data, year);
     solarActivityContainer->month = getField<unsigned int>(data, month);
     solarActivityContainer->day = getField<unsigned int>(data, day);
     solarActivityContainer->bartelsSolarRotationNumber = getField<unsigned int>(
             data, bartelsSolarRotationNumber);
     solarActivityContainer->dayOfBartelsCycle = getField<unsigned int>(data, dayOfBartelsCycle);
     solarActivityContainer->solarRadioFlux107Adjusted = getField<double>(
             data, solarRadioFlux107Adjusted);
     solarActivityContainer->centered81DaySolarRadioFlux107Adjusted = getField<double>(
             data, centered81DaySolarRadioFlux107Adjusted);
     solarActivityContainer->last81DaySolarRadioFlux107Adjusted = getField<double>(
             data, last81DaySolarRadioFlux107Adjusted);
     solarActivityContainer->solarRadioFlux107Observed = getField<double>(
             data, solarRadioFlux107Observed);
     solarActivityContainer->centered81DaySolarRadioFlux107Observed = getField<double>(
             data, centered81DaySolarRadioFlux107Observed);
     solarActivityContainer->last81DaySolarRadioFlux107Observed = getField<double>(
             data, last81DaySolarRadioFlux107Observed);
     solarActivityContainer->dataType = getField<unsigned int>(data, datatype);


     // Make sure only non-empty fields are extracted
     if (!data->find( planetaryRangeIndex0to3 )->second->getRaw( ).empty( ) ) // check if string is empty
     {
         solarActivityContainer->planetaryRangeIndexSum = getField<unsigned int>(
                 data, planetaryRangeIndexSum);
         solarActivityContainer->planetaryEquivalentAmplitudeAverage = getField<unsigned int>(
                 data, planetaryEquivalentAmplitudeAverage);
         solarActivityContainer->planetaryRangeIndexVector = Eigen::VectorXd::Zero( 8 );
         solarActivityContainer->planetaryEquivalentAmplitudeVector = Eigen::VectorXd::Zero( 8 );
         solarActivityContainer->planetaryRangeIndexVector <<
         getField<unsigned int>( data, planetaryRangeIndex0to3 ),
         getField<unsigned int>( data, planetaryRangeIndex3to6 ),
         getField<unsigned int>( data, planetaryRangeIndex6to9 ),
         getField<unsigned int>( data, planetaryRangeIndex9to12 ),
         getField<unsigned int>( data, planetaryRangeIndex12to15 ),
         getField<unsigned int>( data, planetaryRangeIndex15to18 ),
         getField<unsigned int>( data, planetaryRangeIndex18to21 ),
         getField<unsigned int>( data, planetaryRangeIndex21to24 );
         solarActivityContainer->planetaryEquivalentAmplitudeVector <<
         getField<unsigned int>( data, planetaryEquivalentAmplitude0to3 ),
         getField<unsigned int>( data, planetaryEquivalentAmplitude3to6 ),
         getField<unsigned int>( data, planetaryEquivalentAmplitude6to9 ),
         getField<unsigned int>( data, planetaryEquivalentAmplitude9to12 ),
         getField<unsigned int>( data, planetaryEquivalentAmplitude12to15 ),
         getField<unsigned int>( data, planetaryEquivalentAmplitude15to18 ),
         getField<unsigned int>( data, planetaryEquivalentAmplitude18to21 ),
         getField<unsigned int>( data, planetaryEquivalentAmplitude21to24 );
     }

  //   if ( !data->find( planetaryDailyCharacterFigure )->second->getRaw( )->empty( ) )
  //   {
  //       solarActivityContainer->planetaryDailyCharacterFigure = getField<double>(
  //               data, planetaryDailyCharacterFigure);
  //       solarActivityContainer->planetaryDailyCharacterFigureConverted = getField<unsigned int>(
  //               data, planetaryDailyCharacterFigureConverted);
  //   }
  //
  //   if  ( !data->find( internationalSunspotNumber )->second->getRaw( )->empty( ) )
 //    {
 //        solarActivityContainer->internationalSunspotNumber = getField<unsigned int>(
 //                data, internationalSunspotNumber);
 //    }

 //    if ( !data->find( fluxQualifier )->second->getRaw( )->empty( ) )
 //   {
 //        solarActivityContainer->fluxQualifier = getField<unsigned int>(data, fluxQualifier);
 //    }
     return solarActivityContainer;
}

} // namespace solar_activity
} // namespace radiation
} // namespace tudat
