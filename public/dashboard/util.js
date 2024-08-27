import { MachineMode, TimeRange } from '../protoc/js/pump_control_data.js';
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
const toggleElementVisibility = (element, action = 'toggle') => {
    if (!element || !(element instanceof Element)) {
        console.error('Invalid element provided.');
        return;
    }

    switch (action) {
        case 'show':
            element.classList.remove('hidden');
            element.classList.add('visible');
            break;
        case 'hide':
            element.classList.remove('visible');
            element.classList.add('hidden');
            break;
        case 'toggle':
            if (element.classList.contains('hidden')) {
                element.classList.remove('hidden');
                element.classList.add('visible');
            } else {
                element.classList.remove('visible');
                element.classList.add('hidden');
            }
            break;
        default:
            console.error('Invalid action. Use "show", "hide", or "toggle".');
            break;
    }
};

/**
 * Adds a type identifier to a buffer and returns a new buffer.
 * @param {number} typeId - The type identifier to add.
 * @param {ArrayBuffer} dataBuffer - The existing data buffer to prepend the type identifier to.
 * @returns {ArrayBuffer} - The new buffer with the type identifier added.
 */
const addTypeIdToBuffer = (typeId, dataBuffer) => {
    const buffer = new ArrayBuffer(1 + dataBuffer.byteLength);
    const uint8View = new Uint8Array(buffer);
    uint8View[0] = typeId;
    uint8View.set(new Uint8Array(dataBuffer), 1);

    return buffer;
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
    const buffer = addTypeIdToBuffer(typeIdentifier, dataBuffer);

    return buffer;
};

const VOLT_RECEIVE_FROM_SERVER = {
    VOLTAGE: 0,
    MIN_VOLTAGE: 1,
    MAX_VOLTAGE: 2
}

const KEY_CONFIG = {
    CONFIG_MODE: 101,
    CONFIG_RUNNING_TIME: 102,
    CONFIG_RESTING_TIME: 103,
    MIN_VOLT: 105,
};

const TYPE_IDS = {
    NUM_TYPE_ID: 0x01,
    CONTROL_DATA_TYPE_ID: 0x04,
    PUMP_TIME_RANGE_TYPE_ID: 0x05,
    POWER_TYPE_ID: 0x08, //receive only
    POWER_STATUS_ID: 0x09, //receive only
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
        const buffer = serializeAndSendData(num, TYPE_IDS.NUM_TYPE_ID, Num);
        ws.send(buffer);
    } catch (error) {
        console.error('Failed to serialize and send data:', error);
    }
}

const handleTimeRangeChange = (running, resting, ws) => {
    const value = { running, resting };
    try {
        const buffer = TimeRange.encode(value);
        const typeId = TYPE_IDS.PUMP_TIME_RANGE_TYPE_ID;
        const bufferWithTypeId = addTypeIdToBuffer(typeId, buffer);

        ws.send(bufferWithTypeId);
    } catch (error) {
        console.error('Failed to serialize and send data:', error);
    }
}


const getHoursAndMinutes = (totalMilliseconds) => {
    const totalSeconds = Math.floor(totalMilliseconds / 1000);
    const hours = Math.floor(totalSeconds / 3600);
    const minutes = Math.floor((totalSeconds % 3600) / 60);
    return { hours, minutes };
};


/* const getSeconds = ({ hours, minutes }) => {
    const hoursToSeconds = hours * 60 * 60;
    const minutesToSeconds = minutes * 60;
    return hoursToSeconds + minutesToSeconds;
}; */

const getMilliseconds = ({ hours, minutes }) => {
    const hoursToSeconds = hours * 60 * 60;
    const minutesToSeconds = minutes * 60;
    const totalSeconds = hoursToSeconds + minutesToSeconds;
    return totalSeconds * 1000;
};



/**
 * Updates the display style of a list of elements.
 * @param {NodeListOf<HTMLElement>} elements - The elements to update.
 * @param {string} displayValue - The display value to set.
 */
const updateVisibility = (elements, displayValue) => {
    elements.forEach(element => {
        element.style.display = displayValue;
    });
};

export {
    handleVoltageChange,
    handleModeChange,
    toggleElementVisibility,
    getHoursAndMinutes,
    getMilliseconds,
    getModeString,
    handleTimeRangeChange,
    updateVisibility,
    KEY_CONFIG,
    TYPE_IDS, VOLT_RECEIVE_FROM_SERVER
};
