const express = require('express')
router = express.Router()
var pool = require('./database')
var async = require('async')


router.get('/', (req, res)=>{
    var roomsQuery = 'SELECT * FROM room'
    pool.query(roomsQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }

        rooms = result

        async.each(
            rooms, 
            function (room, callback) {
                var sensorsQuery = `SELECT * FROM sensor WHERE sensor.room = ${room.id}`
                pool.query(sensorsQuery, (err, result, fields) => {
                    if (err) {
                        callback(err)
                        throw new Error(err)
                    }
                    sensors = result
                    sensors.map(sensor => {
                        sensor.auto = sensor.auto ? true : false
                        sensor.active = sensor.active ? true : false
                        return sensor
                    })       
                    room.sensors = sensors
                    callback(err, result, fields)
                })
            },  
            function () {
                res.send(rooms)
            }
        )
    })
})

router.get('/:roomId', (req, res)=>{
    var roomsQuery = `SELECT * FROM room WHERE room.id = ${req.params.roomId}`
    
    pool.query(roomsQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }

        room = result
        room.connected = true

        var sensorsQuery = `SELECT * FROM sensor WHERE sensor.room = ${req.params.roomId}`
        
        pool.query(sensorsQuery, (err, result, fields) => {
            if (err) {
                throw new Error(err)
            }
            sensors = result
            sensors.map(sensor => {
                sensor.auto = sensor.auto ? true : false
                sensor.active = sensor.active ? true : false
                return sensor
            })       
            room.sensors = sensors
            res.send(room)
        })
        
    })
})

router.put('/', (req, res)=>{
    var room = req.body;

    var roomQuery = "INSERT INTO room (`name`, `ipv4`, `connected`) " + `VALUES ('${room.name}','${room.ipv4}','${room.connected?1:0}')`
    pool.query(roomQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        room.id = result.insertId
        
        async.each(
            room.sensors, 
            function (sensor, callback) {
                var sensorQuery = "INSERT INTO sensor (`type`, `name`, `auto`, `active`, `room`) " + `VALUES ('${sensor.type}','${sensor.name}','${sensor.auto?1:0}', '${sensor.active?1:0}', '${room.id}')`
                pool.query(sensorQuery, (err, result, fields) => {
                    if (err) {
                        callback(err)
                        throw new Error(err)
                    }
                    sensor.id = result.insertId
                    callback(err, result, fields)
                })
            },  
            function () {
                res.send(room)
            }
        )
    })

})

router.post('/', (req, res)=>{
    var room = req.body;
    var roomQuery = `UPDATE room SET name = '${room.name}', ipv4 = '${room.ipv4}', connected = '${room.connected?1:0}' WHERE room.id = ${room.id}`
    pool.query(roomQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(room)
    })
})

router.delete('/:roomId', (req, res)=>{
    var roomQuery = `DELETE FROM room WHERE room.id = ${req.params.roomId}` 
    pool.query(roomQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(req.params.roomId)
    })
})

router.get('/:roomId/sensors', (req, res)=>{
    var sensorsQuery = `SELECT * FROM sensor WHERE sensor.room = ${req.params.roomId}`
    pool.query(sensorsQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        sensors = result

        async.each(
            sensors, 
            function (sensor, callback) {
                var sensorQuery = `SELECT * FROM measure WHERE measure.sensor = ${sensor.id} ORDER BY measure.datetime DESC LIMIT 1`
                pool.query(sensorQuery, (err, result, fields) => {
                    if (err) {
                        callback(err)
                        throw new Error(err)
                    }
                    sensor.measure = result[0]
                    callback(err, result, fields)
                })
            },  
            function () {
                sensors.map(sensor => {
                    sensor.auto = sensor.auto ? true : false
                    sensor.active = sensor.active ? true : false
                    return sensor
                })
                res.send(sensors)
            }
        )
    })
})

router.get('/:roomId/connected', (req, res)=>{
    res.send(true)
})

module.exports = router