<?php

// require_once 'FeatureContextBase.php';
use Behat\Behat\Hook\Scope\BeforeFeatureScope;
use Behat\Behat\Hook\Scope\AfterFeatureScope;
use Drupal\DrupalExtension\Context\RawDrupalContext;
use Drupal\DrupalExtension\Context\DrushContext;
use Drupal\DrupalUserManager;
use Behat\Behat\Context\SnippetAcceptingContext;
use Behat\Gherkin\Node\PyStringNode;
use Behat\Gherkin\Node\TableNode;
use Behat\Behat\Tester\Exception\PendingException;
use Behat\MinkExtension\Context;
use IslandoraApi\Util;
use Drupal\Component\Utility\Random;


/**
 * Defines application features from the specific context.
 */
class FeatureContext extends RawDrupalContext implements SnippetAcceptingContext {

  /**
   *
   * @Given I am viewing pid :pid
   * @Then I view pid :pid
   */
  public function iVisitPid($pid) {
    $path = '/islandora/object/' . $pid;
    $this->getSession()->visit($this->locatePath($path));
  }

  /**
   * Click on the element with the provided xpath query
   *
   * Posted by Abu Ashraf Masnun, retrieved 2018-06-27
   * http://masnun.com/2012/11/28/behat-and-mink-finding-and-clicking-with-xpath-and-jquery-like-css-selector.html
   * @When /^I click on the element with xpath "([^"]*)"$/
   */
  public function iClickOnTheElementWithXPath($xpath) {
    // Get the mink session.
    $session = $this->getSession();
    // Runs the actual query and returns the element.
    $element = $session->getPage()->find(
        'xpath',
        $session->getSelectorsHandler()->selectorToXpath('xpath', $xpath)
    );
    // Errors must not pass silently.
    if (NULL === $element) {
      throw new \InvalidArgumentException(sprintf('Could not evaluate XPath: "%s"', $xpath));
    }

    // Ok, let's click on it.
    $element->click();
  }

  /**
     * @Then xpath :xpath text should equal :text
     */
    public function xpathTextShouldEqual($xpath, $text) {
        $this->assertXpathTextEquals($xpath, $text);
    }
  
  /**
   * Find whether an xpath exists in the page.
   *
   * Adapted from code posted by Abu Ashraf Masnun, retrieved 2018-06-27
   * http://masnun.com/2012/11/28/behat-and-mink-finding-and-clicking-with-xpath-and-jquery-like-css-selector.html
   *
   * @When /^I should find xpath "([^"]*)"$/
   */
  public function iShouldFindXPath($xpath) {
    // Errors must not pass silently.
    if (!$this->xpathExists($xpath)) {
      throw new \InvalidArgumentException(sprintf('Could not evaluate XPath: "%s"', $xpath));
    }
  }

  /**
   * Find whether an xpath exists in the page.
   *
   * Adapted from code posted by Abu Ashraf Masnun, retrieved 2018-06-27
   * http://masnun.com/2012/11/28/behat-and-mink-finding-and-clicking-with-xpath-and-jquery-like-css-selector.html
   *
   * @When /^I should not find xpath "([^"]*)"$/
   */
  public function iShouldNotFindXPath($xpath) {

    // Errors must not pass silently.
    if ($this->xpathExists($xpath)) {
      throw new \InvalidArgumentException(sprintf('Could not evaluate XPath: "%s"', $xpath));
    }
  }

  /**
  * @Given I create a new collection :pid
  */
  public function iCreateANewCollection($pid) {
    module_load_include('inc', 'islandora_utils', 'includes/util');
    try {
      self::createCollectionNew($pid, 'Test collection', 'islandora:sp_large_image_cmodel');
    }
    catch (Exception $e) {
      throw new Exception ($e);
    }
  }

  /**
  * @Given I create a new collection of :num :type objects with pid :pid and title :title
  */
  public function iCreateANewCollectionForObjectsWithPidAndTitle($num, $type, $pid, $title) {
    self::createCollectionNew($pid, $title, $type);
    self::ingestObjectsIntoCollection($num, $type, $pid);
  }

  /**
   * @Given I create :num objects of type :type in collection :coll
   */
  public function iCreateObjectsOfTypeInCollection($num, $type, $coll) { 
    self::ingestObjectsIntoCollection($num, $type, $coll);
    // future self: be sure to update the collection policy
  }

