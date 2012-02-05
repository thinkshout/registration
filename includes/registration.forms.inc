<?php

/**
 * Form callback: create or edit a registration.
 *
 * @param $registration
 *   The registration object to edit or for a create form an empty registration
 *   object with a node defined.
 */
function registration_form($form, &$form_state, $registration) {
  $form_state['registration'] = $registration;

	// default email from existing registraiton or authenticted user
  global $user;
	$mail = '';
	if (isset($registration->registration_id)) {
		$mail = $registration->mail;
	}
	else if ($user->uid) {
		$mail = $user->mail;
	}

  $form['mail'] = array(
    '#type' => 'textfield',
    '#title' => t('Email'),
    '#description' => t('The email to associate with this registration.'),
    '#default_value' => $mail,
    '#size' => 40,
    '#maxlength' => 255,
    '#required' => TRUE,
  );

  $settings  = registration_entity_settings($registration->entity_id, $registration->entity_type);
  $capacity  = $settings['capacity'];
  $remaining = 20;
  if ($capacity) {
    $remaining = $capacity - registration_event_count($registration->entity_id, $registration->entity_type);
  }
  $options = array();
  for ($i = 1; $i < $remaining + 1; $i++) {
    $options[$i] = $i;
  }
  $form['count'] = array(
    '#type' => 'select',
    '#title' => t('Count'),
    '#description' => t('Select the number of registrations you want to create.'),
    '#options' => $options,
    '#default_value' => 1,
		'#access' => isset($settings['settings']['multiple_registrations']) ? 
				$settings['settings']['multiple_registrations'] : FALSE
  );

  field_attach_form('registration', $registration, $form, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Registration'),
  );

  // add a delete button for existing registration
  if (isset($registration->registration_id) &&
    user_access('administer registration')
  ) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('registration_form_delete_submit'),
    );
  }

  $uri = entity_uri($registration->entity_type,
      $registration->entity);
  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => $uri['path'],
  );

  return $form;
}

/**
 * Validation callback for registration_form().
 */
function registration_form_validate($form, &$form_state) {
  $registration = $form_state['registration'];

  // verify the event hasn't sold out
  if (!registration_has_room($registration->entity_id, $registration->entity_type)) {
    drupal_set_message(t('Sorry, this event has sold out during your registration.'));
  }

  // validate email address
  if (!valid_email_address($form_state['values']['mail'])) {
    form_set_error('mail', t('The email address is invalid.'));
  }
	// check for duplicate registrations
	else {
		$query = db_select('registration', 'r')
			->condition('entity_id', $registration->entity_id)
      ->condition('entity_type', $registration->entity_type)
			->condition('mail', $form_state['values']['mail']);
		
		// exclude existing registration
		if (isset($registration->registration_id)) {
			$query->condition('registration_id', $registration->registration_id, '<>');
		}

		$count = $query->countQuery()->execute()->fetchField();		
		if ($count > 0) {
			form_set_error('mail', t('%mail is already registered for this event.', 
				array('%mail' => $form_state['values']['mail'])));
		}
	}

  // Notify field widgets to validate their data.
  field_attach_form_validate('registration', $registration, $form, $form_state);
}

/**
 * Submit callback for registration_form().
 */
