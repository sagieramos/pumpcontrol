import './_dashboard.css';
import './reset.css';
import { toggleElementVisibility, millisecondsToTime } from './util.js';
import { Countdown } from './countDown.js';

const TYPE_IDS = {
    CONTROL_DATA_TYPE_ID: 0x04,
    VOLTAGE_TYPE_ID: 0x07,
};

let NumModule;
let ControlData;
let TimeRange;
let MachineMode;
let updateChartFunction;
let updateMinVoltCutOffFunction;

const voltageElement = document.getElementById('voltage');
const voltageChartElement = document.getElementById('voltageChart');
const loadingIndicators = document.querySelectorAll('.lds-ellipsis');
const initHideElements = document.querySelectorAll('.init-hide');
const minVoltElement = document.getElementById('min-volt');
const pumpPowerIndicator = document.getElementById('pump-power-indicator');
const runTimeElement = document.getElementById('run-time');
const restTimeElement = document.getElementById('rest-time');
const modeElement = document.getElementById('machine-mode');
const styledButton = document.querySelectorAll('.styled-button')

let ws;
let heartbeat;
let reconnecting;

const HEARTBEAT_TIMEOUT_MS = 3000;

const countdown = new Countdown('#run-rest-ing', 0);

setTimeout(() => {
    countdown.update(140);
}, 3000);

/**
 * Hides loading indicators and shows initialized elements.
 */
const hideLoadingIndicators = () => {
    loadingIndicators.forEach((indicator) => {
        indicator.style.display = 'none';
    });

    initHideElements.forEach((element) => {
        element.style.display = 'block';
    });

    styledButton.forEach((button) => {
        button.style.display = 'block';
    });

    pumpPowerIndicator.style.display = 'block';
};

/**
 * Shows loading indicators and hides initialized elements.
 */
const showLoadingIndicators = () => {
    loadingIndicators.forEach((indicator) => {
        indicator.style.display = 'block';
    });

    initHideElements.forEach((element) => {
        element.style.display = 'none';
    });

    styledButton.forEach((button) => {
        button.style.display = 'none';
    });

    pumpPowerIndicator.style.display = 'none';
};

const reconnectWebSocket = () => {
    // Clear any existing reconnection interval
    if (reconnecting) {
        clearInterval(reconnecting);
    }

    reconnecting = setInterval(() => {
        if (ws.readyState === WebSocket.CLOSED) {
            console.log('Attempting to reconnect WebSocket...');
            connectWebSocket();
        } else {
            console.log('WebSocket is not closed. Clearing reconnection interval.');
            clearInterval(reconnecting);
        }
    }, 3000);
}

/**
 * Resets the heartbeat timer.
 */
const resetHeartbeat = () => {
    if (heartbeat) {
        clearTimeout(heartbeat);
    }

    heartbeat = setTimeout(() => {
        console.log('Heartbeat timeout. Reconnecting WebSocket...');
        if (ws) {
            showLoadingIndicators();
            ws.close();
            reconnectWebSocket();
        }
    }, HEARTBEAT_TIMEOUT_MS);
};

