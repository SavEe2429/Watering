const express = require('express');
const cors = require('cors');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(express.json());
app.use(cors());

let lastCommand = 'STOP';
let state = 'STOP';

app.post('/command' , (req , res) => {
    const {action , from} = req.body;
    if(from == "web"){
        if(action == 'ON' || action == 'OFF' || action == 'STOP'){
            lastCommand = action;
            return res.json({success : true ,command : lastCommand});
        }
    }else if(from == "esp"){
        if(action == 'ON' || action == 'OFF' || action == 'STOP'){
            state = action;
            return res.json({success : true ,command : lastCommand});
        }
    }
    res.status(400).json({success : false , message : 'Invalid commmand'});
});

app.get('/recent' , (req , res) => {
    res.json({command : lastCommand});
});

app.get('/showState' ,(req, res) => {
    res.json({command : state})
})


app.listen(PORT , ()=>{
    console.log(`Server running on http:localhost:${PORT}`);
});


