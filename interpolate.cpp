//#include "mergecurves.h"
#include "interpolate.h"
#include <QDebug>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <qfile.h>
#include <list>
#include <cmath>

using std::vector;

void US_Saxs_Scan::clear()
{
   filename = "";
   header = "";
   header_cols.clear();
   q.clear();
   r.clear();
   s.clear();
}

void interpolate::natural_spline( vector < double > &x,
                                  vector < double > &y,
                                  vector < double > &y2 )
{
   double  p;
   // double  qn;
   double  sig;
   // double  un;
   vector < double > u(x.size());

   y2.resize(x.size());

   y2[ 0 ] = u [ 0 ] = 0e0;

   for ( unsigned int i = 1; i < x.size() - 1; i++ )
   {
      sig     = ( x[ i ] - x[ i - 1 ] ) / ( x[ i + 1 ] - x[ i - 1 ] );
      p       = sig * y2[ i - 1 ] + 2e0;
      y2[ i ] = ( sig -1e0 ) / p;
      u [ i ] = ( y[ i + 1 ] - y[ i ] ) / ( x[ i + 1 ] - x[ i ]) - ( y[ i ] - y[ i - 1 ] ) / ( x[ i ] - x[ i - 1 ]);
      u [ i ] = ( 6e0 * u[ i ] / ( x[ i + 1 ] - x[ i - 1 ] ) - sig * u[ i - 1 ] ) / p;
   }

   // qn =  un = 0e0;

   y2[ x.size() - 1 ] = 0e0;

   for (unsigned int k = x.size() - 2; k >= 1 ; k-- )
   {
      y2[ k ] = y2[ k ] * y2[ k + 1 ] + u[ k ];
   }
}

bool interpolate::apply_natural_spline( vector < double > &xa,
                                         vector < double > &ya,
                                         vector < double > &y2a,
                                         double            x,
                                         double            &y )
{
   unsigned int klo = 0;
   unsigned int khi = xa.size() - 1;

   while ( khi - klo > 1) {
      unsigned int k = ( khi + klo ) >> 1;
      if ( xa[ k ] > x )
      {
         khi = k;
      } else {
         klo = k;
      }
   }

   if ( khi == klo )
   {
      errormsg = "interpolate::apply_natural_spline error finding point";
      return false;
   }

   double h = xa[ khi ] - xa[ klo ];

   if ( h <= 0e0 )
   {
      errormsg = "interpolate::apply_natural_spline zero or negative interval";
      return false;
   }

   double a = ( xa[ khi ] - x ) / h;
   double b = ( x - xa[ klo ] ) / h;

   y =
      a * ya[ klo ] +
      b * ya[ khi ] + ( ( a * a * a - a ) * y2a[ klo ] +
                        ( b * b * b - b ) * y2a[ khi ] ) * ( h * h ) / 6e0;

   return true;
}


