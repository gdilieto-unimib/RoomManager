const express = require('express')
router = express.Router()
var pool = require('./database')
var async = require('async')
const WebSocket = require('ws')

router.get('/', (req, res)=>{
    var configurationQuery = `SELECT * FROM configuration`
    pool.query(configurationQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        let configuration = {}
        for (let config of result) {
            switch (config.name) {
                case "singleMode": {
                    configuration.singleMode = config.value=="1"?true:false
                    break
                }
                case "ecoMode": {
                    configuration.ecoMode = config.value=="1"?true:false
                    break
                }
                case "sleepMode": {
                    configuration.sleepMode = config.value=="1"?true:false
                    break
                }
                case "sleepSchedule": {
                    configuration.sleepSchedule = config.value
                    break    
                }
            }
        } 
        res.send(configuration)
    })
})

router.post('/singleMode', (req, res)=>{
    let singleMode = req.body.singleMode
    var singleQuery = `UPDATE configuration SET value = '${singleMode?1:0}' WHERE name = 'singleMode'`
    pool.query(singleQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(singleMode)
    })
})

router.post('/ecoMode', (req, res)=>{
    let ecoMode = req.body.ecoMode
    var ecoQuery = `UPDATE configuration SET value = '${ecoMode?1:0}' WHERE name = 'ecoMode'`
    pool.query(ecoQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(ecoMode)
    })
})

router.post('/sleepMode', (req, res)=>{
    let sleepMode = req.body.sleepMode
    var sleepQuery = `UPDATE configuration SET value = '${sleepMode?1:0}' WHERE name = 'sleepMode'`
    pool.query(sleepQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(sleepMode)
    })
})

router.post('/schedule', (req, res)=>{
    let sleepSchedule = req.body.sleepSchedule
    var scheduleQuery = `UPDATE configuration SET value = '${sleepSchedule}' WHERE name = 'sleepSchedule'`
    pool.query(scheduleQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(sleepSchedule)
    })
})

module.exports = router