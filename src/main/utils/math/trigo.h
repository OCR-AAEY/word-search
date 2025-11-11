#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846 /* pi */
#endif

#define DEG2RAD(deg) ((deg) * M_PI / 180.0)

/// @brief Computes sine of an angle in degrees.
/// @param[in] degrees Angle in degrees.
/// @return Sine of the angle.
double sind(double degrees);

/// @brief Computes cosine of an angle in degrees.
/// @param[in] degrees Angle in degrees.
/// @return Cosine of the angle.
double cosd(double degrees);