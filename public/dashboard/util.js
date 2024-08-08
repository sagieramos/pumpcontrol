
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
 * Serializes data into a buffer with a type identifier.
 * @param {Object} data - The data to serialize.
 * @param {number} typeIdentifier - The type identifier for the data.
 * @param {Object} messageType - The message type for encoding the data.
 * @returns {Buffer} - The serialized buffer.
 */


const serializeData = (data, typeIdentifier, messageType) => {
    const dataBuffer = messageType.encode(data);
    const buffer = Buffer.alloc(1 + dataBuffer.length);
    buffer[0] = typeIdentifier;
    dataBuffer.copy(buffer, 1);
    return buffer;
};


export { serializeData, toggleElementVisibility };
