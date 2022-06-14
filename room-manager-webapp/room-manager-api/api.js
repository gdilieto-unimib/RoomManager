const express = require('express')
router = express.Router()

router.use('/configuration', require('./configuration'))
router.use('/rooms', require('./rooms'))
router.use('/sensors', require('./sensors'))

module.exports = router