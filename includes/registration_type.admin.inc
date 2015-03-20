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
  // Settings for registration type.
  $form['data'] = array(
    '#type' => 'fieldset',
    '#title' => t('Additional Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
    '#tree' => TRUE,
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
  $states = registration_get_states();
  foreach ($states as $key => $value) {
    $states[$key] = $value->label;
  }
  $form['data']['held_expire_state'] = array(
    '#type' => 'select',
    '#title' => t('Hold expiration state'),
    '#options' => $states,
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
 * Form API submit callback for the type form.
 */
function registration_type_form_submit(&$form, &$form_state) {
  $registration_type = entity_ui_form_submit_build_entity($form, $form_state);
  $registration_type->save();
  $form_state['redirect'] = 'admin/structure/registration/registration_types';
}

/**
 * Form API submit callback for the delete button.
 */
function registration_type_form_submit_delete(&$form, &$form_state) {
  $form_state['redirect'] = 'admin/structure/registration/registration_types/manage/' . $form_state['registration_type']->name . '/delete';
}
