export class Countdown {

    /**
     * @param {String} itemSelector
     * @param {Number} totalSeconds
     */
    constructor(itemSelector, totalSeconds) {
        this.itemSelector = itemSelector;
        this.totalSeconds = totalSeconds;
        this.startTime = Math.floor(Date.now() / 1000); // Initialize start time

        // Ensure canvas elements are available
        let $ = el => document.querySelector(el);
        let timerCanvas = $(itemSelector + ' .timer');
        let meterCanvas = $(itemSelector + ' .meter');

        if (timerCanvas && meterCanvas) {
            this.dateSelectors = {
                timer: timerCanvas.getContext('2d'),
                meter: meterCanvas.getContext('2d')
            };

            // Start the countdown
            this.start();
        } else {
            console.error('Canvas elements not found');
        }
    }

    /**
     * Main function
     */
    set() {
        let now = Math.floor(Date.now() / 1000);
        let remainingSeconds = this.totalSeconds - (now - this.startTime);

        // Ensure remainingSeconds is not negative
        remainingSeconds = Math.max(remainingSeconds, 0);

        let hours = Math.floor(remainingSeconds / 3600);
        let minutes = Math.floor((remainingSeconds % 3600) / 60);
        let seconds = remainingSeconds % 60;

        // Format time as 0:00:00
        let formattedTime = `${this.formatNumber(hours)}:${this.formatNumber(minutes)}:${this.formatNumber(seconds)}`;

        this.clear(this.dateSelectors.timer);
        this.clear(this.dateSelectors.meter);

        // Draw the formatted time and linear meter
        this.drawTime(formattedTime);
        this.drawMeter(remainingSeconds);

        if (remainingSeconds > 0) {
            requestAnimationFrame(() => this.set());
        }
    }

    /**
     * Format number to ensure it has two digits
     * @param {Number} num
     * @returns {String}
     */
    formatNumber(num) {
        return num < 10 ? '0' + num : num.toString();
    }

    /**
     * Draw formatted time on the canvas
     * @param {String} formattedTime
     */
    drawTime(formattedTime) {
        let ctx = this.dateSelectors.timer;
        ctx.font = "0.85rem 'Roboto', sans-serif";
        ctx.fillStyle = "#243b55";
        ctx.textAlign = "center";
        ctx.textBaseline = "middle";
        ctx.fillText(formattedTime, ctx.canvas.width / 2, ctx.canvas.height / 2); // Center text
    }

    /**
     * Draw linear meter
     * @param {Number} remainingSeconds
     */
    drawMeter(remainingSeconds) {
        const width = this.dateSelectors.meter.canvas.width;
        const height = this.dateSelectors.meter.canvas.height;
        const progressWidth = width * (1 - (remainingSeconds / this.totalSeconds)); // Fill from left to right

        // Set styles for the meter
        let ctx = this.dateSelectors.meter;
        ctx.lineWidth = height;
        ctx.fillStyle = '#ecf0f1';

        // Draw background rectangle
        ctx.beginPath();
        ctx.rect(0, 0, width, height);
        ctx.fillStyle = '#36a2eb';  // Background color
        ctx.fill();

        // Draw progress rectangle (loading bar)
        ctx.beginPath();
        ctx.rect(0, 0, progressWidth, height);
        ctx.fillStyle = '#c3cfe2'; // Progress color
        ctx.fill();
    }

    /**
     * Clear canvas
     * @param {CanvasRenderingContext2D} ctx 
     */
    clear(ctx) { ctx.clearRect(0, 0, ctx.canvas.width, ctx.canvas.height); }

    /**
     * Start the countdown
     */
    start() {
        requestAnimationFrame(() => this.set());
    }

    /**
     * Update the countdown
     * @param {Number} totalSeconds
     */
    update(totalSeconds) {
        this.totalSeconds = totalSeconds;
        this.startTime = Math.floor(Date.now() / 1000); // Reset start time for new duration
        this.start(); // Restart the countdown with new time
    }
}
