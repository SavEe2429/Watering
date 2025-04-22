const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.json());
app.use(cors());

let lastCommand = 'OFF';

app.post('/command' , (req , res) => {
    const {action} = req.body;
    if(action == 'ON' || action == 'OFF' || action == 'BREAK'){
        lastCommand = action;
        return res.json({success : true ,command : lastCommand});
    }
    res.status(400).json({success : false , message : 'Invalid commmand'});
});

app.get('/recent' , (req , res) => {
    res.json({command : lastCommand});
});

app.get('/' , (req , res) => {
    res.send("ESP Command  API is Running");
});

app.listen(PORT , ()=>{
    console.log(`Server running on http:localhost:${PORT}`);
});


