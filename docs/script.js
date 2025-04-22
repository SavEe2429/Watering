// function turnOn(){
//     fetch('/on')
//     .then(response => response.json())
//     .then(data => {
//         document.getElementById('status').innerHTML = "Device Status : " + data.status;
//     });
// }

// const bodyParser = require("body-parser");

// function turnOff(){
//     fetch('/off')
//     .then(response => response.json())
//     .then(data => {
//         document.getElementById('status').innerHTML = "Device Status : " + data.status;
//     });
// }

// function checkStatus() {
//     fetch('/status')
//     .then(response => response.json())
//     .then(data => {
//         document.getElementById('status').innerHTML = "Device Status : " + data.status;
//     })
// }

const apiUrl = "https://watering-9xrq.onrender.com";

async function sendCommand(action){
    const response = await fetch(`${apiUrl}/command`,{
        method: 'POST',
        headers:{'Content-type' : 'application/json'},
        body:JSON.stringify({action})
    });

    const data = await response.json();
    updateStatus();
}

async function updateStatus() {
    const response = await fetch(`${apiUrl}/recent`);
    const data = await response.json();
    document.getElementById('status').textContent = "Device Status : "+ data.command;
}

updateStatus();
setInterval(updateStatus,3000);