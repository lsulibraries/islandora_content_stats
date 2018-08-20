<?php

// require_once 'FeatureContextBase.php';
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
class FeatureContext extends FeatureContextBase implements SnippetAcceptingContext {

  static $baseCollection = 'testing-testing-123:collection';
  public $admin, $pidsCreated, $repository;

  /**
   * Initializes context.
   *
   * Every scenario gets its own context instance.
   * You can also pass arbitrary arguments to the
   * context constructor through behat.yml.
   */
  public function __construct() {
    $this->admin = self::getAdminUser();
    $this->repository = islandora_get_tuque_connection($this->admin)->repository;
    parent::__construct();
  }

  
  /** @BeforeScenario */
  public function before($event)
  {
    $this->pidsCreated = [];
  }

  /** @AfterScenario */
  public function after($event)
  {
    foreach ($this->pidsCreated as $pid) {
      $this->repository->purgeObject($pid);
    }
  }

  public function createCollectionNew($pid, $label, $models) {
    // Set up the object and ingest it.
    $properties = array(
      'label' => $label,
      'pid' => $pid,
      'models' => 'islandora:collectionCModel',
      'parent' => 'islandora:root',
      'owner'  => $this->admin->name,
    );
    $ns = explode(':', $pid)[0];
    $policy = CollectionPolicy::emptyPolicy();
    foreach ((array) $models as $model) {
      $policy->addContentModel($this->mapContentType($model), 'New Object', $ns);
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
    $object = $this->ingestConstructedObject($properties, $datastreams);
    $object->state = 'active';
    $this->publishCollection($object->id);
    $this->pidsCreated[] = $object->id;
    return $object;
  }

  public function publishCollection($pid) {
    if (module_exists('islandora_collection_toggle_publish')) {
      module_load_include('inc','islandora_collection_toggle_publish', 'includes/utilities');
      if (!islandora_collection_toggle_publish_collection_is_published($pid)) {
        islandora_collection_toggle_publish_publish_collection($pid);
      }
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
  public function ingestConstructedObject(array $properties = array(), array $datastreams = array()) {
    if (!isset($properties['pid'])) {
      $properties['pid'] = "islandora";
    }
    $object = $this->repository->constructObject($properties['pid']);

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

    $this->repository->ingestObject($object);

    // Add a parent relationship, if necessary.
    if (isset($properties['parent'])) {
      $object->relationships->add(FEDORA_RELS_EXT_URI, 'isMemberOfCollection', $properties['parent']);
    }

    return $object;
  }

  /**
  * @Given I create a new collection :pid
  */
  public function iCreateANewCollection($pid) {
    module_load_include('inc', 'islandora_utils', 'includes/util');
    try {
      $this->createCollectionNew($pid, 'Test collection', 'islandora:sp_large_image_cmodel');
    }
    catch (Exception $e) {
      throw new Exception ($e);
    }
  }

  /**
  * @Given I create a new collection of :num :type objects with pid :pid and title :title
  */
  public function iCreateANewCollectionForObjectsWithPidAndTitle($num, $type, $pid, $title) {
    $this->createCollectionNew($pid, $title, $type);
    $this->ingestObjectsIntoCollection($num, $type, $pid);
  }

  /**
   * @Given I create :num objects of type :type in collection :coll
   */
  public function iCreateObjectsOfTypeInCollection($num, $type, $coll) { 
    $this->ingestObjectsIntoCollection($num, $type, $coll);
    // future self: be sure to update the collection policy
  }

  private function ingestObjectsIntoCollection($num, $type, $coll) {
    if (!ctype_digit(strval($num))) {
      throw new Exception ("Argument for 'number' must be an integer.");
    }
    $model = $this->mapContentType($type);
    $ns = explode(':', $coll)[0];
    $pidsCreatedHere = [];
    foreach (range(1, $num) as $n) {
      $itemLabel = DrupalTestCase::randomName();
      $properties = array(
        'pid'    => $ns,
        'label'  => $itemLabel,
        'models' => $model,
        'parent' => $coll,
        'owner'  => $this->admin->name,
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
      $object = $this->ingestConstructedObject($properties, $datastreams);
      $object->state = 'active';
      $pidsCreatedHere[] = $object->id;
    }
    $this->pidsCreated = array_merge($this->pidsCreated, $pidsCreatedHere);
    return $pidsCreatedHere;
  }
  
  protected function mapContentType($type) {
    $map = [
      'image' => 'sp_large_image_cmodel',
      'audio' => 'sp-audioCModel',
    ];
    if (!array_key_exists($type, $map)) {
      throw new Exception("$type is not recognized in this FeatureContext; you can define it.");
    }
    return "islandora:{$map[$type]}";
  }
  
  /**
   * @Given I am viewing the test collection
   */
  public function iAmViewingTheTestCollection()
  {
      $path = '/islandora/object/' . self::$baseCollection;
      $this->getSession()->visit($this->locatePath($path));
  }

  /**
   * @BeforeSuite
   */
  public static function setup() {
//    $admin = self::getAdminUser();
//    self::createCollection($admin);
//    self::createModsTmpFile();
  }

  /**
   * @AfterSuite
   */
  public static function teardown() {
//    global $user;
//    $original_user = $user;
//    $old_state = drupal_save_session();
//    drupal_save_session(FALSE);
//    $user = $behat = user_load_by_name('behat');
//
//    self::deleteCollection(self::$baseCollection);
//    $user = $original_user;
//    drupal_save_session($old_state);
//
//    if($behat) {
//      user_delete($behat->uid);
//    }
  }

  static function createCollection($user, \stdClass $collection = NULL) {
    module_load_include('inc', 'islandora_utils', 'includes/util');
    if (!$collection) {
      $collection = new stdClass();
      $collection->title = 'Test Collection';
      $collection->descript = "This is a description for a test collection";
      $collection->pid = self::$baseCollection;
    }
    islandora_utils_ingest_collection($collection->title, $collection->descript, $collection->pid, 'islandora:root', array(), $user);
  }

  public static function deleteCollection($pid) {
    $object = islandora_object_load($pid);
    if ($object) {
      module_load_include('inc', 'islandora_solution_pack_collection', 'includes/batch');
      batch_set(islandora_basic_collection_delete_children_batch($object));
      islandora_delete_object($object);
    }
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
