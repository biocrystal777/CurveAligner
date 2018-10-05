#ifndef INTERPOLATE_H
#define INTERPOLATE_H

#include <QString>
#include <vector>
#include <map>
#include <ostream>

class US_Saxs_Scan
{
 public:
   QString header;
   QString filename;
   std::vector < QString > header_cols;

   std::vector < double > q;
   std::vector < double > r;
   std::vector < double > s;
   void clear();
};

class point
{
 public:
   float axis[3];
   friend std::ostream& operator<<(std::ostream&, const point&);
   bool operator < ( const point & objIn ) const
   {
      return
         axis[ 0 ] < objIn.axis[ 0 ] ||
         ( axis[ 0 ] == objIn.axis[ 0 ] &&
           axis[ 1 ] <  objIn.axis[ 1 ] ) ||
         ( axis[ 0 ] == objIn.axis[ 0 ] &&
           axis[ 1 ] == objIn.axis[ 1 ] &&
           axis[ 2 ] <  objIn.axis[ 2 ] );
   }

   bool operator == ( const point & objIn ) const
   {
      return
         axis[ 0 ] == objIn.axis[ 0 ] &&
         axis[ 1 ] == objIn.axis[ 1 ] &&
         axis[ 2 ] == objIn.axis[ 2 ]
         ;
   }
};


class interpolate
{
 public:
   QString errormsg;

   void natural_spline( std::vector < double > &x,
                        std::vector < double > &y,
                        std::vector < double > &y2 );

   bool apply_natural_spline( std::vector < double > &xa,
                              std::vector < double > &ya,
                              std::vector < double > &y2a,
                              double            x,
                              double            &y );

   bool quadratic_interpolate(
                              std::vector < double >       &x1,
                              std::vector < double >       &y1,
                              std::vector < double >       &x2,
                              std::vector < double >       &y2
                               );

   bool linear_interpolate(
                           std::vector < double >       &x1,
                           std::vector < double >       &y1,
                           std::vector < double >       &x2,
                           std::vector < double >       &y2
                            );

   bool do_interpolate(
                       std::vector < double > & results,
                       std::vector < double > & to_r,
                       std::vector < double > & from_r,
                       std::vector < double > & from_pr );


 private:
   int debug;
   std::map < QString, US_Saxs_Scan > wave;

   bool do_interpolate( QString outtag,   QString totag,   QString fromtag );
   bool get_quadratic_interpolation_coefficients(
                                                 std::vector < double > x,
                                                 std::vector < double > y,
                                                 std::vector < double > &c
                                                  );
   point cross( point p1, point p2 );       // returns p1 x p2
   float dot( point p1, point p2 );         // p1 dot p2
   point scale( point p, float m );         // sqrt( (p1 - p2) dot (p1 - p2) )

};

#endif
