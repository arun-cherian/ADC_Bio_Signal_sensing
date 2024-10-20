
% Define global variables
global i;
global newX;
global newY;
global thePlot;

i = 0;  % Initialize counter
newX = [];  % Initialize global X data array
newY = [];  % Initialize global Y data array

% Define the serial port and baud rate
serialPort = "COM12"; % Change to your COM port
baudRate = 912600;

% Open serial port connection
s = serialport(serialPort, baudRate);

% Set the callback to trigger when at least 1 byte is available
try
   configureCallback(s, "byte", 1, @readUART);
    disp('Callback configured successfully. Waiting for incoming UART data...');
catch err
    disp('Error configuring callback:');
    disp(err.message);
    return;
end

% Create a figure and initial plot with initial X and Y data
figure;
x = 0:pi/100:2*pi; % Initial X values
y = sin(x);        % Initial Y values
thePlot = plot(x, y);  % Create the plot with initial data
xlabel('X-axis');
ylabel('Y-axis');
title('Real-Time Data Plot');

% Start the timer to update the plot in real time
plotTimer = timer('ExecutionMode', 'fixedRate', 'Period', 0.01, 'TimerFcn', @plotFromStack);
start(plotTimer);

disp('Waiting for incoming UART data...');

% Callback function to read and store UART data
function readUART(s, ~)
    global newX;
    global newY;
    persistent bufferd;     % Buffer for incoming data
    persistent prev_byte;   % Previous byte type

    % Initialize persistent variables if they are empty
    if isempty(bufferd)
        bufferd = 0;
    end
    if isempty(prev_byte)
        prev_byte = 0;
    end

    % Read all available bytes
    if s.NumBytesAvailable > 0
        data = read(s, s.NumBytesAvailable, 'uint8');
    else
        return; % Exit if no data is available
    end

    % Process each byte
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
                   fprintf('Received 12-bit data: %d\n', bufferd);
                   % Append the new data to global arrays
                   newX = [newX, newX(end) + 1];  % Add new X point
                   newY = [newY, bufferd];        % Add corresponding Y point
                elseif prev_byte == 2
                   % Do something with 16-bit data if needed
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

% Function to plot data from the stack asynchronously
function plotFromStack(~, ~)
    global i;
    global thePlot;
    global newX;
    global newY;

    i = i + 1;  % Increment counter
    
    % Check if newX is empty, initialize if needed
    if isempty(newX)
        newX = 0:pi/100:2*pi;
        newY = sin(newX);
    end

    % Limit the length of the plot data
    maxDataLength = 1000;  % Set the threshold for data length
    if length(newX) > maxDataLength
        newX = newX(end-maxDataLength+1:end);  % Keep only the last 1000 points
        newY = newY(end-maxDataLength+1:end);  % Keep corresponding Y points
    end

    % Update the plot with the new data
    set(thePlot, 'XData', newX, 'YData', newY);
    
    % Print counter for visualization
    fprintf('data: %d\n', i);
end






