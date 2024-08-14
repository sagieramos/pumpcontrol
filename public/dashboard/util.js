import { MachineMode } from '../protoc/js/pump_control_data.js';
import { Num } from '../protoc/js/str_num_msg.js';

/**
 * Toggles the visibility of a DOM element with a transition effect.
 *
 * This function adds or removes visibility classes from the provided DOM element
 * based on the specified action. The `show` action will make the element visible, 
 * and the `hide` action will make the element hidden.
 *
 * @param {HTMLElement} element - The DOM element whose visibility should be toggled.
 * @param {string} action - The action to perform. Can be either 'show' or 'hide'.
 *                            - 'show': Makes the element visible by adding the 'visible' class and removing the 'hidden' class.
 *                            - 'hide': Hides the element by adding the 'hidden' class and removing the 'visible' class.
 * 
 * @throws {Error} Throws an error if `action` is not 'show' or 'hide'.
 * @throws {Error} Throws an error if `element` is not provided or is invalid.
 *
 * @example
 * // Assuming you have an element with ID 'voltageChart' and appropriate CSS classes.
 * const voltageChart = document.getElementById('voltageChart');
 * 
 * // Show the element
 * toggleElementVisibility(voltageChart, 'show');
 *
 * // Hide the element
 * toggleElementVisibility(voltageChart, 'hide');
 */
const toggleElementVisibility = (element, action) => {
    if (element) {
        if (action === 'show') {
            element.classList.remove('hidden');
            element.classList.add('visible');
        } else if (action === 'hide') {
            element.classList.remove('visible');
            element.classList.add('hidden');
        } else {
            console.error('Invalid action. Use "show" or "hide".');
        }
    } else {
        console.error('Element is not provided or is invalid.');
    }
}

/**
 * Serializes data into a buffer with a type identifier and sends it over a WebSocket connection.
 * @param {Object} data - The data to serialize.
 * @param {number} typeIdentifier - The type identifier for the data.
 * @param {Object} messageType - The message type for encoding the data.
 * @param {WebSocket} ws - The WebSocket instance to send the data through.
 * @throws {Error} If serialization fails or WebSocket is not open.
 */
const serializeAndSendData = (data, typeIdentifier, messageType) => {
    const dataBuffer = messageType.encode(data);
    const buffer = new ArrayBuffer(1 + dataBuffer.length);
    const uint8View = new Uint8Array(buffer);
    uint8View[0] = typeIdentifier;
    uint8View.set(new Uint8Array(dataBuffer), 1);

    return uint8View;
};

const KEY_CONFIG = {
    MIN_VOLT: 1,
    CONFIG_MODE: 101,
    CONFIG_RUNNING_TIME: 102,
    CONFIG_RESTING_TIME: 103,
};

const TYPE_IDS = {
    NUM_TYPE_ID: 0x01,
    CONTROL_DATA_TYPE_ID: 0x04,
    PUMP_TIME_RANGE_TYPE_ID: 0x05,
    VOLTAGE_TYPE_ID: 0x07,
};
const { POWER_OFF, POWER_ON, AUTO } = MachineMode;

const modeMapping = {
    [POWER_OFF]: "POWER OFF",
    [POWER_ON]: "POWER ON",
    [AUTO]: "AUTOMATE"
};
/**
 * Returns the mode string based on the integer value.
 * @param {number} intValue - The integer value of the mode.
 * @returns {string} - The mode string.
 */

const getModeString = (intValue) => {
    return modeMapping[intValue] || "Unknown mode";
};

/**
 * Handles the change in machine mode and sends the corresponding data.
 * @param {string} newMode - The new machine mode to set.
 */
const handleModeChange = (newMode, ws) => {
    const modeValue = parseInt(newMode, 10);

    console.log('Mode value:', modeValue);
    if (modeValue < 0 || modeValue > 2) {
        console.error('Invalid mode:', newMode);
        return;
    }
    const num = { key: KEY_CONFIG.CONFIG_MODE, value: modeValue };

    console.log('Sending mode data:', num);

    try {
        const buffer = serializeAndSendData(num, TYPE_IDS.NUM_TYPE_ID, Num);
        ws.send(buffer);
    } catch (error) {
        console.error('Failed to serialize and send data:', error);
    }
};

const handleVoltageChange = (newVoltage, ws) => {
    const num = { key: KEY_CONFIG.MIN_VOLT, value: newVoltage };

    try {
        const buffer = serializeAndSendData(num, TYPE_IDS.VOLTAGE_TYPE_ID, Num);
        ws.send(buffer);
    } catch (error) {
        console.error('Failed to serialize and send data:', error);
    }
}

/**
 * Convert milliseconds to a formatted string of hours and minutes (H:MM).
 * @param {number} totalMilliseconds - The total number of milliseconds to convert.
 * @returns {string} - The formatted time string in H:MM format.
 */

const millisecondsToTime = (totalMilliseconds) => {
    const totalSeconds = totalMilliseconds / 1000;
    const hours = Math.floor(totalSeconds / 3600);
    const minutes = Math.floor((totalSeconds % 3600) / 60);
    /* const seconds = Math.floor(totalSeconds % 60); */
    return `${hours}:${minutes.toString().padStart(2, '0')}`;
}

export {
    handleVoltageChange,
    handleModeChange,
    toggleElementVisibility,
    millisecondsToTime, getModeString, KEY_CONFIG,
    TYPE_IDS
};