bool interpolate::quadratic_interpolate(
                                         vector < double > &x1,
                                         vector < double > &y1,
                                         vector < double > &x2,
                                         vector < double > &y2
                                         )
{
   errormsg = "";
   if ( x1.size() != y1.size() )
   {
      errormsg = "interpolate::quadratic_interpolate x1 & y1 must be the same size";
      return false;
   }

   if ( x1.size() < 3 )
   {
      errormsg = "interpolate::quadratic_interpolate needs at least 3 points";
      return false;
   }

   if ( !x2.size() )
   {
      errormsg = "interpolate::quadratic_interpolate target length needs at least 1 point";
      return false;
   }

   if ( x2[ 0 ] < x1[ 0 ] || x2.back() > x1.back() )
   {
      errormsg = "interpolate::quadratic_interpolate can not extrapolate";
      return false;
   }

   y2.resize( x2.size() );

   for ( unsigned int i = 0; i < x2.size(); i++ )
   {
      bool found = false;
      for ( unsigned int j = 0; !found && j < x1.size() - 1; j++ )
      {
         if ( x2[ i ] >= x1[ j ] &&
              x2[ i ] <= x1[ j + 1 ] )
         {
            if ( j == 0 )
            {
               vector < double > x( 3 );
               vector < double > y( 3 );
               vector < double > c( 3 );

               for ( unsigned int k = 0; k < 3; k++ )
               {
                  x[ k ] = x1[ k ];
                  y[ k ] = y1[ k ];
               }
               if ( !get_quadratic_interpolation_coefficients( x, y, c ) )
               {
                  qDebug() << errormsg << endl;
                  qDebug() << "resorting to linear interpolation\n";
                  return linear_interpolate( x1, y1, x2, y2 );
               }
               y2[ i ] = c[ 0 ] + c[ 1 ] * x2[ i ] + c[ 2 ] * x2[ i ] * x2[ i ];
               found = true;
               continue;
            }

            if ( j >= x1.size() - 2 )
            {
               vector < double > x( 3 );
               vector < double > y( 3 );
               vector < double > c( 3 );

               for ( unsigned int k = 0; k < 3; k++ )
               {
                  x[ k ] = x1[ x1.size() + k - 3 ];
                  y[ k ] = y1[ x1.size() + k - 3 ];
               }
               if ( !get_quadratic_interpolation_coefficients( x, y, c ) )
               {
                  qDebug() << errormsg << endl;
                  qDebug() << "resorting to linear interpolation\n";
                  return linear_interpolate( x1, y1, x2, y2 );
               }
               y2[ i ] = c[ 0 ] + c[ 1 ] * x2[ i ] + c[ 2 ] * x2[ i ] * x2[ i ];
               found = true;
               continue;
            }
            {
               vector < double > xa(3);
               vector < double > ya(3);
               vector < double > ca(3);

               vector < double > xb(3);
               vector < double > yb(3);
               vector < double > cb(3);

               for ( unsigned int k = 0; k < 3; k++ )
               {
                  xa[k] = x1[ j + k - 1 ];
                  ya[k] = y1[ j + k - 1 ];
                  xb[k] = x1[ j + k ];
                  yb[k] = y1[ j + k ];
               }
               if (
                   !get_quadratic_interpolation_coefficients( xa, ya, ca ) ||
                   !get_quadratic_interpolation_coefficients( xb, yb, cb )
                   )
               {
                  qDebug() << errormsg << endl;
                  qDebug() << "resorting to linear interpolation\n";
                  return linear_interpolate( x1, y1, x2, y2 );
               }

               y2[ i ] =  0.5 * ( ca[ 0 ] + cb[ 0 ] +
                                  ( ca[ 1 ] + cb[ 1 ] ) * x2[ i ] +
                                  ( ca[ 2 ] + cb[ 2 ] ) * x2[ i ] * x2[ i ] );

               found = true;
               continue;
            }
         }
      }
      if ( !found )
      {
         errormsg = QString( "point %1 not found\n" ).arg( x2[ i ] );
         return false;
      }
   }
   return true;
}

bool interpolate::get_quadratic_interpolation_coefficients(
                                                           vector < double > x,
                                                           vector < double > y,
                                                           vector < double > &c
                                                            )
{
   errormsg = "";
   if ( x.size() != 3 || y.size() != 3 )
   {
      errormsg = "interpolate::get_quadratic_interpolation_coefficients vector sizes must be 3";
      return false;
   }

   point xp[3];
   point yp;

   for ( unsigned int i = 0; i < 3; i++ )
   {
      xp[0].axis[i] = 1e0;
      xp[1].axis[i] = x[i];
      xp[2].axis[i] = x[i] * x[i];
      yp.axis[i] = y[i];
   }

   // compute determinant

   float det = dot( xp[0], cross( xp[1], xp[2] ) );

   if ( det == 0.0 )
   {
      errormsg = "interpolate::get_quadratic_interpolation_coefficients linearly dependent";
      return false;
   }

   float oneoverdet = 1.0 / det;

   point xi[3];

   xi[0] = scale( cross( xp[1], xp[2] ), oneoverdet );
   xi[1] = scale( cross( xp[2], xp[0] ), oneoverdet );
   xi[2] = scale( cross( xp[0], xp[1] ), oneoverdet );

   c.resize(3);

   for ( unsigned int i = 0; i < 3; i++ )
   {
      c[i] = dot( xi[i], yp );
   }

   return true;
}

