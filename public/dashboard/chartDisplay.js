import Chart from 'chart.js/auto'; // Import chart.js
import AnnotationPlugin from 'chartjs-plugin-annotation';

Chart.register(AnnotationPlugin);

const DATA_COUNT = 21;
let minVoltCutOff = 120;

// Utility functions
const Utils = {
    seconds({ count }) {
        return Array.from({ length: count }, (_, i) => `${i}s`); // Changed to return numbers only
    },

    numbers({ count, min, max }) {
        return Array.from({ length: count }, () => Math.floor(Math.random() * (max - min + 1)) + min);
    },

    CHART_COLORS: {
        red: 'rgb(255, 99, 132)',
        blue: 'rgb(54, 162, 235)'
    },

    transparentize(color, opacity = 0.5) {
        const [r, g, b] = color.match(/\d+/g).map(Number);
        return `rgba(${r}, ${g}, ${b}, ${opacity})`;
    }
};

// Initial data setup
const labels = Utils.seconds({ count: DATA_COUNT });
const data = {
    labels: labels,
    datasets: [
        {
            label: 'Voltage',
            data: Array(DATA_COUNT).fill(null),
            borderColor: Utils.CHART_COLORS.blue,
            borderWidth: 1,
            backgroundColor: Utils.transparentize(Utils.CHART_COLORS.red, 0.5),
            tension: 0.4,
        },
    ]
};

const config = {
    type: 'line',
    data: data,
    options: {
        animation: {
            duration: 0
        },
        responsive: true,
        plugins: {
            tooltip: {
                enabled: true
            },
            legend: {
                display: false
            },
            annotation: {
                annotations: {
                    line1: {
                        type: 'line',
                        scaleID: 'y',
                        value: minVoltCutOff, // Minimum voltage cutoff value
                        borderColor: 'red',
                        borderWidth: 1,
                        label: {
                            content: 'Min Voltage Cutoff',
                            enabled: true,
                            position: 'top'
                        }
                    }
                }
            }
        },
        interaction: {
            mode: 'nearest',
            intersect: false,
            axis: 'x'
        },
        scales: {
            x: {
                title: {
                    display: true,
                    text: 'Time (s)' // Change title as needed
                },
                ticks: {
                    callback: function (value) {
                        return value; // Display just the value (e.g., 0, 1, 2, ...)
                    }
                }
            },
            y: {
                title: {
                    display: true,
                    text: 'Voltage (V)'
                },
                min: 0,
                max: 250,
            }
        }
    },
};

const ctx = document.getElementById('voltageChart').getContext('2d');
const chart = new Chart(ctx, config);

let count = 0;
let timeCount = 0;

// Function to update chart data
export const updateChart = (value) => {
    //const newValue = Math.floor(Math.random() * (230 + 1)); // Random voltage value between 0 and 230
    const newValue = value;
    // Remove the first data point and shift the array
    if (data.labels.length >= DATA_COUNT) {
        data.labels.shift();
        data.datasets.forEach(dataset => {
            dataset.data.shift();
        });
    }

    // Add new data point
    if (data.labels.length <= count) {
        timeCount += 1;
        const currentTime = data.labels.length + timeCount; // Time label without seconds
        data.labels.push(`${currentTime}s`);
        data.datasets.forEach(dataset => {
            dataset.data.push(newValue);
        });
    } else {
        data.datasets.forEach(dataset => {
            dataset.data[count] = newValue;
        });
        count = (count + 1) % DATA_COUNT;
    }

    // Update the chart
    chart.update();
}

const updateChartConfig = () => {
    if (chart) {
        chart.options.plugins.annotation.annotations.line1.value = minVoltCutOff;
    }
};

export const updateMinVoltCutOff = (value, f = false) => {
    minVoltCutOff = value;
    updateChartConfig();

    if (f && chart) {
        chart.update();
    }
};

