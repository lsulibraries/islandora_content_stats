@javascript
@ldl
@api

Feature:

    In order to ensure the correct function of the content stats module
    As a developer,
    I need to be sure that it fulfills the requirements.
    That global counts for each content type are correct and correctly displayed.
    That institution-level counts for each content type are correct and correctly displayed in the 'glance' section.
    That institution-level counts for each content type are correct and correctly displayed in the table.
    That table filtering works as expected.
    That table sorting works as expected.
    That table sorts and filters work together.
    That data is correct even when:
    - institution name is substring of another
    - collection has an extraneous policy 

    Background:

        Given I am on "/data"

    Scenario: Check that the admin page is there and correct

        # This test is mostly a stub; pretty weak as-is.

        Given I am logged in as a user with the 'administrator' role
        When I am on "/admin/islandora/tools/content_stats"
        Then I should see "Content Statistics"

        When I press the "Run Queries Now" button
        Then I should see "Content Statistics"

    Scenario: Check that global counts are there and correct
        
        Given the cache has been cleared
        And I am on "/data"

        # Need to wait just a sec for the animated thing to load.
        When I wait "1" seconds
        And xpath "//div[@class='globalStat']/div[@class='image global']/div[@class='total']" text should equal "23"
        And xpath "//div[@class='globalStat']/div[@class='audio global']/div[@class='total']" text should equal "13"
        Then xpath "//div[@class='globalStat']/div[@class='collections']/div[@class='total']" text should equal "6"
        
    Scenario: Check that institutional cmodel totals are there and correct

        # testinst Parent of 'testinst-subinst' having 9 image objects.
        Then xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image testinst instTotal']/div[@class='total']" text should equal "11"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio testinst instTotal']/div[@class='total']" text should equal "8"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst instTotal']/div[@class='total']" text should equal "2"

        # testinst-subinst - Subinstitution of 'testinst' having 8 audio objects.
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection testinst-subinst instTotal']/div[@class='total']" text should equal "1"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio testinst-subinst instTotal']/div[@class='total']" text should equal "8"
        And I should not find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image testinst-subinst instTotal']/div[@class='total']"

        # emptyinst - Standalone institution. No items in the collection.
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection emptyinst instTotal']/div[@class='total']" text should equal "1"
        And I should not find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image emptyinst instTotal']/div[@class='total']"
        And I should not find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio emptyinst instTotal']/div[@class='total']"

        # otherinst - Standalone institution. Namespace prefix is substring of 'anotherinst'. 7 images.
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image otherinst instTotal']/div[@class='total']" text should equal "7"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection otherinst instTotal']/div[@class='total']" text should equal "1"
        And I should not find xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio otherinst instTotal']/div[@class='total']"

        # anotherinst - Standalone institution. Namespace prefix is superstring of 'anotherinst'. 2 images, 4 audio.
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image anotherinst instTotal']/div[@class='total']" text should equal "2"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio anotherinst instTotal']/div[@class='total']" text should equal "4"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection anotherinst instTotal']/div[@class='total']" text should equal "1"

        # otherinsta - Standalone institution. Namespace prefix is string-overlapping with 'anotherinst' and 'otherinst'. 3 images, 2 audio.
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst image otherinsta instTotal']/div[@class='total']" text should equal "3"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst audio otherinsta instTotal']/div[@class='total']" text should equal "1"
        And xpath "//div[@class='instContainer']//div[@class='cmodel_wrapper_inst collection otherinsta instTotal']/div[@class='total']" text should equal "1"

    Scenario: Check that table rows are there and correct

        Then xpath "//div[@class='column count']/div[@class='row image testinst']" text should equal "11"
        And xpath "//div[@class='column count']/div[@class='row audio testinst']" text should equal "8"
        And xpath "//div[@class='column count']/div[@class='row collection testinst']" text should equal "2"

        And xpath "//div[@class='column count']/div[@class='row audio testinst-subinst']" text should equal "8"
        And xpath "//div[@class='column count']/div[@class='row collection testinst-subinst']" text should equal "1"

        And xpath "//div[@class='column count']/div[@class='row collection otherinst']" text should equal "1"
        And xpath "//div[@class='column count']/div[@class='row image otherinst']" text should equal "7"

        And xpath "//div[@class='column count']/div[@class='row collection anotherinst']" text should equal "1"
        And xpath "//div[@class='column count']/div[@class='row image anotherinst']" text should equal "2"
        And xpath "//div[@class='column count']/div[@class='row audio anotherinst']" text should equal "4"

        And xpath "//div[@class='column count']/div[@class='row collection otherinsta']" text should equal "1"
        And xpath "//div[@class='column count']/div[@class='row image otherinsta']" text should equal "3"
        And xpath "//div[@class='column count']/div[@class='row audio otherinsta']" text should equal "1"

    Scenario: Check initial sorts

        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "anotherinst"
        And xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Audio"
        And xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" text should equal "4"

    Scenario: Check first-column sorts

        And I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "testinst-subinst"
        
        When I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "anotherinst"

    Scenario: Check 'Type' column sorts

        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Audio"
        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Image"

    Scenario: Check 'counts' column sorts

        And I click "Count"
        Then xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" text should equal "1"
        When I click "Count"
        Then xpath "//div[@class='column count']//div[contains(@class, 'row')][1]" text should equal "11"

    Scenario: Check options for filters
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

    Scenario: Check 'type' filter.

        When I select "Collection" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        And select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"

        When I select "Image" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        And select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

        When I select "--none--" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        And select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

    Scenario: Check 'inst' filter.

        Given I select "testinst" from "Filter according to ownership by institution or sub-institution"
        When I press "Filter"
        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='anotherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='emptyinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinsta']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"


    Scenario: Check filters in combination (adding both at once)

        Given I select "otherinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Collection" from "Filter by type of object"
        When I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='anotherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='emptyinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinsta']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"

        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

    Scenario: Check filters in combination (Changing one, then the other)

        # set type to 'audio' and inst to 'testinst-subinst'
        Given I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Audio" from "Filter by type of object"
        When I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        # checking for type 'audio' and inst 'testinst-subinst' only
        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='anotherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='emptyinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinsta']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"

        # switch type to 'collection'
        When I select "Collection" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        # checking for type 'Collection' and inst 'testinst-subinst' only
        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='anotherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='emptyinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinsta']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"

        # switch inst to 'otherinst'
        When I select "otherinst" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        # checking for type 'Collection' and inst 'otherinst' only
        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='anotherinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='emptyinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinsta']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"
        And I should not find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"

        Then I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should not find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"

    Scenario: Check filters in combination (Resetting them)

        Given I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Audio" from "Filter by type of object"
        And I press "Filter"
        And I select "--none--" from "Filter by type of object"
        And I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        When I select "--none--" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"

        Then select list at xpath "//select[@id='edit-inst']" should contain options "testinst, testinst-subinst, otherinst"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        Then I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='anotherinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='emptyinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='otherinsta']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst']"
        And I should find xpath "//div[@class='column inst']//div[contains(@class, 'row') and text()='testinst-subinst']"

        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Image']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Audio']"
        And I should find xpath "//div[@class='column cmodel']//div[contains(@class, 'row') and text()='Collection']"


    Scenario: Check filters in combination (empty result set)

        Given I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I select "Image" from "Filter by type of object"
        When I press "Filter"

        # Ensure options remain
        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, emptyinst,testinst, testinst-subinst, otherinst, otherinsta"
        Then select list at xpath "//select[@id='edit-cmodel']" should contain options "Image,Collection, Audio"

        And I should not find xpath "//div[contains(@class, 'row')]"

    Scenario: Check filters in combination with sorts (filter first, sorts should persist)

        Given I select "testinst" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"

        # Check that initial sort (by inst, asc) is true:
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Audio"

        # Check that initial sort (by inst, asc) is reversed:
        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Image"

        # Check that reversed initial sort (by inst, desc) persists after filter application:
        When I select "testinst-subinst" from "Filter according to ownership by institution or sub-institution"
        And I press "Filter"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Collection"

    Scenario: Check filters in combination with sorts (sort first, see that filters persist)

        # from page-load (order by inst, asc), set the sort column to 'type' (asc is implied)
        And I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Audio"

        # 'otherinst' does not have audio objects, only 'image' and 'collection', 
        # so sorted by type,asc, we expect 'collection' to be first in the list
        Given I select "otherinst" from "Filter according to ownership by institution or sub-institution"
        When I press "Filter"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Collection"

        # reverse the sort (now type,desc); should be 'image' first
        When I click "Type"
        Then xpath "//div[@class='column cmodel']//div[contains(@class, 'row')][1]" text should equal "Image"

        # change the filter, sort by type,desc should persist
        # since 'testinst-subinst' only has audio and collection, we expect 'collection'
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

        And I am on "/emptyinst/settings"
        And for "edit-title" I enter "Emptiness University"
        And I press "Submit"
        # ensure that empty entries (deleted by CA for whatever reason)
        # don't display as emptystring
        And I am on "/emptyinst/settings"
        And for "edit-title" I enter ""
        And I press "Submit"

        And the cache has been cleared

        When I am on "/data"

        Then select list at xpath "//select[@id='edit-inst']" should contain options "anotherinst, otherinsta, emptyinst, Test Institution, Subinstitution of Test, Other Library"
        And xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "anotherinst"

        When I click "Institution/Sub-institution"
        Then xpath "//div[@class='column inst']//div[contains(@class, 'row')][1]" text should equal "Test Institution"
