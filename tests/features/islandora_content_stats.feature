@javascript
@ldl
@api

Feature:
    In order to ensure the correct function of the content stats module
    As a developer,
    I need to be sure that it fulfills the requirements.

    Background:

        Given I am logged in as a user with the 'administrator' role
        And I create a new collection of "9" "image" objects with pid 'testinst-stats:collection' and title "Test Images"
        And I create a new collection of "8" "audio" objects with pid 'testinst-subinst-stats:collection' and title "Subinstitution audio"
        And I create a new collection of "7" "image" objects with pid 'otherinst-images:collection' and title "Another Images collection"
        And I am on "/admin/islandora/tools/content_stats"

        When I press the "Run Queries Now" button
        And the cache has been cleared
        #And breakpoint

    Scenario: Check that the admin page there and correct
        And I am on "/admin/islandora/tools/content_stats"
        Then I should see "Content Statistics"

    Scenario: Check that global counts are there and correct

        Then I should find xpath "//div[@class='globalStat']/div[@class='collections']/div[@class='total' and contains(text(), '3')]"
        And I should find xpath "//div[@class='globalStat']/div[@class='image']/div[@class='total' and contains(text(), '16')]"
        And I should find xpath "//div[@class='globalStat']/div[@class='audio']/div[@class='total' and contains(text(), '8')]"
        
    Scenario: Check that institutional cmodel totals are there and correct

        Then I should find xpath "//div[@class='instContainer']/div[@class='instStats instGroup']/div[@class='inst_wrapper testinst']/div[@class='cmodel-image']/div[@class='total' and contains(text(), '9')]"
        And I should find xpath "//div[@class='instContainer']/div[@class='instStats instGroup']/div[@class='inst_wrapper testinst-subinst']/div[@class='cmodel-audio']/div[@class='total' and contains(text(), '8')]"
        And I should find xpath "//div[@class='instContainer']/div[@class='instStats instGroup']/div[@class='inst_wrapper otherinst']/div[@class='cmodel-image']/div[@class='total' and contains(text(), '6')]"

    Scenario: Check that table rows are there and correct

        Then I should find xpath "//div[@class='tableStats']//div[@class='table']/div[@class='column count']/div[@class=['row image testinst' and contains(text(), '9')]"

    Scenario: Check that table sorts work