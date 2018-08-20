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

    Scenario: Check that global counts are there and correct
        Then I should find xpath "//div[@class='globalStat']/div[@class='collections']/div[@class='total 3' and contains(text(), '3')]"
        And I should find xpath "//div[@class='globalStat']/div[@class='image global']/div[@class='total 16' and contains(text(), '16')]"
        And I should find xpath "//div[@class='globalStat']/div[@class='audio global']/div[@class='total 8' and contains(text(), '8')]"
        
    Scenario: Check that institutional cmodel totals are there and correct#

        And I am on "/data"
        Then I should find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image testinst instTotal']/div[@class='total' and contains(text(), '9')]"
        And I should find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst instTotal']/div[@class='total' and contains(text(), '2')]"
        And I should find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst-subinst instTotal']/div[@class='total' and contains(text(), '1')]"
        And I should find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio testinst-subinst instTotal']/div[@class='total' and contains(text(), '8')]"
        And I should find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image otherinst instTotal']/div[@class='total' and contains(text(), '7')]"
        And I should find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection otherinst instTotal']/div[@class='total' and contains(text(), '1')]"

#    Scenario: Check that table rows are there and correct

#        Then I should find xpath "//div[@class='tableStats']//div[@class='table']/div[@class='column count']/div[@class=['row image testinst' and contains(text(), '9')]"

#    Scenario: Check that table sorts work
        
#        Then I should find xpath ""
#        When I click on "Institution/Sub-institution"
#        Then I should see

    Scenario: Check that the admin page is there and correct
        And I am on "/admin/islandora/tools/content_stats"
        Then I should see "Content Statistics"