bool interpolate::linear_interpolate(
                                      vector < double > &x1,
                                      vector < double > &y1,
                                      vector < double > &x2,
                                      vector < double > &y2
                                      )
{
   errormsg = "";
   if ( x1.size() != y1.size() )
   {
      errormsg = "interpolate::linear_interpolate x1 & y1 must be the same size";
      return false;
   }

   if ( !x2.size() )
   {
      errormsg = "interpolate::linear_interpolate target length needs at least 1 point";
      return false;
   }

   if ( x2[ 0 ] < x1[ 0 ] || x2.back() > x1.back() )
   {
      errormsg = "interpolate::linear_interpolate can not extrapolate";
      return false;
   }

   y2.resize( x2.size() );

   for ( unsigned int i = 0; i < x2.size(); i++ )
   {
      bool found = false;
      for ( unsigned int j = 0; !found && j < x1.size() - 1; j++ )
      {
         if ( x2[ i ] >= x1[ j ] &&
              x2[ i ] <= x1[ j + 1 ] )
         {
            double pos = ( x2[ i ] - x1[ j ] ) / ( x1[ j + 1 ] - x1[ j ] );
            y2[ i ] = y1[ j ] * ( 1e0 - pos ) + y1[ j + 1 ] * pos;
            found = true;
         }
      }
      if ( !found )
      {
         errormsg = QString( "point %1 not found\n" ).arg( x2[ i ] );
         return false;
      }
   }
   return true;
}

bool interpolate::do_interpolate( vector < double > & results,
                                  vector < double > & to_r,
                                  vector < double > & from_r,
                                  vector < double > & from_pr )
{
   errormsg = "";

   vector < double > new_from_r;
   vector < double > new_from_pr;
   new_from_r.push_back(-1);
   new_from_pr.push_back(0);

   for ( int i = 0; i < (int) from_r.size(); i++ )
   {
      new_from_r.push_back(from_r[i]);
      new_from_pr.push_back(from_pr[i]);
   }
   new_from_r.push_back(1e99);
   new_from_pr.push_back(0);

   wave["from"].q = new_from_r;
   wave["from"].r = new_from_pr;
   wave["from"].s = new_from_pr;
   wave["to"].q = to_r;
   wave["to"].r = to_r;

   bool ok = do_interpolate( "out", "to", "from" );
   results = wave["out"].r;
   return ok;
}

bool interpolate::do_interpolate(QString outtag, QString totag, QString fromtag)
{
   // linearly interpolates the "from" q grid to the "to" qgrid returning in outtag
   // important note: "from" grid must cover "to" grid

   wave[outtag].clear();
   wave[outtag].filename = QString("interpolate_%1_%2").arg(totag).arg(fromtag);
   wave[outtag].header = wave[fromtag].header;
   wave[outtag].header_cols = wave[fromtag].header_cols;
   wave[outtag].q = wave[totag].q;
   wave[outtag].r.resize(wave[totag].q.size());
   wave[outtag].s.resize(wave[totag].q.size());

   unsigned int p1 = 0;
   unsigned int p2;
   double pct;

   for( p2 = 0; p2 < wave[totag].q.size(); p2++ )
   {
      while ( wave[fromtag].q[p1] < wave[totag].q[p2] && p1 < wave[fromtag].q.size() )
      {
         p1++;
      }
      if ( p1 < 1 )
      {
         errormsg = "interpolate error 1";
         return false;
      }
      if ( wave[fromtag].q[p1 - 1] > wave[totag].q[p2] )
      {
         errormsg = "interpolate error 2";
         return false;
      }
      if ( p1 >= wave[fromtag].q.size() ) {
         errormsg = "interpolate error 3";
      }
      pct = (wave[totag].q[p2] - wave[fromtag].q[p1 - 1]) / (wave[fromtag].q[p1] - wave[fromtag].q[p1 - 1]);
      wave[outtag].r[p2] = pct * wave[fromtag].r[p1] + ( 1e0 - pct ) * wave[fromtag].r[p1 - 1];
      wave[outtag].s[p2] = pct * wave[fromtag].s[p1] + ( 1e0 - pct ) * wave[fromtag].s[p1 - 1];
   }
   return true;
}

point interpolate::cross( point p1, point p2) // p1 cross p2
{
   point result;
   result.axis[0] = p1.axis[1] * p2.axis[2] -  p1.axis[2] * p2.axis[1];
   result.axis[1] = p1.axis[2] * p2.axis[0] -  p1.axis[0] * p2.axis[2];
   result.axis[2] = p1.axis[0] * p2.axis[1] -  p1.axis[1] * p2.axis[0];
   return result;
}

float interpolate::dot( point p1, point p2) // p1 dot p2
{
   return
      p1.axis[0] * p2.axis[0] +
      p1.axis[1] * p2.axis[1] +
      p1.axis[2] * p2.axis[2];
}

point interpolate::scale( point p, float m )
{
   for ( unsigned int i = 0; i < 3; i++ )
   {
      p.axis[i] *= m;
   }
   return p;
}
