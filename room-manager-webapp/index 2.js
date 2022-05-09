const express = require('express')
const app = express()
const port = 80
var bodyParser = require('body-parser')
app.use(bodyParser.json())
app.use(bodyParser.urlencoded({ extended: true }))


app.use(express.static(process.cwd()+'/room-manager-gui/dist/room-manager-gui'));
//app.use('/api', require('./room-manager-api/api'))
app.use('/api/rooms', require('./room-manager-api/rooms'))
app.use('/api/sensors', require('./room-manager-api/sensors'))

app.listen(port, '0.0.0.0', ()=>{
    console.log(`Server is now listening on port ${port}`)
})

app.get('/', (req,res) => {
    res.sendFile(process.cwd()+'/room-manager-gui/dist/room-manager-gui/index.html')
});