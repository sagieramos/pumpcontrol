(()=>{"use strict";var n={675:(n,e,t)=>{t.d(e,{A:()=>c});var o=t(601),r=t.n(o),a=t(314),i=t.n(a)()(r());i.push([n.id,"body {\n  background: linear-gradient(#141e30, #243b55);\n  font-size: 16px;\n  margin: 0;\n  padding: 0;\n}\n\nheader {\n    position: absolute;\n    width: 100%;\n    padding: 8px 24px;\n    border-bottom: 1px solid #03e8f42d;\n    /* background: linear-gradient(#141e30, #243b55); */\n    background-color: #141e30;\n    display: flex;\n    justify-content: center;\n    align-items: center;\n  }\n\n  nav {\n    width: 100%;\n    min-width: 350px;\n  }\n  \n  #logo {\n    display: flex;\n    justify-content:flex-start;\n    gap: 12px;\n    align-items: center;\n  }\n  \n  span#logo-text {\n    color: #F28C28;\n    font-weight: bold;\n    text-transform: uppercase;\n    letter-spacing: 3px;\n    font-size: 1rem;\n    text-align: end;\n  }\n  \n\n#container {\n    padding: 20px;\n    border-radius: 8px;\n    text-align: center;\n}\n\nh1 {\n    margin-bottom: 20px;\n}\n\n#status {\n    margin-bottom: 20px;\n}\n\n#controls {\n    display: flex;\n    flex-direction: column;\n    align-items: center;\n}\n\n#controls label {\n    margin: 5px 0;\n}\n\n#controls input {\n    margin-bottom: 10px;\n    padding: 5px;\n    width: 150px;\n    box-sizing: border-box;\n}\n\n#controls button {\n    padding: 10px 20px;\n    border: none;\n    border-radius: 4px;\n    background-color: #007bff;\n    color: white;\n    cursor: pointer;\n}\n\n#controls button:hover {\n    background-color: #0056b3;\n}\n",""]);const c=i},314:n=>{n.exports=function(n){var e=[];return e.toString=function(){return this.map((function(e){var t="",o=void 0!==e[5];return e[4]&&(t+="@supports (".concat(e[4],") {")),e[2]&&(t+="@media ".concat(e[2]," {")),o&&(t+="@layer".concat(e[5].length>0?" ".concat(e[5]):""," {")),t+=n(e),o&&(t+="}"),e[2]&&(t+="}"),e[4]&&(t+="}"),t})).join("")},e.i=function(n,t,o,r,a){"string"==typeof n&&(n=[[null,n,void 0]]);var i={};if(o)for(var c=0;c<this.length;c++){var s=this[c][0];null!=s&&(i[s]=!0)}for(var d=0;d<n.length;d++){var u=[].concat(n[d]);o&&i[u[0]]||(void 0!==a&&(void 0===u[5]||(u[1]="@layer".concat(u[5].length>0?" ".concat(u[5]):""," {").concat(u[1],"}")),u[5]=a),t&&(u[2]?(u[1]="@media ".concat(u[2]," {").concat(u[1],"}"),u[2]=t):u[2]=t),r&&(u[4]?(u[1]="@supports (".concat(u[4],") {").concat(u[1],"}"),u[4]=r):u[4]="".concat(r)),e.push(u))}},e}},601:n=>{n.exports=function(n){return n[1]}},72:n=>{var e=[];function t(n){for(var t=-1,o=0;o<e.length;o++)if(e[o].identifier===n){t=o;break}return t}function o(n,o){for(var a={},i=[],c=0;c<n.length;c++){var s=n[c],d=o.base?s[0]+o.base:s[0],u=a[d]||0,l="".concat(d," ").concat(u);a[d]=u+1;var p=t(l),f={css:s[1],media:s[2],sourceMap:s[3],supports:s[4],layer:s[5]};if(-1!==p)e[p].references++,e[p].updater(f);else{var m=r(f,o);o.byIndex=c,e.splice(c,0,{identifier:l,updater:m,references:1})}i.push(l)}return i}function r(n,e){var t=e.domAPI(e);return t.update(n),function(e){if(e){if(e.css===n.css&&e.media===n.media&&e.sourceMap===n.sourceMap&&e.supports===n.supports&&e.layer===n.layer)return;t.update(n=e)}else t.remove()}}n.exports=function(n,r){var a=o(n=n||[],r=r||{});return function(n){n=n||[];for(var i=0;i<a.length;i++){var c=t(a[i]);e[c].references--}for(var s=o(n,r),d=0;d<a.length;d++){var u=t(a[d]);0===e[u].references&&(e[u].updater(),e.splice(u,1))}a=s}}},659:n=>{var e={};n.exports=function(n,t){var o=function(n){if(void 0===e[n]){var t=document.querySelector(n);if(window.HTMLIFrameElement&&t instanceof window.HTMLIFrameElement)try{t=t.contentDocument.head}catch(n){t=null}e[n]=t}return e[n]}(n);if(!o)throw new Error("Couldn't find a style target. This probably means that the value for the 'insert' parameter is invalid.");o.appendChild(t)}},540:n=>{n.exports=function(n){var e=document.createElement("style");return n.setAttributes(e,n.attributes),n.insert(e,n.options),e}},56:(n,e,t)=>{n.exports=function(n){var e=t.nc;e&&n.setAttribute("nonce",e)}},825:n=>{n.exports=function(n){if("undefined"==typeof document)return{update:function(){},remove:function(){}};var e=n.insertStyleElement(n);return{update:function(t){!function(n,e,t){var o="";t.supports&&(o+="@supports (".concat(t.supports,") {")),t.media&&(o+="@media ".concat(t.media," {"));var r=void 0!==t.layer;r&&(o+="@layer".concat(t.layer.length>0?" ".concat(t.layer):""," {")),o+=t.css,r&&(o+="}"),t.media&&(o+="}"),t.supports&&(o+="}");var a=t.sourceMap;a&&"undefined"!=typeof btoa&&(o+="\n/*# sourceMappingURL=data:application/json;base64,".concat(btoa(unescape(encodeURIComponent(JSON.stringify(a))))," */")),e.styleTagTransform(o,n,e.options)}(e,n,t)},remove:function(){!function(n){if(null===n.parentNode)return!1;n.parentNode.removeChild(n)}(e)}}}},113:n=>{n.exports=function(n,e){if(e.styleSheet)e.styleSheet.cssText=n;else{for(;e.firstChild;)e.removeChild(e.firstChild);e.appendChild(document.createTextNode(n))}}}},e={};function t(o){var r=e[o];if(void 0!==r)return r.exports;var a=e[o]={id:o,exports:{}};return n[o](a,a.exports,t),a.exports}t.n=n=>{var e=n&&n.__esModule?()=>n.default:()=>n;return t.d(e,{a:e}),e},t.d=(n,e)=>{for(var o in e)t.o(e,o)&&!t.o(n,o)&&Object.defineProperty(n,o,{enumerable:!0,get:e[o]})},t.o=(n,e)=>Object.prototype.hasOwnProperty.call(n,e),t.nc=void 0;var o=t(72),r=t.n(o),a=t(825),i=t.n(a),c=t(659),s=t.n(c),d=t(56),u=t.n(d),l=t(540),p=t.n(l),f=t(113),m=t.n(f),g=t(675),v={};v.styleTagTransform=m(),v.setAttributes=u(),v.insert=s().bind(null,"head"),v.domAPI=i(),v.insertStyleElement=p(),r()(g.A,v),g.A&&g.A.locals&&g.A.locals,document.addEventListener("DOMContentLoaded",(function(){var n,e,t,o=document.getElementById("mode"),r=document.getElementById("running"),a=document.getElementById("resting"),i=document.getElementById("modeInput"),c=document.getElementById("runningInput"),s=document.getElementById("restingInput"),d=document.getElementById("sendBtn"),u=function(n){var e="; ".concat(document.cookie).split("; ".concat(n,"="));if(2===e.length)return e.pop().split(";").shift()};d.addEventListener("click",(function(){var e=parseInt(i.value),t=parseInt(c.value),o=parseInt(s.value),r={objectName:{}};isNaN(e)||(r.objectName.mode=e),isNaN(t)||(r.objectName.running=t),isNaN(o)||(r.objectName.resting=o),n&&n.readyState===WebSocket.OPEN?(n.send(JSON.stringify(r)),console.log("Data sent:",r)):console.error("WebSocket is not open")})),e=u("_idx"),t=u("_imuwahen"),e&&t?((n=new WebSocket("ws://akowe.org/ws")).onopen=function(){console.log("Connected to WebSocket server"),n.send(JSON.stringify({idx:e,tkn:t}))},n.onmessage=function(n){try{var e=JSON.parse(n.data);e.objectName&&function(n){void 0!==n.mode&&(o.textContent=n.mode),void 0!==n.running&&(r.textContent=n.running),void 0!==n.resting&&(a.textContent=n.resting)}(e.objectName)}catch(n){console.error("Error parsing message:",n)}},n.onclose=function(){console.log("Disconnected from WebSocket server")},n.onerror=function(n){console.error("WebSocket error:",n)}):console.error("Authentication tokens not found in cookies")}))})();