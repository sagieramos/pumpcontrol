import './_dashboard.css';

const TYPE_IDS = {
    CUT_OFF_VOLT_ID: 0x04,
    VOLTAGE_TYPE_ID: 0x07,
};

let Num;
let updateChart;
let updateMinVoltCutOff;

const voltage = document.getElementById('voltage');
const loadingIndicators = document.querySelectorAll('.lds-ellipsis');
const initHideElements = document.querySelectorAll('.init-hide');
const minVolt = document.getElementById('min-volt');
const runRestTimer = document.getElementById('run-rest-timer');

const hideLoadingIndicators = () => {
    loadingIndicators.forEach((indicator) => {
        indicator.style.display = 'none';
    });

    initHideElements.forEach((element) => {
        element.style.display = 'block';
    });
};

const showLoadingIndicators = () => {
    loadingIndicators.forEach((indicator) => {
        indicator.style.display = 'block';
    });

    initHideElements.forEach((element) => {
        element.style.display = 'none';
    });
};

let ws;
let isConnected = false; // Track connection status

const deserializeData = async (bufferData) => {
    const buffer = bufferData;
    const typeIdentifier = buffer[0];

    if (typeIdentifier === TYPE_IDS.VOLTAGE_TYPE_ID || typeIdentifier === TYPE_IDS.CUT_OFF_VOLT_ID) {
        try {
            if (!Num || !updateChart || !updateMinVoltCutOff) {
                // Lazy load modules
                const [NumModule, { updateChart: updateChartFunc, updateMinVoltCutOff: updateMinVoltCutOffFunc }] = await Promise.all([
                    import('../protoc/js/str_num_msg.js'),
                    import('./chartDisplay.js')
                ]);

                Num = NumModule.Num;
                updateChart = updateChartFunc;
                updateMinVoltCutOff = updateMinVoltCutOffFunc;
                runRestTimer.style.display = 'block';
                document.getElementById('voltageChart').style.display = 'block';
            }

            const numValue = Num.decode(buffer.slice(1));
            console.log('numValue:', numValue);
            if (numValue.key === 1) {
                const cutoffVolt = numValue.value;
                updateMinVoltCutOff(cutoffVolt, true);
                minVolt.textContent = `${cutoffVolt}V`;
                runRestTimer.textContent = "00:00:00";
            } else if (numValue.key === 0) {
                const voltageValue = numValue.value;
                voltage.textContent = `${voltageValue}V`;
                updateChart(voltageValue);
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

const serializeData = (data, typeIdentifier, messageType) => {
    const dataBuffer = messageType.encode(data);
    const buffer = Buffer.alloc(1 + dataBuffer.length);
    buffer[0] = typeIdentifier;
    dataBuffer.copy(buffer, 1);
    return buffer;
}

const connectWebSocket = () => {
    ws = new WebSocket('ws://akowe.org/ws');

    ws.onopen = () => {
        console.log('Connected to WebSocket server');
        isConnected = true;
        // Hide the loading indicator
        hideLoadingIndicators();

        // Start sending a heartbeat message every 5 seconds
        setInterval(() => {
            if (ws.readyState === WebSocket.OPEN) {
                ws.send(''); // Send an empty message as a heartbeat
            }
        }, 5000);
    };

    ws.onmessage = async (event) => {
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
        isConnected = false;
        voltage.textContent = '—';
        console.log('Disconnected from WebSocket server');
        // Show the loading indicator
        showLoadingIndicators();
    };

    ws.onerror = error => {
        isConnected = false;
        voltage.textContent = '—';
        console.error('WebSocket error:', error);
        // Show the loading indicator
        showLoadingIndicators();
    };
};

// Check connection status every 5 seconds
const checkConnection = () => {
    setInterval(() => {
        if (!isConnected) {
            showLoadingIndicators();
        }
    }, 5000);
};

// Initial setup
setTimeout(() => {
    connectWebSocket();
    checkConnection();
}, 3000);