function registration_form_submit($form, &$form_state) {
  $registration = $form_state['registration'];

  // Set the registration's author uid
  global $user;
  $registration->author_uid = $user->uid;
  $registration->mail = $form_state['values']['mail'];
  $registration->count = $form_state['values']['count'];

  // Notify field widgets.
  field_attach_submit('registration', $registration, $form, $form_state);

  // Save the registration and redirect to the tour node.
  if (registration_save($registration)) {
    drupal_set_message(t('Thanks you, your registration has been recieved!'));
    $uri = entity_uri($registration->entity_type, $registration->entity);

    $form_state['redirect'] = $uri['path'];
  }
  else {
    drupal_set_message(t('Sorry, there has been a problem submitting your 
				registration.'));
  }
}

/**
 * Button submit function: handle the 'Delete' button on the node form.
 */
function registration_form_delete_submit($form, &$form_state) {
  $destination = array();
  if (isset($_GET['destination'])) {
    $destination = drupal_get_destination();
    unset($_GET['destination']);
  }
  $registration = $form_state['registration'];
  $form_state['redirect'] = array('registration/' .
    $registration->registration_id . '/delete', array('query' => $destination),
  );
}

/**
 * Menu callback -- ask for confirmation of node deletion
 */
function registration_delete_confirm($form, &$form_state, $registration) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['registration'] = array(
    '#type' => 'value',
		'#value' => $registration,
  );
  return confirm_form($form,
    t('Are you sure you want to delete registration %title?',
      array('%title' => $registration->registration_id)
    ),
    'registration/' . $registration->registration_id,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Execute node deletion
 */
function registration_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $registration = $form_state['values']['registration'];
    registration_delete_multiple(array($registration->registration_id));
    watchdog('registration', 'Registration %id deleted.', array('%id' => $registration->registration_id));
    drupal_set_message(t('Registration %id has been deleted.', array('%id' => $registration->registration_id)));

    $uri = entity_uri('registration', $registration);
    $form_state['redirect'] = $uri['path'];
  }
}

/**
 * Return a form for sending a broadcast email to participants.
 */
function registration_registrations_broadcast_form($form, &$form_state, $entity_type, $entity) {
  // we'll need this info when we submit the form
  list($entity_id, ,) = entity_extract_ids($entity_type, $entity);
  $form_state['entity'] = array(
    'entity_id' => $entity_id,
    'entity_type' => $entity_type
  );

  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('The subject of the message.'),
    '#required' => TRUE,
    '#size' => 40,
    '#maxlength' => 40,
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('Message'),
    '#description' => t('Enter the message you want to send to this tour\'s participants.'),
    '#required' => TRUE,
    '#cols' => 60,
    '#rows' => 5,
  );
  $form['send'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );

  return $form;
}

/**
 * Submit handler for registration_registrations_broadcast_form.
 */
function registration_registrations_broadcast_form_submit($form, &$form_state) {
  registration_send_broadcast(
    $form_state['entity']['entity_type'],
    $form_state['entity']['entity_id'],
    $form_state['values']['subject'],
    $form_state['values']['message']
  );
}

/**
 * Return a form for a node's registration settings.
 *
 * @param array $form
 * @param array $form_state
 * @param object $node
 */
