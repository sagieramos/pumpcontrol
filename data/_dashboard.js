/*! For license information please see _dashboard.js.LICENSE.txt */
function _typeof(t){return _typeof="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(t){return typeof t}:function(t){return t&&"function"==typeof Symbol&&t.constructor===Symbol&&t!==Symbol.prototype?"symbol":typeof t},_typeof(t)}function _regeneratorRuntime(){"use strict";_regeneratorRuntime=function(){return e};var t,e={},n=Object.prototype,r=n.hasOwnProperty,o=Object.defineProperty||function(t,e,n){t[e]=n.value},i="function"==typeof Symbol?Symbol:{},a=i.iterator||"@@iterator",c=i.asyncIterator||"@@asyncIterator",u=i.toStringTag||"@@toStringTag";function l(t,e,n){return Object.defineProperty(t,e,{value:n,enumerable:!0,configurable:!0,writable:!0}),t[e]}try{l({},"")}catch(t){l=function(t,e,n){return t[e]=n}}function s(t,e,n,r){var i=e&&e.prototype instanceof g?e:g,a=Object.create(i.prototype),c=new T(r||[]);return o(a,"_invoke",{value:L(t,n,c)}),a}function d(t,e,n){try{return{type:"normal",arg:t.call(e,n)}}catch(t){return{type:"throw",arg:t}}}e.wrap=s;var f="suspendedStart",m="suspendedYield",y="executing",p="completed",h={};function g(){}function v(){}function b(){}var E={};l(E,a,(function(){return this}));var w=Object.getPrototypeOf,_=w&&w(w(R([])));_&&_!==n&&r.call(_,a)&&(E=_);var x=b.prototype=g.prototype=Object.create(E);function C(t){["next","throw","return"].forEach((function(e){l(t,e,(function(t){return this._invoke(e,t)}))}))}function I(t,e){function n(o,i,a,c){var u=d(t[o],t,i);if("throw"!==u.type){var l=u.arg,s=l.value;return s&&"object"==_typeof(s)&&r.call(s,"__await")?e.resolve(s.__await).then((function(t){n("next",t,a,c)}),(function(t){n("throw",t,a,c)})):e.resolve(s).then((function(t){l.value=t,a(l)}),(function(t){return n("throw",t,a,c)}))}c(u.arg)}var i;o(this,"_invoke",{value:function(t,r){function o(){return new e((function(e,o){n(t,r,e,o)}))}return i=i?i.then(o,o):o()}})}function L(e,n,r){var o=f;return function(i,a){if(o===y)throw Error("Generator is already running");if(o===p){if("throw"===i)throw a;return{value:t,done:!0}}for(r.method=i,r.arg=a;;){var c=r.delegate;if(c){var u=O(c,r);if(u){if(u===h)continue;return u}}if("next"===r.method)r.sent=r._sent=r.arg;else if("throw"===r.method){if(o===f)throw o=p,r.arg;r.dispatchException(r.arg)}else"return"===r.method&&r.abrupt("return",r.arg);o=y;var l=d(e,n,r);if("normal"===l.type){if(o=r.done?p:m,l.arg===h)continue;return{value:l.arg,done:r.done}}"throw"===l.type&&(o=p,r.method="throw",r.arg=l.arg)}}}function O(e,n){var r=n.method,o=e.iterator[r];if(o===t)return n.delegate=null,"throw"===r&&e.iterator.return&&(n.method="return",n.arg=t,O(e,n),"throw"===n.method)||"return"!==r&&(n.method="throw",n.arg=new TypeError("The iterator does not provide a '"+r+"' method")),h;var i=d(o,e.iterator,n.arg);if("throw"===i.type)return n.method="throw",n.arg=i.arg,n.delegate=null,h;var a=i.arg;return a?a.done?(n[e.resultName]=a.value,n.next=e.nextLoc,"return"!==n.method&&(n.method="next",n.arg=t),n.delegate=null,h):a:(n.method="throw",n.arg=new TypeError("iterator result is not an object"),n.delegate=null,h)}function S(t){var e={tryLoc:t[0]};1 in t&&(e.catchLoc=t[1]),2 in t&&(e.finallyLoc=t[2],e.afterLoc=t[3]),this.tryEntries.push(e)}function k(t){var e=t.completion||{};e.type="normal",delete e.arg,t.completion=e}function T(t){this.tryEntries=[{tryLoc:"root"}],t.forEach(S,this),this.reset(!0)}function R(e){if(e||""===e){var n=e[a];if(n)return n.call(e);if("function"==typeof e.next)return e;if(!isNaN(e.length)){var o=-1,i=function n(){for(;++o<e.length;)if(r.call(e,o))return n.value=e[o],n.done=!1,n;return n.value=t,n.done=!0,n};return i.next=i}}throw new TypeError(_typeof(e)+" is not iterable")}return v.prototype=b,o(x,"constructor",{value:b,configurable:!0}),o(b,"constructor",{value:v,configurable:!0}),v.displayName=l(b,u,"GeneratorFunction"),e.isGeneratorFunction=function(t){var e="function"==typeof t&&t.constructor;return!!e&&(e===v||"GeneratorFunction"===(e.displayName||e.name))},e.mark=function(t){return Object.setPrototypeOf?Object.setPrototypeOf(t,b):(t.__proto__=b,l(t,u,"GeneratorFunction")),t.prototype=Object.create(x),t},e.awrap=function(t){return{__await:t}},C(I.prototype),l(I.prototype,c,(function(){return this})),e.AsyncIterator=I,e.async=function(t,n,r,o,i){void 0===i&&(i=Promise);var a=new I(s(t,n,r,o),i);return e.isGeneratorFunction(n)?a:a.next().then((function(t){return t.done?t.value:a.next()}))},C(x),l(x,u,"Generator"),l(x,a,(function(){return this})),l(x,"toString",(function(){return"[object Generator]"})),e.keys=function(t){var e=Object(t),n=[];for(var r in e)n.push(r);return n.reverse(),function t(){for(;n.length;){var r=n.pop();if(r in e)return t.value=r,t.done=!1,t}return t.done=!0,t}},e.values=R,T.prototype={constructor:T,reset:function(e){if(this.prev=0,this.next=0,this.sent=this._sent=t,this.done=!1,this.delegate=null,this.method="next",this.arg=t,this.tryEntries.forEach(k),!e)for(var n in this)"t"===n.charAt(0)&&r.call(this,n)&&!isNaN(+n.slice(1))&&(this[n]=t)},stop:function(){this.done=!0;var t=this.tryEntries[0].completion;if("throw"===t.type)throw t.arg;return this.rval},dispatchException:function(e){if(this.done)throw e;var n=this;function o(r,o){return c.type="throw",c.arg=e,n.next=r,o&&(n.method="next",n.arg=t),!!o}for(var i=this.tryEntries.length-1;i>=0;--i){var a=this.tryEntries[i],c=a.completion;if("root"===a.tryLoc)return o("end");if(a.tryLoc<=this.prev){var u=r.call(a,"catchLoc"),l=r.call(a,"finallyLoc");if(u&&l){if(this.prev<a.catchLoc)return o(a.catchLoc,!0);if(this.prev<a.finallyLoc)return o(a.finallyLoc)}else if(u){if(this.prev<a.catchLoc)return o(a.catchLoc,!0)}else{if(!l)throw Error("try statement without catch or finally");if(this.prev<a.finallyLoc)return o(a.finallyLoc)}}}},abrupt:function(t,e){for(var n=this.tryEntries.length-1;n>=0;--n){var o=this.tryEntries[n];if(o.tryLoc<=this.prev&&r.call(o,"finallyLoc")&&this.prev<o.finallyLoc){var i=o;break}}i&&("break"===t||"continue"===t)&&i.tryLoc<=e&&e<=i.finallyLoc&&(i=null);var a=i?i.completion:{};return a.type=t,a.arg=e,i?(this.method="next",this.next=i.finallyLoc,h):this.complete(a)},complete:function(t,e){if("throw"===t.type)throw t.arg;return"break"===t.type||"continue"===t.type?this.next=t.arg:"return"===t.type?(this.rval=this.arg=t.arg,this.method="return",this.next="end"):"normal"===t.type&&e&&(this.next=e),h},finish:function(t){for(var e=this.tryEntries.length-1;e>=0;--e){var n=this.tryEntries[e];if(n.finallyLoc===t)return this.complete(n.completion,n.afterLoc),k(n),h}},catch:function(t){for(var e=this.tryEntries.length-1;e>=0;--e){var n=this.tryEntries[e];if(n.tryLoc===t){var r=n.completion;if("throw"===r.type){var o=r.arg;k(n)}return o}}throw Error("illegal catch attempt")},delegateYield:function(e,n,r){return this.delegate={iterator:R(e),resultName:n,nextLoc:r},"next"===this.method&&(this.arg=t),h}},e}function _slicedToArray(t,e){return _arrayWithHoles(t)||_iterableToArrayLimit(t,e)||_unsupportedIterableToArray(t,e)||_nonIterableRest()}function _nonIterableRest(){throw new TypeError("Invalid attempt to destructure non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.")}function _unsupportedIterableToArray(t,e){if(t){if("string"==typeof t)return _arrayLikeToArray(t,e);var n={}.toString.call(t).slice(8,-1);return"Object"===n&&t.constructor&&(n=t.constructor.name),"Map"===n||"Set"===n?Array.from(t):"Arguments"===n||/^(?:Ui|I)nt(?:8|16|32)(?:Clamped)?Array$/.test(n)?_arrayLikeToArray(t,e):void 0}}function _arrayLikeToArray(t,e){(null==e||e>t.length)&&(e=t.length);for(var n=0,r=Array(e);n<e;n++)r[n]=t[n];return r}function _iterableToArrayLimit(t,e){var n=null==t?null:"undefined"!=typeof Symbol&&t[Symbol.iterator]||t["@@iterator"];if(null!=n){var r,o,i,a,c=[],u=!0,l=!1;try{if(i=(n=n.call(t)).next,0===e){if(Object(n)!==n)return;u=!1}else for(;!(u=(r=i.call(n)).done)&&(c.push(r.value),c.length!==e);u=!0);}catch(t){l=!0,o=t}finally{try{if(!u&&null!=n.return&&(a=n.return(),Object(a)!==a))return}finally{if(l)throw o}}return c}}function _arrayWithHoles(t){if(Array.isArray(t))return t}function asyncGeneratorStep(t,e,n,r,o,i,a){try{var c=t[i](a),u=c.value}catch(t){return void n(t)}c.done?e(u):Promise.resolve(u).then(r,o)}function _asyncToGenerator(t){return function(){var e=this,n=arguments;return new Promise((function(r,o){var i=t.apply(e,n);function a(t){asyncGeneratorStep(i,r,o,a,c,"next",t)}function c(t){asyncGeneratorStep(i,r,o,a,c,"throw",t)}a(void 0)}))}}import"./_dashboard.css";import"./reset.css";import TimePicker from"tui-time-picker";import{toggleElementVisibility,getHoursAndMinutes,getMilliseconds,handleVoltageChange,handleTimeRangeChange,handleModeChange,getModeString,setColorFromMode,updateVisibility,formatTimeFromMinutes,KEY_CONFIG,VOLT_RECEIVE_FROM_SERVER}from"./util.js";import{Countdown}from"./countDown.js";import{ControlData,TimeRange}from"../protoc/js/pump_control_data.js";import{Num}from"../protoc/js/str_num_msg.js";import{Msg1}from"../protoc/js/msg1.js";import"tui-time-picker/dist/tui-time-picker.css";import notificationSound from"./notify.mp3";var updateChartFunction,updateMinVoltCutOffFunction,PowerStatus={POWER_INACTIVE:0,POWER_READY:1,POWER_RUNNING:2,POWER_RESTING:3,POWER_VOLTAGE_LOW:4};document.addEventListener("DOMContentLoaded",(function(){var t,e,n,r=document.getElementById("voltage"),o=document.getElementById("voltageChart"),i=document.querySelectorAll(".lds-ellipsis"),a=document.querySelectorAll(".init-hide"),c=document.getElementById("min-volt"),u=document.getElementById("pump-power-indicator"),l=document.getElementById("run-time"),s=document.getElementById("rest-time"),d=document.getElementById("machine-mode"),f=document.querySelectorAll(".styled-button"),m=document.getElementById("mode"),y=document.getElementById("run-rest-state"),p=document.getElementById("timer"),h=document.getElementById("min-voltage-input"),g=document.getElementById("min-voltage-slider"),v=document.getElementById("mode-bg"),b=!1,E=new Countdown("#run-rest-ing",0),w={initialHour:0,initialMinute:0,inputType:"selectbox",showMeridiem:!1};E.update(0);var _={hour:0,minute:4},x=new TimePicker("#running-time",w);x.setRange(_,{hour:1,minute:59});var C=new TimePicker("#resting-time",w);C.setRange(_,{hour:23,minute:59});var I=function(){var t=_asyncToGenerator(_regeneratorRuntime().mark((function t(){var e,n,r,i,a;return _regeneratorRuntime().wrap((function(t){for(;;)switch(t.prev=t.next){case 0:if(updateChartFunction&&updateMinVoltCutOffFunction){t.next=12;break}return t.next=3,Promise.all([import("./chartDisplay.js")]);case 3:e=t.sent,n=_slicedToArray(e,1),r=n[0],i=r.updateChart,a=r.updateMinVoltCutOff,updateChartFunction=i,updateMinVoltCutOffFunction=a,document.getElementById("voltageChart").style.display="block",toggleElementVisibility(o,"show");case 12:case"end":return t.stop()}}),t)})));return function(){return t.apply(this,arguments)}}(),L=document.getElementById("value-heading"),O=!0,S=0,k=function(t){var e=t.running,n=t.resting,r=getHoursAndMinutes(e),o=r.hours,i=r.minutes,a=getHoursAndMinutes(n),c=a.hours,u=a.minutes;l.textContent="".concat(o,":").concat(i.toString().padStart(2,"0")),s.textContent="".concat(c,":").concat(u.toString().padStart(2,"0")),x.setTime(o,i,!1),C.setTime(c,u,!1)},T=function(){updateVisibility(i,"block"),updateVisibility(a,"none"),updateVisibility(f,"none"),u.style.display="none"},R=function(t){0===t[0]&&(window.location.href="/logout")},M=new Audio(notificationSound);M.addEventListener("canplaythrough",(function(){}),{once:!0}),M.addEventListener("error",(function(t){console.error("Audio playback error:",t)}));var F=[document.getElementById("t-e-c"),document.getElementById("t-i-op"),document.getElementById("l-r-d"),document.getElementById("e-c")],A=[document.getElementById("p-p"),document.getElementById("p-f")],V=[R,function(t){try{var e=Num.decode(t.slice(1));!function(t,e){var n=!1;switch(t){case VOLT_RECEIVE_FROM_SERVER.VOLTAGE:r.textContent=e.toFixed(1),updateChartFunction(e),O!==(n=e<S)&&(L.style.color=n?"#ff6384":"#36a2eb",O=n);break;case VOLT_RECEIVE_FROM_SERVER.MIN_VOLTAGE:case KEY_CONFIG.MIN_VOLT:updateMinVoltCutOffFunction(e,!0),c.textContent=e,h.value=e,g.value=e,S=e;break;case KEY_CONFIG.CONFIG_MODE:d.textContent=getModeString(e),setColorFromMode(e,v),m.value=e}}(e.key,e.value)}catch(t){console.error("Failed to deserialize Num:",t)}},R,R,function(t){try{var e=ControlData.decode(t.slice(1));k(e.time_range),d.textContent=getModeString(e.mode),setColorFromMode(e.mode,v),m.value=e.mode}catch(t){console.error("Failed to deserialize ControlData:",t)}},function(t){try{var e=TimeRange.decode(t.slice(1));k(e)}catch(t){console.error("Failed to deserialize TimeRange:",t)}},R,R,function(t){var e=Num.decode(t.slice(1)),n=e.key,r=e.value,o=Number(r),i="block";switch(u.classList.remove("blinking"),n){case PowerStatus.POWER_INACTIVE:u.style.backgroundColor="#808080",y.textContent="Inactive",i="none",o=1;break;case PowerStatus.POWER_READY:u.classList.add("blinking"),y.textContent="Ready...";break;case PowerStatus.POWER_RUNNING:u.style.backgroundColor="#32CD32",y.textContent="Running...";break;case PowerStatus.POWER_RESTING:u.style.backgroundColor="#1E90FF",y.textContent="Resting...";break;case PowerStatus.POWER_VOLTAGE_LOW:u.style.backgroundColor="#FF0000",y.textContent="Voltage Low",i="none",o=1;break;default:o=1,i="none",y.textContent="..."}p.style.display=i,E.update(o),M.play().catch((function(t){return console.error("Audio play error:",t)})),"vibrate"in navigator&&navigator.vibrate([200,100,200])},function(t){try{var e=Msg1.decode(t.slice(1));console.log("msg1:",e);var n=e.f0,r=e.f1,o=e.f2,i=e.f3;F[0].textContent=n.toFixed(2),F[1].textContent=formatTimeFromMinutes(r),F[2].textContent=formatTimeFromMinutes(o),F[3].textContent=i.toFixed(1)}catch(t){console.error("Failed to deserialize Msg1:",t)}},function(t){try{var e=Msg1.decode(t.slice(1));console.log("msg2:",e);var n,o=e.f0,i=e.f1,a=e.f2;r.textContent=o.toFixed(1),updateChartFunction(o),O!==(n=o<S)&&(L.style.color=n?"#ff6384":"#36a2eb",O=n),A[0].textContent=i.toFixed(1),A[1].textContent=a.toFixed(2)}catch(t){console.error("Failed to deserialize Msg1:",t)}}],N=function(){var t=_asyncToGenerator(_regeneratorRuntime().mark((function t(e){var n;return _regeneratorRuntime().wrap((function(t){for(;;)switch(t.prev=t.next){case 0:return n=e[0],t.next=3,I();case 3:V[n]&&V[n](e);case 4:case"end":return t.stop()}}),t)})));return function(e){return t.apply(this,arguments)}}(),P=function(){n&&clearInterval(n),b||(n=setInterval((function(){if(t.readyState===WebSocket.CLOSED){var e="https:"===window.location.protocol?"wss:":"ws:",r="".concat(e,"//").concat(window.location.host,"/ws");t=new WebSocket(r)}else clearInterval(n)}),3e3))},B=function(){e&&clearTimeout(e),e=setTimeout((function(){t&&(T(),t.close(),P())}),2e3)},G=0,j=document.getElementById("header");window.addEventListener("scroll",(function(){var t=window.scrollY||document.documentElement.scrollB;j.style.top=t<G?"-80px":"0",G=t}));var W=document.getElementById("config-group"),D=W.querySelectorAll(".config"),H=Array.from(document.querySelectorAll(".open-config"));document.addEventListener("click",(function(e){var n=e.target;if(n.matches("#submit-btn-min-volt")){var r=parseInt(h.value,10);if(r.isNaN||r<110||r>230)return;handleVoltageChange(r,t),W.style.display="none"}else if(n.matches("#submit-time-range-btn")){var o=getMilliseconds({hours:x.getHour(),minutes:x.getMinute()}),i=getMilliseconds({hours:C.getHour(),minutes:C.getMinute()});handleTimeRangeChange(o,i,t),W.style.display="none"}else if(n.matches(".open-config")){var a=H.indexOf(n);if(-1===a)return;W.style.display="flex",updateVisibility(D,"none"),D[a].style.display="flex"}else n.matches("#cancel-config-ui")?W.style.display="none":n.matches("#config-group")?n.style.display="none":n.matches("#logout")&&(window.location.href="/logout")})),document.getElementById("voltage-setting").addEventListener("input",(function(t){var e=t.target,n=e.value,r=e.id,o=parseInt(n,10),i=document.getElementById("voltage-label");o<110||o>230?i.innerHTML='Set Minimum Operating Voltage (110-230 V) <br> <span id="error">— Please enter a valid value!</span>':(i.innerHTML="Set Minimum Operating Voltage (110-230 V)","min-voltage-input"===r?g.value=o:"min-voltage-slider"===r&&(h.value=o))})),m.addEventListener("change",(function(e){t.readyState===WebSocket.OPEN?handleModeChange(e.target.value,t):console.error("WebSocket is not open"),W.style.display="none"})),setTimeout((function(){var e,n;e="https:"===window.location.protocol?"wss:":"ws:",n="".concat(e,"//").concat(window.location.host,"/ws"),(t=new WebSocket(n)).onopen=function(){updateVisibility(i,"none"),updateVisibility(a,"block"),updateVisibility(f,"block"),u.style.display="block",B()},t.onmessage=function(){var t=_asyncToGenerator(_regeneratorRuntime().mark((function t(e){var n,r;return _regeneratorRuntime().wrap((function(t){for(;;)switch(t.prev=t.next){case 0:return B(),t.prev=1,t.next=4,e.data.arrayBuffer();case 4:n=t.sent,r=new Uint8Array(n),N(r),t.next=12;break;case 9:t.prev=9,t.t0=t.catch(1),console.error("Error handling WebSocket message:",t.t0);case 12:case"end":return t.stop()}}),t,null,[[1,9]])})));return function(e){return t.apply(this,arguments)}}(),t.onclose=function(){r.textContent="00.0",T(),P()},t.onerror=function(t){r.textContent="00.0",console.error("WebSocket error:",t),T(),b=!0}}),1e3)}));