  /**
    * @Then select list at xpath :xpath should contain options :options
    */
   public function selectListAtXpathShouldContainOptions($xpath, $options) {
     if (!$this->xpathExists($xpath)) {
      throw new \InvalidArgumentException(sprintf('Could not evaluate XPath: "%s"', $xpath));
    }
    $expectedOptions = array_map('trim', explode(',', $options));
    $foundOptions = [];
    $optionsXpath = "$xpath/option";
    $session = $this->getSession();
    // Runs the actual query and returns the element.
    $elements = $session->getPage()->findAll(
        'xpath',
        $session->getSelectorsHandler()->selectorToXpath('xpath', $optionsXpath)
    );
    foreach ($elements as $element) {
      // will this ever run ?
      if (NULL === $element) {
        throw new \InvalidArgumentException(sprintf('Could not evaluate XPath: "%s"', $xpath));
      }
      $foundOptions[] = trim($element->getText());
    }
    $missing = array_diff($expectedOptions, $foundOptions);
    if (!empty($missing)) {
      throw new \Exception(sprintf("Expected options [%s] but found [%s]; missing option(s) [%s] at xpath '%s'", implode(',', $expectedOptions), implode(',', $foundOptions), implode(',', $missing), $xpath));
    }
   }

   /**
     * @Then xpath list :xpathList should start with :start and end with :end
     */
    public function xpathListShouldStartWithAndEndWith($xpathList, $start, $end) {
        $this->assertXpathTextEquals(sprintf("%s[1]", $xpathList), $start);
        $this->assertXpathTextEquals(sprintf("%s[last()]", $xpathList), $end);
    }

  /**
   * Waits a while, for debugging.
   *
   * from http://programsbuzz.com/article/behat-script-wait-n-seconds
   * @param int $seconds
   *   How long to wait.
   *
   * @When I wait :seconds second(s)
   */
  public function wait($seconds) {
    sleep($seconds);
  }

  static $pidsCreated = [];
  static $admin, $repository;

  /**
   * TODO:
   * - add empty collection
   * - add collection with arbitrary policy
   *
   * @BeforeFeature
   */
  public static function setupFeature(BeforeFeatureScope $scope) {
    $fixture = [
      'testinst-images:collection' => [
        'info' => [
          'title' => 'Superinstitution Images Collection',
        ],
        'children' => [
          'image' => 11
        ],
      ],
      'testinst-subinst-audio:collection' => [
        'info' => [
          'title' => 'Subinstitution Audio Collection',
        ],
        'children' => [
          'audio' => 8
        ],
      ],
      'otherinst-images:collection' => [
        'info' => [
          'title' => 'Substring Images Collection',
          'description' => 'The namespace prefix of this collection is one of a group of overlapping namespace prefixes.'
        ],
        'children' => [
          'image' => 7
        ],
      ],
      'emptyinst-empty:collection' => [
        'info' => [
          'title' => 'Empty Collection',
        ],
        'children' => [],
      ],
      'anotherinst-image:collection' => [
        'info' => [
          'title' => 'Superstring Image Collection',
          'description' => 'The namespace prefix of this collection is one of a group of overlapping namespace prefixes.'
        ],
        'children' => [
          'audio' => 4,
          'image' => 2,
        ],
      ],
      'otherinsta-image:collection' => [
        'info' => [
          'title' => 'Superstring Image Collection',
          'description' => 'The namespace prefix of this collection is one of a group of overlapping namespace prefixes.'
        ],
        'children' => [
          'audio' => 1,
          'image' => 3,
        ],
      ],
    ];
    foreach ($fixture as $collection => $data) {
      $title = $data['info']['title'];
      self::createCollectionNew($collection, $title, array_keys($data['children']));
      foreach ($data['children'] as $model => $num) {
        self::ingestObjectsIntoCollection($num, $model, $collection);
      }
    }

    // Adds an extraneous cmodel to one collection policy.
    self::addPolicyToCollection('testinst-subinst-audio:collection', self::mapContentType('image'), 'Image', 'testinst-subinst-audio');
    // Now run the queries and clear the cache so that scenarios are simpler.
    module_load_include('inc', 'islandora_content_stats', 'includes/queries');
    islandora_content_stats_run_queries();
    cache_clear_all();
    cache_clear_all();
  }