function registration_registrations_settings_form($form, &$form_state, $entity_type, $entity) {
  list($entity_id, ,) = entity_extract_ids($entity_type, $entity);
  $label = entity_label($entity_type, $entity);

  // we'll need this info when we submit the form
  $form_state['entity'] = array(
    'entity_id' => $entity_id,
    'entity_type' => $entity_type
  );

  $settings = registration_entity_settings($entity_id, $entity_type);

  $form['status'] = array(
    '#type' => 'checkbox',
    '#title' => t('Enable'),
    '#description' => t('Check to enable registrations for %name.',
      array('%name' => $label)
    ),
    '#default_value' => isset($settings['status']) ? $settings['status'] : -1,
  );
  $form['capacity'] = array(
    '#type' => 'textfield',
    '#title' => t('Capacity'),
    '#description' => t('The maximum number of registrants for %name. Leave at 0
	 			for no limit.', array('%name' => $label)),
    '#size' => 5,
    '#maxlength' => 10,
		'#required' => TRUE,
    '#default_value' => isset($settings['capacity']) ? $settings['capacity'] : 0,
  );

	$form['reminder'] = array(
	  '#type' => 'fieldset',
	  '#title' => t('Reminder'),
	  '#collapsible' => TRUE,
	  '#collapsed' => FALSE,
	);
	$form['reminder']['send_reminder'] = array(
	  '#type' => 'checkbox',
		'#title' => 'Send Reminder',
		'#description' => 'If checked, a reminder will be sent to registrants on the following date.',
	  '#default_value' => isset($settings['send_reminder']) ? $settings['send_reminder'] : -1,
	);
	
	$reminder_date = array();
	if (isset($settings['reminder_date'])) {
		$datetime = strtotime($settings['reminder_date']);
		$reminder_date = array(
			'year' => date('Y', $datetime),
			'month' => date('n', $datetime),
			'day' => date('d', $datetime),
		);		
	}

	$form['reminder']['reminder_settings'] = array(
	  '#type' => 'fieldset',
	  '#title' => t('Settings'),
	  '#collapsible' => FALSE,
		'#states' => array(
		  'visible' => array(
		    ':input[name="send_reminder"]' => array('checked' => TRUE),
		  ),
		)
	);	
	$form['reminder']['reminder_settings']['reminder_date'] = array(
		'#type' => 'date',
		'#title' => 'Reminder Date',
	  '#default_value' => $reminder_date,
	);	
	$form['reminder']['reminder_settings']['reminder_template'] = array(
		'#type' => 'textarea',
		'#title' => 'Template',
	  '#default_value' => isset($settings['reminder_template']) ? $settings['reminder_template'] : '',
		'#description' => t('The reminder message sent to registrants. Tokens are supported if the module is enabled, E.g., [node:title].'),
	);
	// add token support
	if (module_exists('token')) {
	  $form['reminder']['reminder_settings']['token_tree'] = array(
	    '#theme' => 'token_tree', 
	    '#token_types' => array('node', 'content-type', 'current-page', 'registration'),
			'#global_types' => FALSE,
	  );
	}

	$form['settings'] = array(
	  '#type' => 'fieldset',
	  '#title' => t('Additional Settings'),
	  '#collapsible' => TRUE,
	  '#collapsed' => FALSE,
		'#tree' => TRUE
	);
	$form['settings']['multiple_registrations'] = array(
	  '#type' => 'checkbox',
	  '#title' => t('Allow multiple registrations'),
		'#description' => t('If selected, users can register for more than one slot for this event.'),
	  '#default_value' => isset($settings['settings']['multiple_registrations']) ? $settings['settings']['multiple_registrations'] : -1,
	);
	$form['settings']['from_address'] = array(
	  '#type' => 'textfield',
	  '#title' => t('From Address'),
		'#description' => t('From email address to use for confirmations, reminders, and broadcast emails.'),
		'#required' => TRUE,
	  '#default_value' => isset($settings['settings']['from_address']) ? 
				$settings['settings']['from_address'] : 
				variable_get('site_mail', ini_get('sendmail_from')),
	);
	
  $form['save'] = array(
    '#type' => 'submit',
    '#value' => t('Save Settings'),
  );

  return $form;
}

/**
 * Submit handler for registration_registrations_settings_form().
 *
 * @param array $form
 * @param array $form_state
 */
function registration_registrations_settings_form_validate($form, &$form_state) {
	// ensure capacity is a positive integer
	$capacity = $form_state['values']['capacity'];
	if (!is_numeric($capacity) || ((int)$capacity != $capacity) || ($capacity < 0)) {
		form_set_error('capacity', t('Capacity must be a positive integer.'));
	}
	
	// validate from address
	if (!valid_email_address($form_state['values']['settings']['from_address'])) {
		form_set_error('settings][from_address', t('From email address is invalid.'));
	}	
}

/**
 * Submit handler for registration_registrations_settings_form().
 *
 * @param array $form
 * @param array $form_state
 */
function registration_registrations_settings_form_submit($form, &$form_state) {
  $entity = $form_state['entity'];
  $fields = array(
    'status' => $form_state['values']['status'], 
    'capacity' => $form_state['values']['capacity'], 
		'send_reminder' => $form_state['values']['send_reminder'],
		'settings' => serialize($form_state['values']['settings']),
  );
	if ($form_state['values']['send_reminder']) {
		$fields['reminder_date'] = $form_state['values']['reminder_date']['year'] . '-' . $form_state['values']['reminder_date']['month'] . '-' . $form_state['values']['reminder_date']['day'];
		$fields['reminder_template'] = $form_state['values']['reminder_template'];
	}
	registration_update_entity_settings($entity['entity_id'], $entity['entity_type'], $fields);

  $uri = entity_uri($entity['entity_type'],
      entity_load_single($entity['entity_type'], $entity['entity_id']));
  drupal_goto($uri['path']);
}
