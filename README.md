# ADC_Bio_Signal_sensing
This repo contains all the code and details about a project which aims to fuse ADC values together to get better temporal and data resolution
Hardware used:
1.MSP430FR2355
2.NU7802 24 bit ADC
3.USB to UART converter
4.Cyclone 2 FPGA
5.Biopotential amplifier 
6.Electrodes


Challenege:
1. Getting the data from ADC to pc(implemeted using MSP430FR2355)
  1.1.Very High baud rate needed(921600 baud/s) for handling a sampling rate of 200Ksps
  1.2.UART communication is error prone
  1.3.Needs optimised code possibly using a DMA(might neot be needed)
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

  
    
    
   
