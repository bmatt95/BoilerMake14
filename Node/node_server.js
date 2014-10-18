var http = require('http');
var url = require('url');
var fs = require('fs');
var exec = require('child_process').execFile

var server = http.createServer(function(req, res) {
    res.setHeader('Content-Type', 'application/json');
    var queryObject = url.parse(req.url,true).query;
    console.log(queryObject);
    fs.writeFile('ticker.txt', queryObject.symbol, function(err) {
        if(err) {
            console.log(err);
        } else {
            console.log("ok!");
        }
    });
    exec('stockLookup.bat', function(err, data) {
        console.log(err)
        console.log(data.toString());
    });
    fs.readFile('price.txt', function (err, data) {
        if (err) throw err;
    res.end(JSON.stringify({"Data":{"LastPrice":parseFloat(data)}}));
    });
});
server.listen(80, '0.0.0.0');
console.log('Server running!');