  /** @AfterFeature */
  public static function teardownFeature(AfterFeatureScope $scope) {
    foreach (self::$pidsCreated as $pid) {
      self::getRepository()->purgeObject($pid);
      printf("Deleted %s\n", $pid);
    }
    $moduleTablesToTruncate = [
      'islandora_content_stats' => [
        'islandora_content_stats'
      ],
      'islandora_namespace_homepage' => [
        'islandora_namespace_homepage'
      ],
    ];
    foreach ($moduleTablesToTruncate as $module => $tables) {
      if (!module_exists($module)) {
        continue;
      }
      foreach ($tables as $table) {
        db_truncate($table)->execute();
      }
    }
    cache_clear_all();
  }
  
  public static function getRepository() {
    return islandora_get_tuque_connection(self::getAdminUser())->repository;
  }
  
  public static function createCollectionNew($pid, $label, $models) {
    // Set up the object and ingest it.
    $properties = array(
      'label' => $label,
      'pid' => $pid,
      'models' => 'islandora:collectionCModel',
      'parent' => 'islandora:root',
      'owner'  => self::getAdminUser()->name,
    );
    $ns = explode(':', $pid)[0];
    $policy = CollectionPolicy::emptyPolicy();
    foreach ((array) $models as $model) {
      $policy->addContentModel(self::mapContentType($model), 'New Object', $ns);
    }
    $datastreams = array(
      array(
        'dsid' => 'COLLECTION_POLICY',
        'string' => $policy->getXML(),
        'control_group' => 'X',
        'mimetype' => 'text/xml',
      ),
      array(
        'dsid' => 'TN',
        'path' => drupal_get_path('module', 'islandora_basic_collection') . '/images/folder.png',
        'control_group' => 'M',
        'mimetype' => 'image/png',
      ),
      array(
        'dsid' => 'MODS',
        'string' => self::createModsTmpFile($label),
        'control_group' => 'M',
        'mimetype' => 'text/xml',
      ),
    );
    $object = self::ingestConstructedObject($properties, $datastreams);
    $object->state = 'active';
    self::publishCollection($object->id);
    self::$pidsCreated[] = $object->id;
    printf("Created %s\n", $pid);
    return $object;
  }

  public static function publishCollection($pid) {
    if (module_exists('islandora_collection_toggle_publish')) {
      module_load_include('inc','islandora_collection_toggle_publish', 'includes/utilities');
      if (!islandora_collection_toggle_publish_collection_is_published($pid)) {
        islandora_collection_toggle_publish_publish_collection($pid);
      }
    }
  }
  
  public static function addPolicyToCollection($collection_pid, $cmodel_pid, $name, $namespace) {
    $collection = islandora_object_load($collection_pid);
    if (!$collection) {
      throw new Exception("Unable to load object with pid '$collection_pid'");
    }

    if (!isset($collection['COLLECTION_POLICY'])) {
      $policy = CollectionPolicy::emptyPolicy();
      $policy->addContentModel($cmodel_pid, $name, $namespace);
      $cp_ds = $collection->constructDatastream('COLLECTION_POLICY', 'M');
      $cp_ds->mimetype = 'application/xml';
      $cp_ds->label = 'Collection Policy';
      $cp_ds->setContentFromString($policy->getXML());
      $collection->ingestDatastream($cp_ds);
    }
    else {
      $policy = new CollectionPolicy($collection['COLLECTION_POLICY']->content);
      $policy->addContentModel($cmodel_pid, $name, $namespace);
      $collection['COLLECTION_POLICY']->setContentFromString($policy->getXML());
    }
  }
  
