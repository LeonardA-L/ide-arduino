var express = require('express');
var path = require('path');
var favicon = require('serve-favicon');
var logger = require('morgan');
var cookieParser = require('cookie-parser');
var bodyParser = require('body-parser');

var Repeat = require('repeat');

var index = require('./routes/index');
var users = require('./routes/users');
var config = require('./config');

var Twitter = require('twitter');

var app = express();
var router = express.Router();
// view engine setup
app.set('views', path.join(__dirname, 'views'));
app.set('view engine', 'jade');


var lastTweet = -1;

 
var client = new Twitter(config);
 
var params = {screen_name: 'nodejs',
	q: '#DJJochum',
count: 1,
result_type:'recent'
};

function runScript() {
console.log("Polling");
client.get('search/tweets', params, function(error, tweets, response) {
	if(error) {
	console.log(error);
	process.exit();
}
  if (!error && tweets.statuses.length) {
    console.log(tweets.statuses[0].id);
	lastTweet = '' + tweets.statuses[0].id;
  }
});
}


// uncomment after placing your favicon in /public
//app.use(favicon(path.join(__dirname, 'public', 'favicon.ico')));
app.use(logger('dev'));
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));
app.use(cookieParser());
app.use(express.static(path.join(__dirname, 'public')));

app.use('/', index);
app.use('/users', users);
app.use('/api', router);

// catch 404 and forward to error handler
app.use(function(req, res, next) {
  var err = new Error('Not Found');
  err.status = 404;
  next(err);
});

// error handler
app.use(function(err, req, res, next) {
  // set locals, only providing error in development
  res.locals.message = err.message;
  res.locals.error = req.app.get('env') === 'development' ? err : {};

  // render the error page
  res.status(err.status || 500);
  res.render('error');
});

router.get('/', function(req, res) {
    res.send(lastTweet);   
});

function schedule(){
    Repeat(runScript).every(4, 'sec').start.in(1, 'sec');
}

schedule();


module.exports = app;
