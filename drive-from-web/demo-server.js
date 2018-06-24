var http = require('http');
var url = require('url');

http.createServer(function (req, res) {
  res.writeHead(200, {'Content-Type': 'text/html'});
  var params = url.parse(req.url, true).query;
  console.log()
}).listen(8080);

