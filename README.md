# ADC_Bio_Signal_sensing
This repo contains all the code and details about a project which aims to fuse ADC values together to get better temporal and data resolution
Hardware used:
1.MSP430FR2355
2.NU7802 24 bit ADC
3.USB to UART converter
4.Cyclone 2 FPGA
5.Biopotential amplifier 
6.Electrodes
7.Sound Card of PC(audio jack)


Challenege:
1. Getting the data from ADC to pc(implemeted using MSP430FR2355)
  1.1.Very High baud rate needed(921600 baud/s) for handling a sampling rate of 200Ksps
  1.2.UART communication is error prone
  1.3.Needs optimised code possibly using a DMA(might not be needed)
Solutions :
 1.1. Bumped up the MSP430 clock to 24MHz
 1.2.Used an external USB to UART converter
 1.3. Carefully debugged the UART signals by connecting to an FPGA
   1.3.1. Used a verilog HDL file and signal tap in Quartus to sample the signal and determine accurate baud rate divider and modulation bits

 Challenge:
 2. How to differeciate between NU7802 data and MSP data since they are both transmitted through same media(UART)
    solution:
    2.1 sent the NU7802 data as a packet in between the normal UART data like .json
    2.2 It will look something like this{starter_byte,data2,data1,data0,dealy_num,stopper_byte}
    2.3 you  might be wondering what is delay_num, it is the number of UART data that was transmitted after the ADC gave the data ready signal counter through software
    2.4 Data send in packets of 8 bit each; first 6 LSB sends data; first two MSP sends serial number of data ; Matlab code needs to be implemented for proper sorting based on serial number

  
  Challenge:
  3.  ADC is triggering UART faster than it can send the data 
    3.1 A compromise has to be made as there is no way to send data any faster though UART
    3.2 ADC updates the values while UART is sending sometimes but data reliabilty is good 
    3.3 ADC is stopped while sending NU7802 data since data could be corrupted by the very fast ADC data



    //////////////// MSP code has been finalized and uploaded on 14.10.2024 ///////////////////////////////////////////////////////
    Did some minor changes like adding DAC functionality after codefreeze

Challenge:1. Matlab identifying the data as 12 bit or 16 bit
          solutions
          1. Since the data is coming as 2 byte or 3 byte format and each byte has a two bit identifyed in beginning
          2. Matlab code has case statements for each of the identifier and uppends the data into a vaible each time a new data arrives
          3. If the uppending happes 3 times it is identified as 16 bit data
          
Challenge :2  Data coming from the controller is so large that MATLAB is not able to process it live
          solutions
          1. MATLAB buffers the data into an array or a file(file does not work since processing is even slower)
          2. Have to try out optimizing the code or use some harware acceleration
          3. Simulink is the way to go. created a level 2 s function so that I can do all the matlab code stuff in that block
          4. Data rate is still a challenge have to implement to check
          5. Frame based sampling of the scope successfully demostrated the ability to run in real time with sifficient speed

Challenge :3.1 Reliability problem(Actually discovered after challenge 4)
            1. The data that is sampled and sent to the pc seems to have a lot of unwanted artifacts and significant noise
            2. Main reason behind this(which I believe) is improper grouping of data resulting in repetition during simulation
            3. How do I know that the data that results from the algorithm is better than the original?
            Solutions
            1. The level 2 S function should be better optimized, inspiration can be drawn from the dsp.SineWave function of dsp library
            2.  MAIN FINDING- Audio jack of the laptop (sound card to be precise) has the capability to sample at 16 bit depth and 44kHz frequency
            3. This audio jack is a good way to actually check the accuracy of the data after combining the two data
          3.2 unreliable hardware
          1.CUrrent harware for collecting and amplifying bio signals is unreliable
          2. create custom differencial ampplifier(instrument amplifier)
          3. Experimenting with differnt op amps(so far 741 and 324 tried)
          
Challege :4 The final challenge of combining the two data from both microcontrollers
          3.1 Are there some variation in the way the two ADCs responf to voltage variation, if so what is the relation?
          3.2 How to make sense of the EMG data?
              1. lot of artifacts and noise- the laptop charger plug being removed makes a remarkable difference
              2. Bandpass filtering is an option/ kalman filtering/ some kind of filter to reduce noise
              3. Does FFT make any difference? If so what should be the sample for FFT? Will it defeat the real time nature of the project?
              . A lot of unanswered questions, dont know where to start. gemini or chatgpt not of much help
          Solutions:
          1. Simulation was made real time using Real time desktop library of matlab
          2. Filtering began working after this change along with step time adjustment with presice calculation of the data throughput(about 11k samples per second for 12bit data)
          3. The harware seems to not output any meaningful EMG data in 60Hz-300Hz-have to try with differnt components;Hardware high pass filter for 50 Hz may not be working
          
    
   
