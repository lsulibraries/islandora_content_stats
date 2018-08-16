@javascript
@ldl
@api

Feature:
    In order to ensure the correct function of the content stats module
    As a developer,
    I need to be sure that it fulfills the requirements.

    Background:


    Scenario: Check content stats display

        Given I am logged in as a user with the 'administrator' role
        Given the cache has been cleared
        Given breakpoint
        Given I create a new collection of "9" "image" objects with pid 'testinst-stats:collection' and title "Test Images"
        Given I create a new collection of "8" "audio" objects with pid 'testinst-subinst-stats:collection' and title "Subinstitution audio"
        Given I create a new collection of "7" "image" objects with pid 'otherinst-images:collection' and title "Another Images collection"
        Given I am viewing pid "islandora:root"
        Given breakpoint
        Given I am on "/admin/islandora/tools/content_stats"
        Then I should see "Content Statistics"
        When I press the "Run Queries Now" button
        Given the cache has been cleared
        Given breakpoint

        # Check that global counts are there and correct
        Then I should find xpath "//div[@class='globalStat']/div[@class='collections']/div[@class='total' and contains(text(), '3')]"
        Then I should find xpath "//div[@class='globalStat']/div[@class='image']/div[@class='total' and contains(text(), '16')]"
        Then I should find xpath "//div[@class='globalStat']/div[@class='audio']/div[@class='total' and contains(text(), '8')]"
        
        # Check that institutional cmodel totals are there and correct
        Then I should find xpath "//div[@class='instContainer']/div[@class='instStats instGroup']/div[@class='inst_wrapper testinst']/div[@class='cmodel-image']/div[@class='total' and contains(text(), '9')]"
        Then I should find xpath "//div[@class='instContainer']/div[@class='instStats instGroup']/div[@class='inst_wrapper testinst-subinst']/div[@class='cmodel-audio']/div[@class='total' and contains(text(), '8')]"
        Then I should find xpath "//div[@class='instContainer']/div[@class='instStats instGroup']/div[@class='inst_wrapper otherinst']/div[@class='cmodel-image']/div[@class='total' and contains(text(), '6')]"

        # Check that table rows are there and correct
        Then I should find xpath "//div[@class='tableStats']//div[@class='table']/div[@class='column count']/div[@class=['row image testinst' and contains(text(), '9')]"