@javascript
@ldl
@api

Feature:
    In order to ensure the correct function of the content stats module
    As a developer,
    I need to be sure that it fulfills the requirements.

    Background:

        Given I am logged in as a user with the 'administrator' role
        And the cache has been cleared
        And I am on "/admin/islandora/tools/content_stats"

        When I press the "Run Queries Now" button
        And the cache has been cleared

    Scenario: Check that table sorts work
        # check initial sorts
        And I am on "/data"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" should contain text "otherinst"
        And xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" should contain text "Collection"
        And xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" should contain text "1"

        # check first-column sorts
        When I am on "/data"
        And I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" should contain text "testinst"
        
        When I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" should contain text "testinst-subinst"

        # check 'Type' column sorts
        When I am on "/data"
        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" should contain text "Audio"
        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" should contain text "Image"

        # check counts column sorts
        When I am on "/data"
        And I click "Count"
        Then xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" should contain text "1"
        When I click "Count"
        Then xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" should contain text "9"

    Scenario: Check that table rows are there and correct

        Then xpath "//div[@class='column count']/div[@class='row image testinst']" should contain text "9"
        And xpath "//div[@class='column count']/div[@class='row audio testinst']" should contain text "8"
        And xpath "//div[@class='column count']/div[@class='row collection testinst']" should contain text "2"

        And xpath "//div[@class='column count']/div[@class='row audio testinst-subinst']" should contain text "8"
        And xpath "//div[@class='column count']/div[@class='row collection testinst-subinst']" should contain text "1"

        And xpath "//div[@class='column count']/div[@class='row collection otherinst']" should contain text "1"
        And xpath "//div[@class='column count']/div[@class='row image otherinst']" should contain text "9"

    Scenario: Check that global counts are there and correct
        Then xpath "//div[@class='globalStat']/div[@class='collections']/div[@class='total 3']" should contain text "3"
        And xpath "//div[@class='globalStat']/div[@class='image global']/div[@class='total 16']" should contain text "16"
        And xpath "//div[@class='globalStat']/div[@class='audio global']/div[@class='total 8']" should contain text "8"
        
    Scenario: Check that institutional cmodel totals are there and correct#

        And I am on "/data"
        Then xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image testinst instTotal']/div[@class='total']" should contain text "9"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst instTotal']/div[@class='total']" should contain text "2"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst-subinst instTotal']/div[@class='total']" should contain text "1"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio testinst-subinst instTotal']/div[@class='total']" should contain text "8"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image otherinst instTotal']/div[@class='total']" should contain text "7"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection otherinst instTotal']/div[@class='total']" should contain text "1"

    Scenario: Check that the admin page is there and correct
        And I am on "/admin/islandora/tools/content_stats"
        Then I should see "Content Statistics"
