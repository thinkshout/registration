<?php

/**
 * @file
 * Field hooks.
 */

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
 * Implements hook_field_is_empty().
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
  foreach (registration_get_types() as $type) {
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
          $element[0] = array('#markup' => theme('registration_link',
            array(
              'label' => $reg_type->label,
              'path'  => $uri['path'] . '/register')
          ));
        }
        break;
      case 'registration_form':
        // enable registration link if accessible
        list($entity_id) = entity_extract_ids($entity_type, $entity);
        if (registration_register_page_access($entity_type, $entity) && registration_status($entity_type, $entity_id)) {
          $registration = entity_get_controller('registration')->create(array(
            'entity_type' => $entity_type,
            'entity_id' => $entity_id,
            'type' => $reg_type->name,
          ));
          $element[0] = drupal_get_form('registration_form', $registration);
        }
        break;
    }
  }

  return $element;
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Hide the cardinality setting on the field settings for registration fields.
 */
function registration_form_field_ui_field_edit_form_alter(&$form, &$form_state, $form_id) {
  if ($form['#field']['type'] == 'registration') {
    $form['field']['cardinality']['#default_value'] = 1;
    $form['field']['cardinality']['#access'] = FALSE;
    $form['#validate'][] = 'registration_form_field_ui_field_edit_form_validate';
  }
}

/**
 * Validation handler for registration_form_field_ui_field_edit_form.
 *
 * Ensure cardinality is set to 1 on registration fields.
 *
 * @param $form
 * @param $form_state
 */
function registration_form_field_ui_field_edit_form_validate(&$form, &$form_state) {
  if ($form['#field']['type'] == 'registration') {
    if ($form_state['values']['field']['cardinality'] !== 1) {
      form_set_error('cardinality', t('Cardinality on registration fields must be set to one.'));
    }
  }
}

/**
 * Implements hook_form_FORM_ID_alter().
 *
 * Add a validation handler for registration_form_field_ui_field_overview_form().
 * @param $form
 * @param $form_state
 * @param $form_id
 */
function registration_form_field_ui_field_overview_form_alter(&$form, &$form_state, $form_id) {
  $form['#validate'][] = 'registration_form_field_ui_field_overview_form_validate';
}

/**
 * Validation callback for registration_form_field_ui_field_overview_form().
 *
 * Ensure only one registration field is added per entity.
 *
 * @param $form
 * @param $form_state
 */
function registration_form_field_ui_field_overview_form_validate(&$form, &$form_state) {
  $fields = $form_state['values']['fields'];
  if ($fields['_add_new_field']['type'] == 'registration') {
    foreach ($form['#fields'] as $field_name) {
      $field = field_info_field($field_name);
      if ($field['type'] == 'registration') {
        form_set_error('_add_new_field', t('An entity can only have one registration field.'));
      }
    }
  }
}

/**
 * Implements hook_field_create_instance().
 */
function registration_field_create_instance($instance) {
  // rebuild menu to recognize registration paths.
  _registration_menu_rebuild($instance);
}

/**
 * Implements hook_field_delete_instance().
 */
function registration_field_delete_instance($instance) {
  // remove registration paths from menu router
  _registration_menu_rebuild($instance);

  // @TODO: should we delete all registrations at this point?
}

/**
 * Rebuild the menu for a given registraiton field instance.
 *
 * @param $instance
 *   Field instance being added or deleted.
 */
function _registration_menu_rebuild($instance) {
  $registration_fields = field_read_fields(array('type' => 'registration'));
  if (in_array($instance['field_name'], array_keys($registration_fields))) {
    menu_rebuild();
  }
}
