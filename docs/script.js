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

async function sendCommand(action , from){
    const response = await fetch(`${apiUrl}/command`,{
        method: 'POST',
        headers:{'Content-type' : 'application/json'},
        body:JSON.stringify({action , from})
    });

    const data = await response.json();
    updateStatus();
}

async function updateStatus() {
    const responseState = await fetch(`${apiUrl}/showState`);
    const responseDht = await fetch(`${apiUrl}/showDht`);

    const dataState = await responseState.json();
    const dataDht = await responseDht.json();
    
    document.getElementById('status').textContent = "Device Status : "+ dataState.command;
    document.getElementById('dht').textContent = "Temp : "+ dataDht.temperature + "  Humi : " , dataDht.humidity;
}




updateStatus();
setInterval(updateStatus,3000);