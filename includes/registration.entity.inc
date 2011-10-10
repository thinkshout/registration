<?php

/**
 * @file
 * Entity hooks and callbacks for registrations.
 */

/**
 * Implements hook_entity_info().
 */
function registration_entity_info() {
  return array(
    'registration' => array(
      'module' => 'registration',
      'label' => t('Registration'),
      'controller class' => 'EntityAPIController',
      'entity class' => 'RegistrationEntityClass',
      'base table' => 'registration',
      'fieldable' => TRUE,
      'entity keys' => array(
        'id' => 'registration_id',
        'bundle' => 'type',
      ),
      'bundle keys' => array(
        'bundle' => 'type',
      ),
      'bundles' => array(
        'registration' => array(
          'label' => 'Registration',
          'admin' => array(
            'path' => 'admin/structure/registration/manage',
            'access arguments' => array('administer registration'),
          ),
        ),
      ),
      'view modes' => array(
        'full' => array(
          'label' => t('Full Registration'),
          'custom settings' => FALSE,
        ),
      ),
      'uri callback' => 'registration_uri',
      'token type' => 'registration',
    ),
  );
}

function registration_uri($registration) {
  return array(
    'path' => 'registration/' . $registration->id,
  );
}

/**
 * Main class for test entities.
 */
class RegistrationEntityClass extends Entity {

  public function __construct(array$values = array(), $entityType = NULL) {
    parent::__construct($values, 'registration');
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
  $properties['nid'] = array(
    'label' => t("Node"),
    'description' => t("The node this registration is associated with."),
    'type' => 'node',
    'schema field' => 'nid',
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
 *   An array of conditions on the {redhen_contact} table in the form
 *     'field' => $value.
 * @param $reset
 *   Whether to reset the internal contact loading cache.
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
 *   An array of contact IDs to delete.
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

  return entity_get_controller('registration')->save($registration);
}

/**
 * Implements hook_field_extra_fields().
 */
function registration_field_extra_fields() {
	// expose the email property on the fields and display settings forms.
  $extra['registration']['registration'] = array(
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

  return $extra;
}
