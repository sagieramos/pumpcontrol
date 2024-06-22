/*! For license information please see bundle.js.LICENSE.txt */
(()=>{"use strict";var n={248:(n,t,e)=>{e.d(t,{A:()=>s});var r=e(601),o=e.n(r),i=e(314),a=e.n(i)()(o());a.push([n.id,":root {\n  --max-width: 512px;\n}\n\n*,\n*::after,\n*::before {\n  box-sizing: border-box;\n  margin: 0;\n  padding: 0;\n  font-family: 'Roboto', sans-serif;\n  outline: none;\n  user-select: none;\n  text-decoration: none;\n  z-index: auto;\n\n  /* white-space: nowrap; */\n/*   vertical-align: middle;\n  line-height: normal;\n  list-style: none;\n  letter-spacing: inherit;\n  word-break: break-word;\n  overflow-x: hidden;\n  touch-action: manipulation; */\n}\n\nhtml {\n  height: 100%;\n}\n\nbody {\n  background: linear-gradient(#141e30, #243b55);\n  font-size: 16px;\n  margin: 0;\n  padding: 0;\n}\n\nnav {\n  width: 100%;\n  min-width: 350px;\n}\n\n#logo {\n  display: flex;\n  justify-content:flex-start;\n  gap: 12px;\n  align-items: center;\n}\n\nspan#logo-text {\n  color: #F28C28;\n  font-weight: bold;\n  text-transform: uppercase;\n  letter-spacing: 3px;\n  font-size: 1rem;\n  text-align: end;\n}\n\n.login-box {\n  position: absolute;\n  top: 50%;\n  left: 50%;\n  width: 84%;\n  max-width: 400px;\n  min-width: 350px;\n  padding: 40px;\n  transform: translate(-50%, -50%);\n  background: #00000080;\n  box-sizing: border-box;\n  box-shadow: 0 15px 25px rgba(0, 0, 0, 0.6);\n  border-radius: 10px;\n}\n\nheader {\n  position: absolute;\n  width: 100%;\n  padding: 8px 24px;\n  border-bottom: 1px solid #03e8f42d;\n  /* background: linear-gradient(#141e30, #243b55); */\n  background-color: #141e30;\n  display: flex;\n  justify-content: center;\n  align-items: center;\n}\n\n.login-box h2 {\n  margin: 0 0 30px;\n  padding: 0;\n  color: #fff;\n  text-align: center;\n}\n\n.login-box .user-box {\n  position: relative;\n}\n\n.login-box .user-box input {\n  width: 100%;\n  padding: 10px 0;\n  font-size: 16px;\n  color: #fff;\n  margin-bottom: 40px;\n  border: none;\n  border-bottom: 1px solid #03e9f4;\n  outline: none;\n  background: transparent;\n  text-align: center;\n}\n\n.login-box .user-box label {\n  position: absolute;\n  top: 0;\n  left: 0;\n  padding: 10px 0;\n  font-size: 16px;\n  color: #03e9f4;\n  pointer-events: none;\n  transition: 0.5s;\n}\n\n.login-box .user-box input:focus ~ label {\n  top: 40px;\n  left: 0;\n  color: #F28C28;\n  font-size: 12px;\n}\n\n.login-box .user-box input:valid ~ label {\n  top: 40px;\n  left: 0;\n  color: #66FF00;\n  font-size: 12px;\n}\n\n.login-box form {\n    display: flex;\n    flex-direction: column;\n    justify-content: center;\n}\n\n.login-box form button {\n  position: relative;\n  display: inline-block;\n  background: none;\n  padding: 5px 10px;\n  color: #03e9f4;\n  font-weight: bolder;\n  font-size: 12px;\n  border: solid 1px #03e9f4;\n  text-transform: uppercase;\n  overflow: hidden;\n  margin-top: 40px;\n  letter-spacing: 4px;\n  cursor: default;\n  align-self: center;\n  transition: background-color 0.3s, color 0.3s;\n}\n\n.login-box button:active {\n  background: #03e9f4;\n  color: #fff;\n  border-radius: 5px;\n  box-shadow: 0 0 5px #03e9f4,\n              0 0 25px #03e9f4,\n              0 0 50px #03e9f4,\n              0 0 100px #03e9f4;\n}\n\n@keyframes shake {\n  0% { transform: translateX(0); }\n  25% { transform: translateX(-5px); }\n  50% { transform: translateX(5px); }\n  75% { transform: translateX(-5px); }\n  100% { transform: translateX(0); }\n}\n\n.login-box button span {\n  position: absolute;\n  display: block;\n}\n\n.loading-anim span:nth-child(1) {\n  top: 0;\n  left: -100%;\n  width: 100%;\n  height: 2px;\n  background: linear-gradient(90deg, transparent, #03e9f4);\n  animation: btn-anim1 1s linear infinite;\n}\n\n@keyframes btn-anim1 {\n  0% {\n    left: -100%;\n  }\n\n  50%,\n  100% {\n    left: 100%;\n  }\n}\n\n.loading-anim span:nth-child(2) {\n  top: -100%;\n  right: 0;\n  width: 2px;\n  height: 100%;\n  background: linear-gradient(180deg, transparent, #03e9f4);\n  animation: btn-anim2 1s linear infinite;\n  animation-delay: 0.25s;\n}\n\n@keyframes btn-anim2 {\n  0% {\n    top: -100%;\n  }\n\n  50%,\n  100% {\n    top: 100%;\n  }\n}\n\n.loading-anim span:nth-child(3) {\n  bottom: 0;\n  right: -100%;\n  width: 100%;\n  height: 2px;\n  background: linear-gradient(270deg, transparent, #03e9f4);\n  animation: btn-anim3 1s linear infinite;\n  animation-delay: 0.5s;\n}\n\n@keyframes btn-anim3 {\n  0% {\n    right: -100%;\n  }\n\n  50%,\n  100% {\n    right: 100%;\n  }\n}\n\n.loading-anim span:nth-child(4) {\n  bottom: -100%;\n  left: 0;\n  width: 2px;\n  height: 100%;\n  background: linear-gradient(360deg, transparent, #03e9f4);\n  animation: btn-anim4 1s linear infinite;\n  animation-delay: 0.75s;\n}\n\n@keyframes btn-anim4 {\n  0% {\n    bottom: -100%;\n  }\n\n  50%,\n  100% {\n    bottom: 100%;\n  }\n}\n\n.pass-label {\n  color: #c1aba6;\n}\n\n.flash-message {\n  position: absolute;\n  top: 70px;\n  left: 50%;\n  transform: translateX(-50%);\n  width: 100%;\n  max-width: 400px;\n  min-width: 350px;\n  padding: 8px 24px;\n  color: #FA5F55;\n  text-align: center;\n  border-radius: 5px;\n  display: flex;\n  justify-content: center;\n  align-items: center;\n}\n\n.flash-message img {\n  width: 16px;\n  height: 16px;\n  margin-right: 5px;\n}\n\n.flash-message span {\n  font-size: 0.8rem;\n}\n\n.footer-content {\n  position: absolute;\n  bottom: 0;\n  width: 100%;\n  padding: 8px 24px;\n  border-top: 1px solid #03e8f42d;\n  color: #fff;\n  display: flex;\n  justify-content: center;\n  align-items: center;\n}\n\n.footer-content img {\n  width: 24px;\n  height: 24px;\n  margin-right: 8px;\n}\n\n@media screen and (max-width: 768px) {\n  .footer-content {\n    font-size: 0.8rem;\n  }\n}",""]);const s=a},314:n=>{n.exports=function(n){var t=[];return t.toString=function(){return this.map((function(t){var e="",r=void 0!==t[5];return t[4]&&(e+="@supports (".concat(t[4],") {")),t[2]&&(e+="@media ".concat(t[2]," {")),r&&(e+="@layer".concat(t[5].length>0?" ".concat(t[5]):""," {")),e+=n(t),r&&(e+="}"),t[2]&&(e+="}"),t[4]&&(e+="}"),e})).join("")},t.i=function(n,e,r,o,i){"string"==typeof n&&(n=[[null,n,void 0]]);var a={};if(r)for(var s=0;s<this.length;s++){var c=this[s][0];null!=c&&(a[c]=!0)}for(var l=0;l<n.length;l++){var u=[].concat(n[l]);r&&a[u[0]]||(void 0!==i&&(void 0===u[5]||(u[1]="@layer".concat(u[5].length>0?" ".concat(u[5]):""," {").concat(u[1],"}")),u[5]=i),e&&(u[2]?(u[1]="@media ".concat(u[2]," {").concat(u[1],"}"),u[2]=e):u[2]=e),o&&(u[4]?(u[1]="@supports (".concat(u[4],") {").concat(u[1],"}"),u[4]=o):u[4]="".concat(o)),t.push(u))}},t}},601:n=>{n.exports=function(n){return n[1]}},72:n=>{var t=[];function e(n){for(var e=-1,r=0;r<t.length;r++)if(t[r].identifier===n){e=r;break}return e}function r(n,r){for(var i={},a=[],s=0;s<n.length;s++){var c=n[s],l=r.base?c[0]+r.base:c[0],u=i[l]||0,f="".concat(l," ").concat(u);i[l]=u+1;var p=e(f),d={css:c[1],media:c[2],sourceMap:c[3],supports:c[4],layer:c[5]};if(-1!==p)t[p].references++,t[p].updater(d);else{var h=o(d,r);r.byIndex=s,t.splice(s,0,{identifier:f,updater:h,references:1})}a.push(f)}return a}function o(n,t){var e=t.domAPI(t);return e.update(n),function(t){if(t){if(t.css===n.css&&t.media===n.media&&t.sourceMap===n.sourceMap&&t.supports===n.supports&&t.layer===n.layer)return;e.update(n=t)}else e.remove()}}n.exports=function(n,o){var i=r(n=n||[],o=o||{});return function(n){n=n||[];for(var a=0;a<i.length;a++){var s=e(i[a]);t[s].references--}for(var c=r(n,o),l=0;l<i.length;l++){var u=e(i[l]);0===t[u].references&&(t[u].updater(),t.splice(u,1))}i=c}}},659:n=>{var t={};n.exports=function(n,e){var r=function(n){if(void 0===t[n]){var e=document.querySelector(n);if(window.HTMLIFrameElement&&e instanceof window.HTMLIFrameElement)try{e=e.contentDocument.head}catch(n){e=null}t[n]=e}return t[n]}(n);if(!r)throw new Error("Couldn't find a style target. This probably means that the value for the 'insert' parameter is invalid.");r.appendChild(e)}},540:n=>{n.exports=function(n){var t=document.createElement("style");return n.setAttributes(t,n.attributes),n.insert(t,n.options),t}},56:(n,t,e)=>{n.exports=function(n){var t=e.nc;t&&n.setAttribute("nonce",t)}},825:n=>{n.exports=function(n){if("undefined"==typeof document)return{update:function(){},remove:function(){}};var t=n.insertStyleElement(n);return{update:function(e){!function(n,t,e){var r="";e.supports&&(r+="@supports (".concat(e.supports,") {")),e.media&&(r+="@media ".concat(e.media," {"));var o=void 0!==e.layer;o&&(r+="@layer".concat(e.layer.length>0?" ".concat(e.layer):""," {")),r+=e.css,o&&(r+="}"),e.media&&(r+="}"),e.supports&&(r+="}");var i=e.sourceMap;i&&"undefined"!=typeof btoa&&(r+="\n/*# sourceMappingURL=data:application/json;base64,".concat(btoa(unescape(encodeURIComponent(JSON.stringify(i))))," */")),t.styleTagTransform(r,n,t.options)}(t,n,e)},remove:function(){!function(n){if(null===n.parentNode)return!1;n.parentNode.removeChild(n)}(t)}}}},113:n=>{n.exports=function(n,t){if(t.styleSheet)t.styleSheet.cssText=n;else{for(;t.firstChild;)t.removeChild(t.firstChild);t.appendChild(document.createTextNode(n))}}}},t={};function e(r){var o=t[r];if(void 0!==o)return o.exports;var i=t[r]={id:r,exports:{}};return n[r](i,i.exports,e),i.exports}e.n=n=>{var t=n&&n.__esModule?()=>n.default:()=>n;return e.d(t,{a:t}),t},e.d=(n,t)=>{for(var r in t)e.o(t,r)&&!e.o(n,r)&&Object.defineProperty(n,r,{enumerable:!0,get:t[r]})},e.o=(n,t)=>Object.prototype.hasOwnProperty.call(n,t),e.nc=void 0;var r=e(72),o=e.n(r),i=e(825),a=e.n(i),s=e(659),c=e.n(s),l=e(56),u=e.n(l),f=e(540),p=e.n(f),d=e(113),h=e.n(d),g=e(248),m={};function y(n){return y="function"==typeof Symbol&&"symbol"==typeof Symbol.iterator?function(n){return typeof n}:function(n){return n&&"function"==typeof Symbol&&n.constructor===Symbol&&n!==Symbol.prototype?"symbol":typeof n},y(n)}function v(){v=function(){return t};var n,t={},e=Object.prototype,r=e.hasOwnProperty,o=Object.defineProperty||function(n,t,e){n[t]=e.value},i="function"==typeof Symbol?Symbol:{},a=i.iterator||"@@iterator",s=i.asyncIterator||"@@asyncIterator",c=i.toStringTag||"@@toStringTag";function l(n,t,e){return Object.defineProperty(n,t,{value:e,enumerable:!0,configurable:!0,writable:!0}),n[t]}try{l({},"")}catch(n){l=function(n,t,e){return n[t]=e}}function u(n,t,e,r){var i=t&&t.prototype instanceof b?t:b,a=Object.create(i.prototype),s=new A(r||[]);return o(a,"_invoke",{value:S(n,e,s)}),a}function f(n,t,e){try{return{type:"normal",arg:n.call(t,e)}}catch(n){return{type:"throw",arg:n}}}t.wrap=u;var p="suspendedStart",d="suspendedYield",h="executing",g="completed",m={};function b(){}function x(){}function w(){}var E={};l(E,a,(function(){return this}));var k=Object.getPrototypeOf,L=k&&k(k(F([])));L&&L!==e&&r.call(L,a)&&(E=L);var j=w.prototype=b.prototype=Object.create(E);function T(n){["next","throw","return"].forEach((function(t){l(n,t,(function(n){return this._invoke(t,n)}))}))}function O(n,t){function e(o,i,a,s){var c=f(n[o],n,i);if("throw"!==c.type){var l=c.arg,u=l.value;return u&&"object"==y(u)&&r.call(u,"__await")?t.resolve(u.__await).then((function(n){e("next",n,a,s)}),(function(n){e("throw",n,a,s)})):t.resolve(u).then((function(n){l.value=n,a(l)}),(function(n){return e("throw",n,a,s)}))}s(c.arg)}var i;o(this,"_invoke",{value:function(n,r){function o(){return new t((function(t,o){e(n,r,t,o)}))}return i=i?i.then(o,o):o()}})}function S(t,e,r){var o=p;return function(i,a){if(o===h)throw Error("Generator is already running");if(o===g){if("throw"===i)throw a;return{value:n,done:!0}}for(r.method=i,r.arg=a;;){var s=r.delegate;if(s){var c=I(s,r);if(c){if(c===m)continue;return c}}if("next"===r.method)r.sent=r._sent=r.arg;else if("throw"===r.method){if(o===p)throw o=g,r.arg;r.dispatchException(r.arg)}else"return"===r.method&&r.abrupt("return",r.arg);o=h;var l=f(t,e,r);if("normal"===l.type){if(o=r.done?g:d,l.arg===m)continue;return{value:l.arg,done:r.done}}"throw"===l.type&&(o=g,r.method="throw",r.arg=l.arg)}}}function I(t,e){var r=e.method,o=t.iterator[r];if(o===n)return e.delegate=null,"throw"===r&&t.iterator.return&&(e.method="return",e.arg=n,I(t,e),"throw"===e.method)||"return"!==r&&(e.method="throw",e.arg=new TypeError("The iterator does not provide a '"+r+"' method")),m;var i=f(o,t.iterator,e.arg);if("throw"===i.type)return e.method="throw",e.arg=i.arg,e.delegate=null,m;var a=i.arg;return a?a.done?(e[t.resultName]=a.value,e.next=t.nextLoc,"return"!==e.method&&(e.method="next",e.arg=n),e.delegate=null,m):a:(e.method="throw",e.arg=new TypeError("iterator result is not an object"),e.delegate=null,m)}function P(n){var t={tryLoc:n[0]};1 in n&&(t.catchLoc=n[1]),2 in n&&(t.finallyLoc=n[2],t.afterLoc=n[3]),this.tryEntries.push(t)}function _(n){var t=n.completion||{};t.type="normal",delete t.arg,n.completion=t}function A(n){this.tryEntries=[{tryLoc:"root"}],n.forEach(P,this),this.reset(!0)}function F(t){if(t||""===t){var e=t[a];if(e)return e.call(t);if("function"==typeof t.next)return t;if(!isNaN(t.length)){var o=-1,i=function e(){for(;++o<t.length;)if(r.call(t,o))return e.value=t[o],e.done=!1,e;return e.value=n,e.done=!0,e};return i.next=i}}throw new TypeError(y(t)+" is not iterable")}return x.prototype=w,o(j,"constructor",{value:w,configurable:!0}),o(w,"constructor",{value:x,configurable:!0}),x.displayName=l(w,c,"GeneratorFunction"),t.isGeneratorFunction=function(n){var t="function"==typeof n&&n.constructor;return!!t&&(t===x||"GeneratorFunction"===(t.displayName||t.name))},t.mark=function(n){return Object.setPrototypeOf?Object.setPrototypeOf(n,w):(n.__proto__=w,l(n,c,"GeneratorFunction")),n.prototype=Object.create(j),n},t.awrap=function(n){return{__await:n}},T(O.prototype),l(O.prototype,s,(function(){return this})),t.AsyncIterator=O,t.async=function(n,e,r,o,i){void 0===i&&(i=Promise);var a=new O(u(n,e,r,o),i);return t.isGeneratorFunction(e)?a:a.next().then((function(n){return n.done?n.value:a.next()}))},T(j),l(j,c,"Generator"),l(j,a,(function(){return this})),l(j,"toString",(function(){return"[object Generator]"})),t.keys=function(n){var t=Object(n),e=[];for(var r in t)e.push(r);return e.reverse(),function n(){for(;e.length;){var r=e.pop();if(r in t)return n.value=r,n.done=!1,n}return n.done=!0,n}},t.values=F,A.prototype={constructor:A,reset:function(t){if(this.prev=0,this.next=0,this.sent=this._sent=n,this.done=!1,this.delegate=null,this.method="next",this.arg=n,this.tryEntries.forEach(_),!t)for(var e in this)"t"===e.charAt(0)&&r.call(this,e)&&!isNaN(+e.slice(1))&&(this[e]=n)},stop:function(){this.done=!0;var n=this.tryEntries[0].completion;if("throw"===n.type)throw n.arg;return this.rval},dispatchException:function(t){if(this.done)throw t;var e=this;function o(r,o){return s.type="throw",s.arg=t,e.next=r,o&&(e.method="next",e.arg=n),!!o}for(var i=this.tryEntries.length-1;i>=0;--i){var a=this.tryEntries[i],s=a.completion;if("root"===a.tryLoc)return o("end");if(a.tryLoc<=this.prev){var c=r.call(a,"catchLoc"),l=r.call(a,"finallyLoc");if(c&&l){if(this.prev<a.catchLoc)return o(a.catchLoc,!0);if(this.prev<a.finallyLoc)return o(a.finallyLoc)}else if(c){if(this.prev<a.catchLoc)return o(a.catchLoc,!0)}else{if(!l)throw Error("try statement without catch or finally");if(this.prev<a.finallyLoc)return o(a.finallyLoc)}}}},abrupt:function(n,t){for(var e=this.tryEntries.length-1;e>=0;--e){var o=this.tryEntries[e];if(o.tryLoc<=this.prev&&r.call(o,"finallyLoc")&&this.prev<o.finallyLoc){var i=o;break}}i&&("break"===n||"continue"===n)&&i.tryLoc<=t&&t<=i.finallyLoc&&(i=null);var a=i?i.completion:{};return a.type=n,a.arg=t,i?(this.method="next",this.next=i.finallyLoc,m):this.complete(a)},complete:function(n,t){if("throw"===n.type)throw n.arg;return"break"===n.type||"continue"===n.type?this.next=n.arg:"return"===n.type?(this.rval=this.arg=n.arg,this.method="return",this.next="end"):"normal"===n.type&&t&&(this.next=t),m},finish:function(n){for(var t=this.tryEntries.length-1;t>=0;--t){var e=this.tryEntries[t];if(e.finallyLoc===n)return this.complete(e.completion,e.afterLoc),_(e),m}},catch:function(n){for(var t=this.tryEntries.length-1;t>=0;--t){var e=this.tryEntries[t];if(e.tryLoc===n){var r=e.completion;if("throw"===r.type){var o=r.arg;_(e)}return o}}throw Error("illegal catch attempt")},delegateYield:function(t,e,r){return this.delegate={iterator:F(t),resultName:e,nextLoc:r},"next"===this.method&&(this.arg=n),m}},t}function b(n,t,e,r,o,i,a){try{var s=n[i](a),c=s.value}catch(n){return void e(n)}s.done?t(c):Promise.resolve(c).then(r,o)}m.styleTagTransform=h(),m.setAttributes=u(),m.insert=c().bind(null,"head"),m.domAPI=a(),m.insertStyleElement=p(),o()(g.A,m),g.A&&g.A.locals&&g.A.locals,document.addEventListener("DOMContentLoaded",(function(){var n=document.getElementById("login-form"),t=document.getElementById("passcode"),e=document.getElementById("pass-lebel"),r=document.getElementById("flash-message"),o=document.getElementById("loading-anim"),i=document.getElementById("flash-text"),a=document.getElementById("warning-icon"),s=function(n){i.innerText=n,r.style.display="flex",a.style.animation="shake 0.5s",a.addEventListener("animationend",(function(){a.style.animation=""})),o.classList.remove("loading-anim"),t.disabled=!1,e.innerText="Enter 4-digit pin"},c=function(n){n?(o.classList.add("loading-anim"),t.disabled=!0,o.disabled=!0,e.innerText="Authenticating..."):(o.classList.remove("loading-anim"),t.disabled=!1,o.disabled=!1,e.innerText="Enter 4-digit pin")},l=function(){var e,r=(e=v().mark((function e(){var r,o,i,a,l;return v().wrap((function(e){for(;;)switch(e.prev=e.next){case 0:if(r=t.value,o=new FormData(n),/^\d{4}$/.test(r)){e.next=6;break}return s("Invalid PIN. Please enter a 4-digit PIN."),e.abrupt("return");case 6:return c(!0),e.prev=7,e.next=10,fetch("/login",{method:"POST",body:o,credentials:"include"});case 10:if(i=e.sent,!(l=i.headers.get("Content-Type"))||!l.includes("application/json")){e.next=18;break}return e.next=15,i.json();case 15:a=e.sent,e.next=21;break;case 18:return e.next=20,i.text();case 20:a=e.sent;case 21:e.t0=i.status,e.next=200===e.t0?24:401===e.t0?26:302===e.t0?28:30;break;case 24:return window.location.href="/dashboard",e.abrupt("break",33);case 26:return s("string"==typeof a?a:a.message||"Unauthorized"),e.abrupt("break",33);case 28:return s("string"==typeof a?a:a.message||"Server Unavailable."),e.abrupt("break",33);case 30:return s("string"==typeof a?a:a.message||"Faulty Request. Please try again."),t.value="",e.abrupt("break",33);case 33:e.next=39;break;case 35:e.prev=35,e.t1=e.catch(7),console.error("Error:",e.t1),s("An error occurred. Please try again.");case 39:return e.prev=39,c(!1),e.finish(39);case 42:case"end":return e.stop()}}),e,null,[[7,35,39,42]])})),function(){var n=this,t=arguments;return new Promise((function(r,o){var i=e.apply(n,t);function a(n){b(i,r,o,a,s,"next",n)}function s(n){b(i,r,o,a,s,"throw",n)}a(void 0)}))});return function(){return r.apply(this,arguments)}}();n.addEventListener("submit",(function(n){n.preventDefault(),l()})),t.addEventListener("input",(function(){t.value=t.value.replace(/\D/g,""),t.setCustomValidity("")})),t.addEventListener("keydown",(function(n){"Enter"===n.key&&(n.preventDefault(),l())})),t.addEventListener("invalid",(function(n){n.preventDefault()}))}))})();