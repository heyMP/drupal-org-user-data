casper.options.viewportSize = {width: 1024, height: 768};
var testCount = 1;
casper.test.begin("Testing Reddit", testCount, function redditTest(test) {
    casper.start("https://www.drupal.org/u/btopro", function() {
        test.assertExists('#user_user_full_group_profile_main dd ul li', 'The full_group_profile element is present');
    }).run(function() {
        test.done();
    });
});
