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
  $options = array('' => t('-- Select --'));
  foreach(registration_get_types() as $type) {
    $options[$type->name] = $type->label;
  }
  $element += array(
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => isset($items[$delta]) ? $items[$delta] : array(),
  );

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
  );
}

/**
 * Implements hook_field_formatter_view().
 */
function registration_field_formatter_view($entity_type, $entity, $field, $instance, $langcode, $items, $display) {
  $element = array();

  switch ($display['type']) {
    case 'registration_default':
      // Common case: each value is displayed individually in a sub-element
      // keyed by delta. The field.tpl.php template specifies the markup
      // wrapping each value.
      foreach ($items as $delta => $item) {
        $reg_type = registration_type_load($item['registration_type']);
        $element[$delta] = array('#markup' => $reg_type->label);
      }
      break;
  }

  return $element;

}
