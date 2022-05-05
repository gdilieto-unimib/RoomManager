const express = require('express')
const app = express()
const port = 80

app.use(express.static(process.cwd()+'/room-manager-gui/dist/room-manager-gui'));
app.use('/api', require('./room-manager-api/api'))

app.listen(port, '0.0.0.0', ()=>{
    console.log(`Server is now listening on port ${port}`)
})

app.get('/', (req,res) => {
    res.sendFile(process.cwd()+'/room-manager-gui/dist/room-manager-gui/index.html')
});