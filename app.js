// Node Modules
var express = require('express');
var Repeat = require('repeat');
var Twitter = require('twitter');

// Relative
var config = require('./config');

// Init server stuff
var app = express();
var router = express.Router();

// Twitter related struff
var lastTweet = '-1';
var client = new Twitter(config);
var params = {
  screen_name: 'nodejs',
	q: '#DJJochum',
  count: 1,
  result_type:'recent'
};

// This job fetches the last interesting tweet
function runScript() {
  //console.log("Polling");
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

app.use('/api', router);

// Expose API To Get the Last Tweet
router.get('/', function(req, res) {
    res.send(lastTweet);   
});

// Schedule the cron to fetch the last script
function schedule(){
    Repeat(runScript).every(4, 'sec').start.in(1, 'sec');
}
schedule();


module.exports = app;