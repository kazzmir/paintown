/**
 * @author Adam McLaurin
 * @date   September 2006
 */
/*
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GN6U General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
#include "RandomNumber.h"
#include <limits>
#include <time.h>

#ifdef SOCKETS_NAMESPACE
namespace SOCKETS_NAMESPACE {
#endif

const unsigned long int RandomNumber::X_SEED_DEFAULT = 123456789UL;
const unsigned long int RandomNumber::Y_SEED_DEFAULT = 362436069UL;
const unsigned long int RandomNumber::Z_SEED_DEFAULT = 521288629UL;
const unsigned long int RandomNumber::W_SEED_DEFAULT = 88675123UL;


RandomNumber::RandomNumber(bool time_shuffle)
:mXSeed(time_shuffle ? (unsigned long)time(NULL) ^ X_SEED_DEFAULT : X_SEED_DEFAULT)
,mYSeed(time_shuffle ? (unsigned long)time(NULL) ^ Y_SEED_DEFAULT : Y_SEED_DEFAULT)
,mZSeed(time_shuffle ? (unsigned long)time(NULL) ^ Z_SEED_DEFAULT : Z_SEED_DEFAULT)
,mWSeed(time_shuffle ? (unsigned long)time(NULL) ^ W_SEED_DEFAULT : W_SEED_DEFAULT)
{
  reset();
}

RandomNumber::RandomNumber(
  unsigned long int x_seed,
  unsigned long int y_seed,
  unsigned long int z_seed,
  unsigned long int w_seed)
:mXSeed(x_seed)
,mYSeed(y_seed)
,mZSeed(z_seed)
,mWSeed(w_seed)
{
  reset();
}

RandomNumber::~RandomNumber()
{
}

void RandomNumber::reset()
{
  mX = mXSeed;
  mY = mYSeed;
  mZ = mZSeed;
  mW = mWSeed;
}

RandomNumber::operator unsigned long int() const
{
  return(mW);
}

unsigned long int RandomNumber::next()
{
  register unsigned long int t = (mX ^ (mX<<11));

  mX = mY;

  mY = mZ;

  mZ = mW;

  return(mW = (mW ^ (mW>>19)) ^ (t ^ (t>>8)));
}

unsigned long int RandomNumber::skip(unsigned long int s)
{
  for(register unsigned long int i = 0 ; i < s ; ++i)
  {
    (void)next();
  }

  return(mW);
}

void RandomNumber::getSeed(
  unsigned long int& x_seed,
  unsigned long int& y_seed,
  unsigned long int& z_seed,
  unsigned long int& w_seed)
{
  x_seed = mXSeed;
  y_seed = mYSeed;
  z_seed = mZSeed;
  w_seed = mWSeed;
}

unsigned long int RandomNumber::max_random()
{
  return(std::numeric_limits<unsigned long int>::max());
}

#ifdef SOCKETS_NAMESPACE
} // namespace SOCKETS_NAMESPACE {
#endif
