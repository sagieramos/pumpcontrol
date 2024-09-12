/*! For license information please see _dashboard.js.LICENSE.txt */
function _typeof(t){return _typeof="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(t){return typeof t}:function(t){return t&&"function"==typeof Symbol&&t.constructor===Symbol&&t!==Symbol.prototype?"symbol":typeof t},_typeof(t)}function _regeneratorRuntime(){"use strict";_regeneratorRuntime=function(){return e};var t,e={},n=Object.prototype,r=n.hasOwnProperty,o=Object.defineProperty||function(t,e,n){t[e]=n.value},i="function"==typeof Symbol?Symbol:{},a=i.iterator||"@@iterator",u=i.asyncIterator||"@@asyncIterator",c=i.toStringTag||"@@toStringTag";function l(t,e,n){return Object.defineProperty(t,e,{value:n,enumerable:!0,configurable:!0,writable:!0}),t[e]}try{l({},"")}catch(t){l=function(t,e,n){return t[e]=n}}function s(t,e,n,r){var i=e&&e.prototype instanceof g?e:g,a=Object.create(i.prototype),u=new R(r||[]);return o(a,"_invoke",{value:S(t,n,u)}),a}function d(t,e,n){try{return{type:"normal",arg:t.call(e,n)}}catch(t){return{type:"throw",arg:t}}}e.wrap=s;var f="suspendedStart",y="suspendedYield",m="executing",p="completed",h={};function g(){}function v(){}function b(){}var E={};l(E,a,(function(){return this}));var w=Object.getPrototypeOf,_=w&&w(w(T([])));_&&_!==n&&r.call(_,a)&&(E=_);var C=b.prototype=g.prototype=Object.create(E);function L(t){["next","throw","return"].forEach((function(e){l(t,e,(function(t){return this._invoke(e,t)}))}))}function O(t,e){function n(o,i,a,u){var c=d(t[o],t,i);if("throw"!==c.type){var l=c.arg,s=l.value;return s&&"object"==_typeof(s)&&r.call(s,"__await")?e.resolve(s.__await).then((function(t){n("next",t,a,u)}),(function(t){n("throw",t,a,u)})):e.resolve(s).then((function(t){l.value=t,a(l)}),(function(t){return n("throw",t,a,u)}))}u(c.arg)}var i;o(this,"_invoke",{value:function(t,r){function o(){return new e((function(e,o){n(t,r,e,o)}))}return i=i?i.then(o,o):o()}})}function S(e,n,r){var o=f;return function(i,a){if(o===m)throw Error("Generator is already running");if(o===p){if("throw"===i)throw a;return{value:t,done:!0}}for(r.method=i,r.arg=a;;){var u=r.delegate;if(u){var c=x(u,r);if(c){if(c===h)continue;return c}}if("next"===r.method)r.sent=r._sent=r.arg;else if("throw"===r.method){if(o===f)throw o=p,r.arg;r.dispatchException(r.arg)}else"return"===r.method&&r.abrupt("return",r.arg);o=m;var l=d(e,n,r);if("normal"===l.type){if(o=r.done?p:y,l.arg===h)continue;return{value:l.arg,done:r.done}}"throw"===l.type&&(o=p,r.method="throw",r.arg=l.arg)}}}function x(e,n){var r=n.method,o=e.iterator[r];if(o===t)return n.delegate=null,"throw"===r&&e.iterator.return&&(n.method="return",n.arg=t,x(e,n),"throw"===n.method)||"return"!==r&&(n.method="throw",n.arg=new TypeError("The iterator does not provide a '"+r+"' method")),h;var i=d(o,e.iterator,n.arg);if("throw"===i.type)return n.method="throw",n.arg=i.arg,n.delegate=null,h;var a=i.arg;return a?a.done?(n[e.resultName]=a.value,n.next=e.nextLoc,"return"!==n.method&&(n.method="next",n.arg=t),n.delegate=null,h):a:(n.method="throw",n.arg=new TypeError("iterator result is not an object"),n.delegate=null,h)}function I(t){var e={tryLoc:t[0]};1 in t&&(e.catchLoc=t[1]),2 in t&&(e.finallyLoc=t[2],e.afterLoc=t[3]),this.tryEntries.push(e)}function k(t){var e=t.completion||{};e.type="normal",delete e.arg,t.completion=e}function R(t){this.tryEntries=[{tryLoc:"root"}],t.forEach(I,this),this.reset(!0)}function T(e){if(e||""===e){var n=e[a];if(n)return n.call(e);if("function"==typeof e.next)return e;if(!isNaN(e.length)){var o=-1,i=function n(){for(;++o<e.length;)if(r.call(e,o))return n.value=e[o],n.done=!1,n;return n.value=t,n.done=!0,n};return i.next=i}}throw new TypeError(_typeof(e)+" is not iterable")}return v.prototype=b,o(C,"constructor",{value:b,configurable:!0}),o(b,"constructor",{value:v,configurable:!0}),v.displayName=l(b,c,"GeneratorFunction"),e.isGeneratorFunction=function(t){var e="function"==typeof t&&t.constructor;return!!e&&(e===v||"GeneratorFunction"===(e.displayName||e.name))},e.mark=function(t){return Object.setPrototypeOf?Object.setPrototypeOf(t,b):(t.__proto__=b,l(t,c,"GeneratorFunction")),t.prototype=Object.create(C),t},e.awrap=function(t){return{__await:t}},L(O.prototype),l(O.prototype,u,(function(){return this})),e.AsyncIterator=O,e.async=function(t,n,r,o,i){void 0===i&&(i=Promise);var a=new O(s(t,n,r,o),i);return e.isGeneratorFunction(n)?a:a.next().then((function(t){return t.done?t.value:a.next()}))},L(C),l(C,c,"Generator"),l(C,a,(function(){return this})),l(C,"toString",(function(){return"[object Generator]"})),e.keys=function(t){var e=Object(t),n=[];for(var r in e)n.push(r);return n.reverse(),function t(){for(;n.length;){var r=n.pop();if(r in e)return t.value=r,t.done=!1,t}return t.done=!0,t}},e.values=T,R.prototype={constructor:R,reset:function(e){if(this.prev=0,this.next=0,this.sent=this._sent=t,this.done=!1,this.delegate=null,this.method="next",this.arg=t,this.tryEntries.forEach(k),!e)for(var n in this)"t"===n.charAt(0)&&r.call(this,n)&&!isNaN(+n.slice(1))&&(this[n]=t)},stop:function(){this.done=!0;var t=this.tryEntries[0].completion;if("throw"===t.type)throw t.arg;return this.rval},dispatchException:function(e){if(this.done)throw e;var n=this;function o(r,o){return u.type="throw",u.arg=e,n.next=r,o&&(n.method="next",n.arg=t),!!o}for(var i=this.tryEntries.length-1;i>=0;--i){var a=this.tryEntries[i],u=a.completion;if("root"===a.tryLoc)return o("end");if(a.tryLoc<=this.prev){var c=r.call(a,"catchLoc"),l=r.call(a,"finallyLoc");if(c&&l){if(this.prev<a.catchLoc)return o(a.catchLoc,!0);if(this.prev<a.finallyLoc)return o(a.finallyLoc)}else if(c){if(this.prev<a.catchLoc)return o(a.catchLoc,!0)}else{if(!l)throw Error("try statement without catch or finally");if(this.prev<a.finallyLoc)return o(a.finallyLoc)}}}},abrupt:function(t,e){for(var n=this.tryEntries.length-1;n>=0;--n){var o=this.tryEntries[n];if(o.tryLoc<=this.prev&&r.call(o,"finallyLoc")&&this.prev<o.finallyLoc){var i=o;break}}i&&("break"===t||"continue"===t)&&i.tryLoc<=e&&e<=i.finallyLoc&&(i=null);var a=i?i.completion:{};return a.type=t,a.arg=e,i?(this.method="next",this.next=i.finallyLoc,h):this.complete(a)},complete:function(t,e){if("throw"===t.type)throw t.arg;return"break"===t.type||"continue"===t.type?this.next=t.arg:"return"===t.type?(this.rval=this.arg=t.arg,this.method="return",this.next="end"):"normal"===t.type&&e&&(this.next=e),h},finish:function(t){for(var e=this.tryEntries.length-1;e>=0;--e){var n=this.tryEntries[e];if(n.finallyLoc===t)return this.complete(n.completion,n.afterLoc),k(n),h}},catch:function(t){for(var e=this.tryEntries.length-1;e>=0;--e){var n=this.tryEntries[e];if(n.tryLoc===t){var r=n.completion;if("throw"===r.type){var o=r.arg;k(n)}return o}}throw Error("illegal catch attempt")},delegateYield:function(e,n,r){return this.delegate={iterator:T(e),resultName:n,nextLoc:r},"next"===this.method&&(this.arg=t),h}},e}function _slicedToArray(t,e){return _arrayWithHoles(t)||_iterableToArrayLimit(t,e)||_unsupportedIterableToArray(t,e)||_nonIterableRest()}function _nonIterableRest(){throw new TypeError("Invalid attempt to destructure non-iterable instance.\nIn order to be iterable, non-array objects must have a [Symbol.iterator]() method.")}function _unsupportedIterableToArray(t,e){if(t){if("string"==typeof t)return _arrayLikeToArray(t,e);var n={}.toString.call(t).slice(8,-1);return"Object"===n&&t.constructor&&(n=t.constructor.name),"Map"===n||"Set"===n?Array.from(t):"Arguments"===n||/^(?:Ui|I)nt(?:8|16|32)(?:Clamped)?Array$/.test(n)?_arrayLikeToArray(t,e):void 0}}function _arrayLikeToArray(t,e){(null==e||e>t.length)&&(e=t.length);for(var n=0,r=Array(e);n<e;n++)r[n]=t[n];return r}function _iterableToArrayLimit(t,e){var n=null==t?null:"undefined"!=typeof Symbol&&t[Symbol.iterator]||t["@@iterator"];if(null!=n){var r,o,i,a,u=[],c=!0,l=!1;try{if(i=(n=n.call(t)).next,0===e){if(Object(n)!==n)return;c=!1}else for(;!(c=(r=i.call(n)).done)&&(u.push(r.value),u.length!==e);c=!0);}catch(t){l=!0,o=t}finally{try{if(!c&&null!=n.return&&(a=n.return(),Object(a)!==a))return}finally{if(l)throw o}}return u}}function _arrayWithHoles(t){if(Array.isArray(t))return t}function asyncGeneratorStep(t,e,n,r,o,i,a){try{var u=t[i](a),c=u.value}catch(t){return void n(t)}u.done?e(c):Promise.resolve(c).then(r,o)}function _asyncToGenerator(t){return function(){var e=this,n=arguments;return new Promise((function(r,o){var i=t.apply(e,n);function a(t){asyncGeneratorStep(i,r,o,a,u,"next",t)}function u(t){asyncGeneratorStep(i,r,o,a,u,"throw",t)}a(void 0)}))}}import"./_dashboard.css";import"./reset.css";import{toggleElementVisibility,getHoursAndMinutes,getMilliseconds,handleVoltageChange,handleTimeRangeChange,handleModeChange,getModeString,setColorFromMode,updateVisibility,KEY_CONFIG,VOLT_RECEIVE_FROM_SERVER}from"./util.js";import{Countdown}from"./countDown.js";import{ControlData,TimeRange}from"../protoc/js/pump_control_data.js";import{Num}from"../protoc/js/str_num_msg.js";import TimePicker from"tui-time-picker";import"tui-time-picker/dist/tui-time-picker.css";import notificationSound from"./notify.mp3";var updateChartFunction,updateMinVoltCutOffFunction,PowerStatus={POWER_INACTIVE:0,POWER_READY:1,POWER_RUNNING:2,POWER_RESTING:3,POWER_VOLTAGE_LOW:4};document.addEventListener("DOMContentLoaded",(function(){var t,e,n,r=document.getElementById("voltage"),o=document.getElementById("voltageChart"),i=document.querySelectorAll(".lds-ellipsis"),a=document.querySelectorAll(".init-hide"),u=document.getElementById("min-volt"),c=document.getElementById("pump-power-indicator"),l=document.getElementById("run-time"),s=document.getElementById("rest-time"),d=document.getElementById("machine-mode"),f=document.querySelectorAll(".styled-button"),y=document.getElementById("mode"),m=document.getElementById("run-rest-state"),p=document.getElementById("timer"),h=document.getElementById("min-voltage-input"),g=document.getElementById("min-voltage-slider"),v=document.getElementById("mode-bg"),b=!1,E=new Countdown("#run-rest-ing",0),w={initialHour:0,initialMinute:0,inputType:"selectbox",showMeridiem:!1};E.update(0);var _={hour:0,minute:4},C=new TimePicker("#running-time",w);C.setRange(_,{hour:2,minute:0});var L=new TimePicker("#resting-time",w);L.setRange(_,{hour:23,minute:59});var O=function(){var t=_asyncToGenerator(_regeneratorRuntime().mark((function t(){var e,n,r,i,a;return _regeneratorRuntime().wrap((function(t){for(;;)switch(t.prev=t.next){case 0:if(updateChartFunction&&updateMinVoltCutOffFunction){t.next=12;break}return t.next=3,Promise.all([import("./chartDisplay.js")]);case 3:e=t.sent,n=_slicedToArray(e,1),r=n[0],i=r.updateChart,a=r.updateMinVoltCutOff,updateChartFunction=i,updateMinVoltCutOffFunction=a,document.getElementById("voltageChart").style.display="block",toggleElementVisibility(o,"show");case 12:case"end":return t.stop()}}),t)})));return function(){return t.apply(this,arguments)}}(),S=document.getElementById("value-heading"),x=!0,I=0,k=function(t){var e=t.running,n=t.resting,r=getHoursAndMinutes(e),o=r.hours,i=r.minutes,a=getHoursAndMinutes(n),u=a.hours,c=a.minutes;l.textContent="".concat(o,":").concat(i.toString().padStart(2,"0")),s.textContent="".concat(u,":").concat(c.toString().padStart(2,"0")),C.setTime(o,i,!1),L.setTime(u,c,!1)},R=function(){updateVisibility(i,"block"),updateVisibility(a,"none"),updateVisibility(f,"none"),c.style.display="none"},T=function(){n&&clearInterval(n),b||(n=setInterval((function(){t.readyState===WebSocket.CLOSED?j():clearInterval(n)}),3e3))},A=function(){e&&clearTimeout(e),e=setTimeout((function(){t&&(R(),t.close(),T())}),2e3)},V=function(t){t[0]},M=0,N=document.getElementById("header");window.addEventListener("scroll",(function(){var t=window.scrollY||document.documentElement.scrollB;N.style.top=t<M?"-80px":"0",M=t}));var F=new Audio(notificationSound);F.addEventListener("canplaythrough",(function(){}),{once:!0}),F.addEventListener("error",(function(t){console.error("Audio playback error:",t)}));var P=[V,function(t){try{var e=Num.decode(t.slice(1));!function(t,e){switch(t){case VOLT_RECEIVE_FROM_SERVER.VOLTAGE:r.textContent=e.toFixed(1),updateChartFunction(e);var n=e<I;x!==n&&(S.style.color=n?"#ff6384":"#36a2eb",x=n);break;case VOLT_RECEIVE_FROM_SERVER.MIN_VOLTAGE:case KEY_CONFIG.MIN_VOLT:updateMinVoltCutOffFunction(e,!0),u.textContent=e,h.value=e,g.value=e,I=e;break;case KEY_CONFIG.CONFIG_MODE:d.textContent=getModeString(e),setColorFromMode(e,v),y.value=e}}(e.key,e.value)}catch(t){console.error("Failed to deserialize Num:",t)}},V,V,function(t){try{var e=ControlData.decode(t.slice(1));k(e.time_range),d.textContent=getModeString(e.mode),setColorFromMode(e.mode,v),y.value=e.mode}catch(t){console.error("Failed to deserialize ControlData:",t)}},function(t){try{var e=TimeRange.decode(t.slice(1));k(e)}catch(t){console.error("Failed to deserialize TimeRange:",t)}},V,V,function(t){var e=Num.decode(t.slice(1)),n=e.key,r=e.value,o=Number(r),i="block";switch(c.classList.remove("blinking"),n){case PowerStatus.POWER_INACTIVE:c.style.backgroundColor="#808080",m.textContent="Inactive",i="none",o=1;break;case PowerStatus.POWER_READY:c.classList.add("blinking"),m.textContent="Ready...";break;case PowerStatus.POWER_RUNNING:c.style.backgroundColor="#32CD32",m.textContent="Running...";break;case PowerStatus.POWER_RESTING:c.style.backgroundColor="#1E90FF",m.textContent="Resting...";break;case PowerStatus.POWER_VOLTAGE_LOW:c.style.backgroundColor="#FF0000",m.textContent="Voltage Low",i="none",o=1;break;default:o=1,i="none",m.textContent="..."}p.style.display=i,E.update(o),F.play().catch((function(t){return console.error("Audio play error:",t)})),"vibrate"in navigator&&navigator.vibrate([200,100,200])}],G=function(){var t=_asyncToGenerator(_regeneratorRuntime().mark((function t(e){var n;return _regeneratorRuntime().wrap((function(t){for(;;)switch(t.prev=t.next){case 0:return n=e[0],t.next=3,O();case 3:P[n]&&P[n](e);case 4:case"end":return t.stop()}}),t)})));return function(e){return t.apply(this,arguments)}}(),j=function(){var e="https:"===window.location.protocol?"wss:":"ws:",n="".concat(e,"//").concat(window.location.host,"/ws");(t=new WebSocket(n)).onopen=function(){updateVisibility(i,"none"),updateVisibility(a,"block"),updateVisibility(f,"block"),c.style.display="block",A()},t.onmessage=function(){var t=_asyncToGenerator(_regeneratorRuntime().mark((function t(e){var n,r;return _regeneratorRuntime().wrap((function(t){for(;;)switch(t.prev=t.next){case 0:return A(),t.prev=1,t.next=4,e.data.arrayBuffer();case 4:n=t.sent,r=new Uint8Array(n),G(r),t.next=12;break;case 9:t.prev=9,t.t0=t.catch(1),console.error("Error handling WebSocket message:",t.t0);case 12:case"end":return t.stop()}}),t,null,[[1,9]])})));return function(e){return t.apply(this,arguments)}}(),t.onclose=function(){r.textContent="00.0",R(),T()},t.onerror=function(t){r.textContent="00.0",console.error("WebSocket error:",t),R(),b=!0}},B=document.getElementById("config-group"),W=B.querySelectorAll(".config"),D=Array.from(document.querySelectorAll(".open-config"));document.addEventListener("click",(function(e){var n=e.target;if(n.matches("#submit-btn-min-volt")){var r=parseInt(h.value,10);if(isNaN(r)||r<110||r>230)return;handleVoltageChange(r,t),B.style.display="none"}else if(n.matches("#submit-time-range-btn")){var o=getMilliseconds({hours:C.getHour(),minutes:C.getMinute()}),i=getMilliseconds({hours:L.getHour(),minutes:L.getMinute()});handleTimeRangeChange(o,i,t),B.style.display="none"}else if(n.matches(".open-config")){var a=D.indexOf(n);if(-1===a)return;B.style.display="flex",updateVisibility(W,"none"),W[a].style.display="flex"}else n.matches("#cancel-config-ui")?B.style.display="none":n.matches("#config-group")?n.style.display="none":n.matches("#logout")&&(window.location.href="/logout")})),document.getElementById("voltage-setting").addEventListener("input",(function(t){var e=t.target,n=e.value,r=e.id,o=parseInt(n,10),i=document.getElementById("voltage-label");o<110||o>230?i.innerHTML='Set Minimum Operating Voltage (110-230 V) <br> <span id="error">— Please enter a valid value!</span>':(i.innerHTML="Set Minimum Operating Voltage (110-230 V)","min-voltage-input"===r?g.value=o:"min-voltage-slider"===r&&(h.value=o))})),y.addEventListener("change",(function(e){t.readyState===WebSocket.OPEN?handleModeChange(e.target.value,t):console.error("WebSocket is not open"),B.style.display="none"})),setTimeout((function(){j()}),1e3)}));