const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.json());
app.use(cors());

let lastCommand = 'STOP';
let state = 'STOP';
let hum ;
let temp;

app.post('/command', (req, res) => {
    const { action, from } = req.body;
    if (from == "WEB") {
        if (action == 'ON' || action == 'OFF' || action == 'STOP') {
            lastCommand = action;
            return res.json({ success: true, command: lastCommand });
        }
    } if (from == "ESP") {
        if (action == 'ON' || action == 'OFF' || action == 'STOP') {
            state = action;
            lastCommand = state;
            return res.json({ success: true, command: lastCommand });
        }
    }
    res.status(400).json({ success: false, message: 'Invalid commmand' });
});

app.post('/dhtvalue', (req, res) => {
    const { humidity, temperature , from} = req.body;
    if (from == "ESP") {
        hum = humidity;
        temp = temperature;
        return res.json({ success: true, humidity: hum, temperature: temp });
    }res.status(400).json({success : false,message : 'Invalid value'});
});

app.get('/recent', (req, res) => {
    res.json({ command: lastCommand });
});

app.get('/showState', (req, res) => {
    res.json({ command: state });
});

app.get('/showDht', (req, res) => {
    res.json({ temperature: temp, humidity: hum });
});


app.listen(PORT, () => {
    console.log(`Server running on http:localhost:${PORT}`);
});


