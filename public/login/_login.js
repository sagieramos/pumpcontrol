import './_login.css';

const APname = 'Imuwahen_2024';
let envSupportSSE = false;

document.addEventListener('DOMContentLoaded', () => {
    const form = document.getElementById('login-form');
    const passcodeInput = document.getElementById('passcode');
    const passLabel = document.getElementById('pass-lebel');
    const flashMessage = document.getElementById('flash-message');
    const loadingAnim = document.getElementById('loading-anim');
    const flashText = document.getElementById('flash-text');
    const warningIcon = document.getElementById('warning-icon');

    const displayError = (message) => {
        flashText.innerHTML = message;
        flashMessage.style.display = 'flex';
        warningIcon.style.animation = 'shake 0.5s';
        warningIcon.addEventListener('animationend', () => {
            warningIcon.style.animation = '';
        });
        loadingAnim.classList.remove('loading-anim');
        passcodeInput.disabled = false;
        passLabel.innerText = 'Enter 4-digit pin';
    };

    const setLoadingState = (isLoading) => {
        if (isLoading) {
            loadingAnim.classList.add('loading-anim');
            passcodeInput.disabled = true;
            loadingAnim.disabled = true;
            passLabel.innerText = 'Authenticating...';
        } else {
            loadingAnim.classList.remove('loading-anim');
            passcodeInput.disabled = false;
            loadingAnim.disabled = false;
            passLabel.innerText = 'Enter 4-digit pin';
        }
    };

    const authTokenElement = document.getElementById("auth-token");

    const updateSessionId = (newToken) => {
        const formattedToken = newToken.toString().padStart(9, '0');
        const formattedString = `${formattedToken.slice(0, 4)} ${formattedToken.slice(4, 7)} ${formattedToken.slice(7)}`;
        authTokenElement.textContent = formattedString;
        authTokenElement.classList.add('drop');
    };

    const validateAndSubmit = async () => {
        const passcode = passcodeInput.value;
        const formData = new FormData(form);
        const isValid = /^\d{4}$/.test(passcode);

        if (!isValid) {
            displayError('Invalid PIN. Please enter a 4-digit PIN.');
            return;
        }

        setLoadingState(true);

        try {
            const response = await fetch('/login', {
                method: 'POST',
                body: formData,
                credentials: 'include',
            });

            let result;
            const contentType = response.headers.get('Content-Type');

            if (contentType && contentType.includes('application/json')) {
                result = await response.json();
            } else {
                result = await response.text();
            }

            console.log(result);

            switch (response.status) {
                case 200:
                    window.location.href = '/dashboard';
                    break;
                case 401:
                    displayError(result.error || 'Unauthorized');
                    passcodeInput.value = '';
                    break;
                case 302:
                    displayError(result.error || 'Server Unavailable.');
                    passcodeInput.value = '';
                    break;
                case 500:
                    displayError(result.error || `Connect your device WiFi to Access Point: <span style="font-style: italic;">${APname}</span> and try again.`);
                    break;
                default:
                    displayError(result.error || 'Faulty Request. Please try again.');
                    passcodeInput.value = '';
                    break;
            }

            if (result.sessionId && envSupportSSE) {
                updateSessionId(result.sessionId);
            }

        } catch (error) {
            console.error('Error:', error);
            displayError('An error occurred. Please try again.');
        } finally {
            setLoadingState(false);
        }
    };


    form.addEventListener('submit', (event) => {
        if (!loadingAnim.classList.contains('loading-anim')) {
            event.preventDefault();
            validateAndSubmit();
        }
    });

    loadingAnim.addEventListener('click', (event) => {
        event.preventDefault();
        if (!loadingAnim.classList.contains('loading-anim')) {
            validateAndSubmit();
        }
    });

    passcodeInput.addEventListener('input', () => {
        passcodeInput.value = passcodeInput.value.replace(/\D/g, '');
        passcodeInput.setCustomValidity('');
    });

    passcodeInput.addEventListener('keydown', (event) => {
        if (event.key === 'Enter' && !loadingAnim.classList.contains('loading-anim')) {
            event.preventDefault();
            validateAndSubmit();
        }
    });

    passcodeInput.addEventListener('invalid', (event) => {
        event.preventDefault();
    });

    authTokenElement.addEventListener('animationend', function () {
        authTokenElement.classList.remove('drop');
    });

    setTimeout(async () => {
        try {
            const response = await fetch('/getsessionid', {
                method: 'GET',
                credentials: 'omit',
            });

            if (!response.ok) {
                throw new Error('Network response was not ok');
            }

            const text = await response.text();

            updateSessionId(text);

            setTimeout(() => {
                passcodeInput.focus();
            }, 500)

        } catch (error) {
            console.error('Error:', error);
        }
    }, 1000);


    if (!!window.EventSource) {
        const source = new EventSource('/events');

        source.addEventListener('open', () => {
            envSupportSSE = false;
        }, false);

        source.addEventListener('error', (e) => {
            if (e.target.readyState != EventSource.OPEN) {
                envSupportSSE = true;
            }
        }, false);

        source.addEventListener('sessionId', (e) => {
            updateSessionId(e.data);
        }, false);
    }
});