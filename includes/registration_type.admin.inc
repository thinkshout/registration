<?php

/**
 * @file
 * Model type editing UI.
 */

/**
 * Generates the model type editing form.
 */
function registration_type_form($form, &$form_state, $registration_type, $op = 'edit') {
  if ($op == 'clone') {
    $registration_type->label .= ' (cloned)';
    $registration_type->name = '';
  }

  $form['label'] = array(
    '#title' => t('Label'),
    '#type' => 'textfield',
    '#default_value' => $registration_type->label,
    '#description' => t('The human-readable name of this model type.'),
    '#required' => TRUE,
    '#size' => 30,
  );
  // Machine-readable type name.
  $form['name'] = array(
    '#type' => 'machine_name',
    '#default_value' => isset($registration_type->name) ? $registration_type->name : '',
    '#maxlength' => 32,
    '#disabled' => $registration_type->locked && $op != 'clone',
    '#machine_name' => array(
      'exists' => 'registration_get_types',
      'source' => array('label'),
    ),
    '#description' => t('A unique machine-readable name for this registration type. It must only contain lowercase letters, numbers, and underscores.'),
  );

  // Greg
  $form['drupal_entity'] = array(
    '#title' => t('Drupal entity'),
    '#type' => 'fieldset',
    '#attributes' => array(
      'id' => array('registration-drupal-entity'),
    ),
  );

  // Prep the entity type list before creating the form item:
  $entity_types = array('' => t('-- Select --'));
  foreach (entity_get_info() as $entity_type => $info) {

    // Ignore registration entity types:
    if (strpos($entity_type, 'registration') === FALSE) {
      $entity_types[$entity_type] = $info['label'];
    }
  }
  asort($entity_types);
  $form['drupal_entity']['registrant_entity_type'] = array(
    '#title' => t('Entity type'),
    '#type' => 'select',
    '#options' => $entity_types,
    '#default_value' => isset($registration_type->registrant_entity_type) ? $registration_type->registrant_entity_type : '',
    '#required' => TRUE,
    '#description' => t('Select a registrant entity type (default: User).'),
    '#ajax' => array(
      'callback' => 'registration_mapping_form_callback',
      'wrapper' => 'registration-drupal-entity',
    ),
  );

  $form_entity_type = & $form_state['values']['registrant_entity_type'];
  if ($form_entity_type) {

    // Prep the bundle list before creating the form item:
    $bundles = array('' => t('-- Select --'));
    $info  = entity_get_info($form_entity_type);
    foreach ($info['bundles'] as $key => $bundle) {
      $bundles[$key] = $bundle['label'];
    }
    asort($bundles);
    $form['drupal_entity']['registrant_bundle'] = array(
      '#title' => t('Entity bundle'),
      '#type' => 'select',
      '#required' => TRUE,
      '#description' => t('Select a registrant entity bundle with a Drupal user id and email address.'),
      '#options' => $bundles,
      '#default_value' => isset($registration_type->registrant_bundle) ? $registration_type->registrant_bundle : '',
      '#ajax' => array(
        'callback' => 'registration_mapping_form_callback',
        'wrapper' => 'registration-drupal-entity',
      ),
    );

    $form_bundle = & $form_state['values']['registrant_bundle'];
    if ($form_bundle) {

      // Prep the field & properties list before creating the form item:
      $fields = registration_email_fieldmap_options($form_entity_type, $form_bundle);

      $form['drupal_entity']['registrant_uid_property'] = array(
        '#title' => t('User id Property'),
        '#type' => 'select',
        '#required' => TRUE,
        '#description' => t('Select the field which contains the Drupal user id.'),
        '#options' => $fields,
        '#default_value' => isset($registration_type->registrant_uid_property) ? $registration_type->registrant_uid_property : '',
        '#maxlength' => 127,
      );

      $form['drupal_entity']['registrant_email_property'] = array(
        '#title' => t('Email property'),
        '#type' => 'select',
        '#required' => TRUE,
        '#description' => t('Select the field which contains the email address.'),
        '#options' => $fields,
        '#default_value' => isset($registration_type->registrant_email_property) ? $registration_type->registrant_email_property : '',
        '#maxlength' => 127,
      );
    }
  }

  // Setting for default state for regsitrations of this type.
  // Overrides global default state setting.
  $state_options = registration_get_states_options();
  $form['default_state'] = array(
    '#title' => 'Default state',
    '#type' => 'select',
    '#description' => t('The default state for this registration type. Overrides the global default state set at /admin/structure/registration/registration_states.'),
    '#default_value' => isset($registration_type->default_state) ? $registration_type->default_state : 0,
    '#options' => array('none' => 'None') + $state_options,
  );

  // Setting for how long a held registration will exist before it is removed from held state.
  $form['data']['held_expire'] = array(
    '#type' => 'textfield',
    '#title' => t('Hold expiration hours'),
    '#size' => 5,
    '#maxlength' => 5,
    '#required' => FALSE,
    '#description' => t('The minimum number of hours a registration can remain held before it is taken out of held state and no longer counts against capacity. For no limit, use 0 (default is 1).<br><strong>Note</strong>: registrations are removed from held state by cron, so the number of hours specified is the minimum amount of time a registration will be held for; it can be held for longer depending on when the next cron run is after the minimum amount of time has elapsed.'),
    '#default_value' => isset($registration_type->held_expire) ? $registration_type->held_expire : 1,
  );
  // Setting for which state a registration will be put in if its hold expires.
  $form['data']['held_expire_state'] = array(
    '#type' => 'select',
    '#title' => t('Hold expiration state'),
    '#options' => $state_options,
    '#required' => FALSE,
    '#description' => t('The state a registration will be put into when its hold expires (default is "canceled").'),
    '#default_value' => isset($registration_type->held_expire_state) ? $registration_type->held_expire_state : 'canceled',
  );

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save registration type'),
    '#weight' => 40,
  );

  if (!$registration_type->locked && $op != 'add') {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete registration type'),
      '#weight' => 45,
      '#limit_validation_errors' => array(),
      '#submit' => array('registration_type_form_submit_delete'),
    );
  }

  return $form;
}

