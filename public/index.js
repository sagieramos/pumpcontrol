import './styles_a.css';

document.addEventListener('DOMContentLoaded', () => {
    const form = document.querySelector('form');
    const passcodeInput = document.getElementById('passcode');
    const passLabel = document.getElementById('pass-lebel');
  
    form.addEventListener('submit', (event) => {
      const passcode = passcodeInput.value;
      const isValid = /^\d{4}$/.test(passcode);

      if (!isValid) {
        event.preventDefault();
        passLabel.style.animation = 'shake 0.5s';
        passLabel.addEventListener('animationend', () => {
          passLabel.style.animation = '';
        });
      }
    });
  
    passcodeInput.addEventListener('input', () => {
        passcodeInput.value = passcodeInput.value.replace(/\D/g, '');
      passcodeInput.setCustomValidity('');
    });

    passcodeInput.addEventListener('keydown', (event) => {
        if (event.key === 'Enter') {
          event.preventDefault();
          form.dispatchEvent(new Event('submit'));
          form.requestSubmit();
        }
    });
  
    passcodeInput.addEventListener('invalid', (event) => {
      event.preventDefault();
    });
  });
  