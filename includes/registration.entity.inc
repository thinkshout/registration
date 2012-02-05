<?php

/**
 * @file
 * Entity hooks and callbacks for registrations.
 */

/**
 * Implements hook_entity_info().
 */
function registration_entity_info() {
  $entities = array(
    'registration' => array(
      'label' => t('Registration'),
      'plural label' => t('Registrations'),
      'controller class' => 'EntityAPIController',
      'entity class' => 'RegistrationEntityClass',
      'views controller class' => 'RegistrationViewsController',
      'base table' => 'registration',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'registration_id',
        'bundle' => 'type',
      ),
      'bundle keys' => array(
        'bundle' => 'name',
      ),
      'bundles' => array(),
      'view modes' => array(
        'full' => array(
          'label' => t('Full Registration'),
          'custom settings' => FALSE,
        ),
      ),
      'uri callback' => 'registration_uri',
      'token type' => 'registration',
      'module' => 'registration',
    ),
    'registration_type' => array(
      'label' => t('Registration type'),
      'entity class' => 'RegistrationType',
      'controller class' => 'EntityAPIControllerExportable',
      'base table' => 'registration_type',
      'fieldable' => FALSE,
      'bundle of' => 'registration',
      'exportable' => TRUE,
      'entity keys' => array(
        'id' => 'id',
        'name' => 'name',
        'label' => 'label',
      ),
      'access callback' => 'registration_type_access',
      'module' => 'registration',
      // Enable the entity API's admin UI.
      'admin ui' => array(
        'path' => 'admin/structure/registration_types',
        'file' => 'registration_type.admin.inc',
        'file path' => drupal_get_path('module', 'registration') . '/includes',
        'controller class' => 'RegistrationTypeUIController',
      ),
    ),
  );

  // Support entity cache module.
  if (module_exists('entitycache')) {
    $return['registration']['field cache'] = FALSE;
    $return['registration']['entity cache'] = TRUE;
  }

  return $entities;
}

/**
 * Implements hook_entity_info_alter().
 *
 * We are adding the info about the registration types via a hook to avoid a recursion
 * issue as loading the model types requires the entity info as well.
 */
function registration_entity_info_alter(&$entity_info) {
  foreach (registration_get_types() as $type => $info) {
    $entity_info['registration']['bundles'][$type] = array(
      'label' => $info->label,
      'admin' => array(
        'path' => 'admin/structure/registration_types/manage/%registration_type',
        'real path' => 'admin/structure/registration_types/manage/' . $type,
        'bundle argument' => 4,
        'access arguments' => array('administer registration types'),
      ),
    );
  }
}

function registration_uri($registration) {
  return array(
    'path' => 'registration/' . $registration->registration_id,
  );
}

/**
 * Main class for test entities.
 */
class RegistrationEntityClass extends Entity {

  public function __construct(array$values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);
  }

  /**
   * Override buildContent() to add registration properties.
   */
  public function buildContent($view_mode = 'full', $langcode = NULL) {
    // invoke theme handler to return markup for registration properties
    $content['registration'] = array(
      '#markup' => theme('registration', array('registration' => $this)),
    );

    return entity_get_controller($this->entityType)
			->buildContent($this, $view_mode, $langcode, $content);
  }

  /**
   * Specifies the default label, which is picked up by label() by default.
   */
  protected function defaultLabel() {
    $type = entity_test_get_types($this->name);
    return $type->label;
  }
}

/**
 * Implements hook_entity_property_info_alter().
 */
function registration_entity_property_info_alter(&$info) {
  $properties = &$info['registration']['properties'];

  // Convert timestamps props to dates
  $properties['created'] = array(
    'label' => t("Created"),
    'description' => t("The date the registration was created."),
    'type' => 'date',
    'schema field' => 'created',
  );
  $properties['updated'] = array(
    'label' => t("Updated"),
    'description' => t("The date the registration was updated."),
    'type' => 'date',
    'schema field' => 'updated',
  );
  // convert author uid to a drupal user
  $properties['author'] = array(
    'label' => t('Registration Author'),
    'description' => t('Drupal user who created this registration.'),
    'type' => 'user',
    'schema field' => 'author_uid'
  );

  return $info;
}

