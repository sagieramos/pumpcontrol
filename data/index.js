import"./styles_a.css";document.addEventListener("DOMContentLoaded",(function(){var e=document.querySelector("form"),t=document.getElementById("passcode"),n=document.getElementById("pass-lebel");e.addEventListener("submit",(function(e){var a=t.value;/^\d{4}$/.test(a)||(e.preventDefault(),n.style.animation="shake 0.5s",n.addEventListener("animationend",(function(){n.style.animation=""})))})),t.addEventListener("input",(function(){t.value=t.value.replace(/\D/g,""),t.setCustomValidity("")})),t.addEventListener("keydown",(function(t){"Enter"===t.key&&(t.preventDefault(),e.dispatchEvent(new Event("submit")),e.requestSubmit())})),t.addEventListener("invalid",(function(e){e.preventDefault()}))}));