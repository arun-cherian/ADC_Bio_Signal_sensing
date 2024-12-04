
function UART_model2(block)
    setup(block);
end

function setup(block)
    % Register number of ports
    block.NumInputPorts  = 1;
    block.NumOutputPorts = 2;
    % Setup port properties to be inherited or dynamic
    block.SetPreCompInpPortInfoToDynamic;
    block.SetPreCompOutPortInfoToDynamic;
    % Override input port properties
    block.InputPort(1).Dimensions        = 1;
    block.InputPort(1).DatatypeID  = 0;  % double
    block.InputPort(1).Complexity  = 'Real';
    block.InputPort(1).DirectFeedthrough = true;
    % Override output port properties
    block.OutputPort(1).Dimensions       = 1;
    block.OutputPort(1).DatatypeID  = 0; % double
    block.OutputPort(1).Complexity  = 'Real';
    % Register parameters
    block.NumDialogPrms     = 0;
    % Register sample times
    block.SampleTimes = [0 0];
    block.SimStateCompliance = 'DefaultSimState';

    block.RegBlockMethod('Start', @Start);
    block.RegBlockMethod('Outputs', @Outputs);     % Required
    block.RegBlockMethod('Update', @Update);
    block.RegBlockMethod('Terminate', @Terminate); % Required
end

function Start(block)
     global s;
    
    global new12;
    global new16;
    
    global head12;
    global tail12;
    global head16;
    global tail16;
    
    global buffer_size12;
    global buffer_size16;
    
    global bufferd;
    global prev_byte;
    serialPort = "COM3";  % Change to your COM port
    baudRate = 912600;
    s = serialport(serialPort, baudRate);
    buffer_size16 = 5000; % Adjust buffer size as needed
    buffer_size12 = 10000;
    new12 = zeros(1, buffer_size12);
    new16 = zeros(1, buffer_size16);
    head12 = 1;
    tail12 = 1;
    head16 = 1;
    tail16 = 1;
    bufferd = 0;
    prev_byte = 0;
end

function Outputs(block)
    % ... (Rest of your Outputs function)
end

function Update(block)
     global s;
    
    global new12;
    global new16;
    
    global head12;
    global tail12;
    global head16;
    global tail16;
    
    global buffer_size12;
    global buffer_size16;
    
    global bufferd;
    global prev_byte;

    if s.NumBytesAvailable > 0
        data = read(s, s.NumBytesAvailable, 'uint8');

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
                        new12(head12) = bufferd;
                        head12 = mod(head12, buffer_size12) + 1;
                        if head12 == tail12
                            tail12 = mod(tail12, buffer_size12) + 1; % Overwrite oldest data
                        end
                    elseif prev_byte == 2
                        % Store 16-bit data in the buffer
                        new16(head16) = bufferd;
                        head16 = mod(head16, buffer_size16) + 1;
                        if head16 == tail16
                            tail16 = mod(tail16, buffer_size16) + 1; % Overwrite oldest data
                        end
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
    
    global new12;
    global new16;
    
    global head12;
    global tail12;
    global head16;
    global tail16;
    
    global buffer_size12;
    global buffer_size16;
    
    fprintf("12-bit data received:\n");
    for i = tail12:buffer_size12
        if i == head12
            break;
        end
        fprintf('%d ', new12(i));
    end
    fprintf('\n');

    fprintf("16-bit data received:\n");
    for i = tail16:buffer_size16
        if i == head16
            break;
        end
        fprintf('%d ', new16(i));
    end
    fprintf('\n');

    delete(s);
end