/**
 * Ajax callback for registration_mapping_form().
 */
function registration_mapping_form_callback($form, &$form_state) {
  return $form['drupal_entity'];
}

/**
 * Form API submit callback for the type form.
 */
function registration_type_form_submit(&$form, &$form_state) {
  // Set default state to NULL if 'none' is selected for database insert.
  if ($form_state['values']['default_state'] == 'none') {
    $form_state['values']['default_state'] = NULL;
  }

  $registration_type = entity_ui_form_submit_build_entity($form, $form_state);
  $registration_type->save();
  menu_rebuild();
  $form_state['redirect'] = 'admin/structure/registration/registration_types';
}

/**
 * Form API submit callback for the delete button.
 */
function registration_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/registration/registration_types/manage/' . $form_state['registration_type']->name . '/delete';
}

/**
 * Return all possible Drupal properties for a given entity type.
 *
 * @param string $entity_type
 *   Name of entity whose properties to list.
 * @param string $entity_bundle
 *   Optional entity bundle to get properties for.
 *
 * @return array
 *   List of entities that can be used as an #options list.
 */
function registration_email_fieldmap_options($entity_type, $entity_bundle = NULL) {
  $options = array('' => t('-- Select --'));

  $properties = entity_get_all_property_info($entity_type);
  if (isset($entity_bundle)) {
    $info = entity_get_property_info($entity_type);
    $properties = $info['properties'];
    if (isset($info['bundles'][$entity_bundle])) {
      $properties += $info['bundles'][$entity_bundle]['properties'];
    }
  }

  foreach ($properties as $key => $property) {
    $type = isset($property['type']) ? entity_property_extract_innermost_type($property['type']) : 'text';
    $is_entity = ($type == 'entity') || (bool) entity_get_info($type);
    // Leave entities out of this.
    if (!$is_entity) {
      if (isset($property['field']) && $property['field'] && !empty($property['property info'])) {
        foreach ($property['property info'] as $sub_key => $sub_prop) {
          $options[$property['label']][$key . ':' . $sub_key] = $sub_prop['label'];
        }
      }
      else {
        $options[$key] = $property['label'];
      }
    }
  }

  return $options;
}