const handleControlData = async (buffer) => {
    try {
        // Check if the necessary modules are loaded
        if (!ControlData || !TimeRange || !MachineMode) {
            // Lazy load the module
            const ControlDataImport = await import('../protoc/js/pump_control_data.js');

            // Assign the imported module exports
            ControlData = ControlDataImport.ControlData;
            TimeRange = ControlDataImport.TimeRange;
            MachineMode = ControlDataImport.MachineMode;

            if (!ControlData || !TimeRange || !MachineMode) {
                throw new Error('Failed to load ControlData, TimeRange, or MachineMode from the module.');
            }
        }

        // Decode the buffer
        const controlData = ControlData.decode(buffer.slice(1));
        console.log('controlData:', controlData);

        // Destructure the controlData object
        const { mode, is_running, time_range } = controlData;
        const { running, resting } = time_range;

        // Update the UI elements
        runTimeElement.textContent = millisecondsToTime(running);
        restTimeElement.textContent = millisecondsToTime(resting);

        // Update the countdown and indicator based on running state
        if (is_running) {
            countdown.update(running / 1000);
            pumpPowerIndicator.style.backgroundColor = 'green';
        } else {
            countdown.update(resting / 1000);
            pumpPowerIndicator.style.backgroundColor = 'red';
        }

        // Update mode element based on mode
        switch (mode) {
            case MachineMode.POWER_OFF:
                modeElement.textContent = 'Power Off';
                break;
            case MachineMode.POWER_ON:
                modeElement.textContent = 'Power On';
                break;
            case MachineMode.AUTO:
                modeElement.textContent = 'Automate';
                break;
            default:
                console.warn('Unexpected mode:', mode);
                modeElement.textContent = 'Unknown Mode';
                break;
        }
    } catch (error) {
        console.error('Failed to deserialize ControlData:', error);
    }
};

/**
 * Deserializes data from the given buffer and updates UI elements.
 * @param {Uint8Array} bufferData - The data buffer to deserialize.
 */
const deserializeData = async (bufferData) => {
    const buffer = bufferData;
    const typeIdentifier = buffer[0];

    if (typeIdentifier === TYPE_IDS.VOLTAGE_TYPE_ID) {
        try {
            if (!NumModule || !updateChartFunction || !updateMinVoltCutOffFunction) {
                // Lazy load modules
                const [NumModuleImport, { updateChart, updateMinVoltCutOff }] = await Promise.all([
                    import('../protoc/js/str_num_msg.js'),
                    import('./chartDisplay.js')
                ]);

                NumModule = NumModuleImport.Num;
                updateChartFunction = updateChart;
                updateMinVoltCutOffFunction = updateMinVoltCutOff;
                document.getElementById('voltageChart').style.display = 'block';
                toggleElementVisibility(voltageChartElement, 'show');
            }

            const numValue = NumModule.decode(buffer.slice(1));
            console.log('numValue:', numValue);
            if (numValue.key === 1) {
                const cutoffVolt = numValue.value;
                updateMinVoltCutOffFunction(cutoffVolt, true);
                minVoltElement.textContent = cutoffVolt;
            } else if (numValue.key === 0) {
                const voltageValue = numValue.value;
                voltageElement.textContent = voltageValue;
                updateChartFunction(voltageValue);
            } else {
                console.log(`Unexpected key: ${numValue.key}`);
            }
        } catch (error) {
            console.error('Failed to deserialize Num:', error);
        }
    } else if (typeIdentifier === TYPE_IDS.CONTROL_DATA_TYPE_ID) {
        handleControlData(buffer);
    }
    else {
        console.log(`Unexpected type identifier: ${typeIdentifier}`);
    }
};

/**
 * Connects to the WebSocket server and sets up event handlers.
 */
const connectWebSocket = () => {
    ws = new WebSocket('ws://akowe.org/ws');

    ws.onopen = () => {
        console.log('Connected to WebSocket server');
        hideLoadingIndicators();
        resetHeartbeat();
    };

    ws.onmessage = async (event) => {
        resetHeartbeat();
        try {
            // Convert Blob to ArrayBuffer
            const arrayBuffer = await event.data.arrayBuffer();
            // Convert ArrayBuffer to Uint8Array
            const buffer = new Uint8Array(arrayBuffer);
            deserializeData(buffer);
        } catch (error) {
            console.error('Error handling WebSocket message:', error);
        }
    };

    ws.onclose = () => {
        voltageElement.textContent = '00.0';
        console.log('Disconnected from WebSocket server');
        showLoadingIndicators();
        reconnectWebSocket();

    };

    ws.onerror = error => {
        voltageElement.textContent = '00.0';
        console.error('WebSocket error:', error);
        showLoadingIndicators();
    };
};

// Initial setup
setTimeout(() => {
    connectWebSocket();
}, 3000);
