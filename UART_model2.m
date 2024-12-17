function UART_model2(block)
    setup(block);
end

function setup(block)
    global length12;
    global length16;
    length12 = 100;
    length16 = 5;
    % Register number of ports
    block.NumInputPorts  = 1;
    block.NumOutputPorts = 3;
    % Setup port properties to be inherited or dynamic
    block.SetPreCompInpPortInfoToDynamic;
    block.SetPreCompOutPortInfoToDynamic;
    % Override input port properties
    block.InputPort(1).Dimensions        = 1;
    block.InputPort(1).DatatypeID  = 0;  % double
    block.InputPort(1).Complexity  = 'Real';
    block.InputPort(1).DirectFeedthrough = true;
    % Override output port properties
    block.OutputPort(1).DatatypeID = 0;  % uint32
    block.OutputPort(1).Dimensions = length12;
    block.OutputPort(1).Complexity  = 'Real';
    block.OutputPort(2).DatatypeID = 0;  % uint32
    block.OutputPort(2).Dimensions = length16;
    block.OutputPort(2).Complexity  = 'Real';
    block.OutputPort(3).DatatypeID = 0;  % uint32
    block.OutputPort(3).Dimensions = 1;
    block.OutputPort(3).Complexity  = 'Real';
    % Register parameters
    block.NumDialogPrms     = 0;
    % Register sample times
    block.SampleTimes = [-1 0];
    block.SimStateCompliance = 'DefaultSimState';
    block.RegBlockMethod('Start', @Start);
    block.RegBlockMethod('Outputs', @Outputs);     % Required
    block.RegBlockMethod('Update', @Update);
    block.RegBlockMethod('Terminate', @Terminate); % Required
end

function Start(block)
    global s;
    global count;
    global fifo12;
    global fifo16;

    fifo12 = [];
    fifo16 = [];

    serialPort = "COM4";  % Change to your COM port
    baudRate = 912600;
    s = serialport(serialPort, baudRate);
    bufferd = 0;
    prev_byte = 0;
end

function Outputs(block)
    global count;
    global s;
    global fifo12;
    global fifo16;
    global length12;
    global length16;

    % Output data from FIFO buffers
    if length(fifo12) >= length12
        block.OutputPort(1).Data = fifo12(1:length12);
        fifo12(1:length12) = [];
        if length(fifo12)>10000
            fifo12=[];
        end
    else
        %block.RegBlockMethod('Outputs', @Update); 
    end
    if length(fifo16) >= length16
        block.OutputPort(2).Data = fifo16(1:length16);
        fifo16(1:length16) = [];
        if length(fifo16)>10000
            fifo16=[];
        end
    end

    block.OutputPort(3).Data = length(fifo12);
end

function Update(block)
    global s;
    global fifo12;
    global fifo16;
    global bufferd;
    global prev_byte;
    global count;
    if s.NumBytesAvailable > 0
        data = read(s, s.NumBytesAvailable, 'uint8');
        count = 0;
        for i = 1:length(data)
            byte = data(i);
            % Get the first two bits of the byte to determine the type
            typeIndicator = bitshift(byte, -6); % Shift right by 6 to get the first two bits
            switch typeIndicator
                case 1 % '01xxxxxx' - 12-bit data LSB
                    bufferd = bitshift(bufferd, 6);         % Shift buffer 6 bits to the left
                    bufferd = bitor(bufferd, bitand(byte, 63)); % Store lower 6 bits
                    prev_byte = 1;
                case 0 % '00xxxxxx' - 16-bit data LSB
                    if prev_byte == 1
                        % Store 12-bit data in the buffer
                        fifo12 = [fifo12; bufferd];
                    elseif prev_byte == 2
                        % Store 16-bit data in the buffer
                        fifo16 = [fifo16; bufferd];
                    end
                    bufferd = bitand(byte, 63); % Store lower 6 bits for 16-bit data
                case 2 % '10xxxxxx' - 16-bit data MSB
                    bufferd = bitshift(bufferd, 6);         % Shift buffer 6 bits to the left
                    bufferd = bitor(bufferd, bitand(byte, 63)); % Store upper 6 bits
                    prev_byte = 2;
                otherwise
                    fprintf('Unknown type indicator: %d\n', typeIndicator);
            end
        end
    end
end

function Terminate(block)
    global s;
    delete(s);
end