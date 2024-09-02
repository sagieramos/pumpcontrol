import './_dashboard.css';
import './reset.css';
import {
    toggleElementVisibility, getHoursAndMinutes, getMilliseconds,
    handleVoltageChange, handleTimeRangeChange,
    handleModeChange,
    getModeString, setColorFromMode,
    updateVisibility,
    KEY_CONFIG,
    VOLT_RECEIVE_FROM_SERVER
} from './util.js';
import { Countdown } from './countDown.js';
import { ControlData, TimeRange } from '../protoc/js/pump_control_data.js';
import { Num } from '../protoc/js/str_num_msg.js';
import TimePicker from 'tui-time-picker';
import 'tui-time-picker/dist/tui-time-picker.css';
import notificationSound from './notify.mp3'

let updateChartFunction;
let updateMinVoltCutOffFunction;

const PowerStatus = {
    POWER_INACTIVE: 0,
    POWER_READY: 1,
    POWER_RUNNING: 2,
    POWER_RESTING: 3,
    POWER_VOLTAGE_LOW: 4,
};

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
    const modeConfig = document.getElementById('mode');
    const runRestState = document.getElementById('run-rest-state');
    const timer = document.getElementById('timer');
    const minVoltInput = document.getElementById('min-voltage-input');
    const slider = document.getElementById('min-voltage-slider');
    const modeBg = document.getElementById('mode-bg');

    let ws;
    let heartbeat;
    let reconnecting;
    let wsError = false;

    const HEARTBEAT_TIMEOUT_MS = 2000;

    const countdown = new Countdown('#run-rest-ing', 0);

    const timePickerObj = {
        initialHour: 0,
        initialMinute: 0,
        inputType: 'selectbox',
        showMeridiem: false,
    };

    countdown.update(0);

    const timeRangeBegin = { hour: 0, minute: 4 };
    const timeRangeEnd = { hour: 23, minute: 59 };
    const timeRangeEndRunning = { hour: 2, minute: 0 };

    const timePickerRunning = new TimePicker('#running-time', timePickerObj);
    timePickerRunning.setRange(timeRangeBegin, timeRangeEndRunning);

    const timePickerResting = new TimePicker('#resting-time', timePickerObj);
    timePickerResting.setRange(timeRangeBegin, timeRangeEnd);
    const loadModules = async () => {
        if (!updateChartFunction || !updateMinVoltCutOffFunction) {
            const [{ updateChart, updateMinVoltCutOff }] = await Promise.all([
                import('./chartDisplay.js')
            ]);

            updateChartFunction = updateChart;
            updateMinVoltCutOffFunction = updateMinVoltCutOff;

            document.getElementById('voltageChart').style.display = 'block';
            toggleElementVisibility(voltageChartElement, 'show');
        }
    }

    const valueHeading = document.getElementById('value-heading');
    let flagP = true;
    let minValueStorage = 0;

    const handleKeyAction = (key, value) => {
        switch (key) {
            case VOLT_RECEIVE_FROM_SERVER.VOLTAGE:
                voltageElement.textContent = value;
                updateChartFunction(value);
                const f = value < minValueStorage
                if (flagP !== f) {
                    valueHeading.style.color = f ? '#ff6384' : '#36a2eb';
                    flagP = f;
                }
                break;
            case VOLT_RECEIVE_FROM_SERVER.MIN_VOLTAGE:
            case KEY_CONFIG.MIN_VOLT:
                updateMinVoltCutOffFunction(value, true);
                minVoltElement.textContent = value;
                minVoltInput.value = value;
                slider.value = value;
                minValueStorage = value;
                break;
            case KEY_CONFIG.CONFIG_MODE:
                modeElement.textContent = getModeString(value);
                setColorFromMode(value, modeBg);
                modeConfig.value = value;
                break;
            default:
            //console.log(`Unexpected key: ${key}`);
        }
    }

    const handleSingleConfig = (buffer) => {
        try {
            const numValue = Num.decode(buffer.slice(1));
            //console.log('numValue:', numValue);
            const { key, value } = numValue;
            handleKeyAction(key, value);
        } catch (error) {
            console.error('Failed to deserialize Num:', error);
        }
    };

    const insertTimeRange = (timeRangeObj) => {
        const { running, resting } = timeRangeObj;
        const { hours: runningHours, minutes: runningMinutes } = getHoursAndMinutes(running);
        const { hours: restingHours, minutes: restingMinutes } = getHoursAndMinutes(resting);

        runTimeElement.textContent = `${runningHours}:${runningMinutes.toString().padStart(2, '0')}`;
        restTimeElement.textContent = `${restingHours}:${restingMinutes.toString().padStart(2, '0')}`;
        timePickerRunning.setTime(runningHours, runningMinutes, false);
        timePickerResting.setTime(restingHours, restingMinutes, false);
    }

    const handleTimeRange = (buffer) => {
        try {
            const timeRange = TimeRange.decode(buffer.slice(1));
            //console.log('timeRange:', timeRange);
            insertTimeRange(timeRange);
        } catch (error) {
            console.error('Failed to deserialize TimeRange:', error);
        }
    }

    /**
     * Hides loading indicators and shows initialized elements.
     */
    const hideLoadingIndicators = () => {
        updateVisibility(loadingIndicators, 'none');
        updateVisibility(initHideElements, 'block');
        updateVisibility(styledButton, 'block');
        pumpPowerIndicator.style.display = 'block';
    };

    /**
     * Shows loading indicators and hides initialized elements.
     */
    const showLoadingIndicators = () => {
        updateVisibility(loadingIndicators, 'block');
        updateVisibility(initHideElements, 'none');
        updateVisibility(styledButton, 'none');
        pumpPowerIndicator.style.display = 'none';
    };

    const reconnectWebSocket = () => {
        // Clear any existing reconnection interval
        if (reconnecting) {
            clearInterval(reconnecting);
        }

        if (wsError) {
            return;
        }

        reconnecting = setInterval(() => {
            if (ws.readyState === WebSocket.CLOSED) {
                //console.log('Attempting to reconnect WebSocket...');
                connectWebSocket();
            } else {
                //console.log('WebSocket is not closed. Clearing reconnection interval.');
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
            //console.log('Heartbeat timeout. Reconnecting WebSocket...');
            if (ws) {
                showLoadingIndicators();
                ws.close();
                reconnectWebSocket();
            }
        }, HEARTBEAT_TIMEOUT_MS);
    };

    const voidHandler = (buffer) => {
        const typeIdentifier = buffer[0];
        //console.log(`No handler for type identifier: ${typeIdentifier}`);
    };

    const handleControlData = (buffer) => {
        try {
            // Decode the buffer
            const controlData = ControlData.decode(buffer.slice(1));
            //console.log('controlData:', controlData);

            insertTimeRange(controlData.time_range);

            modeElement.textContent = getModeString(controlData.mode);
            setColorFromMode(controlData.mode, modeBg);
            modeConfig.value = controlData.mode;
        } catch (error) {
            console.error('Failed to deserialize ControlData:', error);
        }
    };

    const audio = new Audio(notificationSound);

    audio.addEventListener('canplaythrough', () => {
        //console.log('Audio is ready to play');
    }, { once: true });

    audio.addEventListener('error', (e) => {
        console.error('Audio playback error:', e);
    });

    const triggerVibration = () => {
        if ("vibrate" in navigator) {
            navigator.vibrate([200, 100, 200]);
        } else {
            //console.log("Vibration API is not supported.");
        }
    };

    const notifyUser = () => {
        audio.play().catch(error => console.error('Audio play error:', error));
        triggerVibration();
    };

    const handlePowerStatus = (buffer) => {
        const numValue = Num.decode(buffer.slice(1));
        const { key, value } = numValue;
        let valueToNumber = Number(value);

        let visibility = 'block';

        // Remove blinking class initially
        pumpPowerIndicator.classList.remove('blinking');

        switch (key) {
            case PowerStatus.POWER_INACTIVE:
                pumpPowerIndicator.style.backgroundColor = '#808080';
                runRestState.textContent = 'Inactive';
                visibility = 'none';
                valueToNumber = 1;
                break;
            case PowerStatus.POWER_READY:
                pumpPowerIndicator.classList.add('blinking'); // Add blinking class for POWER_READY
                runRestState.textContent = 'Ready...';
                break;
            case PowerStatus.POWER_RUNNING:
                pumpPowerIndicator.style.backgroundColor = '#32CD32'; 
                runRestState.textContent = 'Running...';
                break;
            case PowerStatus.POWER_RESTING:
                pumpPowerIndicator.style.backgroundColor = '#1E90FF';
                runRestState.textContent = 'Resting...';
                break;
            case PowerStatus.POWER_VOLTAGE_LOW:
                pumpPowerIndicator.style.backgroundColor = '#FF0000'; 
                runRestState.textContent = 'Voltage Low';
                visibility = 'none';
                valueToNumber = 1;
                break;
            default:
                valueToNumber = 1;
                visibility = 'none';
                runRestState.textContent = '...';
        }

        timer.style.display = visibility;
        countdown.update(valueToNumber);

        notifyUser();
    }


    const handlers = [
        voidHandler,    // Index 0
        handleSingleConfig,    // Index 1
        voidHandler,    // Index 2
        voidHandler,    // Index 3
        handleControlData,  // Index 4
        handleTimeRange,    // Index 5
        voidHandler,    // Index 6
        voidHandler,   // Index 7
        handlePowerStatus,  // Index 8
    ];

    /**
     * Deserializes data from the given buffer and updates UI elements.
     * @param {Uint8Array} bufferData - The data buffer to deserialize.
     */
    const deserializeData = async (bufferData) => {
        const typeIdentifier = bufferData[0];
        //console.log('Type identifier from deserialize:', typeIdentifier);

        await loadModules();

        if (handlers[typeIdentifier]) {
            handlers[typeIdentifier](bufferData);
        } else {
            //console.log(`No handler for type identifier: ${typeIdentifier}`);
        }
    };

    /**
     * Connects to the WebSocket server and sets up event handlers.
     */
    const connectWebSocket = () => {
        ws = new WebSocket('ws://akowe.org/ws');

        ws.onopen = () => {
            //console.log('Connected to WebSocket server');
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
            //console.log('Disconnected from WebSocket server');
            showLoadingIndicators();
            reconnectWebSocket();

        };

        ws.onerror = error => {
            voltageElement.textContent = '00.0';
            console.error('WebSocket error:', error);
            showLoadingIndicators();
            wsError = true;
        };
    };

    const configGroup = document.getElementById('config-group');
    const config = configGroup.querySelectorAll('.config');
    const openConfigButtons = Array.from(document.querySelectorAll('.open-config'));

    document.addEventListener('click', (event) => {
        const { target } = event;

        if (target.matches('#submit-btn-min-volt')) {
            const voltage = parseInt(minVoltInput.value, 10);

            if (isNaN(voltage) || voltage < 110 || voltage > 230) {
                return;
            } else {
                //console.log('Minimum operating voltage set to:', voltage);
                handleVoltageChange(voltage, ws);
            }

            configGroup.style.display = 'none';
        } else if (target.matches('#submit-time-range-btn')) {
            const runningSecs = getMilliseconds({
                hours: timePickerRunning.getHour(),
                minutes: timePickerRunning.getMinute()
            });
            const restingSecs = getMilliseconds({
                hours: timePickerResting.getHour(),
                minutes: timePickerResting.getMinute()
            });

            handleTimeRangeChange(runningSecs, restingSecs, ws);
            configGroup.style.display = 'none';
        }
        else if (target.matches('.open-config')) {
            const index = openConfigButtons.indexOf(target);
            if (index === -1) return;
            configGroup.style.display = 'flex';
            updateVisibility(config, 'none');
            config[index].style.display = 'flex';
        }
        else if (target.matches('#cancel-config-ui')) {
            configGroup.style.display = 'none';
        } else if (target.matches('#config-group')) {
            target.style.display = 'none';
        } else if (target.matches('#logout')) {
            window.location.href = '/logout';
        }
    });

    document.getElementById('voltage-setting').addEventListener('input', (event) => {
        const { value, id } = event.target;
        const sliderValue = parseInt(value, 10);
        const voltageLabel = document.getElementById('voltage-label');

        // Validate the value range
        if (sliderValue < 110 || sliderValue > 230) {
            voltageLabel.innerHTML = 'Set Minimum Operating Voltage (110-230 V) <br> <span id="error">— Please enter a valid value!</span>';
        } else {
            voltageLabel.innerHTML = 'Set Minimum Operating Voltage (110-230 V)';
            // Synchronize the values between the input field and the slider
            if (id === 'min-voltage-input') {
                slider.value = sliderValue;
            } else if (id === 'min-voltage-slider') {
                minVoltInput.value = sliderValue;
            }
        }
    });
    modeConfig.addEventListener('change', (event) => {
        if (ws.readyState === WebSocket.OPEN) {
            handleModeChange(event.target.value, ws);
        } else {
            console.error('WebSocket is not open');
        }

        configGroup.style.display = 'none';
    });

    // Initial setup
    setTimeout(() => {
        connectWebSocket();
    }, 1000);

});

