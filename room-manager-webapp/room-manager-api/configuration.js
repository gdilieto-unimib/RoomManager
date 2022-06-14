const express = require('express')
router = express.Router()
var pool = require('./database')
var async = require('async')
const WebSocket = require('ws')

router.get('/', (req, res)=>{
    res.send(true)
})

router.post('/singleMode', (req, res)=>{
    let singleMode = req.body.singleMode
    var roomQuery = `UPDATE configuration SET value = '${singleMode?1:0}' WHERE name = 'singleMode'`
    pool.query(roomQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(singleMode)
    })
})

router.post('/ecoMode', (req, res)=>{
    let ecoMode = req.body.ecoMode
    var roomQuery = `UPDATE configuration SET value = '${ecoMode?1:0}' WHERE name = 'ecoMode'`
    pool.query(roomQuery, (err, result, fields) => {
        if (err) {
            throw new Error(err)
        }
        res.send(ecoMode)
    })
})

module.exports = router