  /**
   * Constructs and ingests a Fedora object and datastream(s) via tuque.
   *
   * All keys inside the parameter arrays for this function are optional. it
   * can be run simply by calling the method with no arguments.
   *
   * If your test case supports logged in Drupal users, IslandoraTestUtilities
   * can be instantiated with $params['logged_in_user'] as that user object, and
   * this method will set the owner of the ingested object as that user by
   * default.
   *
   * @param array $properties
   *   An array containing object information using these keys:
   *   'label' - The object label; randomized if not set.
   *   'pid' - 'namespace:pid', or just 'namespace' to generate the suffix.
   *   'models' - An array that can contain multiple content model PIDs, or a
   *   string containing a single content model PID.
   *   'owner' - The object's owner. Defaults to the currently logged-in user,
   *   if available. It is recommended to set this to a value that can be found
   *   in $this->users; otherwise, this object will have to be manually deleted.
   *   'parent' - The PID of the parent collection.
   * @param array $datastreams
   *   An array containing zero or more datastream arrays that use the keys:
   *   'dsid' - The datastream ID; randomized if not set.
   *   'path' - The path to the file to use; defaults to fixtures/test.jpg.
   *   'string' - A string to set the datastream from; overrides 'path'.
   *   'control_group' - The single-letter control group identifier.
   *   'mimetype' - The datastream's mimetype.
   *
   * @return bool|AbstractObject
   *   FALSE if the object ingest failed, or the object if successful.
   */
  public static function ingestConstructedObject(array $properties = array(), array $datastreams = array()) {
    if (!isset($properties['pid'])) {
      $properties['pid'] = "islandora";
    }
    $object = self::getRepository()->constructObject($properties['pid']);

    // Set the object properties before ingesting it.
    if (isset($properties['label'])) {
      $object->label = $properties['label'];
    }
    else {
      $properties['label'] = DrupalUnitTestCase::randomName();
      $object->label = $properties['label'];
    }

    if (isset($properties['owner'])) {
      $object->owner = $properties['owner'];
    }
    elseif (isset($this->params['logged_in_user'])) {
      $object->owner = $this->params['logged_in_user']->name;
    }

    if (isset($properties['models'])) {
      try {
        $object->models = (array) $properties['models'];
      }
      catch (Exception $e) {

      }
    }

    // Chuck in some datastreams.
    if (!empty($datastreams)) {
      foreach ($datastreams as $datastream) {
        if (!isset($datastream['dsid'])) {
          $datastream['dsid'] = DrupalUnitTestCase::randomName();
        }
        if (!isset($datastream['control_group'])) {
          $new_datastream = $object->constructDatastream($datastream['dsid']);
        }
        else {
          $new_datastream = $object->constructDatastream($datastream['dsid'], $datastream['control_group']);
        }
        $new_datastream->label = $datastream['dsid'];
        if (isset($datastream['mimetype'])) {
          $new_datastream->mimetype = $datastream['mimetype'];
        }
        if (isset($datastream['string'])) {
          $new_datastream->setContentFromString($datastream['string']);
        }
        else {
          $path = isset($datastream['path']) ? $datastream['path'] : drupal_get_path('module', 'islandora') . '/tests/fixtures/test.jpg';
          $new_datastream->setContentFromFile($path);
        }
        $object->ingestDatastream($new_datastream);
      }
    }

    self::getRepository()->ingestObject($object);

    // Add a parent relationship, if necessary.
    if (isset($properties['parent'])) {
      $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $properties['parent']);
    }

