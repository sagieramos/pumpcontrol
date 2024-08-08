import './_dashboard.css';
import { toggleElementVisibility } from './util.js';

const TYPE_IDS = {
    CUT_OFF_VOLT_ID: 0x04,
    VOLTAGE_TYPE_ID: 0x07,
};

let NumModule;
let updateChartFunction;
let updateMinVoltCutOffFunction;

const voltageElement = document.getElementById('voltage');
const voltageChartElement = document.getElementById('voltageChart');
const loadingIndicators = document.querySelectorAll('.lds-ellipsis');
const initHideElements = document.querySelectorAll('.init-hide');
const minVoltElement = document.getElementById('min-volt');
const runRestTimerElement = document.getElementById('run-rest-timer');
const pumpPowerIndicator = document.getElementById('pump-power-indicator');

let ws;
let heartbeat;

const HEARTBEAT_TIMEOUT_MS = 3000;

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
        }
    }, HEARTBEAT_TIMEOUT_MS);
};

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

    toggleElementVisibility(voltageChartElement, 'hide');

    pumpPowerIndicator.style.display = 'none';
};

/**
 * Deserializes data from the given buffer and updates UI elements.
 * @param {Uint8Array} bufferData - The data buffer to deserialize.
 */
const deserializeData = async (bufferData) => {
    const buffer = bufferData;
    const typeIdentifier = buffer[0];

    if (typeIdentifier === TYPE_IDS.VOLTAGE_TYPE_ID || typeIdentifier === TYPE_IDS.CUT_OFF_VOLT_ID) {
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
                runRestTimerElement.style.display = 'block';
                document.getElementById('voltageChart').style.display = 'block';
                toggleElementVisibility(voltageChartElement, 'show');
            }

            const numValue = NumModule.decode(buffer.slice(1));
            console.log('numValue:', numValue);
            if (numValue.key === 1) {
                const cutoffVolt = numValue.value;
                updateMinVoltCutOffFunction(cutoffVolt, true);
                minVoltElement.textContent = `${cutoffVolt}V`;
                runRestTimerElement.textContent = "00:00:00";
            } else if (numValue.key === 0) {
                const voltageValue = numValue.value;
                voltageElement.textContent = `${voltageValue}V`;
                updateChartFunction(voltageValue);
            } else {
                console.log(`Unexpected key: ${numValue.key}`);
            }
        } catch (error) {
            console.error('Failed to deserialize Num:', error);
        }
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
        voltageElement.textContent = '—';
        console.log('Disconnected from WebSocket server');
        showLoadingIndicators();
    };

    ws.onerror = error => {
        voltageElement.textContent = '—';
        console.error('WebSocket error:', error);
        showLoadingIndicators();
    };
};

// Initial setup
setTimeout(() => {
    connectWebSocket();
}, 3000);
