function turnOn(){
    fetch('/on')
    .then(response => response.json())
    .then(data => {
        document.getElementById('status').innerHTML = "Device Status : " + data.status;
    });
}

function turnOff(){
    fetch('/off')
    .then(response => response.json())
    .then(data => {
        document.getElementById('status').innerHTML = "Device Status : " + data.status;
    });
}