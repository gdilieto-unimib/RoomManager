const express = require('express')
router = express.Router()
var pool = require('./database')
var async = require('async')


router.put('/', (req, res)=>{
    var sensor = req.body;
    var sensorQuery = "INSERT INTO sensor (`type`, `name`, `auto`, `active`, `room`) " + `VALUES ('${sensor.type}','${sensor.name}','${sensor.auto}', '${sensor.active}', '${room.id}')`
    
    pool.query(sensorQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        sensor.id = result.insertId
        res.send(sensor)
    })
})

router.post('/', (req, res)=>{
    var sensor = req.body;
    var sensorQuery = `UPDATE sensor SET name = '${sensor.name}', auto = '${sensor.auto?1:0}', active = '${sensor.active?1:0}' WHERE sensor.id = ${sensor.id}`
    
    pool.query(sensorQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(sensor)
    })
})

router.delete('/:sensorId', (req, res)=>{
    var sensor = req.body;
    var sensorQuery = `DELETE FROM sensor WHERE sensor.id = ${req.params.sensorId}`
    
    pool.query(sensorQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(sensor)
    })
})

module.exports = router