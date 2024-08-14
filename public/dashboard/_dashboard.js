import './_dashboard.css';
import './reset.css';
import {
    toggleElementVisibility, millisecondsToTime,
    handleVoltageChange,
    handleModeChange, 
    getModeString,
    TYPE_IDS,
    KEY_CONFIG
} from './util.js';
import { Countdown } from './countDown.js';
import { ControlData, TimeRange } from '../protoc/js/pump_control_data.js';

let NumModule;
let updateChartFunction;
let updateMinVoltCutOffFunction;
document.addEventListener('DOMContentLoaded', () => {

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
    const minVoltInput = document.getElementById('min-voltage');
    const modeConfig = document.getElementById('mode');
    

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

    const handleControlData = (buffer) => {
        try {
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

            modeElement.textContent = getModeString(mode);
            modeConfig.value = mode;
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

        if (typeIdentifier === TYPE_IDS.VOLTAGE_TYPE_ID || typeIdentifier === TYPE_IDS.NUM_TYPE_ID) {
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
                const { key, value } = numValue;
                if (key === 1) {
                    const cutoffVolt = value;
                    updateMinVoltCutOffFunction(cutoffVolt, true);
                    minVoltElement.textContent = cutoffVolt;
                    minVoltInput.value = cutoffVolt;
                } else if (key === 0) {
                    const voltageValue = value;
                    voltageElement.textContent = voltageValue;
                    updateChartFunction(voltageValue);
                } else if (key === KEY_CONFIG.CONFIG_MODE) {
                    modeElement.textContent = getModeString(value);
                    modeConfig.value = value;
                } else {
                    console.log(`Unexpected key: ${numValue.key}`);
                }
            } catch (error) {
                console.error('Failed to deserialize Num:', error);
            }
        } else if (typeIdentifier === TYPE_IDS.CONTROL_DATA_TYPE_ID) {
            handleControlData(buffer);
        } else {
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

    const minVoltageInput = document.getElementById('min-voltage');
    const errorMessage = document.getElementById('error-message');
    const submitBtn = document.getElementById('submit-btn');

    submitBtn.addEventListener('click', () => {
        const voltage = parseInt(minVoltageInput.value, 10);

        if (isNaN(voltage) || voltage < 110 || voltage > 230) {
            errorMessage.style.display = 'block';
        } else {
            errorMessage.style.display = 'none';
            console.log('Minimum operating voltage set to:', voltage);
            handleVoltageChange(voltage, ws);
            errorMessage.style.display = 'none';
        }
    });

    modeConfig.addEventListener('change', (event) => {
        if (ws.readyState === WebSocket.OPEN) {
            handleModeChange(event.target.value, ws);
        } else {
            console.error('WebSocket is not open');
        }
    });

    // Initial setup
    setTimeout(() => {
        connectWebSocket();
    }, 1000);

});

