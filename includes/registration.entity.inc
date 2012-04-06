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
      'entity class' => 'Registration',
      'metadata controller class' => 'RegistrationMetadataController',
      'views controller class' => 'RegistrationViewsController',
      'base table' => 'registration',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'registration_id',
        'bundle' => 'type',
      ),
      'access callback' => 'registration_access',
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
      'uri callback' => 'entity_class_uri',
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

/**
 * Main class for Registration entities.
 */
class Registration extends Entity {

  public
    $registration_id,
    $entity_id,
    $entity_type,
    $entity,
    $mail,
    $anon_mail = NULL,
    $user_uid = NULL,
    $user = NULL,
    $count,
    $author,
    $author_uid,
    $created,
    $updated;

  public function __construct(array $values = array(), $entityType = NULL) {
    parent::__construct($values, $entityType);

    $this->author = $this->author();
    $this->entity = $this->entity();
    if ($this->user_uid) {
      $this->user = user_load($this->user_uid);
    }
    $this->mail = $this->mail();
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

  protected function entity() {
    $ret = FALSE;
    if (isset($this->entity_id) && isset($this->entity_type)) {
      $ret = entity_load_single($this->entity_type, $this->entity_id);
    }
    return $ret;
  }

  protected function author() {
    return (isset($this->author_uid)) ? user_load($this->author_uid) : FALSE;
  }

  /**
   * Save registration.
   *
   * @return mixed
   */
  public function save() {
    $this->updated = REQUEST_TIME;
    if (!$this->registration_id) {
      $this->created = REQUEST_TIME;
    }
    return parent::save();
  }

  /**
   * Specifify URI
   */
  protected function defaultUri() {
    return array('path' => 'registration/' . $this->internalIdentifier());
  }

  protected function mail() {
    if ($this->user) {
      return $this->user->mail;
    }
    else {
      return $this->anon_mail;
    }
  }

}

/**
 * @see hook_entity_property_info().
 */
class RegistrationMetadataController extends EntityDefaultMetadataController {
  public function entityPropertyInfo() {
    $info = parent::entityPropertyInfo();
    $properties = &$info[$this->type]['properties'];

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
    $properties['entity'] = array(
      'label' => t('Entity'),
      'type' => 'entity',
      'schema field' => 'entity_id',
      'description' => t('The entity the registration belongs to.'),
      'getter callback' => 'registration_get_properties',
    );
    $properties['author'] = array(
      'label' => t("Author"),
      'type' => 'user',
      'description' => t("The author of the registration."),
      'schema field' => 'author_uid'
    );
    $properties['user'] = array(
      'label' => t("User"),
      'type' => 'user',
      'description' => t("The user associated with the registration."),
      'schema field' => 'user_uid'
    );

    return $info;
  }
}

/**
 * Required by RegistrationMetadataController for Views integration.
 *
 * @param $entity
 * @param array $options
 * @param $name
 * @param $type
 *
 * @return EntityMetadataWrapper
 */
function registration_get_properties($entity, array $options, $name, $type) {
  switch ($name) {
    case 'entity':
      return entity_metadata_wrapper($entity->entity_type, $entity->entity);
      break;
  }
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
 *   An array of registration objects indexed by registration_id.
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
  return entity_get_controller('registration')->delete($registration_ids);
}

/**
 * Saves a registration.
 *
 * @param $registration
 *   The full registration object to save.
 *
 * @return
 *   If the record insert or update failed, returns FALSE. If it succeeded,
 *   returns SAVED_NEW or SAVED_UPDATED, depending on the operation performed.
 */
function registration_save(Registration $registration) {
  return $registration->save();
}

/**
 * Access callback for the entity API.
 */
function registration_access($op, $registration = NULL, $account = NULL, $entity_type) {
  $account = isset($account) ? $account : $GLOBALS['user'];

  // First grant access to the entity for the specified operation if no other
  // module denies it and at least one other module says to grant access.
  $access_results = module_invoke_all('registration_access', $op, $registration, $account, $entity_type);
  if (in_array(FALSE, $access_results, TRUE)) {
    return FALSE;
  }
  elseif (in_array(TRUE, $access_results, TRUE)) {
    return TRUE;
  }

  // bypass further access checks if user can administer registration
  if (user_access('administer registration', $account)) {
    return TRUE;
  }

  $account_own = (isset($registration->author->uid) && ($registration->author->uid == $account->uid));

  $type = $registration->type;
  switch ($op) {
    case 'view':
      return ($account_own && user_access("view own $type registration", $account)) || user_access("view $type registration", $account);
    case 'update':
      return ($account_own && user_access("update own $type registration", $account)) || user_access("update any $type registration", $account);
    case 'create':
      return user_access("create $type registration", $account);
    case 'delete':
      return ($account_own && user_access("delete own $type registration", $account)) || user_access("delete any $type registration", $account);
  }
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
function registration_type_access($op, $entity = NULL, $account = NULL, $entity_type = NULL) {
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

