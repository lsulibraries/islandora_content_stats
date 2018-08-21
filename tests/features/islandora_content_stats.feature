@javascript
@ldl
@api

Feature:

    In order to ensure the correct function of the content stats module
    As a developer,
    I need to be sure that it fulfills the requirements.


    Background:

        Given I am on "/data"

    Scenario: Check that the admin page is there and correct
        Given I am logged in as a user with the 'administrator' role
        When I am on "/admin/islandora/tools/content_stats"
        Then I should see "Content Statistics"

        # this is pretty weak as-is.
        When I press the "Run Queries Now" button
        Then I should see "Content Statistics"

    Scenario: Check that global counts are there and correct
        # Need to wait just a sec for the animated thing to load.
        When I wait "1" seconds
        And xpath "//div[@class='globalStat']/div[@class='image global']/div[@class='total 16']" text should equal "16"
        And xpath "//div[@class='globalStat']/div[@class='audio global']/div[@class='total 8']" text should equal "8"
        Then xpath "//div[@class='globalStat']/div[@class='collections']/div[@class='total 3']" text should equal "3"
        
    Scenario: Check that institutional cmodel totals are there and correct

        Then xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image testinst instTotal']/div[@class='total']" text should equal "9"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst instTotal']/div[@class='total']" text should equal "2"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst-subinst instTotal']/div[@class='total']" text should equal "1"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio testinst-subinst instTotal']/div[@class='total']" text should equal "8"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image otherinst instTotal']/div[@class='total']" text should equal "7"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection otherinst instTotal']/div[@class='total']" text should equal "1"


    Scenario: Check that table rows are there and correct

        Then xpath "//div[@class='column count']/div[@class='row image testinst']" text should equal "9"
        And xpath "//div[@class='column count']/div[@class='row audio testinst']" text should equal "8"
        And xpath "//div[@class='column count']/div[@class='row collection testinst']" text should equal "2"

        And xpath "//div[@class='column count']/div[@class='row audio testinst-subinst']" text should equal "8"
        And xpath "//div[@class='column count']/div[@class='row collection testinst-subinst']" text should equal "1"

        And xpath "//div[@class='column count']/div[@class='row collection otherinst']" text should equal "1"
        And xpath "//div[@class='column count']/div[@class='row image otherinst']" text should equal "7"

    Scenario: Check initial sorts

        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "otherinst"
        And xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Collection"
        And xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" text should equal "1"

    Scenario: Check first-column sorts

        And I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "testinst"
        
        When I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "testinst-subinst"

    Scenario: Check 'Type' column sorts

        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Audio"
        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Image"

    Scenario: Check 'counts' column sorts

        And I click "Count"
        Then xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" text should equal "1"
        When I click "Count"
        Then xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" text should equal "9"

    Scenario: Check options for filters
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

    Scenario: Check 'type' filter.

        When I select "Collection" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        Then I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

        When I select "Image" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        Then I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        Then I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

        When I select "--none--" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

    Scenario: Check 'inst' filter.

        Given I select "testinst" from "Filter according to ownership by institution or sub-institution"
        When I press "Filter"
        Then I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

    Scenario: Check filters in combination (adding both at once)

        Given I select "otherinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Collection" from "Filter by type of object"
        When I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

    Scenario: Check filters in combination (Changing one, then the other)

        Given I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Audio" from "Filter by type of object"
        When I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

        When I select "Collection" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

        When I select "otherinst" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

    Scenario: Check filters in combination (Resetting them)

        Given I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Audio" from "Filter by type of object"
        And I press "Filter"
        And I select "--none--" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

        When I select "--none--" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"

        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"


    Scenario: Check filters in combination (empty result set)

        Given I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Image" from "Filter by type of object"
        When I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        And select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        And I should not find xpath "//div[contains(@class, 'row')]"

    Scenario: Check filters in combination with sorts (filter first, sorts should persist)

        Given I select "testinst" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Audio"

        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Image"

        When I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Collection"

    Scenario: Check filters in combination with sorts (sort first, see that filters persist)

        And I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Audio"

        Given I select "otherinst" from "Filter according to ownership by institution or sub-institution"
        When I press "Filter"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Collection"

        # reverse the sort
        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Image"

        # change the filter
        Given I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        When I press "Filter"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Collection"

    Scenario: Ensure sorting works when institution names are provided.

        Given I am logged in as a user with the 'administrator' role
        And I am on "/testinst/settings"
        And for "edit-title" I enter "Test Institution"
        And I press "Submit"

        And I am on "/testinst-subinst/settings"
        And for "edit-title" I enter "Subinstitution of Test"
        And I press "Submit"

        And I am on "/otherinst/settings"
        And for "edit-title" I enter "Other Library"
        And I press "Submit"

        And the cache has been cleared

        When I am on "/data"
        Then select list at xpath "//select[@id='edit-inst']" should contain options "Test Institution, Subinstitution of Test, Other Library"
        And xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "Other Library"

        When I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "Test Institution"