/**
 * Loads a registration by ID.
 */
function registration_load($registration_id) {
  if (empty($registration_id)) {
    return FALSE;
  }

  $registrations = registration_load_multiple(array($registration_id), array());
  return $registrations ? reset($registrations) : FALSE;
}

/**
 * Loads multiple registrations by ID or based on a set of matching conditions.
 *
 * @see entity_load()
 *
 * @param $registration_ids
 * @param $conditions
 *   An array of conditions on the {registration} table in the form
 *     'field' => $value.
 * @param $reset
 *   Whether to reset the internal registration loading cache.
 *
 * @return
 *   An array of contact objects indexed by registration_id.
 */
function registration_load_multiple($registration_ids = array(), $conditions = array(), $reset = FALSE) {
  if (empty($registration_ids) && empty($conditions)) {
    return array();
  }

  return entity_load('registration', $registration_ids, $conditions, $reset);
}

/**
 * Deletes multiple registrations by ID.
 *
 * @param $registration_ids
 *   An array of registration IDs to delete.
 *
 * @return
 *   TRUE on success, FALSE otherwise.
 */
function registration_delete_multiple($registration_ids) {
  // Let modules act on a registration about to be deleted.
  foreach ($registration_ids as $regid) {
    $reg = registration_load($regid);
    module_invoke_all('registration_delete', $reg);
  }

  return entity_get_controller('registration')->delete($registration_ids);
}

/**
 * Saves a registration.
 *
 * @param $registration
 *   The full contact object to save.
 *
 * @return
 *   The saved registration object.
 */
function registration_save($registration) {
  $registration->updated = REQUEST_TIME;
  // Set the creation timestamp if not set.
  if (!isset($registration->created) || empty($registration->created)) {
    $registration->created = REQUEST_TIME;
  }

  $ret = entity_get_controller('registration')->save($registration);

  // Let other modules act on a registration save
  module_invoke_all('registration_save', $ret);

  return $ret;
}

/**
 * Implements hook_field_extra_fields().
 */
function registration_field_extra_fields() {
	// expose the email property on the fields and display settings forms.
  $extra = array();
  foreach (registration_get_types() as $type => $reg_type) {
    $extra['registration'][$type] = array(
      'form' => array(
        'mail' => array(
          'label' => t('Email'),
          'description' => t('Registrant\'s email address.'),
          'weight' => 0,
        ),
      ),
      'display' => array(
        'mail' => array(
          'label' => t('Email'),
          'description' => t('Registrant\'s email address.'),
          'weight' => 0,
        ),
      )
    );
  }

  return $extra;
}

/**
 * The class used for model type entities
 */
class RegistrationType extends Entity {

  public $name;
  public $label;
  public $locked;

  public function __construct($values = array()) {
    parent::__construct($values, 'registration_type');
  }
}

/**
 * Registration Type UI controller.
 */
class RegistrationTypeUIController extends EntityDefaultUIController {

  /**
   * Overrides hook_menu() defaults.
   */
  public function hook_menu() {
    $items = parent::hook_menu();
		$items[$this->path]['description'] = 'Manage registration entity types, including adding
		and removing fields and the display of fields.';
    return $items;
  }
}

/**
 * Gets an array of all registration types, keyed by the name.
 *
 * @param $name
 *   If set, the type with the given name is returned.
 */
function registration_get_types($name = NULL) {
  $types = entity_load_multiple_by_name('registration_type', isset($name) ? array($name) : FALSE);
  return isset($name) ? reset($types) : $types;
}

/**
 * Menu argument loader; Load a registration type by string.
 *
 * @param $type
 *   The machine-readable name of a registration type to load.
 * @return
 *   A registration type array or FALSE if $type does not exist.
 */
function registration_type_load($type) {
  return registration_get_types($type);
}

/**
 * Access callback for the entity API.
 */
function registration_type_access($op, $type = NULL, $account = NULL) {
  return user_access('administer registration types', $account);
}

/**
 * Saves a model type to the db.
 */
function registration_type_save(RegistrationType $type) {
  $type->save();
}

/**
 * Deletes a model type from the db.
 */
function registration_type_delete(RegistrationType $type) {
  $type->delete();
}
