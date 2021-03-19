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
        self.a = np.zeros(3, dtype=np.uint32)
        self.b = np.zeros(3, dtype=np.uint32)
        self.x = np.zeros(3, dtype=np.uint16)
        self.y = np.zeros(3, dtype=np.uint32)
        self.scale = np.zeros(2, dtype=np.uint8)

    def CreatePT1(self, Gain: float, T: float, Fs: float,
            SampleBits:int, ExtraBitsResult: int):
        # Reset the filter
        self.__init__()

        # Set the scaling bitshifts
        self.scale[0] = ExtraBitsResult
        self.scale[1] = 28 - SampleBits

        # Calculate the a coefficients and scale them
        self.a[0] = 1
        _a1 = (T*Fs)/(1 + T*Fs)
        _shift = self.scale[1] - self.scale[0]
        self.a[1] = np.uint32( math.floor(_a1 * (2**_shift)) )

        # calculate b coefficients and scale them
        _b0 = Gain / (1 + T*Fs)
        _shift = self.scale[1]
        self.b[0] = np.uint32( math.floor(_b0 * (2**_shift)) )


    def __str__(self) -> str:
        print(f'a = {self.a}')
        print(f'b = {self.b}')
        print(f'x = {self.x}')
        print(f'y = {self.y}')
        return ''


# ****** Functions ******
def CompareFixedStep( StepVal: int = 5, Duration: float = 10.0,
        StartBits: int = 0, EndBits: int = 10):
    """Compares the step response of a discrete filter with different
    numbers of extra bits to increase filter resolution. The resulting
    plots show the influence of the increased resolution versus the
    increased rounding errors.

    Args:
        StepVal (int, optional): 1x1 The amplitude of the ideal step input. Defaults to 5.
        Duration (float, optional): 1x1 The duration of the step intput. Defaults to 10.0.
        StartBits (int, optional): 1x1 The number of extra bits when stopping the sweep. Defaults to 0.
        EndBits (int, optional): 1x1 Th number of extra bits when starting the sweep. Defaults to 10.
    
    ---
    """
    # Define filter constants
    Gain = 1.0
    T = 0.3
    Fs = 100

    # Get number of samples
    N_Samples = math.floor(Duration*Fs) + 1

    # Create Filter
    _filt = Filter_t()

    # Calculate the responses
    Response = []
    for iBits in range(StartBits, EndBits + 1):
        _response = []
        _filt.CreatePT1( Gain, T, Fs, 12, iBits)

        for iSample in range(N_Samples):
            _response.append( ApplyIntegerFilter(_filt, StepVal))
        
        Response.append( _response )

    # Create time vector for plotting
    Time = []
    for iSample in range(N_Samples):
        Time.append( iSample/Fs )

    # Plot Results
    fig = plt.figure()
    plt.rcParams.update({'font.size': 22})
    plt.title(f'Time Response IIR - PT1, Step = {StepVal}')
    for i, iBits in enumerate(range(StartBits, EndBits + 1)):
        plt.plot(Time, Response[i],'-o', label=f'Extra Bits = {iBits}')
    #plt.legend()
    plt.grid(True)
    plt.xlabel('Time in $[s]$')
    plt.ylabel('Filtered Value')

    fig = plt.figure()
    plt.title('Max Response Value')
    _MaxResponse = []
    for iResponse in Response:
        _MaxResponse.append( 100*iResponse[-1]/StepVal )
    plt.plot(range(StartBits, EndBits +1), _MaxResponse, '-o')
    plt.xlabel('Number of extra bits')
    plt.ylabel('Endvalue compared to input value in [%]')
    plt.grid(True)
    plt.show()

def CompareVariableStep( StepVal: int = 5, Duration: float = 10.0,
        StartBits: int = 0, EndBits: int = 10):
    """Compares the maximum response of different numbers of extra bits of an discrete
    filter. 

    Args:
        StepVal (int, optional): 1x1 The amplitude of the ideal step input. Defaults to 5.
        Duration (float, optional): 1x1 The duration of the step intput. Defaults to 10.0.
        StartBits (int, optional): 1x1 The number of extra bits when stopping the sweep. Defaults to 0.
        EndBits (int, optional): 1x1 Th number of extra bits when starting the sweep. Defaults to 10.
    
    ---
    """
    # Define filter constants
    Gain = 1.0
    T = 0.3
    Fs = 100
    _filt = Filter_t()

    # Get number of samples
    _SampleStep = 50
    N_Samples = math.floor(Duration*Fs) + 1
    N_Sweeps = len( range(StartBits, EndBits + 1) )

    # Initialize the response vector
    Response = np.zeros((N_Sweeps, math.floor(StepVal/_SampleStep)))
    _Steps = range(1, StepVal+1, _SampleStep)

    # Calculate the responses
    # For every range of extra bits
    for i, iBit in enumerate(range(StartBits, EndBits +1)):
        # Get the filter response for one step amplitude
        for k, iStep in enumerate(range(1, StepVal+1, _SampleStep)):
            _response = []
            _filt.CreatePT1(Gain, T, Fs, 12, iBit)
            for iSample in range(N_Samples):
                _response.append( ApplyIntegerFilter(_filt, iStep))
            # The step value is the last response value
            Response[i][k] = 100*_response[-1]/iStep

    # Plot results
    plt.figure()
    plt.rcParams.update({'font.size': 22})
    
    for i, iBit in enumerate(range(StartBits, EndBits +1)):
        plt.plot(_Steps, Response[i], 'o',label=f'Extra Bits = {iBit}')

    #plt.legend()
    plt.grid(True)
    plt.title('Settled value for different step amplitudes IIR - PT1', fontsize=28)
    plt.xlabel('Amplitude Step Input', fontsize=28)
    plt.ylabel('Settled value compared to input value in [%]', fontsize=28)
    plt.show()

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
    Filter.x[0] = Sample
    
    # Apply the filter:
    # y0 = (b0*x0 + b1*x1 + a1*y1 + a2*y2) >> a0
    Accumulator = 0
    Accumulator += np.uint32( Filter.b[0] )* Filter.x[0]
    Accumulator += np.uint32( Filter.b[1] )* Filter.x[1] 
    Accumulator += np.uint32( Filter.a[1] )* Filter.y[1] 
    Accumulator += np.uint32( Filter.a[2] )* Filter.y[2] 

    _shift = Filter.scale[1] - Filter.scale[0]
    Filter.y[0] = np.uint32(Accumulator >> _shift)

    # Update the value arrays
    Filter.x[2] = Filter.x[1];
    Filter.x[1] = Filter.x[0];
    Filter.y[2] = Filter.y[1];
    Filter.y[1] = Filter.y[0];


    return np.uint16( Filter.y[0] >> Filter.scale[0] )

# ****** Main ******
if __name__ == "__main__":
   CompareVariableStep(StepVal=4000, StartBits=0, EndBits=0)
   #CompareFixedStep(StepVal=200, Duration=4, StartBits=0, EndBits=0)
