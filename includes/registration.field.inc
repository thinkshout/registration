<?php

/**
 * Implements hook_field_info().
 */
function registration_field_info() {
  return array(
    'registration' => array(
      'label' => t('Registration'),
      'description' => t('Enables registrations of a selected type for an entity.'),
      'settings' => array(),
      'instance_settings' => array(),
      'default_widget' => 'registration_select',
      'default_formatter' => 'registration_default',
      'no_ui' => FALSE
    ),
  );
}

/**
 * Implements hook_field_validate().
 */
function registration_field_validate($entity_type, $entity, $field, $instance, $langcode, $items, &$errors) {

}

/**
 * Define what constitutes an empty item for a field type.
 *
 * @param $item
 *   An item that may or may not be empty.
 * @param $field
 *   The field to which $item belongs.
 *
 * @return
 *   TRUE if $field's type considers $item not to contain any data;
 *   FALSE otherwise.
 */
function registration_field_is_empty($item, $field) {
  if (empty($item['registration_type']) && (string) $item['registration_type'] !== '') {
    return TRUE;
  }
  return FALSE;
}

/**
 * Implements hook_field_widget_info().
 */
function registration_field_widget_info() {
    return array(
    'registration_select' => array(
      'label' => t('Registration Type'),
      'field types' => array('registration'),
      'settings' => array(),
      'behaviors' => array(
        'multiple values' => FIELD_BEHAVIOR_DEFAULT,
        'default value' => FIELD_BEHAVIOR_DEFAULT,
      ),
    ),
  );
}

/**
 * Implements hook_field_widget_form().
 */
function registration_field_widget_form(&$form, &$form_state, $field, $instance, $langcode, $items, $delta, $element) {
  $options = array('' => t('-- Disable Registrations --'));
  foreach(registration_get_types() as $type) {
    $options[$type->name] = $type->label;
  }
  $element += array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => isset($items[$delta]) ? $items[$delta] : array(),
  );

  // force some help text into the field, appending anything the user addded.
  $element['#description'] .= ' ' . t('Select what type of registrations should be
    enabled for this @type. Depending on the display settings, it will appear
    as either string, registration link, or form.', array('@type' => $instance['bundle']));

  return array('registration_type' => $element);
}

/**
 * Implements hook_field_formatter_info().
 */
function registration_field_formatter_info() {
  return array(
    'registration_default' => array(
      'label' => t('Default'),
      'field types' => array('registration'),
    ),
    'registration_link' => array(
      'label' => t('Registration Link'),
      'field types' => array('registration'),
    ),
    'registration_form' => array(
      'label' => t('Registration Form'),
      'field types' => array('registration'),
    ),
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function registration_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  // we know we should only have a single item
  if (isset($items[0]['registration_type']) && !empty($items[0]['registration_type'])) {
    $reg_type = registration_type_load($items[0]['registration_type']);
    switch ($display['type']) {
      case 'registration_default':
        $element[0] = array('#markup' => $reg_type->label);
        break;
      case 'registration_link':
        // enable registration link if accessible
        if (registration_register_page_access($entity_type, $entity)) {
          $uri = entity_uri($entity_type, $entity);
          $element[0] = array('#markup' => l($reg_type->label, $uri['path']));
        }
        break;
      case 'registration_form':
        // enable registration link if accessible
        if (registration_register_page_access($entity_type, $entity)) {
          module_load_include('inc', 'registration', 'includes/registration.forms');
          list($entity_id, ,) = entity_extract_ids($entity_type, $entity);
          $registration = entity_get_controller('registration')->create(array('type' => $reg_type->name));
          $registration->entity_id = $entity_id;
          $registration->entity_type = $entity_type;
          $element[0] = drupal_get_form('registration_form', $registration);
        }
        break;
    }
  }

  return $element;

}
