var mysql = require('mysql')

var localPool = mysql.createPool({
    //connectionLimit: 1000,
    host: 'sql11.freemysqlhosting.net',
    port: 3306,
    user: 'sql11490846',
    password: 'QbAhRVpYRU',
    database: 'sql11490846'
})

var labPool = mysql.createPool({
    connectionLimit: 7,
    host: '149.132.178.180',
    port: 3306,
    user: 'gdilieto',
    password: 'iot874143',
    database: 'gdilieto'
})

pool = labPool

pool.getConnection((err, connection) => {
    if (err) {
        if (err.code === 'PROTOCOL_CONNECTION_LOST') {
            console.error('Database connection was closed.')
        }
        if (err.code === 'ER_CON_COUNT_ERROR') {
            console.error('Database has too many connections.')
        }
        if (err.code === 'ECONNREFUSED') {
            console.error('Database connection was refused.')
        }
    } else {
        console.error('Database connection successful.')
    }
    if (connection) connection.release()
    return
})

module.exports = pool