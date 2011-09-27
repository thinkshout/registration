<?php

/**
 * Form callback: create or edit a contact.
 *
 * @param $contact
 *   The contact object to edit or for a create form an empty contact object
 *     with only a contact type defined.
 */
function registration_form($form, &$form_state, $registration) {
  $form_state['registration'] = $registration;

	global $user;
	$form['mail'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Email'),
	  '#description' => t('The email to associate with this registration.'),
		'#default_value' => $user->uid ? $user->mail : '',
	  '#size' => 40,
	  '#maxlength' => 255,
		'#required' => TRUE
	);
	
	$settings = registration_node_settings($registration->nid);
  $capacity = $settings['capacity'];
	$remaining = 20;
	if ($capacity) {
		$remaining = $capacity - registration_event_count($registration->nid);		
	}
	$options = array();
	for ($i=1; $i < $remaining+1; $i++) { 
		$options[$i] = $i;
	}
	$form['count'] = array(
	  '#type' => 'select',
	  '#title' => t('Count'),
	  '#description' => t('Select the number of registrations you want to create.'),
	  '#options' => $options,
	  '#default_value' => 1,
	);
	

  field_attach_form('registration', $registration, $form, $form_state);

  $form['actions'] = array('#type' => 'actions');
  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save Registration'),
  );
  
  // add a delete button for existing registration
  if(isset($registration->registration_id)) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
    );
  }
  
  if (!empty($registration->registration_id) && 
			user_access('administer registration')) {
    $form['actions']['delete'] = array(
      '#type' => 'submit',
      '#value' => t('Delete'),
      '#submit' => array('registration_form_delete_submit'),
    );
  }

  $form['actions']['cancel'] = array(
    '#type' => 'link',
    '#title' => t('Cancel'),
    '#href' => 'node/' . $registration->nid,
  );


  return $form;
}

/**
 * Validation callback for registration_form().
 */
function registration_form_validate($form, &$form_state) {
  $registration = $form_state['registration'];

	// verify the event hasn't sold out
	if (!registration_has_room($registration->nid)) {
		drupal_set_message(t('Sorry, %name has sold out during your registration.', 
				array('%name' => $node->title)));
	}
	
	// validate email address
  if (!valid_email_address($form_state['values']['mail'])) {
		form_set_error('mail', t('The email address is invalid.'));
  }
	
  // Notify field widgets to validate their data.
  field_attach_form_validate('registration', $registration, $form, $form_state);
}

/**
 * Submit callback for registration_form().
 */
function registration_form_submit($form, &$form_state) {
  $registration = $form_state['registration'];

  // Set the contact's author uid
  global $user;
  $registration->author_uid = $user->uid;
  $registration->mail = $form_state['values']['mail'];
	$registration->count = $form_state['values']['count'];

  // Notify field widgets.
  field_attach_submit('registration', $registration, $form, $form_state);
  
  // Save the registration and redirect to the tour node.
  if (registration_save($registration)) {
    drupal_set_message(t('Thanks you, your registration has been recieved!'));
    $form_state['redirect'] = 'node/' . $registration->nid;    
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
		$registration->registration_id . '/delete', array('query' => $destination));
}


/**
 * Menu callback -- ask for confirmation of node deletion
 */
function registration_delete_confirm($form, &$form_state, $registration) {
  // Always provide entity id in the same form key as in the entity edit form.
  $form['registration_id'] = array(
		'#type' => 'value', '
		#value' => $registration->registration_id
	);
  return confirm_form($form,
    t('Are you sure you want to delete registration %title?', 
				array('%title' => $registration->registration_id)),
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
    $id = $form_state['values']['registration_id'];
    registration_delete_multiple(array($id));
    watchdog('registration', 'Registration %id deleted.', array('%id' => $id));
    drupal_set_message(t('Registration %id has been deleted.', array('%id' => $id)));
  }

  $form_state['redirect'] = '<front>';
}

/**
 * Return a form for sending a broadcast email to participants.
 */
function registration_registrations_broadcast_form($form, &$form_state, $node) {
  $form_state['node'] = $node;

  $form['subject'] = array(
    '#type' => 'textfield',
    '#title' => t('Subject'),
    '#description' => t('The subject of the message.'),
    '#required' => TRUE,
    '#size' => 40,
    '#maxlength' => 255,
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
		$form_state['node'], 
		$form_state['values']['subject'],
		$form_state['values']['message']);
}

/**
 * Return a form for a node's registration settings.
 *
 * @param array $form 
 * @param array $form_state 
 * @param object $node 
 */
function registration_registrations_settings_form($form, &$form_state, $node) {
	$form_state['node'] = $node;
	
	$settings = registration_node_settings($node->nid);

	$form['status'] = array(
	  '#type' => 'checkbox',
	  '#title' => t('Enable'),
	  '#description' => t('Check to enable registrations for %name.', 
			array('%name' => $node->title)),
	  '#default_value' => isset($settings['status']) ? $settings['status'] : -1,
	);
	
  
	$form['capacity'] = array(
	  '#type' => 'textfield',
	  '#title' => t('Capacity'),
	  '#description' => t('The maximum number of regsitrants for %name. Leave at 0
	 			for no limit.', array('%name' => $node->title)),
	  '#size' => 5,
	  '#maxlength' => 10,
		'#default_value' => isset($settings['capacity']) ? $settings['capacity'] : 0
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
function registration_registrations_settings_form_submit($form, &$form_state) {
  $node = $form_state['node'];
	$fields = array(
		'nid' => $node->nid,
		'status' => $form_state['values']['status'],
		'capacity' => $form_state['values']['capacity'],
	);
  db_update('registration_node')
    ->fields($fields)
	  ->condition('nid', $node->nid, '=')
    ->execute();

	drupal_set_message(t('Registration settings have been saved.'));
	drupal_goto('node/' . $node->nid);
}
