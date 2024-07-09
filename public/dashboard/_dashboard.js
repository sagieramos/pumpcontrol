import './_dashboard.css';

document.addEventListener('DOMContentLoaded', () => {
    const modeElement = document.getElementById('mode');
    const runningElement = document.getElementById('running');
    const restingElement = document.getElementById('resting');
    const modeInput = document.getElementById('modeInput');
    const runningInput = document.getElementById('runningInput');
    const restingInput = document.getElementById('restingInput');
    const sendBtn = document.getElementById('sendBtn');

    let ws;

    const getCookie = name => {
        const value = `; ${document.cookie}`;
        const parts = value.split(`; ${name}=`);
        if (parts.length === 2) return parts.pop().split(';').shift();
    };

    const connectWebSocket = () => {
        const idx = getCookie('_idx');
        const tkn = getCookie('_imuwahen');

        if (!idx || !tkn) {
            console.error('Authentication tokens not found in cookies');
            return;
        }

        ws = new WebSocket('ws://akowe.org/ws');

        ws.onopen = () => {
            console.log('Connected to WebSocket server');
            ws.send(JSON.stringify({ idx, tkn }));
        };

        ws.onmessage = event => {
            try {
                const data = JSON.parse(event.data);
                if (data.objectName) {
                    updateStatus(data.objectName);
                }
            } catch (error) {
                console.error('Error parsing message:', error);
            }
        };

        ws.onclose = () => {
            console.log('Disconnected from WebSocket server');
        };

        ws.onerror = error => {
            console.error('WebSocket error:', error);
        };
    };

    const updateStatus = data => {
        if (data.mode !== undefined) {
            modeElement.textContent = data.mode;
        }
        if (data.running !== undefined) {
            runningElement.textContent = data.running;
        }
        if (data.resting !== undefined) {
            restingElement.textContent = data.resting;
        }
    };

    const sendData = () => {
        const mode = parseInt(modeInput.value);
        const running = parseInt(runningInput.value);
        const resting = parseInt(restingInput.value);

        const data = { objectName: {} };
        if (!isNaN(mode)) data.objectName.mode = mode;
        if (!isNaN(running)) data.objectName.running = running;
        if (!isNaN(resting)) data.objectName.resting = resting;

        if (ws && ws.readyState === WebSocket.OPEN) {
            ws.send(JSON.stringify(data));
            console.log('Data sent:', data);
        } else {
            console.error('WebSocket is not open');
        }
    };

    sendBtn.addEventListener('click', sendData);

    connectWebSocket();
});
