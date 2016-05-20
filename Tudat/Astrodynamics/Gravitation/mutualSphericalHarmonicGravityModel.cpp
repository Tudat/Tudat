#include "Tudat/Astrodynamics/Gravitation/mutualSphericalHarmonicGravityModel.h"

namespace tudat
{

namespace gravitation
{

Eigen::MatrixXd setDegreeAndOrderCoefficientToZero( const boost::function< Eigen::MatrixXd( ) > originalCosineCoefficientFunction )
{
    Eigen::MatrixXd newCoefficients = originalCosineCoefficientFunction( );
    newCoefficients( 0, 0 ) = 0.0;
    return newCoefficients;
}

}

}