    return $object;
  }

  private static function ingestObjectsIntoCollection($num, $type, $coll) {
    if (!ctype_digit(strval($num))) {
      throw new Exception ("Argument for 'number' must be an integer.");
    }
    $model = self::mapContentType($type);
    $ns = explode(':', $coll)[0];
    $pidsCreatedHere = [];
    foreach (range(1, $num) as $n) {
      $itemLabel = DrupalTestCase::randomName();
      $properties = array(
        'pid'    => $ns,
        'label'  => $itemLabel,
        'models' => $model,
        'parent' => $coll,
        'owner'  => self::getAdminUser()->name,
      );
      $datastreams = array(
        array(
          'dsid' => 'MODS',
          'string' => self::createModsTmpFile($itemLabel),
          'control_group' => 'M',
          'mimetype' => 'text/xml',
        ),
        array(
          'dsid' => 'TN',
          'path' => drupal_get_path('module', 'islandora_basic_collection') . '/images/folder.png',
          'control_group' => 'M',
          'mimetype' => 'image/png',
        ),
      );
      $object = self::ingestConstructedObject($properties, $datastreams);
      $object->state = 'active';
      $pidsCreatedHere[] = $object->id;
      printf("Created %s\n", $object->id);
    }
    self::$pidsCreated = array_merge(self::$pidsCreated, $pidsCreatedHere);
    return self::$pidsCreated;
  }

  /**
   * Helper fn for xpath exists.
   *
   * Adapted from code posted by Abu Ashraf Masnun, retrieved 2018-06-27
   * http://masnun.com/2012/11/28/behat-and-mink-finding-and-clicking-with-xpath-and-jquery-like-css-selector.html
   *
   * @param string $xpath
   *   xpath to find
   */
  public function xpathExists($xpath) {
    // Get the mink session.
    $session = $this->getSession();
    // Runs the actual query and returns the element.
    $element = $session->getPage()->find(
        'xpath',
        $session->getSelectorsHandler()->selectorToXpath('xpath', $xpath)
    );
    if (NULL === $element) {
      return FALSE;
    }
    return TRUE;
  }

  public function assertXpathTextEquals($xpath, $text) {
    $session = $this->getSession();
    // Runs the actual query and returns the element.
    $element = $session->getPage()->find(
        'xpath',
        $session->getSelectorsHandler()->selectorToXpath('xpath', $xpath)
    );
    if (NULL === $element) {
      throw new \InvalidArgumentException(sprintf('Could not evaluate XPath: "%s"', $xpath));
    }
    if ($element->getText() != $text) {
      throw new \Exception(sprintf("Expected text '%s' but found '%s' at xpath '%s'", $text, $element->getText(), $xpath));
    }
  }

  /**
   * @Given /^breakpoint$/
   */
  public function breakpoint() {
    fwrite(STDOUT, "\033[s \033[93m[Breakpoint] Press \033[1;93m[RETURN]\033[0;93m to continue...\033[0m");
    while (fgets(STDIN, 1024) == '') {}
    fwrite(STDOUT, "\033[u");
    return;
  }
  
  protected static function mapContentType($type) {
    $map = [
      'image' => 'sp_large_image_cmodel',
      'audio' => 'sp-audioCModel',
    ];
    if (!array_key_exists($type, $map)) {
      throw new Exception("$type is not recognized in this FeatureContext; you can define it.");
    }
    return "islandora:{$map[$type]}";
  }

  public static function getAdminUser() {
    $user_exists = user_load_by_name('behat');
    if ($user_exists) {
      user_delete($user_exists->uid);
    }
    $account = new stdClass();
    $account->name = 'behat';
    $account->pass = 'behat';
    $account->email = 'behat@example.com';
    $user = user_save($account);
    $role = user_role_load_by_name("administrator");
    user_multiple_role_edit(array($user->uid), 'add_role', $role->rid);
    return user_load_by_name('behat');
  }

  public static function createModsTmpFile($label = '', $description = '') {
    $rnd = new Random();
    $label = $label === '' ? $rnd->sentences(10) : $label;
    $description = $description === '' ? $rnd->paragraphs(2) : $description;
    $mods = <<<EOF
<?xml version="1.0"?>
<mods xmlns="http://www.loc.gov/mods/v3" xmlns:mods="http://www.loc.gov/mods/v3" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xmlns:xlink="http://www.w3.org/1999/xlink">
  <titleInfo>
    <title>$label</title>
  </titleInfo>
  <name type="personal">
    <namePart></namePart>
    <role>
      <roleTerm authority="marcrelator" type="text"></roleTerm>
    </role>
  </name>
  <typeOfResource collection="yes"></typeOfResource>
  <genre authority="lctgm"></genre>
  <originInfo>
    <dateCreated></dateCreated>
  </originInfo>
  <language>
    <languageTerm authority="iso639-2b" type="code">eng</languageTerm>
  </language>
  <abstract>$description</abstract>
  <identifier type="local"></identifier>
  <physicalDescription>
    <form authority="marcform"></form>
    <extent></extent>
  </physicalDescription>
  <note></note>
  <accessCondition></accessCondition>
  <subject>
    <topic></topic>
    <geographic></geographic>
    <temporal></temporal>
    <hierarchicalGeographic>
      <continent></continent>
      <country></country>
      <province></province>
      <region></region>
      <county></county>
      <city></city>
      <citySection></citySection>
    </hierarchicalGeographic>
    <cartographics>
      <coordinates></coordinates>
    </cartographics>
  </subject>
</mods>
EOF;
  return $mods;
  }

}
