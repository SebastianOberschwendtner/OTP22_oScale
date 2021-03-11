#
# OTP-22 oScale Firmware
# Copyright (c) 2020 Sebastian Oberschwendtner, sebastian.oberschwendtner@gmail.com
#
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#
"""
### Details
- *File:*     SampleFilter.py
- *Details:*  Python 3.9
- *Date:*     2021-03-10
- *Version:*  v1.0.1
- *Description*:
            This script simulates the integer behavior of the implemented discrete
            filter. It can be used to see the effects of decimation, filter gains,
            filter time constants and the influence of the coefficient scaling.

### Author
Sebastian Oberschwendtner, :email: sebastian.oberschwendtner@gmail.com
"""
# ****** Modules ******
import math
import numpy as np
import matplotlib.pyplot as plt

# ****** Variables ******
Accumulator = np.uint32(0)

# ****** Classes ******
class Filter_t():
    """Defines a discrete filter.

    ---
    """
    # ****** Properties ******
    
    # ****** Methods ******
    def __init__(self) -> None:
        self.a = np.zeros(3, dtype=np.uint16)
        self.b = np.zeros(3, dtype=np.uint16)
        self.x = np.zeros(3, dtype=np.uint16)
        self.y = np.zeros(3, dtype=np.uint16)

    def __str__(self) -> str:
        print(f'a = {self.a}')
        print(f'b = {self.b}')
        print(f'x = {self.x}')
        print(f'y = {self.y}')
        return ''


# ****** Functions ******
def main():
    pass

def ApplyIntegerFilter(Filter: Filter_t, Sample: np.uint16) -> np.uint16:
    """Applies the filter to the input samples. The current filter value
    is stored inside the filter object.

    Args:
        Filter (Filter_t): 1x1 [-] Filter Object
        Sample (np.uint16): 1x1 [-] Discrete Sample Value

    Returns:
        np.uint16: 1x1 [-] Current Filter Value with the new Sample

    ---
    """
    # Update current Sample
    Filter.x[0] = Sample * 16
    
    # Apply the filter:
    # y0 = (b0*x0 + b1*x1 + a1*y1 + a2*y2) >> a0
    Accumulator = 0;
    Accumulator += np.uint32( Filter.b[0] )* Filter.x[0]
    Accumulator += np.uint32( Filter.b[1] )* Filter.x[1] 
    Accumulator += np.uint32( Filter.a[1] )* Filter.y[1] 
    Accumulator += np.uint32( Filter.a[2] )* Filter.y[2] 

    Filter.y[0] = np.uint16(Accumulator >> 12)
    #/@todo Replace the explicit calculation of the filtered value with the bitshifting

    # Update the value arrays
    Filter.x[2] = Filter.x[1];
    Filter.x[1] = Filter.x[0];
    Filter.y[2] = Filter.y[1];
    Filter.y[1] = Filter.y[0];


    return np.uint16( math.floor(Filter.y[0] / 16))

# ****** Main ******
if __name__ == "__main__":
    filt = Filter_t()
    filt.a[1] = 4015
    filt.b[0] = 80